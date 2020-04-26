#pragma once

#include "coords.h"

namespace GenBrains {
    class DirectionVector : public Coords
    {
    public:
        DirectionVector(int x, int y);
        void moveLeft();
        void moveRight();
        DirectionVector getNewDirection(int rotationSize) const;
        void setDirection(int x, int y);
        DirectionVector clone() const;
        void randomize();
        void log();
    };
}
