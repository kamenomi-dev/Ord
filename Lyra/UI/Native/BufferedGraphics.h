#pragma once
#define GDIPVER 0x0110 // NOSONAR, this is determined by Win32 Header. It's very bad
#include <exception>
#include <Windows.h>
#include <Windowsx.h>
#include <gdiplus.h>
#undef GDIPVER

#include "GdipPtr.h"

#pragma comment(lib, "gdiplus.lib")

namespace Lyra::UI::Native {
class BufferedGraphics final {
  public:
    BufferedGraphics()                                   = default;
    BufferedGraphics(BufferedGraphics&&)                 = delete;
    BufferedGraphics(const BufferedGraphics&)            = delete;
    BufferedGraphics& operator=(BufferedGraphics&&)      = delete;
    BufferedGraphics& operator=(const BufferedGraphics&) = delete;
    ~BufferedGraphics() {
        if (_bufferDefaultBitmap) {
            SelectBitmap(_bufferContext, _bufferDefaultBitmap);
            _bufferDefaultBitmap = nullptr;
        }

        if (_bufferBitmap) {
            DeleteBitmap(_bufferBitmap);
            _bufferBitmap = nullptr;
        }

        if (_bufferContext) {
            ::DeleteDC(_bufferContext);
            _bufferContext = nullptr;
        }

        if (_windowContext) {
            ::ReleaseDC(_windowHandle, _windowContext);
            _windowHandle  = nullptr;
            _windowContext = nullptr;
        }
    }

    bool Invalid() const { return _bufferContext == nullptr; }

    void UpdateSize(LPARAM lParam = NULL) {
        if (lParam == NULL) {
            RECT rect{};
            ::GetWindowRect(_windowHandle, &rect);
            _windowSize.cx = rect.right - rect.left;
            _windowSize.cy = rect.bottom - rect.top;
        } else {
            _windowSize.cx = GET_X_LPARAM(lParam);
            _windowSize.cy = GET_Y_LPARAM(lParam);
        }

        if (_bufferBitmap) {
            DeleteBitmap(_bufferBitmap);
            _bufferBitmap = nullptr;
        };

        _bufferBitmap = CreateBufferBitmap(_windowSize.cx, _windowSize.cy);

        if (_bufferDefaultBitmap) {
            SelectBitmap(_bufferContext, _bufferBitmap);
        } else {
            _bufferDefaultBitmap = SelectBitmap(_bufferContext, _bufferBitmap);
        }

        UpdateGraphics();
    };
    bool PresentBuffer() const {
        if (!_windowContext || !_bufferContext) {
            return false;
        }

        return ::BitBlt(_windowContext, 0, 0, _windowSize.cx, _windowSize.cy, _bufferContext, 0, 0, SRCCOPY);
    };

    auto GetWindow() const { return _windowHandle; };
    auto GetGraphics() {
        if (!_pGraphics) {
            UpdateGraphics();
        }
        return _pGraphics.Get();
    }
    void BindToWindow(HWND hWnd) {
        if (!::IsWindow(hWnd)) {
            return;
        }

        if (hWnd == _windowHandle) {
            // As meeting this abort, it means the window has been binded.
            abort();
        }

        _windowHandle  = hWnd;
        _windowContext = ::GetDC(hWnd);
        _bufferContext = ::CreateCompatibleDC(nullptr);
        UpdateSize(NULL);
    }

  private:
    void UpdateGraphics() {
        _pGraphics.Reset();
        DllExports::GdipCreateFromHDC(_bufferContext, _pGraphics.AddressOf());
        DllExports::GdipSetSmoothingMode(_pGraphics.Get(), Gdiplus::SmoothingModeAntiAlias8x8);
        DllExports::GdipSetTextRenderingHint(_pGraphics.Get(), Gdiplus::TextRenderingHintClearTypeGridFit);
    }

    HBITMAP CreateBufferBitmap(int32_t width, int32_t height) const {
        if (_windowContext == nullptr) {
            return nullptr;
        }

        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth       = width;
        bmi.bmiHeader.biHeight      = height;
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        return CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    }

  private:
    GdipPtr<Gdiplus::GpGraphics> _pGraphics = {};

    SIZE    _windowSize          = {};
    HWND    _windowHandle        = nullptr;
    HDC     _windowContext       = nullptr;
    HDC     _bufferContext       = nullptr;
    HBITMAP _bufferBitmap        = nullptr;
    HBITMAP _bufferDefaultBitmap = nullptr;
};
} // namespace Lyra::UI::Native