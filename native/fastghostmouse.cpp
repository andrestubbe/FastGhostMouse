/**
 * @file FastGhostMouse.cpp
 * @brief FastGhostMouse native implementation
 *
 * @details Implements high-performance native overlay with Direct2D and 120Hz support.
 *
 * @author FastJava Team
 * @version 4.0.0
 * @copyright MIT License
 */
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <gdiplus.h>
#include <d3d11.h>
#include <d2d1_1.h>
#include <dcomp.h>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <jni.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dcomp.lib")

using namespace Gdiplus;

// Simple overlay structure
struct Overlay {
    HMONITOR hMon;
    RECT rc;
    HWND hWnd;
    HDC hdcMem;
    HBITMAP hbmMem;
    HBITMAP hbmOld;
    void* bits;
};

std::vector<Overlay> g_overlays;

// Global State
float g_targetX = 0;
float g_targetY = 0;
float g_currentX = 0;
float g_currentY = 0;
float g_smoothing = 0.12f;

ID2D1Bitmap* g_d2dCursorBmp = nullptr;
int g_cursorW = 0, g_cursorH = 0;
ID2D1Bitmap* g_d2dTextBmp = nullptr;
int g_textW = 0, g_textH = 0;
float g_textDx = 20, g_textDy = -40;

// GDI+ Fallback Bitmaps
Bitmap* g_pCursorBmpGDI = nullptr;
Bitmap* g_pTextBmpGDI = nullptr;

std::mutex g_mutex;
bool g_running = false;
std::thread g_thread;
ULONG_PTR g_gdiplusToken;

// D3D / D2D / DComp Globals
ID3D11Device*            g_d3dDevice = nullptr;
ID3D11DeviceContext*     g_d3dCtx    = nullptr;
ID2D1Factory1*           g_d2dFactory = nullptr;
ID2D1Device*             g_d2dDevice = nullptr;
ID2D1DeviceContext*      g_d2dCtx = nullptr;
ID2D1DCRenderTarget*     g_d2dRT = nullptr;
IDCompositionDevice*     g_dcompDevice = nullptr;

HCURSOR g_hOldCursor = NULL;
bool g_systemCursorHidden = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) return 0;
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void RegisterGhostClass() {
    static bool done = false;
    if (done) return;
    done = true;

    WNDCLASSEXW wc{ sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"GhostMouseClass";
    wc.hCursor = NULL; // No cursor for our window
    wc.hbrBackground = nullptr;
    RegisterClassExW(&wc);
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMon, HDC, LPRECT rc, LPARAM) {
    Overlay ov{};
    ov.hMon = hMon;
    ov.rc = *rc;

    int w = rc->right - rc->left;
    int h = rc->bottom - rc->top;

    ov.hWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        L"GhostMouseClass", L"", WS_POPUP,
        rc->left, rc->top, w, h,
        nullptr, nullptr, GetModuleHandle(NULL), nullptr
    );

    if (!ov.hWnd) return TRUE;

    HDC hdcScreen = GetDC(nullptr);
    ov.hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    ov.hbmMem = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &ov.bits, NULL, 0);
    ov.hbmOld = (HBITMAP)SelectObject(ov.hdcMem, ov.hbmMem);
    ReleaseDC(nullptr, hdcScreen);

    ShowWindow(ov.hWnd, SW_SHOW);
    UpdateWindow(ov.hWnd);

    g_overlays.push_back(ov);
    return TRUE;
}

void InitGraphics() {
    // D3D11 Device
    D3D_FEATURE_LEVEL fl;
    D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr, 0, D3D11_SDK_VERSION,
        &g_d3dDevice, &fl, &g_d3dCtx
    );

    // D2D Factory
    D2D1_FACTORY_OPTIONS opts = {};
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                      __uuidof(ID2D1Factory1),
                      &opts,
                      (void**)&g_d2dFactory);

    // D2D Device
    IDXGIDevice* dxgiDevice = nullptr;
    g_d3dDevice->QueryInterface(&dxgiDevice);
    g_d2dFactory->CreateDevice(dxgiDevice, &g_d2dDevice);
    dxgiDevice->Release();

    // D2D DeviceContext
    g_d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        &g_d2dCtx
    );

    // D2D DC RenderTarget for GDI interop
    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE, // Use software for max compatibility with GDI DCs
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0,
        D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE
    );
    g_d2dFactory->CreateDCRenderTarget(&rtProps, &g_d2dRT);

    // DirectComposition Device
    IDXGIDevice* dxgiDeviceForDComp = nullptr;
    g_d3dDevice->QueryInterface(&dxgiDeviceForDComp);
    DCompositionCreateDevice(
        dxgiDeviceForDComp,
        __uuidof(IDCompositionDevice),
        (void**)&g_dcompDevice
    );
    dxgiDeviceForDComp->Release();
}

