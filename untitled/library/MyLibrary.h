#pragma once
#ifndef MY_LIBRARY
#define MY_LIBRARY
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <unordered_map>
#include <vector>
#include <functional>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb/stb_image.h"
#include "math_utils.h"
#include "ErrorHandler.h"

using namespace std;

// struct Vertex {
//     float x;
//     float y;
//     float z;
//     Vertex(float xx=0,float yy=0,float zz=0)
//     {
//         x=xx;
//         y=yy;
//         z=zz;
//     }
// };
namespace MyLibrary {
    enum VertexProps { VertexPosition = 1, VertexColor = 2, VertexTexture = 4};
}

struct Vertex {
    static int vertexType; 
    /*
        VertexPosition 0x1
        VertexColor 0x2
        VertexTexture 0x4 
    */
    glm::vec3 position;
//     glm::vec3 color;
     glm::vec3 normal;
    // glm::vec2 texture;
    Vertex(glm::vec3 Iposition = glm::vec3(0),glm::vec3 Icolor = glm::vec3(1),glm::vec3 Inormal = glm::vec3(0),glm::vec2 Itexture=glm::vec3(0)) 
    {
        position = Iposition ;
        // color = Icolor ;
        // texture = Itexture ;
         normal = Inormal;
    }
    friend istream & operator >> (istream &in,  Vertex &c);
};

/**========================================================================
 *                      VBO ( Vertex Buffer Object)
 * 
 *      On Creating VBO object, it does the following
 * 
 *          -> Generate a new Vertex Buffer.
 *          -> Bind it to OpenGL context
 *                      
 *
 *========================================================================**/

class VBO
{
    private:
        GLuint ID;
    public:
        VBO();
        void create();
        void bind();
        void bufferData(vector<Vertex> &vertices, GLenum usage = GL_STATIC_DRAW);
        void bufferData(int bufferSize,GLenum usage = GL_DYNAMIC_DRAW);
        void bufferSubData(vector<Vertex> &vertices, GLintptr offset);
        void unbind();
        ~VBO();
};

struct BufferLayoutData{
    GLint noOfProperties;
    GLenum type;
    GLboolean shouldNormalize;
    GLuint sizeOfProperty;
};

class BufferLayout
{
    public:
        vector<BufferLayoutData> arr;
        int total_size;
        BufferLayout();
        template<typename T>
        void addAttrib(GLint noOfProperties,GLboolean shouldNormalize=false);
        void print();
        ~BufferLayout();
};

/**========================================================================
 *                      IBO ( Index Buffer Object)
 * 
 *      On Creating IBO object, it does the following
 * 
 *          -> Generate a new Index Buffer (Element Array Buffer).
 *                      
 *
 *========================================================================**/

class IBO
{
    private:
        GLuint ID;
    public:
        IBO();
        void create();
        void bind();
        void bufferData(vector<GLuint> &indices, GLenum usage = GL_STATIC_DRAW);
        void bufferSubData(vector<GLuint> &indices, GLintptr offset);
        void unbind();
        ~IBO();
};

/**========================================================================
 *                      VAO ( Vertex Array Object)
 * 
 *      On Creating VBO object, it does the following
 * 
 *          -> Generate a new Vertex Array Object.
 *          -> Bind it to OpenGL context
 *                      
 *
 *========================================================================**/

class VAO
{
    private:
        GLuint ID;
        BufferLayout layout;
    public:
        VAO();
        void create();
        GLuint getID() const;
        void addLayout(BufferLayout &l);
        void bind();
        void unbind();
        ~VAO();
};

struct ShaderProps{
    string code;
    GLenum type;
    GLuint ID;
};

class Shader
{
    private:
        vector<ShaderProps> arr;
        unordered_map<string,int> location;
        void AddShader(const char* pShaderText, GLenum ShaderType);
    public:
        GLuint ID;
        Shader();
        void add(const char * fileName,GLenum shaderType);
        void load(string& data,GLenum shaderType);
        void compile();
        void bind();
        void unbind();
        int getUniformLocation(const string &name);
        void setUniformMat4f(const string &name,glm::mat4 &data);
        void setUniformVec3(const string &name,glm::vec3 &data);
        void setUniform1i(const string &name,GLuint inp);
        static bool ReadFile(const char *pFileName, string &outFile);
        ~Shader();
};

/**========================================================================
 *                      Model ( Model Object)
 * 
 *
 *========================================================================**/

class Model
{
    private:
        void normalizeData();
        void readModel(const string &fileName);
    public:
        string name;
        VAO vao;
        VBO vbo;
        IBO vio;
        GLenum mode;
        Shader shader;
        int noOfVertices,noOfPolygons,noOfEdges;
        bool shouldUseCommonShader;
        bool shouldNormalize;
        bool meshMode;
        vector<Vertex> vertices;
        vector<GLuint> indices;
        glm::mat4 World;

        Model(const string &modelName="myModel");
        Model(const string &modelName,const string &modelPath,bool useCommonShader = true,bool normalize = true,GLenum renderMode=GL_TRIANGLES);
        void create(const string &modelName,const string &modelPath,bool useCommonShader = true, bool normalize = true ,GLenum mode=GL_TRIANGLES);
        void createEmpty();
        void render(function<void(Shader&)> f=[](Shader &shader){});
        
        ~Model();

};


class Texture
{
public:
	GLuint ID;
	GLenum type;
    Texture();
	Texture(const char* image, GLenum texType=GL_TEXTURE_2D, GLenum slot=GL_TEXTURE0, GLenum format=GL_RGBA, GLenum pixelType=GL_UNSIGNED_BYTE);
    void create(const char* image, GLenum texType=GL_TEXTURE_2D, GLenum slot=GL_TEXTURE0, GLenum format=GL_RGBA, GLenum pixelType=GL_UNSIGNED_BYTE);
	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void bind(GLenum slot = GL_TEXTURE0);
	// Unbinds a texture
	void unbind();
};

extern Shader commonShader;
extern Shader mainShader;
#endif
