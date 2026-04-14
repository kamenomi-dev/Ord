#pragma once
#include <gsl/gsl>

#include "../Native/GdipPtr.h"
#include "../Native/BufferedGraphics.h"

namespace Lyra::UI::Components {
class Window;
};

namespace Lyra::UI::Foundation {
namespace Managers {
class Renderer;
};

struct RenderContext {
    gsl::owner<Managers::Renderer*> renderer  = nullptr;
    Gdiplus::Rect                   dirtyRect = {};

    static RenderContext Build(Managers::Renderer* renderer, RECT invalidRect) {
        return {
            renderer, {invalidRect.left, invalidRect.top, invalidRect.right - invalidRect.left, invalidRect.bottom - invalidRect.top}
        };
    };
};

namespace Managers {
class Renderer final {
  public:
    Renderer()                           = default;
    Renderer(Renderer&&)                 = delete;
    Renderer(const Renderer&)            = delete;
    Renderer& operator=(Renderer&&)      = delete;
    Renderer& operator=(const Renderer&) = delete;
    ~Renderer()                          = default;

    auto& AllocGraphics() { return _bufferedGraphics; };

    void Invalidate(const Gdiplus::Rect invalidatedRect) const {
        RECT rect{};
        rect.left   = invalidatedRect.X;
        rect.top    = invalidatedRect.Y;
        rect.right  = invalidatedRect.GetRight();
        rect.bottom = invalidatedRect.GetBottom();
        InvalidateRect(_bufferedGraphics.GetWindow(), &rect, FALSE);
    }

  private:
    friend class Components::Window;

    void ApplyWindow(HWND windowHandle) { _bufferedGraphics.BindToWindow(windowHandle); };

    bool Present() { return _bufferedGraphics.PresentBuffer(); };
    void UpdateSize(LPARAM lParam) { _bufferedGraphics.UpdateSize(lParam); };

  private:
    Native::BufferedGraphics _bufferedGraphics = {};
};

// Font Management

struct FontDescriptor {
    std::wstring       family = L"Segoe UI";
    float              size   = 24.0f;
    Gdiplus::FontStyle style  = Gdiplus::FontStyleRegular;

    bool operator<(const FontDescriptor& other) const { return std::tie(family, size, style) < std::tie(other.family, other.size, other.style); }

    bool operator==(const FontDescriptor& other) const { return family == other.family && size == other.size && style == other.style; }
};

class FontManager final {
  public:
    using FontCache = Native::GdipPtr<Gdiplus::GpFont>;

  public:
    FontManager(FontManager&&)                 = delete;
    FontManager(const FontManager&)            = delete;
    FontManager& operator=(FontManager&&)      = delete;
    FontManager& operator=(const FontManager&) = delete;
    ~FontManager()                             = default;

    static FontManager& Instance() {
        static FontManager _instance{};
        return _instance;
    }

    void Clear() {
        _aliases.clear();
        _fontCache.clear();
    }

    void RegisterAlias(const std::string& alias, const FontDescriptor& desc) { _aliases[alias] = desc; }
    void RegisterAlias(const std::string& alias, const std::wstring& fontFamily, float size, Gdiplus::FontStyle style = Gdiplus::FontStyleRegular) {
        _aliases[alias] = {fontFamily, size, style};
    }

    FontCache GetFont(const FontDescriptor& desc) {
        const auto it = _fontCache.find(desc);
        if (it != _fontCache.end()) {
            return it->second;
        }

        FontCache                              font{};
        Native::GdipPtr<Gdiplus::GpFontFamily> fontFamily{};
        Native::DllExports::GdipCreateFontFamilyFromName(desc.family.c_str(), nullptr, fontFamily.AddressOf());
        auto status = Native::DllExports::GdipCreateFont(fontFamily.Get(), desc.size, desc.style, Gdiplus::UnitPixel, font.AddressOf());

        if (status != Gdiplus::Ok) {
            Native::DllExports::GdipCreateFontFamilyFromName(L"Segoe UI", nullptr, fontFamily.AddressOf());
            Native::DllExports::GdipCreateFont(fontFamily.Get(), 24.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel, font.AddressOf());
        }

        font.Clone(_fontCache[desc]);
        return font;
    }
    FontCache GetFont(const std::wstring& fontFamily, float size, Gdiplus::FontStyle style = Gdiplus::FontStyleRegular) { return GetFont({fontFamily, size, style}); }
    FontCache GetFont(const std::string& alias) {
        const auto it = _aliases.find(alias);
        if (it != _aliases.end()) {
            return GetFont(it->second);
        }

        return GetFont("GenericFont");
    };

  private:
    FontManager() { RegisterAlias("GenericFont", FontDescriptor{}); };

    std::map<FontDescriptor, FontCache>             _fontCache = {};
    std::unordered_map<std::string, FontDescriptor> _aliases   = {};
};
}; // namespace Managers
} // namespace Lyra::UI::Foundation