void InitMonitors() {
    RegisterGhostClass();
    g_overlays.clear();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0);
}

void AnimationThread() {
    auto last = std::chrono::high_resolution_clock::now();

    while (g_running) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::milli>(now - last).count();
        last = now;

        float k = (dt / 16.0f) * g_smoothing;
        if (k > 1.0f) k = 1.0f;

        {
            std::lock_guard<std::mutex> lock(g_mutex);
            g_currentX += (g_targetX - g_currentX) * k;
            g_currentY += (g_targetY - g_currentY) * k;
        }

        for (auto& ov : g_overlays) {
            if (!ov.hdcMem) continue;

            float lx = g_currentX - ov.rc.left;
            float ly = g_currentY - ov.rc.top;
            RECT rc = { 0, 0, ov.rc.right - ov.rc.left, ov.rc.bottom - ov.rc.top };

            bool d2dSuccess = false;
            if (g_d2dRT && g_d2dRT->BindDC(ov.hdcMem, &rc) == S_OK) {
                g_d2dRT->BeginDraw();
                g_d2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
                g_d2dRT->Clear(D2D1::ColorF(0, 0, 0, 0));

                std::lock_guard<std::mutex> lock(g_mutex);
                if (g_d2dCursorBmp) {
                    D2D1_RECT_F destRect = D2D1::RectF(lx - g_cursorW / 2.0f, ly - g_cursorH / 2.0f, lx + g_cursorW / 2.0f, ly + g_cursorH / 2.0f);
                    g_d2dRT->DrawBitmap(g_d2dCursorBmp, destRect);
                }
                if (g_d2dTextBmp) {
                    D2D1_RECT_F destRect = D2D1::RectF(lx + g_textDx, ly + g_textDy, lx + g_textDx + g_textW, ly + g_textDy + g_textH);
                    g_d2dRT->DrawBitmap(g_d2dTextBmp, destRect);
                }
                
                if (SUCCEEDED(g_d2dRT->EndDraw())) {
                    d2dSuccess = true;
                }
            }

            if (!d2dSuccess) {
                Graphics graphics(ov.hdcMem);
                graphics.SetSmoothingMode(SmoothingModeAntiAlias);
                graphics.Clear(Color(0, 0, 0, 0));

                std::lock_guard<std::mutex> lock(g_mutex);
                if (g_pCursorBmpGDI) {
                    graphics.DrawImage(g_pCursorBmpGDI, (float)(lx - g_cursorW / 2.0f), (float)(ly - g_cursorH / 2.0f), (float)g_cursorW, (float)g_cursorH);
                }
                if (g_pTextBmpGDI) {
                    graphics.DrawImage(g_pTextBmpGDI, (float)(lx + g_textDx), (float)(ly + g_textDy), (float)g_textW, (float)g_textH);
                }
            }

            POINT ptSrc = { 0, 0 };
            SIZE sizeWnd = { ov.rc.right - ov.rc.left, ov.rc.bottom - ov.rc.top };
            POINT ptDst = { ov.rc.left, ov.rc.top };
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
            UpdateLayeredWindow(ov.hWnd, nullptr, &ptDst, &sizeWnd, ov.hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }
}

// Native implementation functions
void native_init(JNIEnv*, jobject, jint startX, jint startY) {
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    InitGraphics();
    InitMonitors();
    if (g_overlays.empty()) return;

    g_currentX = (float)startX;
    g_currentY = (float)startY;
    g_targetX = (float)startX;
    g_targetY = (float)startY;

    g_running = true;
    g_thread = std::thread(AnimationThread);
}

void native_moveTo(JNIEnv*, jobject, jfloat x, jfloat y) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_targetX = x;
    g_targetY = y;
}

void native_setSmoothing(JNIEnv*, jobject, jfloat factor) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_smoothing = factor;
}

