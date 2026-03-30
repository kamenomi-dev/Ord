#pragma once
namespace Lyra::UI::Foundation::Internal {
struct FontDescriptor {
    std::wstring family = L"";
    float        size   = 0.f;
    int          style  = NULL;

    bool operator<(const FontDescriptor& other) const {
        return std::tie(family, size, style) < std::tie(other.family, other.size, other.style);
    }
};

class FontCache final {
  public:
    FontCache()                            = default;
    FontCache(FontCache&&)                 = delete;
    FontCache(const FontCache&)            = delete;
    FontCache& operator=(FontCache&&)      = delete;
    FontCache& operator=(const FontCache&) = delete;
    ~FontCache()                           = default;
};
} // namespace Lyra::UI::Foundation::Internal