#How to use the library

**OFF file example**

square.off

```
OFF
4 1 0
-0.25 -0.25 0
0.25 -0.25 0
0.25 0.25 0
-0.25 0.25 0
4 0 1 2 3
```

coordinate.off

```
OFF
4 2 0
-1 0 0
1 0 0
0 -1 0
0 1 0
2 0 1 
2 2 3
```
**Model::create**

```cpp

void create(const string &modelName,
            const string &modelPath, // specify where model (off) file is located
            bool useCommonShader = true, // Should use common shader or not
            bool normalize = true , // Normalize values to [0,1] range
            GLenum mode=GL_TRIANGLES)
```

**Using Default Shader**

```cpp
auto &squareModel = models["square"];
squareModel.create("square","data/square.off",true,false,GL_POLYGON);
```
In onDisplay function

```cpp
auto &squareModel = models["square"];
squareModel.World = Matrix4f().InitIdentity() * 
                    Matrix4f().InitRotateTransform(0,0,squareRotate) *
                    Matrix4f().InitTranslationTransform(0.5,0.5,0);
squareModel.render();
```
Note : if useCommonShader is set to false then we should pass lambda as argument to render function.

**Using Custom Shader**

```cpp


unordered_map<string,Model> models;

void initSquareModel()
{
    auto &squareModel = models["square"];
    squareModel.create("square","data/square.off",false,false,GL_POLYGON);
    squareModel.meshMode = true;
    squareModel.shader.add("src/shaders/shader.vs",GL_VERTEX_SHADER);
    squareModel.shader.add("src/shaders/shader.fs",GL_FRAGMENT_SHADER);
    squareModel.shader.compile();
}
```
In onDisplay function, now pass lambda function that sets required uniforms

```cpp

auto &squareModel = models["square"];
Matrix4f world = Matrix4f().InitIdentity() * 
                    Matrix4f().InitRotateTransform(0,0,squareRotate) *
                    Matrix4f().InitTranslationTransform(0.5,0.5,0);
squareModel.render([&world](Shader& shader){
    shader.bind();
    shader.setUniformMat4f("gWorld",world);
});
```

**Complete Program**

```cpp

#include "library/Window.h"

Camera camera;

unordered_map<string,Model> models;

void initSquareModel()
{
    auto &squareModel = models["square"];
    squareModel.create("square","data/square.off",false,false,GL_POLYGON);
    squareModel.meshMode = true;
    squareModel.shader.add("src/shaders/shader.vs",GL_VERTEX_SHADER);
    squareModel.shader.add("src/shaders/shader.fs",GL_FRAGMENT_SHADER);
    squareModel.shader.compile();
}

int main(int argc, char** argv) {
	
	Window window(argc,argv);

    initSquareModel();

    auto &coordinateModel = models["coordinate"];
    coordinateModel.create("coordinate","data/coordinate.off",true,false,GL_LINES);

	/* give control over to glut to handle rendering and interaction  */
	glutMainLoop();
	
	return 0;
}

void Window::onDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static float squareRotate = 0.1;

    auto &squareModel = models["square"];
    Matrix4f world = Matrix4f().InitIdentity() * 
                        Matrix4f().InitRotateTransform(0,0,squareRotate) *
                        Matrix4f().InitTranslationTransform(0.5,0.5,0);
    squareModel.render([&world](Shader& shader){
        shader.bind();
        shader.setUniformMat4f("gWorld",world);
    });

    squareRotate+=0.5;
    
    auto &coordinateModel = models["coordinate"];
    coordinateModel.World.InitIdentity();
    coordinateModel.render();

    GLenum errorCode = glGetError();

    if (errorCode == GL_NO_ERROR) glutSwapBuffers();
    else fprintf(stderr, "OpenGL rendering error %d\n", errorCode);
}

```