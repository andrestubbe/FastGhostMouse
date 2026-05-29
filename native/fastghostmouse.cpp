#define UNICODE
#define _UNICODE
#include <windows.h>
#include <vector>
#include <jni.h>

bool g_systemCursorHidden = false;

jbyteArray native_captureSystemCursorNative(JNIEnv* env, jobject, jintArray dimensions) {
    CURSORINFO ci = { sizeof(CURSORINFO) };
    if (!GetCursorInfo(&ci)) return nullptr;

    ICONINFO ii;
    if (!GetIconInfo(ci.hCursor, &ii)) return nullptr;

    int cw = GetSystemMetrics(SM_CXCURSOR);
    int ch = GetSystemMetrics(SM_CYCURSOR);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cw;
    bmi.bmiHeader.biHeight = -ch; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hbm = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);

    memset(pBits, 0, cw * ch * 4);
    DrawIconEx(hdcMem, 0, 0, ci.hCursor, cw, ch, 0, NULL, DI_NORMAL);

    jbyteArray result = env->NewByteArray(cw * ch * 4);
    env->SetByteArrayRegion(result, 0, cw * ch * 4, (jbyte*)pBits);

    jint dims[2] = { cw, ch };
    env->SetIntArrayRegion(dimensions, 0, 2, dims);

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbm);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);

    return result;
}

void native_setSystemCursorVisible(JNIEnv*, jobject, jboolean visible) {
    if (visible == !g_systemCursorHidden) return;

    if (!visible) {
        int cw = GetSystemMetrics(SM_CXCURSOR);
        int ch = GetSystemMetrics(SM_CYCURSOR);
        std::vector<BYTE> andMask(cw * ch / 8, 0xFF);
        std::vector<BYTE> xorMask(cw * ch / 8, 0x00);
        HCURSOR hBlank = CreateCursor(GetModuleHandle(NULL), 0, 0, cw, ch, andMask.data(), xorMask.data());
        SetSystemCursor(hBlank, 32512); // OCR_NORMAL
        g_systemCursorHidden = true;
    } else {
        SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
        g_systemCursorHidden = false;
    }
}

extern "C" {
JNIEXPORT void JNICALL Java_fastghostmouse_FastGhostMouse_registerNatives(JNIEnv* env, jclass cls) {
    JNINativeMethod methods[] = {
        { (char*)"captureSystemCursorNative", (char*)"([I)[B", (void*)native_captureSystemCursorNative },
        { (char*)"setSystemCursorVisible", (char*)"(Z)V", (void*)native_setSystemCursorVisible }
    };
    env->RegisterNatives(cls, methods, 2);
}
}
