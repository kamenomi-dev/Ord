#pragma once
#include <string>

#include "../Foundation/Base.h"
#include "Declare.h"

namespace Lyra::UI::Components {
class Text : public Foundation::Base::RenderableNode<false> {
  public:
    Text(Text&&)                 = delete;
    Text(const Text&)            = delete;
    Text& operator=(Text&&)      = delete;
    Text& operator=(const Text&) = delete;
    ~Text()                      = default;

  public:
    Text() { Type = L"Object.Renderable.Text"; }

    bool Render(Foundation::Managers::Renderer& renderer) override {
        auto& graphics = renderer.AllocGraphics();

        Native::GdipPtr<Gdiplus::GpStringFormat> stringFormat;
        Native::DllExports::GdipCreateStringFormat(NULL, NULL, stringFormat.At());
        if (_alignX & Align::Center) Native::DllExports::GdipSetStringFormatAlign(stringFormat.Get(), Gdiplus::StringAlignmentCenter);
        else if (_alignX & Align::End) Native::DllExports::GdipSetStringFormatAlign(stringFormat.Get(), Gdiplus::StringAlignmentFar);
        if (_alignY & Align::Center) Native::DllExports::GdipSetStringFormatLineAlign(stringFormat.Get(), Gdiplus::StringAlignmentCenter);
        else if (_alignY & Align::End) Native::DllExports::GdipSetStringFormatLineAlign(stringFormat.Get(), Gdiplus::StringAlignmentFar);

        const auto& font = Foundation::Managers::FontManager::Instance().GetFont(L"Segoe UI", 16);

        Native::GdipPtr<Gdiplus::GpSolidFill> fontBrush{};
        Native::GdipPtr<Gdiplus::GpSolidFill> backgroundBrush{};
        Native::DllExports::GdipCreateSolidFill(_fontColor.GetValue(), fontBrush.At());
        Native::DllExports::GdipCreateSolidFill(_backgroundColor.GetValue(), backgroundBrush.At());

        const auto     pGraphics = graphics.GetGraphics();
        auto           rect      = GetLayoutRect();
        Gdiplus::RectF textRect{0.f, 0.f, rect.Width * 1.f, rect.Height * 1.f};

        Native::DllExports::GdipFillRectangle(pGraphics, backgroundBrush.Get(), 0.f, 0.f, textRect.Width, textRect.Height);
        Native::DllExports::GdipDrawString(pGraphics, _content.c_str(), (int32_t)_content.length(), font.Get(), &textRect, stringFormat.Get(), fontBrush.Get());
        return true;
    };

    auto GetContent() const { return _content; }
    void SetContent(const std::wstring& content) { _content = content; }

    void SetAlign(Align x, Align y) noexcept {
        _alignX = x;
        _alignY = y;
    }
    void SetHorizontalAlign(Align x) noexcept { _alignX = x; }
    void SetVerticalAlign(Align y) noexcept { _alignY = y; }

  private:
    Gdiplus::Color _fontColor       = Gdiplus::Color::White;
    Gdiplus::Color _backgroundColor = Gdiplus::Color::Transparent;
    std::wstring   _content         = L"文本";
    Align          _alignX          = Align::Center;
    Align          _alignY          = Align::Center;
};
} // namespace Lyra::UI::Components