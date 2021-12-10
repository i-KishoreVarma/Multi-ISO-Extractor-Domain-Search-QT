#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <glm/glm.hpp>
#include <string.h>
#include "Camera.h"
#include "MyLibrary.h"

using namespace std;

extern Camera camera;

class Window
{
    private:
        static const char * theProgramTitle;
        static int theWindowPositionX, theWindowPositionY;
        static bool isFullScreen ;
        static const int ANIMATION_DELAY;
        static bool isAnimating;
        static const char* commonVShader;
        static const char* commonGShader;
        static const char* commonFShader;
        static string commonVShaderCode;
        static string commonFShaderCode;
    public:
        static int theWindowWidth, theWindowHeight;
        Window(int argc,char** argv)
        {
            glutInit(&argc, argv);
            glutInitWindowSize(theWindowWidth, theWindowHeight);
            glutInitWindowPosition(theWindowPositionX, theWindowPositionY);
            /* request full color with double buffering and depth-based rendering */
            glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
            /* create window whose title is the name of the executable */
            glutCreateWindow(theProgramTitle);

            InitializeGlutCallbacks();
            GLenum res = glewInit();
            if (res != GLEW_OK) {
                fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
                exit(-1);
            }

	        printf("GL version: %s\n", glGetString(GL_VERSION));

            /* by default the back ground color is black */
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            /* set to draw in window based on depth  */
            glEnable(GL_DEPTH_TEST); 
            glEnable(GL_TEXTURE_2D);

            initCommonShader();
        }
        static void initCommonShader()
        {
            commonShader.add(commonVShader,GL_VERTEX_SHADER);
            commonShader.add(commonFShader,GL_FRAGMENT_SHADER);
            // commonShader.load(commonVShaderCode,GL_VERTEX_SHADER);
            // commonShader.load(commonFShaderCode,GL_FRAGMENT_SHADER);
            commonShader.compile();
        }
        static void computeFPS() {
            static int frameCount = 0;
            static int lastFrameTime = 0;
            static char * title = NULL;
            int currentTime;

            if (!title)
                title = (char*) malloc((strlen(theProgramTitle) + 20) * sizeof (char));
            frameCount++;
            currentTime = glutGet((GLenum) (GLUT_ELAPSED_TIME));
            if (currentTime - lastFrameTime > 1000) {
                sprintf(title, "%s [ FPS: %4.2f ]",
                    theProgramTitle,
                    frameCount * 1000.0 / (currentTime - lastFrameTime));
                glutSetWindowTitle(title);
                lastFrameTime = currentTime;
                frameCount = 0;
            }
        }

        static void onReshape(int width, int height) 
        {
            glViewport(0, 0, width, height);
            if (!isFullScreen) {
                theWindowWidth = width;
                theWindowHeight = height;
            }
            // update scene based on new aspect ratio....
        }

        static void onIdle() 
        {
            static int oldTime = 0;
            if (isAnimating) {
                int currentTime = glutGet((GLenum) (GLUT_ELAPSED_TIME));
                /* Ensures fairly constant framerate */
                if (currentTime - oldTime > ANIMATION_DELAY) {

                    oldTime = currentTime;
                    /* compute the frame rate */
                    computeFPS();
                    /* notify window it has to be repainted */
                    glutPostRedisplay();
                }
            }
        }
        
        static void onMouseMotion(int x, int y) {
            /* notify window that it has to be re-rendered */
            camera.mouseUpdate(glm::vec2(x,y));
            glutPostRedisplay();
        }

        /* pre:  mouse button has been pressed while within glut window
         * post: scene is updated and re-rendered 
         */
        static void onMouseButtonPress(int button, int state, int x, int y) {
            if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
                // Left button pressed
                // camera.mouseUpdate(glm::vec2(x,y));
            }
            if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
                // Left button un pressed
                camera.firstClick = true;
            }
            /* notify window that it has to be re-rendered */
            glutPostRedisplay();
        }

        /* pre:  key has been pressed
         * post: scene is updated and re-rendered 
         */
        static void onAlphaNumericKeyPress(unsigned char key, int x, int y) {
            switch (key) {
                    /* toggle animation running */
                case 'i':
                    camera.moveUp();
                    break;
                case 'k':
                    camera.moveDown();
                    break;
                case 's':
                    camera.moveBackward();
                    break;
                case 'w':
                    camera.moveForward();
                    break;
                case 'a':
                    camera.strafeLeft();
                    break;
                case 'd':
                    camera.strafeRight();
                    break;
                // case 'a':
                //     isAnimating = !isAnimating;
                //     break;
                    /* reset */
                case 'r':
                    break;
                    /* quit! */
                case 'Q':
                case 'q':
                case 27:
                    exit(0);
            }

            /* notify window that it has to be re-rendered */
            glutPostRedisplay();
        }

        /* pre:  arrow or function key has been pressed
         * post: scene is updated and re-rendered 
         */
        static void onSpecialKeyPress(int key, int x, int y) {
            /* please do not change function of these keys */
            switch (key) {
                    /* toggle full screen mode */
                case GLUT_KEY_F1:
                    isFullScreen = !isFullScreen;
                    if (isFullScreen) {
                        theWindowPositionX = glutGet((GLenum) (GLUT_WINDOW_X));
                        theWindowPositionY = glutGet((GLenum) (GLUT_WINDOW_Y));
                        glutFullScreen();
                    } else {
                        glutReshapeWindow(theWindowWidth, theWindowHeight);
                        glutPositionWindow(theWindowPositionX, theWindowPositionY);
                    }
                    break;
            }

            /* notify window that it has to be re-rendered */
            glutPostRedisplay();
        }

        /* pre:  glut window has just been iconified or restored 
         * post: if window is visible, animate model, otherwise don't bother 
         */
        static void onVisible(int state) {
            if (state == GLUT_VISIBLE) {
                /* tell glut to show model again */
                glutIdleFunc(onIdle);
            } else {
                glutIdleFunc(NULL);
            }
        }

        static void onDisplay();

        /*
         * Callback Functions
         * These functions are registered with the glut window and called 
         * when certain events occur.
         */
        static void InitializeGlutCallbacks() {
            /* tell glut how to display model */
            glutDisplayFunc(onDisplay);
            /* tell glutwhat to do when it would otherwise be idle */
            glutIdleFunc(onIdle);
            /* tell glut how to respond to changes in window size */
            glutReshapeFunc(onReshape);
            /* tell glut how to handle changes in window visibility */
            glutVisibilityFunc(onVisible);
            /* tell glut how to handle key presses */
            glutKeyboardFunc(onAlphaNumericKeyPress);
            glutSpecialFunc(onSpecialKeyPress);
            /* tell glut how to handle the mouse */
            glutMotionFunc(onMouseMotion);
            glutMouseFunc(onMouseButtonPress);
        }

        ~Window()
        {
            
        }
};
