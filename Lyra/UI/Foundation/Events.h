#pragma once
#include <functional>
#include <optional>
#include <variant>

#include <Gdiplus.h>

namespace Lyra::UI::Foundation::Base {
class RenderableObject;
};

namespace Lyra::UI::Foundation::Events {
namespace EventPayloads {
// Todo: 后续这个结构体或被弃用，因为IME
struct KeyData {
    int  virtualKeyCode = NULL;
    bool isRepeat       = false;
};

struct MouseData {
    Gdiplus::Point position  = {};
    int            keyId     = 0;
    bool           isPressed = false;
};
} // namespace EventPayloads

using EventPayload = std::variant<EventPayloads::MouseData, EventPayloads::KeyData>;

struct EventArgs {
    enum class EventTypes {
        None = NULL,
        KeyEvent,
        MouseEvent,
    };

    EventTypes              type    = EventTypes::None;
    Base::RenderableObject* target  = nullptr;
    EventPayload            payload = {};
    bool                    handled = false;
};

using EventCallback = std::function<void(const EventArgs&)>;
}; // namespace Lyra::UI::Foundation::Events