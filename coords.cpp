#include "coords.h"

namespace GenBrains {
    Coords::Coords(int x, int y): x(x), y(y) {

    }

    Coords Coords::clone() const {
        return Coords(x, y);
    }

    bool Coords::isNullVector() const {
        return x == 0 && y == 0;
    }

    std::ostream& operator <<(std::ostream& stream, const Coords& coords) {
        return stream << "{x: " << coords.x << ", y: " << coords.y << "}";
    }

    bool operator ==(const Coords& lhs, const Coords& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    bool operator !=(const Coords& lhs, const Coords& rhs) {
        return lhs.x != rhs.x && lhs.y == rhs.y;
    }
}
