#pragma once

#include <iostream>

namespace packets {
    enum class PacketType : int {
        POSITION,
        POWERUP_SPAWN
    };

    std::ostream& operator<<(std::ostream& output, const PacketType to_write);
    std::istream& operator>>(std::istream& input, PacketType& to_read);
}
