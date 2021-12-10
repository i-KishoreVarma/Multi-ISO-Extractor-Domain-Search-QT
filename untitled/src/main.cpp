#include "library/Window.h"
#include <fstream>
#include "RawModel.h"

// Camera camera(glm::vec3(0.5,0.5,-2.5),glm::vec3(0,0,1));
Camera camera(glm::vec3(0.5,0.5,4),glm::vec3(0,0,-1));
unordered_map<string, Model> models;

// #define MODEL_FILE_NAME "models/pancreas_240x512x512_int16.raw"
// #define ISO_VALUE -150
// RawModel<int16_t> teaPot;

#define MODEL_FILE_NAME "models/bostonteapot_256x256x178_uint8.raw"
#define ISO_VALUE 5
RawModel<uint8_t> teaPot;

// #define MODEL_FILE_NAME "models/present_492x492x442_uint16.raw"
// #define ISO_VALUE 1000
// RawModel<uint16_t> teaPot;

#define BOUNDING_CUBE "models/cube.off"


Texture atexture;


void initCubeModel()
{
    auto &model = models["cube"];
    Vertex::vertexType = MyLibrary::VertexPosition;
    model.create("cube", BOUNDING_CUBE,false,true,GL_QUADS);
    model.meshMode = true;
    model.shader = commonShader;
}

void initTeaPotModel()
{
    teaPot.create("Tea Pot",MODEL_FILE_NAME,false);
    teaPot.shader = commonShader;
    // teaPot.meshMode = true;
    auto begin = TIME_NOW;
    teaPot.marchingTetrahedra(ISO_VALUE);
    auto end = TIME_NOW;
    cout << "Normal time: " << 
            (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";    
    begin = TIME_NOW;
    teaPot.build();
    end = TIME_NOW;
    cout << "Build time: " << 
            (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";    
    begin = TIME_NOW;
    teaPot.marchingTetrahedraDomainSearch(ISO_VALUE);
    // auto lastdata = teaPot.minmaxData.back()[0][0][0];
    // cout<<lastdata.first<<" "<<lastdata.second<<'\n';
    end = TIME_NOW;
    cout << "Optimized execution time: " << 
            (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";    
    atexture.create("planks.png",GL_TEXTURE_2D,GL_TEXTURE0);
}

int main(int argc, char **argv)
{

    Window window(argc, argv);

    initCubeModel();
    initTeaPotModel();

    /* give control over to glut to handle rendering and interaction  */
    glutMainLoop();

    return 0;
}

float zIndex = 0;
float curISO = 5;

void Window::onDisplay()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glm::mat4 projection = glm::ortho(-5.0f, 5.0f , -5.0f , 5.0f,-5.0f,5.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)Window::theWindowWidth / (float)Window::theWindowHeight, 0.1f, 100.0f);
    // glm::mat4 view = camera.getWorldToViewMatrix();
    // glm::mat4 projection = glm::mat4(1);

    glm::mat4 view = camera.getWorldToViewMatrix();
    
    auto &cubeModel = models["cube"];

    // cubeModel.World = glm::translate(glm::vec3(-0.5,-0.5,-0.5)) ;s
    cubeModel.World = glm::mat4(1) ;
    atexture.bind();
    
    cubeModel.render([&cubeModel,&view,&projection](Shader &shader){
        shader.bind();
        shader.setUniformMat4f("model",cubeModel.World);
        shader.setUniformMat4f("view",view);
        shader.setUniformMat4f("projection",projection);
        shader.setUniform1i("sampleTexture",0);
    });

    // auto modelTrans = glm::translate(glm::vec3(-0.5,-0.5,-0.5));
    auto modelTrans = glm::mat4(1);

    // teaPot.marchingTetrahedraDomainSearch(curISO);

    // teaPot.generateISOPlaneXY(zIndex);

    zIndex+=0.5;
    // curISO+=1;

    if(curISO>250) curISO = 0;
    
    teaPot.render([&modelTrans,&view,&projection](Shader &shader){
        shader.bind();
        shader.setUniformMat4f("model",modelTrans);
        shader.setUniformMat4f("view",view);
        shader.setUniformMat4f("projection",projection);
    });

    errorCode = glGetError();

    if (errorCode == GL_NO_ERROR)
        glutSwapBuffers();
    else
        fprintf(stderr, "OpenGL rendering error %d\n", errorCode);
}
