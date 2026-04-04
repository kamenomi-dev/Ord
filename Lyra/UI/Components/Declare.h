#pragma once
#include <cstdint>

namespace Lyra::UI::Components {
enum class Align : uint8_t {
    Start  = 0b00,
    Center = 0b01,
    End    = 0b10
};

uint8_t operator&(Align lhs, Align rhs) {
    return (uint8_t)lhs & (uint8_t)rhs;
}
} // namespace Lyra::UI::Components