void native_setCursorImage(JNIEnv* env, jobject, jbyteArray rgba, jint width, jint height) {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_d2dCursorBmp) { g_d2dCursorBmp->Release(); g_d2dCursorBmp = nullptr; }
    if (g_pCursorBmpGDI) { delete g_pCursorBmpGDI; g_pCursorBmpGDI = nullptr; }

    if (rgba && width > 0 && height > 0) {
        jbyte* pData = env->GetByteArrayElements(rgba, NULL);
        
        // 1. D2D Bitmap
        if (g_d2dRT) {
            D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
            );
            g_d2dRT->CreateBitmap(D2D1::SizeU(width, height), pData, width * 4, &props, &g_d2dCursorBmp);
        }

        // 2. GDI+ Fallback Bitmap
        Bitmap* temp = new Bitmap(width, height, PixelFormat32bppARGB);
        BitmapData data;
        Rect rect(0, 0, width, height);
        temp->LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &data);
        BYTE* pOut = (BYTE*)data.Scan0;
        for (int i = 0; i < width * height; i++) {
            pOut[i * 4 + 0] = (BYTE)pData[i * 4 + 2]; // B
            pOut[i * 4 + 1] = (BYTE)pData[i * 4 + 1]; // G
            pOut[i * 4 + 2] = (BYTE)pData[i * 4 + 0]; // R
            pOut[i * 4 + 3] = (BYTE)pData[i * 4 + 3]; // A
        }
        temp->UnlockBits(&data);
        g_pCursorBmpGDI = temp;

        g_cursorW = width;
        g_cursorH = height;
        env->ReleaseByteArrayElements(rgba, pData, JNI_ABORT);
    }
}

void native_setTextImage(JNIEnv* env, jobject, jbyteArray rgba, jint width, jint height) {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_d2dTextBmp) { g_d2dTextBmp->Release(); g_d2dTextBmp = nullptr; }
    if (g_pTextBmpGDI) { delete g_pTextBmpGDI; g_pTextBmpGDI = nullptr; }

    if (rgba && width > 0 && height > 0) {
        jbyte* pData = env->GetByteArrayElements(rgba, NULL);
        
        // 1. D2D Bitmap
        if (g_d2dRT) {
            D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
            );
            g_d2dRT->CreateBitmap(D2D1::SizeU(width, height), pData, width * 4, &props, &g_d2dTextBmp);
        }

        // 2. GDI+ Fallback Bitmap
        Bitmap* temp = new Bitmap(width, height, PixelFormat32bppARGB);
        BitmapData data;
        Rect rect(0, 0, width, height);
        temp->LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &data);
        BYTE* pOut = (BYTE*)data.Scan0;
        for (int i = 0; i < width * height; i++) {
            pOut[i * 4 + 0] = (BYTE)pData[i * 4 + 2]; // B
            pOut[i * 4 + 1] = (BYTE)pData[i * 4 + 1]; // G
            pOut[i * 4 + 2] = (BYTE)pData[i * 4 + 0]; // R
            pOut[i * 4 + 3] = (BYTE)pData[i * 4 + 3]; // A
        }
        temp->UnlockBits(&data);
        g_pTextBmpGDI = temp;

        g_textW = width;
        g_textH = height;
        env->ReleaseByteArrayElements(rgba, pData, JNI_ABORT);
    }
}

void native_setTextOffset(JNIEnv*, jobject, jfloat dx, jfloat dy) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_textDx = dx;
    g_textDy = dy;
}

void native_captureSystemCursor(JNIEnv* env, jobject) {
    CURSORINFO ci = { sizeof(CURSORINFO) };
    if (!GetCursorInfo(&ci)) return;

    ICONINFO ii;
    if (!GetIconInfo(ci.hCursor, &ii)) return;

    int cw = GetSystemMetrics(SM_CXCURSOR);
    int ch = GetSystemMetrics(SM_CYCURSOR);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cw;
    bmi.bmiHeader.biHeight = -ch;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hbm = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);

    // Clear with transparent
    memset(pBits, 0, cw * ch * 4);

    DrawIconEx(hdcMem, 0, 0, ci.hCursor, cw, ch, 0, NULL, DI_NORMAL);

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_d2dCursorBmp) { g_d2dCursorBmp->Release(); g_d2dCursorBmp = nullptr; }
        if (g_pCursorBmpGDI) { delete g_pCursorBmpGDI; g_pCursorBmpGDI = nullptr; }

        if (g_d2dRT) {
            D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
            );
            g_d2dRT->CreateBitmap(D2D1::SizeU(cw, ch), pBits, cw * 4, &props, &g_d2dCursorBmp);
        }

        Bitmap* temp = new Bitmap(cw, ch, PixelFormat32bppARGB);
        BitmapData data;
        Rect rect(0, 0, cw, ch);
        temp->LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &data);
        memcpy(data.Scan0, pBits, cw * ch * 4);
        temp->UnlockBits(&data);
        g_pCursorBmpGDI = temp;

        g_cursorW = cw;
        g_cursorH = ch;

        g_cursorW = cw;
        g_cursorH = ch;
    }

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbm);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
}

