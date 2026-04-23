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
        auto* graphics = renderer.AllocGraphics().GetGraphics();

        Native::GdiplusPointer<Gdiplus::GpStringFormat> stringFormat;
        GdipCreateStringFormat(NULL, NULL, stringFormat.AddressOf());
        if (_alignX & Align::Center) GdipSetStringFormatAlign(stringFormat.Get(), Gdiplus::StringAlignmentCenter);
        else if (_alignX & Align::End) GdipSetStringFormatAlign(stringFormat.Get(), Gdiplus::StringAlignmentFar);
        if (_alignY & Align::Center) GdipSetStringFormatLineAlign(stringFormat.Get(), Gdiplus::StringAlignmentCenter);
        else if (_alignY & Align::End) GdipSetStringFormatLineAlign(stringFormat.Get(), Gdiplus::StringAlignmentFar);

        const auto& font = Foundation::Managers::FontManager::Instance().GetFont(L"Segoe UI", 16);

        Native::GdiplusPointer<Gdiplus::GpSolidFill> fontBrush{};
        Native::GdiplusPointer<Gdiplus::GpSolidFill> backgroundBrush{};
        GdipCreateSolidFill(_fontColor.GetValue(), fontBrush.AddressOf());
        GdipCreateSolidFill(_backgroundColor.GetValue(), backgroundBrush.AddressOf());

        const auto           rect = GetLayoutRect();
        const Gdiplus::RectF textRect{0.f, 0.f, rect.Width * 1.f, rect.Height * 1.f};

        GdipFillRectangle(graphics, backgroundBrush.Get(), 0.f, 0.f, textRect.Width, textRect.Height);
        GdipDrawString(graphics, _content.c_str(), (int32_t)_content.length(), font.Get(), &textRect, stringFormat.Get(), fontBrush.Get());
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
    Align          _alignX          = Align::Center;
    Align          _alignY          = Align::Center;

    std::wstring   _content        = L"文本";
    FontDescriptor _fontDescriptor = {L"Segoe UI", 16.0f, Gdiplus::FontStyleRegular};
};
} // namespace Lyra::UI::Components