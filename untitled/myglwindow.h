#ifndef MYGLWINDOW_H
#define MYGLWINDOW_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <QOpenGLWidget>
#include <QWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <iostream>
#include <glm/glm.hpp>
#include <string.h>
#include "library/Camera.h"
#include "library/MyLibrary.h"
#include "RawModel.h"


class MyGlWindow : public QOpenGLWidget
{
    Q_OBJECT
private:

//    Shader commonShader;
//    void initCommonShader();

    /* For debugging */

    void initCubeModel();

public:
    RawModel rawModel;
    Camera camera;
    MyGlWindow(QWidget *parent=0) : QOpenGLWidget(parent)
    {
        camera = Camera(glm::vec3(0.5f,0.5f,4.0f),glm::vec3(0.0f,0.0f,-1.0f));
    }

    void setModelFile(QString &fileName);
    bool modelAvailable = false;

protected:

    void initializeGL();

    void paintGL();

    /* When mouse moved over OpenGL Widget */
    void mouseMoveEvent(QMouseEvent *e)
    {
        camera.mouseUpdate(glm::vec2(e->x(),e->y()));
        repaint();
    }

    /* When a Key is Pressed */
    void keyPressEvent(QKeyEvent* e)
    {
        switch (e->key())
        {
            case Qt::Key::Key_W:
                camera.moveForward();
                break;
            case Qt::Key::Key_S:
                camera.moveBackward();
                break;
            case Qt::Key::Key_A:
                camera.strafeLeft();
                break;
            case Qt::Key::Key_D:
                camera.strafeRight();
                break;
            case Qt::Key::Key_R:
                camera.moveUp();
                break;
            case Qt::Key::Key_F:
                camera.moveDown();
                break;
        }
        repaint();
    }

    /* Mouse Click Event in OpenGL Widget */
    void mousePressEvent(QMouseEvent *e)
    {
        if(e->button()==Qt::NoButton)
            return;
        if(e->button()==Qt::LeftButton)
            camera.firstClick = true;
    }
};

#endif // MYGLWINDOW_H
