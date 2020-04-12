#include <GL/freeglut.h>
#include <GL/gl.h>
#include <thread>
#include <unistd.h>
#include "drawer.h"
#include "type.h"

namespace GenBrains {
    bool Drawer::isActive = true;
    GroupManager* Drawer::gm = nullptr;
    Map* Drawer::map = nullptr;
    std::thread* Drawer::process = nullptr;
    int Drawer::cellSize = 0;

    void Drawer::init(GroupManager& gm, Map& map, std::thread& process, int cellSize) {
        Drawer::gm = &gm;
        Drawer::map = &map;
        Drawer::process = &process;
        Drawer::cellSize = cellSize;

        int argc = 0;
        glutInit(&argc, nullptr);
        glutInitDisplayMode(GLUT_SINGLE);
        glutInitWindowSize(map.getWidth()*cellSize, map.getHeight()*cellSize);
        glutInitWindowPosition(500, 100);
        glutCreateWindow("GenBrains");
        glutDisplayFunc(render);
        glutTimerFunc(100, timer, 0);
        glutCloseFunc(destroy);
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
        glutMainLoop();
    }

    void Drawer::start() {
        Drawer::isActive = true;
    }

    void Drawer::pause() {
        Drawer::isActive = false;
    }

    void Drawer::timer(int value) {
        glutPostRedisplay();
        glutTimerFunc(100, timer, 0);
    }

    void Drawer::render() {
        if(!Drawer::isActive) return;

        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        Drawer::gm->getWritableMutex().lock();
        Drawer::gm->each([](Cell* cell) {
            try {
                if(cell == nullptr) return;
                Drawer::drawCell(cell->getCoords(), Drawer::gm->getDrawPreset(cell->getType())(cell));
            } catch(std::runtime_error e) {}
        });
        Drawer::gm->getWritableMutex().unlock();

        glFlush();
    }

    void Drawer::destroy() {
        Drawer::gm->setTerminated();
        if(Drawer::process->joinable()) {
            Drawer::process->join();
        }
        Drawer::gm->removeAll();
    }

    void Drawer::drawCell(Coords coords, std::vector<double> color) {
        try {
            //auto* mutex = cell->getMutex();
            //mutex->lock();
            //const Coords& coords = cell->getCoords();
            //int type = cell->getType();
            //mutex->unlock();

            GLfloat cellSizeX = static_cast<GLfloat>(1.0/Drawer::map->getWidth()*2.0);
            GLfloat cellSizeY = static_cast<GLfloat>(1.0/Drawer::map->getHeight()*2.0);
            GLfloat offsetX = coords.x*cellSizeX;
            GLfloat offsetY = coords.y*cellSizeY;

            glColor3f(static_cast<GLfloat>(color[0]), static_cast<GLfloat>(color[1]), static_cast<GLfloat>(color[2]));
            glBegin(GL_POLYGON);
            glVertex2f(-1+offsetX, 1-offsetY-cellSizeY);
            glVertex2f(-1+offsetX, 1-offsetY);
            glVertex2f(-1+offsetX+cellSizeX, 1-offsetY);
            glVertex2f(-1+offsetX+cellSizeX, 1-offsetY-cellSizeY);
            glEnd();
        } catch(std::exception& e) {
            std::cout << "drawer error" << std::endl;
        }
    }
}

