#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <chrono>
#include <thread>

#pragma comment(lib, "gdiplus.lib")

namespace Overlay {

    HWND hwndOverlay = nullptr;
    HDC hdcBackbuffer = nullptr;
    HBITMAP hbmBackbuffer = nullptr;
    void* pBits = nullptr;
    RECT screenRect{};
    const wchar_t* g_className = nullptr;
    int desiredFPS = 300;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    ULONG_PTR gdiplusToken;

    int G_Width = GetSystemMetrics(SM_CXSCREEN);
    int G_Height = GetSystemMetrics(SM_CYSCREEN);

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    bool InitOverlay(const wchar_t* windowName = L"overlay", const wchar_t* className = L"overlayclass") {
        g_className = className;
        GetWindowRect(GetDesktopWindow(), &screenRect);

        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) != Gdiplus::Ok)
            return false;

        WNDCLASS wc = {};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = className;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        RegisterClass(&wc);

        hwndOverlay = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            className,
            windowName,
            WS_POPUP,
            0, 0, screenRect.right, screenRect.bottom,
            nullptr, nullptr, GetModuleHandle(nullptr), nullptr
        );

        if (!hwndOverlay)
            return false;

        LONG_PTR exStyle = GetWindowLongPtr(hwndOverlay, GWL_EXSTYLE);
        exStyle |= WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
        SetWindowLongPtr(hwndOverlay, GWL_EXSTYLE, exStyle);

        ShowWindow(hwndOverlay, SW_SHOW);
        UpdateWindow(hwndOverlay);

        HDC screenDC = GetDC(nullptr);
        hdcBackbuffer = CreateCompatibleDC(screenDC);

        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = screenRect.right;
        bmi.bmiHeader.biHeight = -screenRect.bottom;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        hbmBackbuffer = CreateDIBSection(hdcBackbuffer, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
        SelectObject(hdcBackbuffer, hbmBackbuffer);

        ReleaseDC(nullptr, screenDC);

        return true;
    }

    Gdiplus::Graphics* BeginRender() {
        if (pBits)
            memset(pBits, 0, screenRect.right * screenRect.bottom * 4);
        Gdiplus::Graphics* g = new Gdiplus::Graphics(hdcBackbuffer);
        g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
        g->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
        return g;
    }

    void EndRender(Gdiplus::Graphics* g) {
        delete g;
        POINT ptZero = { 0, 0 };
        SIZE size = { screenRect.right, screenRect.bottom };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION blend = {};
        blend.BlendOp = AC_SRC_OVER;
        blend.BlendFlags = 0;
        blend.SourceConstantAlpha = 255;
        blend.AlphaFormat = AC_SRC_ALPHA;

        HDC hdcScreen = GetDC(nullptr);
        UpdateLayeredWindow(hwndOverlay, hdcScreen, &ptZero, &size, hdcBackbuffer, &ptSrc, 0, &blend, ULW_ALPHA);
        ReleaseDC(nullptr, hdcScreen);
    }

    void CloseOverlay() {
        if (hbmBackbuffer) DeleteObject(hbmBackbuffer);
        if (hdcBackbuffer) DeleteDC(hdcBackbuffer);
        if (hwndOverlay) DestroyWindow(hwndOverlay);
        if (g_className) UnregisterClass(g_className, GetModuleHandle(nullptr));
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }

    bool PumpMessages() {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                return false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        auto now = std::chrono::high_resolution_clock::now();
        auto frameDuration = std::chrono::milliseconds(1000 / desiredFPS);
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrameTime);
        if (elapsed < frameDuration)
            std::this_thread::sleep_for(frameDuration - elapsed);
        lastFrameTime = std::chrono::high_resolution_clock::now();
        return true;
    }
}
