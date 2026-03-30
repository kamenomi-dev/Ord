#pragma once
namespace Lyra::UI::Components {
class Window;
};

namespace Lyra::UI::Foundation::Managers {
class Renderer final {
  public:
    Renderer()                           = default;
    Renderer(Renderer&&)                 = delete;
    Renderer(const Renderer&)            = delete;
    Renderer& operator=(Renderer&&)      = delete;
    Renderer& operator=(const Renderer&) = delete;
    ~Renderer()                          = default;

    Gdiplus::Graphics& AllocGraphics() {
        if (!_graphics) {
            _graphics = std::make_shared<Gdiplus::Graphics>(_swapchain.GetSwapchainContext());
        }

        return *_graphics;
    };

    void Invalidate(const Gdiplus::Rect invalidatedRect) const {
        RECT rect{};
        rect.left   = invalidatedRect.X;
        rect.top    = invalidatedRect.Y;
        rect.right  = invalidatedRect.GetRight();
        rect.bottom = invalidatedRect.GetBottom();
        InvalidateRect(_swapchain.GetTargetWindow(), &rect, FALSE);
    }

  private:
    friend class Components::Window;

    void ApplyWindow(HWND windowHandle) { _swapchain.BindToWindow(windowHandle); };

    bool Present() { return _swapchain.PresentBuffer(); };
    void UpdateSize(LPARAM lParam) {
        _swapchain.UpdateSize(lParam);

        if (_swapchain.Invalid()) {
            return;
        }

        _graphics = std::make_shared<Gdiplus::Graphics>(_swapchain.GetSwapchainContext());
    };

  private:
    Native::Swapchain                  _swapchain = {};
    std::shared_ptr<Gdiplus::Graphics> _graphics  = nullptr;
};

// Singleton

class ResourceManager final {
  public:
    ResourceManager(ResourceManager&&)                 = delete;
    ResourceManager(const ResourceManager&)            = delete;
    ResourceManager& operator=(ResourceManager&&)      = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ~ResourceManager()                                 = default;

    static ResourceManager& Get() {
        static ResourceManager instance; // NOSONAR, this is thread safe since C++11.
        return instance;
    }

  private:
    ResourceManager() = default;

  public:
};
} // namespace Lyra::UI::Foundation::Managers