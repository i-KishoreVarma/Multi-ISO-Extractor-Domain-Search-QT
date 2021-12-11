#include "myglwindow.h"
#include <iostream>
#include <glm/glm.hpp>
#include <string.h>
#include "library/Camera.h"
#include "library/MyLibrary.h"

//#include "RawModel.h"
#include <QWidget>

#define DEBUG 1
//#define BOUNDING_CUBE "models/sphere.off"
//#define BOUNDING_CUBE "models/1grm.off"
#define BOUNDING_CUBE "../untitled/models/cube.off"

// #define MODEL_FILE_NAME "models/pancreas_240x512x512_int16.raw"
// #define ISO_VALUE -150

#define MODEL_FILE_NAME "../untitled/models/bostonteapot_256x256x178_uint8.raw"
#define ISO_VALUE 5

//#define MODEL_FILE_NAME "models/present_492x492x442_uint16.raw"
//#define ISO_VALUE 1000

unordered_map<string, Model> models;

Shader commonShader;
Shader mainShader;

void initShaders()
{
    commonShader.add("../untitled/shaders/commonShader.vs",GL_VERTEX_SHADER);
    commonShader.add("../untitled/shaders/commonShader.fs",GL_FRAGMENT_SHADER);
    commonShader.compile();

    mainShader.add("../untitled/shaders/mainShader.vs",GL_VERTEX_SHADER);
    mainShader.add("../untitled/shaders/mainShader.fs",GL_FRAGMENT_SHADER);
    mainShader.compile();
}


void MyGlWindow::initCubeModel()
{
    auto &model = models["cube"];
    Vertex::vertexType = MyLibrary::VertexPosition;
    model.create("cube", BOUNDING_CUBE,true,true,GL_QUADS);
    model.meshMode = true;
}

void MyGlWindow::initRawModel()
{
    rawModel.create("Tea Pot",MODEL_FILE_NAME,true);

    /*
     *  if you want enable mesh mode, then uncomment below
     *
     *     teaPot.meshMode = true;
     */

    /*
     *  Marching Tetrahedra using 4 Threads
     *  Need to Uncomment code in RawModel as well
     */

    auto begin = TIME_NOW;
    // teaPot.marchingTetrahedra(ISO_VALUE);
    auto end = TIME_NOW;
    cout << "Normal time: " <<
            (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";

    /*
     *  Domain Search Tree Build
     */

    rawModel.addISOSurface(0,144);
    rawModel.addISOSurface(1,50);

//    begin = TIME_NOW;
//    rawModel.build();
//    end = TIME_NOW;
//    cout << "Build time: " <<
//            (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";

//    /*
//     *  Marching Tetrahedra using Domain Search
//     */

//    begin = TIME_NOW;
//    rawModel.marchingTetrahedraDomainSearch(ISO_VALUE);
//    end = TIME_NOW;

//    cout << "Optimized execution time: " <<
//            (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";

}

void MyGlWindow::initializeGL()
{
    setMouseTracking(false);

    /* bind functions to OpenGL */
    glewInit();

    /* Set Black Background Color */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    /* set to draw in window based on depth  */
    glEnable(GL_DEPTH_TEST);

    /* if you want texture enable it */
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* init Common Shader for all model use */
    initShaders();

    initCubeModel();

    /* For Debugging (Checking whether models is working or not) */
    #ifdef DEBUG

//        initRawModel();
//        modelAvailable = true;
    #endif

    /* To Enable interactivity in OpenGL Widget, set Focus Policy */
    setFocusPolicy(Qt::StrongFocus);
}

void MyGlWindow::setModelFile(QString &fileName)
{
    rawModel = RawModel();
    rawModel.create("Tea Pot",fileName.toStdString(),true);

    /*
     *  if you want enable mesh mode, then uncomment below
     *
     *     teaPot.meshMode = true;
     */

    /*
     *  Marching Tetrahedra using 4 Threads
     *  Need to Uncomment code in RawModel as well
     */

    auto begin = TIME_NOW;
    // rawModel.marchingTetrahedra(ISO_VALUE);
    auto end = TIME_NOW;
    cout << "Normal time: " <<
            (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";
}


void MyGlWindow::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(!context()) return;

    if(!modelAvailable) return;

    auto &cubeModel = models["cube"];

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)width() / (float)height(), 0.1f, 100.0f);

    glm::mat4 view = camera.getWorldToViewMatrix();

    cubeModel.World = projection * view * glm::translate(glm::vec3(-0.5,-0.5,-0.5)) ;

    cubeModel.render([&](Shader& shader){

        /*
         *  Shader is already binded
         *
         *  Just set all uniforms
         *
         */

        shader.setUniformMat4f("mvp",cubeModel.World);

    });

    rawModel.World = projection * view * glm::translate(glm::vec3(-0.5,-0.5,-0.5));

    rawModel.render([&](Shader& shader){

        /*
         *  Shader is already binded
         *
         *  Just set all uniforms
         *
         */

        shader.setUniformMat4f("mvp",rawModel.World);

        /* Per ISO Surface Properties need to be set in Render Funcion */

    });

}
