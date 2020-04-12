#include <stdexcept>
#include <sstream>
#include "directionvector.h"
#include "randomizer.h"


namespace GenBrains {
    DirectionVector::DirectionVector(int x,  int y) : Coords(x, y) {

    }

    void DirectionVector::moveLeft() {
        switch(x) {
            case 0:
                switch(y) {
                    case -1:    x = -1;    break;
                    case 1:     x = 1;     break;
                    default:
                        throw std::runtime_error("bad x, y in direction vector");
                }
                break;
            case -1:
                switch(y) {
                    case 0:     y = 1;     break;
                    case -1:    y = 0;     break;
                    case 1:     x = 0;     break;
                    default:
                        throw std::runtime_error("bad x, y in direction vector");
                }
                break;
            case 1:
                switch(y) {
                    case 0:     y = -1;    break;
                    case -1:    x = 0;     break;
                    case 1:     y = 0;     break;
                    default:
                        throw std::runtime_error("bad x, y in direction vector");
                }
                break;
            default:
                throw std::runtime_error("bad x in direction vector");
        }
    }

    void DirectionVector::moveRight() {
        switch(x) {
            case 0:
                switch(y) {
                    case -1:    x = 1;     break;
                    case 1:     x = -1;    break;
                    default:
                        throw std::runtime_error("bad x, y in direction vector");
                }
                break;
            case -1:
                switch(y) {
                    case 0:     y = -1;    break;
                    case -1:    x = 0;     break;
                    case 1:     y = 0;     break;
                    default:
                        throw std::runtime_error("bad x, y in direction vector");
                }
                break;
            case 1:
                switch(y) {
                    case 0:     y = 1;     break;
                    case -1:    y = 0;     break;
                    case 1:     x = 0;     break;
                    default:
                        throw std::runtime_error("bad x, y in direction vector");
                }
                break;
            default:
                throw std::runtime_error("bad x in direction vector");
        }
    }

    DirectionVector DirectionVector::getNewDirection(int rotationSize) const {
        DirectionVector newDirection = clone();

        if(rotationSize > 0) {
            for(int i=0; i<rotationSize; i++) {
                newDirection.moveRight();
            }
        } else {
            for(int i=0; i>rotationSize; i--) {
                newDirection.moveLeft();
            }
        }
        return newDirection;
    }

    void DirectionVector::setDirection(int _x, int _y) {
        x = _x;
        y = _y;
    }

    DirectionVector DirectionVector::clone() const {
        return DirectionVector(x, y);
    }

    DirectionVector DirectionVector::randomize() {
        x = Randomizer::getInteger(-1, 3);
        y = Randomizer::getInteger(-1, 3);

        if(x == 0 && y == 0) {
            randomize();
        }
    }

    void DirectionVector::log() {
        std::stringstream ss;

        for(int _y=-1; _y<=1; _y++) {
            for(int _x=-1; _x<=1; _x++) {
                if(x == _x && y == _y) {
                    ss << 'x';
                } else {
                    ss << '_';
                }
                ss << '|';
            }
            ss << std::endl;
        }

        std::cout << ss.str() << std::endl;
    }
}

