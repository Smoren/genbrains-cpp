#include "program.h"
#include "type.h"
#include "cellbot.h"
#include "randomizer.h"

namespace GenBrains {
    Program::Program(
        int base, std::vector<int> data, const std::map<int, std::function<void(Program&, CellBot*, Map&, GroupManager&)>>& commands
    ) : base(base), pointer(0), toStop(false), _canDivide(true), data(data), commands(commands) {

    }
    Program::Program(
        int base, int commandValue, const std::map<int, std::function<void(Program&, CellBot*, Map&, GroupManager&)>>& commands
    ) : base(base), pointer(0), toStop(false), _canDivide(true), data({}), commands(commands) {
        //data.resize(static_cast<unsigned long>(base));
        for(int i=0; i<base; i++) {
            data.push_back(commandValue);
        }
    }

    void Program::run(CellBot* bot, Map& map, GroupManager& gm) {
        toStop = false;
        _canDivide = true;

        //gm.logSubscription(bot->getType(), bot->getId(), 'start', {energy: bot.energy, pointer: this.pointer});

        for(int i=0; i<16; i++) {
            int cmd = data[static_cast<unsigned long>(pointer)];
            //gm.logSubscription(bot->getType(), bot->getId(), 'cmd', cmd);
            if(toStop) {
                break;
            }

            // TODO растворять органику в воде, повышать уровень минералов, минералы расходуются
            // TODO общение
            // TODO сколько ботов отпочковано, отпочковать по горизонтали или вертикали
            // TODO двигаться только на свободную или жрать
            // TODO подвинуть яд перед собой
            // TODO есть только тех, кто слабее
            if(commands.find(cmd) != commands.end()) {
                auto& action = commands.at(cmd);
                //gm.addProgramStat(this.commands.at(cmd).name);
                action(*this, bot, map, gm);
            } else {
                //gm.addProgramStat();
                movePointer(1);
                pointer = data[static_cast<unsigned long>(pointer)];
            }
        }
        bot->subEnergy(0.1);
        bot->updateFeedStat();
        //gm.logSubscription(bot->getType(), bot->getId(), 'end', {energy: bot.energy, pointer: this.pointer});
    }

    int Program::getBase() const {
        return base;
    }

    const std::vector<int>& Program::getData() const {
        return data;
    }

    int Program::getPointer() const {
        return pointer;
    }

    void Program::setPointer(int value) {
        pointer = value;
    }

    int Program::getPointerValue() const {
        return data[static_cast<unsigned long>(pointer)];
    }

    int Program::getPointerValue(int _pointer) const {
        return data[static_cast<unsigned long>(_pointer)];
    }

    int Program::movePointer(int offset) {
        pointer += offset;

        if(pointer >= base) {
            pointer -= base;
        } else if(pointer < 0) {
            pointer += base;
        }

        return pointer;
    }

    int Program::getMovedPointer(int offset) const {
        int _pointer = pointer;
        _pointer += offset;

        if(_pointer >= base) {
            _pointer -= base;
        } else if(_pointer < 0) {
            _pointer += base;
        }

        return _pointer;
    }

    std::vector<int> Program::getPart(int start, int count) const {
        std::vector<int> result;
        unsigned long j;
        for(int i=0; i<count; i++) {
            if(start+i >= base) {
                j = static_cast<unsigned long>(start+i - base);
            } else {
                j = static_cast<unsigned long>(start+i);
            }
            result.push_back(data[j]);
        }

        return result;
    }

    void Program::setPart(std::vector<int> part, int start) {
        int j;
        int partSize = static_cast<int>(part.size());
        for(int i=0; i<partSize; i++) {
            if(start+i >= base) {
                j = start+i - base;
            } else {
                j = start+i;
            }
            data[static_cast<unsigned long>(j)] = part[static_cast<unsigned long>(i)];
        }
    }

    bool Program::canDivide() {
        return _canDivide;
    }

    bool Program::forbidDivide() {
        _canDivide = false;
    }

    Program Program::clone() const {
        std::vector<int> newData(data);
        if(Randomizer::getInteger(0, 100) < 25) {
            newData[static_cast<unsigned long>(Randomizer::getInteger(0, base))] = Randomizer::getInteger(0, base);
        }

        return Program(base, newData, commands);
    }

    void Program::stop() {
        toStop = true;
    }

    void Program::actionGrab(CellBot* bot, Map& map, GroupManager& gm) {
        Cell* target = map.get(bot->getCoords(), bot->getDirection());
        switch(target->getType()) {
            case Type::BOT: {
                CellBot* targetBot = dynamic_cast<CellBot*>(target);
                if(targetBot == bot) {
                    movePointer(1);
                } else if(bot->getPredationPower() >= targetBot->getPredationPower()) {
                    bot->feedPredation(targetBot->getEnergy()/2, map, gm);
                    target->setRemoved();
                    movePointer(2);
                    stop();
                } else {
                    movePointer(3);
                }
                break;
            }
            case Type::ORGANIC: {
                movePointer(1);

                if(data[static_cast<unsigned long>(pointer)] >= 32) {
                    bot->feedMinerals(2, map, gm);
                } else  {
                    bot->feedPredation(1, map, gm);
                }
                target->setRemoved();
                stop();
                movePointer(3);
                break;
            }
            case Type::POISON: {
                bot->setType(Type::ORGANIC);
                stop();
                break;
            }
            case Type::WALL:
                movePointer(1);
                break;
            default:
                movePointer(1);
        }
    }

    void Program::actionMoveDirect(CellBot* bot, Map& map, GroupManager& gm) {
        map.move(bot, bot->getDirection(), [this, bot](Cell* lhs, Cell* rhs) {
            if(rhs->getType() != Type::EMPTY) {
                if(rhs == lhs) {
                    movePointer(1);
                    return false;
                }

                switch(rhs->getType()) {
                    case Type::BOT:
                        movePointer(2);
                        return false;
                    case Type::ORGANIC:
                        movePointer(3);
                        return false;
                    case Type::POISON:
                        bot->die();
                        bot->setType(Type::POISON);
                        stop();
                        return false;
                    case Type::WALL:
                        movePointer(1);
                        return false;
                    default:
                        movePointer(1);
                        return false;
                }
            } else {
                movePointer(1);
                return true;
            }
        });
        bot->subEnergy(0.35);
        stop();
    }

    void Program::actionObserve(CellBot* bot, Map& map, const DirectionVector& direction) {
        Cell* found = map.get(bot->getCoords(), direction);
        if(found == bot) {
            movePointer(1);
        } else {
            switch(found->getType()) {
                case Type::EMPTY: {
                    movePointer(2);
                    break;
                }
                case Type::ORGANIC: {
                    movePointer(3);
                    break;
                }
                case Type::BOT: {
                    CellBot* targetBot = dynamic_cast<CellBot*>(found);
                    if(bot->getViralStat().at("underwent")) {
                        movePointer(4);
                    } else if(bot->compareProgram(targetBot, 0)) {
                        movePointer(5);
                    } else if(bot->compareProgram(targetBot, 2)) {
                        movePointer(6);
                    } else if(bot->compareProgram(targetBot, 5)) {
                        movePointer(7);
                    } else {
                        movePointer(8); // TODO отдельным флагом???
                    }
                    break;
                }
                case Type::POISON: {
                    movePointer(9);
                    break;
                }
                case Type::WALL: {
                    movePointer(10);
                    break;
                }
                default: {
                    movePointer(11);
                }
            }
            movePointer(found->getType()+2);
        }
    }
}
