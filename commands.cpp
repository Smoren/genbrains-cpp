#include <algorithm>
#include "commands.h"
#include "program.h"
#include "map.h"
#include "groupmanager.h"
#include "cellbot.h"
#include "type.h"
#include "randomizer.h"


namespace GenBrains {
    const std::map<int, std::function<void(Program&, CellBot*, Map&, GroupManager&)>> Commands::MAP = {
        {
            0, // двигаться вперед
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                prog.actionMoveDirect(bot, map, gm);
            }
        },
        {
            5, // Схватить
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                prog.actionGrab(bot, map, gm);
            }
        },
        {
            10, // Налево
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                bot->getDirection().moveLeft();
                prog.movePointer(1);
            }
        },
        {
            11, // Направо
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                bot->getDirection().moveRight();
                prog.movePointer(1);
            }
        },
        {
            12, // Развернуться
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                bot->setDirection(bot->getDirection().getNewDirection(4));
                prog.movePointer(1);
            }
        },
        {
            13, // Наверх
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                bot->getDirection().y = -1;
                prog.movePointer(1);
            }
        },
        {
            14, // Вниз
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                bot->getDirection().y = 1;
                prog.movePointer(1);
            }
        },
        {
            15, // На запад
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                bot->getDirection().x = -1;
                prog.movePointer(1);
            }
        },
        {
            16, // На восток
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                bot->getDirection().x = 1;
                prog.movePointer(1);
            }
        },
        {
            20, // Что спереди?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                prog.actionObserve(bot, map, bot->getDirection());
            }
        },
        {
            21, // Что слева?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                prog.actionObserve(bot, map, bot->getDirection().getNewDirection(-1));
            }
        },
        {
            22, // Что справа?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                prog.actionObserve(bot, map, bot->getDirection().getNewDirection(1));
            }
        },
        {
            23, // Что сзади?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                prog.actionObserve(bot, map, bot->getDirection().getNewDirection(-4));
            }
        },
        {
            32, // Создать яд
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                double minerals = bot->getMinerals();
                if(minerals > 10 && bot->subEnergy(15)) {
                    bot->subMinerals(10);
                    bot->divide(map, gm, Type::POISON, 0);
                    prog.movePointer(1);
                } else if(minerals > 10) {
                    bot->setType(Type::POISON);
                    prog.stop();
                } else {
                    prog.movePointer(1);
                }
                prog.stop();
            }
        },
        {
            33, // Яд в органику
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                Cell* cell = map.get(bot->getCoords(), bot->getDirection());
                if(cell->getType() == Type::POISON) {
                    bot->subEnergy(1);
                    cell->setType(Type::ORGANIC);
                    prog.movePointer(1);
                } else {
                    prog.movePointer(2);
                }
            }
        },
        {
            40, // Фотосинтез
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                bot->feedPhoto(map.getDistributionValue("photo", bot->getCoords()), map, gm);
                prog.movePointer(1);
                prog.stop();
            }
        },
        {
            43, // Получить минералы
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                bot->feedMinerals(map.getDistributionValue("minerals", bot->getCoords()), map, gm);
                prog.movePointer(1);
                prog.stop();
            }
        },
        {
            47, // Минералы в энергию
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                bot->convertMineralsToEnergy(map, gm);
                prog.movePointer(1);
            }
        },
        {
            48, // Поделиться энергией
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                Cell* recipient = map.get(bot->getCoords(), bot->getDirection());
                if(recipient->getType() == Type::BOT) {
                    CellBot* recipientBot = dynamic_cast<CellBot*>(recipient);
                    if(bot->subEnergy(2)) {
                        recipientBot->addEnergy(2, map, gm);
                    }
                }

                prog.movePointer(1);
            }
        },
        {
            49, // Запрет деления
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                prog.forbidDivide();
                prog.movePointer(1);
            }
        },
        {
            50, // Окружен ли я?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                if(map.findEmptyCoordsAround(bot->getCoords()).size() == 7) {
                    prog.movePointer(1);
                } else {
                    prog.movePointer(2);
                }
            }
        },
        {
            51, // Одинок ли я?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                if(map.findEmptyCoordsAround(bot->getCoords()).size() == 0) {
                    prog.movePointer(1);
                } else {
                    prog.movePointer(2);
                }
            }
        },
        {
            52, // Энергии >= 20?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                if(bot->getEnergy() <= 20) {
                    prog.movePointer(1);
                } else {
                    prog.movePointer(2);
                }
            }
        },
        {
            53, // Энергии >= 10?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                if(bot->getEnergy() <= 10) {
                    prog.movePointer(1);
                } else {
                    prog.movePointer(2);
                }
            }
        },
        {
            54, // Энергии >= 5?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                if(bot->getEnergy() <= 5) {
                    prog.movePointer(1);
                } else {
                    prog.movePointer(2);
                }
            }
        },
        {
            55, // Статус фотосинтеза?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                double photoInflow = map.getDistributionValue("photo", bot->getCoords());
                if(photoInflow > 3) {
                    prog.movePointer(1);
                } else if(photoInflow > 1) {
                    prog.movePointer(2);
                } else if(photoInflow < 0.0001) {
                    prog.movePointer(3);
                } else {
                    prog.movePointer(4);
                }
            }
        },
        {
            56, // Статус минералов?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                double mineralsInflow = map.getDistributionValue("minerals", bot->getCoords());
                if(mineralsInflow > 6) {
                    prog.movePointer(1);
                } else if(mineralsInflow > 3) {
                    prog.movePointer(2);
                } else if(mineralsInflow < 0.0001) {
                    prog.movePointer(3);
                } else {
                    prog.movePointer(4);
                }
            }
        },
        {
            57, // В начало
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                prog.setPointer(0);
            }
        },
        {
            60, // Заражение
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                int count = prog.getPointerValue(prog.getMovedPointer(1));
                int startOffset = prog.getPointerValue(prog.getMovedPointer(2));
                double virality = prog.getPointerValue(prog.getMovedPointer(3))/static_cast<double>(prog.getBase());
                double mortality = std::max(static_cast<double>(0.1), static_cast<double>(prog.getPointerValue(prog.getMovedPointer(4))/static_cast<double>(prog.getBase())));
                auto recipients = map.findCellsAround(bot->getCoords());
                auto viralPart = prog.getPart(prog.getPointer(), count);

                // TODO реальный инкубационный период в шагах симуляции??

                int start;

                if(viralPart.size()) {
                    for(auto& recipient : recipients) {
                        if(recipient->getType() != Type::BOT) {
                            continue;
                        }

                        CellBot* targetBot = dynamic_cast<CellBot*>(recipient);

                        // TODO рядом со мной зараженный?
                        // TODO не заражать повторно??? уменьшать вероятность?
                        if((virality/(targetBot->getViralStat().at("underwent")+1))*100 < Randomizer::getInteger(0, 100)) {
                            continue;
                        }

                        auto& targetBotProgram = targetBot->getProgramUnsafe();

                        std::vector<int> vp(viralPart);
                        if(Randomizer::getInteger(0, 100) < 20) {
                            vp[static_cast<unsigned long>(Randomizer::getInteger(0, static_cast<int>(vp.size())))] = Randomizer::getInteger(0, targetBotProgram.getBase());
                        }

                        bot->addViralStat(0, 1);
                        targetBot->addViralStat(1, 0);

                        //start = targetBotProgram.getPointerValue(targetBotProgram.getMovedPointer(startOffset));
                        start = targetBotProgram.getMovedPointer(startOffset);
                        targetBotProgram.setPart(vp, start);
                        int a = 0;
                    }
                }
                if(mortality*100 > Randomizer::getInteger(0, 100)) {
                    bot->die();
                }
                prog.movePointer(1);
            }
        },
        {
            61, // Я заражен?
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                if(bot->getViralStat().at("underwent")) {
                    prog.movePointer(1);
                } else {
                    prog.movePointer(2);
                }
            }
        },
        {
            63, // Апоптоз
            [] (Program& prog, CellBot* bot, Map& map, GroupManager& gm) {
                bot->die();
            }
        },
    };
}