void native_setSystemCursorVisible(JNIEnv*, jobject, jboolean visible) {
    if (visible == !g_systemCursorHidden) return;

    if (!visible) {
        // Hide
        int cw = GetSystemMetrics(SM_CXCURSOR);
        int ch = GetSystemMetrics(SM_CYCURSOR);
        std::vector<BYTE> andMask(cw * ch / 8, 0xFF);
        std::vector<BYTE> xorMask(cw * ch / 8, 0x00);
        HCURSOR hBlank = CreateCursor(GetModuleHandle(NULL), 0, 0, cw, ch, andMask.data(), xorMask.data());
        SetSystemCursor(hBlank, 32512); // OCR_NORMAL
        g_systemCursorHidden = true;
    } else {
        // Show
        SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
        g_systemCursorHidden = false;
    }
}

void native_show(JNIEnv*, jobject) {
    for (auto& ov : g_overlays) ShowWindow(ov.hWnd, SW_SHOW);
}

void native_hide(JNIEnv*, jobject) {
    for (auto& ov : g_overlays) ShowWindow(ov.hWnd, SW_HIDE);
}

void native_dispose(JNIEnv*, jobject) {
    g_running = false;
    if (g_thread.joinable()) g_thread.join();

    if (g_systemCursorHidden) {
        SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
    }

    for (auto& ov : g_overlays) {
        if (ov.hdcMem) {
            SelectObject(ov.hdcMem, ov.hbmOld);
            DeleteObject(ov.hbmMem);
            DeleteDC(ov.hdcMem);
        }
        DestroyWindow(ov.hWnd);
    }
    g_overlays.clear();

    if (g_d2dCursorBmp) { g_d2dCursorBmp->Release(); g_d2dCursorBmp = nullptr; }
    if (g_d2dTextBmp) { g_d2dTextBmp->Release(); g_d2dTextBmp = nullptr; }
    if (g_pCursorBmpGDI) { delete g_pCursorBmpGDI; g_pCursorBmpGDI = nullptr; }
    if (g_pTextBmpGDI) { delete g_pTextBmpGDI; g_pTextBmpGDI = nullptr; }

    if (g_d2dRT) { g_d2dRT->Release(); g_d2dRT = nullptr; }
    if (g_d2dFactory) { g_d2dFactory->Release(); g_d2dFactory = nullptr; }
    if (g_d3dCtx) { g_d3dCtx->Release(); g_d3dCtx = nullptr; }
    if (g_d3dDevice) { g_d3dDevice->Release(); g_d3dDevice = nullptr; }
    if (g_dcompDevice) { g_dcompDevice->Release(); g_dcompDevice = nullptr; }

    GdiplusShutdown(g_gdiplusToken);
}

extern "C" {
JNIEXPORT void JNICALL Java_fastghostmouse_FastGhostMouse_registerNatives(JNIEnv* env, jobject obj) {
    jclass cls = env->FindClass("fastghostmouse/FastGhostMouse");
    if (!cls) return;

    JNINativeMethod methods[] = {
        { (char*)"init", (char*)"(II)V", (void*)native_init },
        { (char*)"moveTo", (char*)"(FF)V", (void*)native_moveTo },
        { (char*)"setSmoothing", (char*)"(F)V", (void*)native_setSmoothing },
        { (char*)"setCursorImage", (char*)"([BII)V", (void*)native_setCursorImage },
        { (char*)"setTextImage", (char*)"([BII)V", (void*)native_setTextImage },
        { (char*)"setTextOffset", (char*)"(FF)V", (void*)native_setTextOffset },
        { (char*)"captureSystemCursor", (char*)"()V", (void*)native_captureSystemCursor },
        { (char*)"setSystemCursorVisible", (char*)"(Z)V", (void*)native_setSystemCursorVisible },
        { (char*)"show", (char*)"()V", (void*)native_show },
        { (char*)"hide", (char*)"()V", (void*)native_hide },
        { (char*)"dispose", (char*)"()V", (void*)native_dispose }
    };

    env->RegisterNatives(cls, methods, 11);
}
}
