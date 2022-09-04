#include "packet_types.h"

std::ostream& packets::operator<<(std::ostream& output, const PacketType to_write) {
    return output << (int)to_write;
}

std::istream& packets::operator>>(std::istream& input, PacketType& to_read) {
    return input >> (int&)to_read;
}