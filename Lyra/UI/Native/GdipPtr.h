#pragma once
// GdipPtr.h — a smart & unique pointer for gdiplus objects.
// Thanks for qk's help — https://github.com/QingKong-s/WinEzCtrlKit/blob/master/eck/GpPtr.h

#include <concepts>
#include <gdiplus.h>

namespace Lyra::UI::Native {
namespace DllExports = Gdiplus::DllExports;

template <class T>
class GdipPtr final {
  public:
    GdipPtr() = default;
    GdipPtr(GdipPtr& ptr) { ptr.Clone(*this); };
    GdipPtr& operator=(const GdipPtr&) = delete;

    explicit GdipPtr(T* ptr) noexcept : _ptr(ptr) {};
    GdipPtr(GdipPtr&& ptr) noexcept { std::swap(_ptr, ptr._ptr); };

    ~GdipPtr() {
        if (_ptr) {
            Reset();
        }
    }

  public:
    GdipPtr& operator=(nullptr_t) const noexcept {
        Reset();
        return *this;
    }
    GdipPtr& operator=(T* ptr) const noexcept {
        Reset();
        _ptr = ptr;
        return *this;
    }
    GdipPtr& operator=(GdipPtr&& ptr) noexcept {
        std::swap(_ptr, ptr._ptr);
        return *this;
    }
    T*  Get() const noexcept { return _ptr; }
    T*  operator->() const noexcept { return _ptr; }
    T** At() noexcept { return &_ptr; }

    explicit operator bool() const noexcept { return _ptr != nullptr; }

    Gdiplus::GpStatus Move(GdipPtr& ptr) {
        if (!_ptr) {
            ptr.Reset();
            return Gdiplus::GenericError;
        }

        Clone(ptr);
        Reset();
    };

    Gdiplus::GpStatus Clone(GdipPtr& ptr) {
        if (!_ptr) {
            return Gdiplus::GenericError;
        }

        if constexpr (std::is_same_v<T, Gdiplus::CGpEffect> || std::is_same_v<T, Gdiplus::GpCachedBitmap>) static_assert(!sizeof(T), "Unsupported Clone GDI+ type");
        else if constexpr (std::is_base_of_v<Gdiplus::GpBrush, T>) return DllExports::GdipCloneBrush(_ptr, (Gdiplus::GpBrush**)ptr.At());
        else if constexpr (std::is_same_v<T, Gdiplus::GpPen>) return DllExports::GdipClonePen(_ptr, ptr.At());
        else if constexpr (std::is_base_of_v<Gdiplus::GpCustomLineCap, T>) return DllExports::GdipCloneCustomLineCap(_ptr, (Gdiplus::GpCustomLineCap**)ptr.At());
        else if constexpr (std::is_base_of_v<Gdiplus::GpImage, T>) return DllExports::GdipCloneImage(_ptr, (Gdiplus::GpImage**)ptr.At());
        else if constexpr (std::is_same_v<T, Gdiplus::GpImageAttributes>) return DllExports::GdipCloneImageAttributes(_ptr, ptr.At());
        else if constexpr (std::is_same_v<T, Gdiplus::GpPath>) return DllExports::GdipClonePath(_ptr, ptr.At());
        else if constexpr (std::is_same_v<T, Gdiplus::GpRegion>) return DllExports::GdipCloneRegion(_ptr, ptr.At());
        else if constexpr (std::is_same_v<T, Gdiplus::GpMatrix>) return DllExports::GdipCloneMatrix(_ptr, ptr.At());
        else if constexpr (std::is_same_v<T, Gdiplus::GpFontFamily>) return DllExports::GdipCloneFontFamily(_ptr, ptr.At());
        else if constexpr (std::is_same_v<T, Gdiplus::GpFont>) return DllExports::GdipCloneFont(_ptr, ptr.At());
        else if constexpr (std::is_same_v<T, Gdiplus::GpStringFormat>) return DllExports::GdipCloneStringFormat(_ptr, ptr.At());
        else static_assert(!sizeof(T), "Unsupported Clone GDI+ type");
    };

    void Reset() {
        if (!_ptr) {
            return;
        }

        if constexpr (std::is_same_v<T, Gdiplus::CGpEffect>) {
            Gdiplus::GdipDeleteEffect(_ptr);
        } else if constexpr (std::is_base_of_v<Gdiplus::GpBrush, T>) {
            DllExports::GdipDeleteBrush(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpCachedBitmap>) {
            DllExports::GdipDeleteCachedBitmap(_ptr);
        } else if constexpr (std::is_base_of_v<Gdiplus::GpCustomLineCap, T>) {
            DllExports::GdipDeleteCustomLineCap(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpFont>) {
            DllExports::GdipDeleteFont(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpFontFamily>) {
            DllExports::GdipDeleteFontFamily(_ptr);
        } else if constexpr (std::is_base_of_v<Gdiplus::GpFontCollection, T>) {
            DllExports::GdipDeletePrivateFontCollection(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpGraphics>) {
            DllExports::GdipDeleteGraphics(_ptr);
        } else if constexpr (std::is_base_of_v<Gdiplus::GpImage, T>) {
            DllExports::GdipDisposeImage(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpImageAttributes>) {
            DllExports::GdipDisposeImageAttributes(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpMatrix>) {
            DllExports::GdipDeleteMatrix(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpPen>) {
            DllExports::GdipDeletePen(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpPath>) {
            DllExports::GdipDeletePath(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpPathIterator>) {
            DllExports::GdipDeletePathIter(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpRegion>) {
            DllExports::GdipDeleteRegion(_ptr);
        } else if constexpr (std::is_same_v<T, Gdiplus::GpStringFormat>) {
            DllExports::GdipDeleteStringFormat(_ptr);
        } else static_assert(!sizeof(T), "Unsupported GDI+ type");

        _ptr = nullptr;
    }
    T* Release() { return std::exchange(_ptr, nullptr); }

  private:
    T* _ptr = nullptr;
};
} // namespace Lyra::UI::Native