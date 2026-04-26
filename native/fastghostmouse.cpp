// ghostmouse.cpp
// GhostMouse 3.0 – DirectComposition + Multi-Monitor

#include <windows.h>
#include <d3d11.h>
#include <d2d1_1.h>
#include <dcomp.h>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <jni.h>

// D3D / D2D / DComp
ID3D11Device*            g_d3dDevice = nullptr;
ID3D11DeviceContext*     g_d3dCtx    = nullptr;
ID2D1Factory1*           g_d2dFactory = nullptr;
ID2D1Device*             g_d2dDevice = nullptr;
ID2D1DeviceContext*      g_d2dCtx = nullptr;
IDCompositionDevice*     g_dcompDevice = nullptr;

// Cursor + Text Visuals
struct Overlay {
    HMONITOR hMon;
    RECT rc;
    HWND hWnd;

    IDCompositionTarget* target = nullptr;
    IDCompositionVisual* cursorVisual = nullptr;
    IDCompositionVisual* textVisual = nullptr;

    IDCompositionSurface* cursorSurface = nullptr;
    IDCompositionSurface* textSurface = nullptr;
};

std::vector<Overlay> g_overlays;

// Cursor state
float g_targetX = 0;
float g_targetY = 0;
float g_currentX = 0;
float g_currentY = 0;

std::mutex g_mutex;
bool g_running = false;
std::thread g_thread;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
        SetTextColor(hdc, RGB(255, 255, 255));
        TextOutW(hdc, 10, 10, L"FastGhostMouse Debug", 18);
        EndPaint(hWnd, &ps);
        return 0;
    }
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
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    if (!RegisterClassExW(&wc)) {
        // Class might already be registered, that's OK
    }
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMon, HDC, LPRECT rc, LPARAM) {
    Overlay ov{};
    ov.hMon = hMon;
    ov.rc = *rc;

    // Create a small visible window in the center of the monitor for debugging
    int w = 400;
    int h = 300;
    int x = rc->left + (rc->right - rc->left) / 2 - w / 2;
    int y = rc->top + (rc->bottom - rc->top) / 2 - h / 2;

    ov.hWnd = CreateWindowExW(
        0, // No extended styles
        L"GhostMouseClass", L"FastGhostMouse Debug", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        x, y, w, h,
        nullptr, nullptr, GetModuleHandle(NULL), nullptr
    );

    if (!ov.hWnd) return TRUE; // Skip this monitor if window creation fails

    MessageBoxW(ov.hWnd, L"Window created successfully!", L"FastGhostMouse Debug", MB_OK);

    InvalidateRect(ov.hWnd, nullptr, TRUE);
    UpdateWindow(ov.hWnd);

    g_overlays.push_back(ov);
    return TRUE;
}

void InitMonitors() {
    RegisterGhostClass();
    g_overlays.clear();

    // Create a single test window at fixed position
    Overlay ov{};
    ov.hMon = nullptr;
    ov.rc = {0, 0, 1920, 1080};

    ov.hWnd = CreateWindowExW(
        0,
        L"GhostMouseClass", L"FastGhostMouse Debug", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100, 400, 300,
        nullptr, nullptr, GetModuleHandle(NULL), nullptr
    );

    if (!ov.hWnd) {
        DWORD error = GetLastError();
        wchar_t msg[256];
        swprintf_s(msg, L"Window creation failed! Error: %lu", error);
        MessageBoxW(nullptr, msg, L"FastGhostMouse Error", MB_OK | MB_ICONERROR);
        return;
    }

    MessageBoxW(nullptr, L"Window created successfully!", L"FastGhostMouse Debug", MB_OK);
    g_overlays.push_back(ov);
}

void InitGraphics() {
    D3D_FEATURE_LEVEL fl;
    HRESULT hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr, 0, D3D11_SDK_VERSION,
        &g_d3dDevice, &fl, &g_d3dCtx
    );
    if (FAILED(hr) || !g_d3dDevice) return;

    D2D1_FACTORY_OPTIONS opts = {};
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                      __uuidof(ID2D1Factory1),
                      &opts,
                      (void**)&g_d2dFactory);
    if (FAILED(hr) || !g_d2dFactory) return;

    IDXGIDevice* dxgiDevice = nullptr;
    hr = g_d3dDevice->QueryInterface(&dxgiDevice);
    if (FAILED(hr) || !dxgiDevice) return;

    hr = g_d2dFactory->CreateDevice(dxgiDevice, &g_d2dDevice);
    if (FAILED(hr) || !g_d2dDevice) {
        dxgiDevice->Release();
        return;
    }

    hr = g_d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        &g_d2dCtx
    );
    if (FAILED(hr) || !g_d2dCtx) {
        dxgiDevice->Release();
        return;
    }

    IDXGIDevice* dxgiDevice2 = nullptr;
    hr = g_d3dDevice->QueryInterface(&dxgiDevice2);
    if (FAILED(hr) || !dxgiDevice2) {
        dxgiDevice->Release();
        return;
    }

    hr = DCompositionCreateDevice(
        dxgiDevice2,
        __uuidof(IDCompositionDevice),
        (void**)&g_dcompDevice
    );
    dxgiDevice->Release();
    dxgiDevice2->Release();
    if (FAILED(hr) || !g_dcompDevice) return;
}

