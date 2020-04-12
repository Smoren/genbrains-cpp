#pragma once

#include <iostream>


namespace GenBrains {
    class Coords
    {
    public:
        Coords(int x, int y);
        Coords clone() const;
        int x;
        int y;

        void removeCoords();
        bool isNullVector() const;
    };

    std::ostream& operator <<(std::ostream& stream, const Coords& coords);
    bool operator ==(const Coords& lhs, const Coords& rhs);
    bool operator !=(const Coords& lhs, const Coords& rhs);
}
