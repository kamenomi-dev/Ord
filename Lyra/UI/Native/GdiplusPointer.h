// GdipPtr.h — a smart & unique pointer for gdiplus objects.
// Thanks for qk's help — https://github.com/QingKong-s/WinEzCtrlKit/blob/master/eck/GpPtr.h
#pragma once

#define GDIPVER 0x0110 // NOSONAR, this is determined by Win32 Header. It's very bad
#include <concepts>
#include <gdiplus.h>
#undef GDIPVER

namespace Lyra::UI::Native {
template <class T>
class GdiplusPointer final {
  public:
    GdiplusPointer() = default;
    GdiplusPointer(GdiplusPointer& ptr) { ptr.Clone(*this); };
    GdiplusPointer& operator=(const GdiplusPointer&) = delete;

    explicit GdiplusPointer(T* ptr) noexcept : _ptr(ptr) {};
    GdiplusPointer(GdiplusPointer&& ptr) noexcept { std::swap(_ptr, ptr._ptr); };

    ~GdiplusPointer() {
        if (_ptr) {
            Reset();
        }
    }

  public:
    GdiplusPointer& operator=(nullptr_t) const noexcept {
        Reset();
        return *this;
    }
    GdiplusPointer& operator=(T* ptr) const noexcept {
        Reset();
        _ptr = ptr;
        return *this;
    }
    GdiplusPointer& operator=(GdiplusPointer&& ptr) noexcept {
        std::swap(_ptr, ptr._ptr);
        return *this;
    }
    T*  Get() const noexcept { return _ptr; }
    T*  operator->() const noexcept { return _ptr; }
    T** AddressOf() noexcept { return &_ptr; }

    explicit operator bool() const noexcept { return _ptr != nullptr; }

    Gdiplus::GpStatus Move(GdiplusPointer& ptr) {
        if (!_ptr) {
            ptr.Reset();
            return Gdiplus::GenericError;
        }

        Clone(ptr);
        Reset();
    };

    Gdiplus::GpStatus Clone(GdiplusPointer& ptr) {
        if (!_ptr) {
            return Gdiplus::GenericError;
        }

        if constexpr (std::is_same_v<T, Gdiplus::CGpEffect> || std::is_same_v<T, Gdiplus::GpCachedBitmap>) static_assert(!sizeof(T), "Unsupported Clone GDI+ type");
        else if constexpr (std::is_base_of_v<Gdiplus::GpBrush, T>) return ::GdipCloneBrush(_ptr, (Gdiplus::GpBrush**)ptr.AddressOf());
        else if constexpr (std::is_same_v<T, Gdiplus::GpPen>) return ::GdipClonePen(_ptr, ptr.AddressOf());
        else if constexpr (std::is_base_of_v<Gdiplus::GpCustomLineCap, T>) return ::GdipCloneCustomLineCap(_ptr, (Gdiplus::GpCustomLineCap**)ptr.At());
        else if constexpr (std::is_base_of_v<Gdiplus::GpImage, T>) return ::GdipCloneImage(_ptr, (Gdiplus::GpImage**)ptr.AddressOf());
        else if constexpr (std::is_same_v<T, Gdiplus::GpImageAttributes>) return ::GdipCloneImageAttributes(_ptr, ptr.AddressOf());
        else if constexpr (std::is_same_v<T, Gdiplus::GpPath>) return ::GdipClonePath(_ptr, ptr.AddressOf());
        else if constexpr (std::is_same_v<T, Gdiplus::GpRegion>) return ::GdipCloneRegion(_ptr, ptr.AddressOf());
        else if constexpr (std::is_same_v<T, Gdiplus::GpMatrix>) return ::GdipCloneMatrix(_ptr, ptr.AddressOf());
        else if constexpr (std::is_same_v<T, Gdiplus::GpFontFamily>) return ::GdipCloneFontFamily(_ptr, ptr.AddressOf());
        else if constexpr (std::is_same_v<T, Gdiplus::GpFont>) return ::GdipCloneFont(_ptr, ptr.AddressOf());
        else if constexpr (std::is_same_v<T, Gdiplus::GpStringFormat>) return ::GdipCloneStringFormat(_ptr, ptr.AddressOf());
        else static_assert(!sizeof(T), "Unsupported Clone GDI+ type");
    };

    void Reset() {
        if (!_ptr) {
            return;
        }

        // Here is an incomplete class.
        if constexpr (std::is_same_v<T, Gdiplus::CGpEffect>) {
            Gdiplus::GdipDeleteEffect(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpCachedBitmap>) {
            ::GdipDeleteCachedBitmap(_ptr);
        }

        else if constexpr (std::is_base_of_v<Gdiplus::GpBrush, T>) {
            ::GdipDeleteBrush(_ptr);
        } else if constexpr (std::is_base_of_v<Gdiplus::GpCustomLineCap, T>) {
            ::GdipDeleteCustomLineCap(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpFont>) {
            ::GdipDeleteFont(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpFontFamily>) {
            ::GdipDeleteFontFamily(_ptr);
        } else if constexpr (std::is_base_of_v<Gdiplus::GpFontCollection, T>) {
            ::GdipDeletePrivateFontCollection(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpGraphics>) {
            ::GdipDeleteGraphics(_ptr);
        } else if constexpr (std::is_base_of_v<Gdiplus::GpImage, T>) {
            ::GdipDisposeImage(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpImageAttributes>) {
            ::GdipDisposeImageAttributes(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpMatrix>) {
            ::GdipDeleteMatrix(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpPen>) {
            ::GdipDeletePen(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpPath>) {
            ::GdipDeletePath(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpPathIterator>) {
            ::GdipDeletePathIter(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpRegion>) {
            ::GdipDeleteRegion(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpStringFormat>) {
            ::GdipDeleteStringFormat(_ptr);
        } else static_assert(!sizeof(T), "Unsupported GDI+ type");

        _ptr = nullptr;
    }
    T* Release() { return std::exchange(_ptr, nullptr); }

  private:
    T* _ptr = nullptr;
};
} // namespace Lyra::UI::Native