void InitCompositionForMonitor(Overlay& ov) {
    HRESULT hr = g_dcompDevice->CreateTargetForHwnd(ov.hWnd, TRUE, &ov.target);
    if (FAILED(hr) || !ov.target) return;

    hr = g_dcompDevice->CreateVisual(&ov.cursorVisual);
    if (FAILED(hr) || !ov.cursorVisual) return;

    hr = g_dcompDevice->CreateVisual(&ov.textVisual);
    if (FAILED(hr) || !ov.textVisual) return;

    // Set cursor visual as root
    hr = ov.target->SetRoot(ov.cursorVisual);
    if (FAILED(hr)) return;
}

IDCompositionSurface* CreateCursorSurface() {
    if (!g_dcompDevice || !g_d2dCtx) return nullptr;

    const int w = 32;
    const int h = 32;

    IDCompositionSurface* surface = nullptr;
    HRESULT hr = g_dcompDevice->CreateSurface(
        w, h,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_ALPHA_MODE_PREMULTIPLIED,
        &surface
    );
    if (FAILED(hr) || !surface) return nullptr;

    POINT offset;
    ID2D1DeviceContext* ctx = nullptr;
    hr = surface->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), (void**)&ctx, &offset);
    if (FAILED(hr) || !ctx) {
        surface->Release();
        return nullptr;
    }

    ctx->Clear(D2D1::ColorF(0,0,0,0));

    // Draw a simple white circle as fallback cursor
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(16, 16), 12, 12);
    ID2D1SolidColorBrush* brush = nullptr;
    ctx->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &brush);
    if (brush) {
        ctx->FillEllipse(ellipse, brush);
        brush->Release();
    }

    surface->EndDraw();
    ctx->Release();
    return surface;
}

IDCompositionSurface* CreateTextSurface(const BYTE* rgba, int w, int h) {
    if (!g_dcompDevice || !g_d2dCtx || !rgba || w <= 0 || h <= 0) return nullptr;

    IDCompositionSurface* surface = nullptr;
    HRESULT hr = g_dcompDevice->CreateSurface(
        w, h,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_ALPHA_MODE_PREMULTIPLIED,
        &surface
    );
    if (FAILED(hr) || !surface) return nullptr;

    POINT offset;
    ID2D1DeviceContext* ctx = nullptr;
    hr = surface->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), (void**)&ctx, &offset);
    if (FAILED(hr) || !ctx) {
        surface->Release();
        return nullptr;
    }

    ctx->Clear(D2D1::ColorF(0,0,0,0));

    D2D1_BITMAP_PROPERTIES1 props =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_NONE,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED)
        );

    ID2D1Bitmap1* bmp = nullptr;
    hr = ctx->CreateBitmap(
        D2D1::SizeU(w, h),
        rgba,
        w * 4,
        props,
        &bmp
    );
    if (SUCCEEDED(hr) && bmp) {
        ctx->DrawBitmap(bmp);
        bmp->Release();
    }

    surface->EndDraw();
    ctx->Release();
    return surface;
}

void AnimationThread() {
    auto last = std::chrono::high_resolution_clock::now();

    while (g_running) {
        // Process window messages
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::milli>(now - last).count();
        last = now;

        float k = (dt / 16.0f) * 0.12f;

        {
            std::lock_guard<std::mutex> lock(g_mutex);
            g_currentX += (g_targetX - g_currentX) * k;
            g_currentY += (g_targetY - g_currentY) * k;

            for (auto& ov : g_overlays) {
                if (!ov.cursorVisual) continue;

                bool inside =
                    g_currentX >= ov.rc.left &&
                    g_currentX <= ov.rc.right &&
                    g_currentY >= ov.rc.top &&
                    g_currentY <= ov.rc.bottom;

                if (!inside) {
                    ov.cursorVisual->SetContent(nullptr);
                    if (ov.textVisual) ov.textVisual->SetContent(nullptr);
                    continue;
                }

                float lx = g_currentX - ov.rc.left;
                float ly = g_currentY - ov.rc.top;

                ov.cursorVisual->SetOffsetX(lx);
                ov.cursorVisual->SetOffsetY(ly);

                if (ov.textVisual) {
                    ov.textVisual->SetOffsetX(lx + 20);
                    ov.textVisual->SetOffsetY(ly - 40);
                }

                ov.cursorVisual->SetContent(ov.cursorSurface);
                if (ov.textVisual && ov.textSurface) {
                    ov.textVisual->SetContent(ov.textSurface);
                }
            }
        }

        if (g_dcompDevice) {
            g_dcompDevice->Commit();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// Native implementation functions (no JNI prefix)
void native_init(JNIEnv*, jobject, jint startX, jint startY) {
    // Temporarily disable DirectComposition to test window visibility
    // Initialize COM for DirectComposition
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) return;

    InitMonitors();
    if (g_overlays.empty()) {
        CoUninitialize();
        return;
    }

    // Set initial cursor position
    g_currentX = startX;
    g_currentY = startY;
    g_targetX = startX;
    g_targetY = startY;

    // Don't initialize DirectComposition for now - just show windows
    // for (auto& ov : g_overlays) {
    //     InitCompositionForMonitor(ov);
    // }

    g_running = true;
    g_thread = std::thread(AnimationThread);
}

void native_moveTo(JNIEnv*, jobject, jfloat x, jfloat y) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_targetX = x;
    g_targetY = y;
}

void native_setTextImage(JNIEnv* env, jobject, jbyteArray data, jint w, jint h) {
    std::lock_guard<std::mutex> lock(g_mutex);

    if (!data || w <= 0 || h <= 0) {
        for (auto& ov : g_overlays)
            ov.textVisual->SetContent(nullptr);
        return;
    }

    jbyte* bytes = env->GetByteArrayElements(data, NULL);

    for (auto& ov : g_overlays) {
        if (ov.textSurface) ov.textSurface->Release();
        ov.textSurface = CreateTextSurface((BYTE*)bytes, w, h);
        ov.textVisual->SetContent(ov.textSurface);
    }

    env->ReleaseByteArrayElements(data, bytes, 0);
}

void native_setCursorImage(JNIEnv* env, jobject, jbyteArray data, jint w, jint h) {
    std::lock_guard<std::mutex> lock(g_mutex);

    for (auto& ov : g_overlays) {
        if (ov.cursorSurface) {
            ov.cursorSurface->Release();
            ov.cursorSurface = nullptr;
        }

        if (data && w > 0 && h > 0) {
            jbyte* bytes = env->GetByteArrayElements(data, NULL);
            ov.cursorSurface = CreateTextSurface((BYTE*)bytes, w, h);
            env->ReleaseByteArrayElements(data, bytes, 0);
        }

        // Always fall back to system cursor if custom image fails
        if (!ov.cursorSurface) {
            ov.cursorSurface = CreateCursorSurface();
        }

        if (ov.cursorSurface && ov.cursorVisual) {
            ov.cursorVisual->SetContent(ov.cursorSurface);
        }
    }
}

void native_setTextOffset(JNIEnv*, jobject, jfloat dx, jfloat dy) {
    std::lock_guard<std::mutex> lock(g_mutex);
}

void native_show(JNIEnv*, jobject) {
    for (auto& ov : g_overlays)
        ShowWindow(ov.hWnd, SW_SHOW);
}

void native_hide(JNIEnv*, jobject) {
    for (auto& ov : g_overlays)
        ShowWindow(ov.hWnd, SW_HIDE);
}

void native_dispose(JNIEnv*, jobject) {
    g_running = false;
    if (g_thread.joinable()) g_thread.join();

    for (auto& ov : g_overlays) {
        DestroyWindow(ov.hWnd);
    }
    g_overlays.clear();

    if (g_dcompDevice) g_dcompDevice->Release();
    if (g_d2dCtx) g_d2dCtx->Release();
    if (g_d2dDevice) g_d2dDevice->Release();
    if (g_d2dFactory) g_d2dFactory->Release();
    if (g_d3dCtx) g_d3dCtx->Release();
    if (g_d3dDevice) g_d3dDevice->Release();
}

extern "C" {

JNIEXPORT void JNICALL Java_fastghostmouse_FastGhostMouse_registerNatives(JNIEnv* env, jclass cls) {
    static JNINativeMethod methods[] = {
        { (char*)"init", (char*)"(II)V", (void*)native_init },
        { (char*)"moveTo", (char*)"(FF)V", (void*)native_moveTo },
        { (char*)"setCursorImage", (char*)"([BII)V", (void*)native_setCursorImage },
        { (char*)"setTextImage", (char*)"([BII)V", (void*)native_setTextImage },
        { (char*)"setTextOffset", (char*)"(FF)V", (void*)native_setTextOffset },
        { (char*)"show", (char*)"()V", (void*)native_show },
        { (char*)"hide", (char*)"()V", (void*)native_hide },
        { (char*)"dispose", (char*)"()V", (void*)native_dispose }
    };

    env->RegisterNatives(cls, methods, sizeof(methods)/sizeof(methods[0]));
}

}
