#include "MyLibrary.h"
#include <limits>
#include <assert.h>
#include <iostream>
#include <fstream>

glm::vec3 calcNormal(glm::vec3 &a,glm::vec3 &b,glm::vec3 &c)
{
    /*
        a --> c
        |
        \/
        b
    */
    return glm::cross(b-a,c-a); 
}


// void generateNormals(vector<Vertex> &v,vector<uint> &indices)
// {
//     vector<int> cou(v.size());
//     for(uint i=0;i<indices.size();i+=3)
//     {
//         for(int j=0;j<3;j++)
//         {
//             v[indices[i+j%3]].normal += calcNormal(v[indices[i+j%3]].position,v[indices[i+(j+1)%3]].position,v[indices[i+(j+2)%3]].position);
//             cou[indices[i+j%3]]+=1;
//         }
//     }
//     for(uint i=0;i<v.size();i++)
//     {
//         if(cou[i]==0) v[i].normal = glm::vec3(1);
//         else v[i].normal/=cou[i];
//     }
// }

Model::Model(const string &modelName):name(modelName),vao(),vbo(),vio(),shader()
{
    noOfVertices = 0,noOfEdges = 0,noOfPolygons = 0;
    mode = GL_TRIANGLES;
    meshMode = false;
    shouldNormalize = true;
}

Model::Model(const string &modelName,const string &modelPath,bool useCommonShader,bool normalize,GLenum renderMode):name(modelName),vao(),vbo(),vio(),shader()
{
    noOfVertices = 0,noOfEdges = 0,noOfPolygons = 0;
    create(modelName,modelPath,useCommonShader,normalize,renderMode);
}

void Model::create(const string &modelName,const string &modelPath,bool useCommonShader,bool normalize,GLenum renderMode)
{
    name = modelName;
    meshMode = false; 
    shouldNormalize = normalize;
    shader = commonShader;
    mode = renderMode;
    vao.create();
    vbo.create();
    vio.create();
    
    BufferLayout layout;

    layout.addAttrib<float>(3);
    // layout.addAttrib<float>(3);
    // layout.addAttrib<float>(3);
    // layout.addAttrib<float>(2);

    vao.addLayout(layout);
    
    readModel(modelPath);

    vbo.bufferData(vertices);
    vio.bufferData(indices);

    int vType = Vertex::vertexType;

    vao.unbind();
    vbo.unbind();
    vio.unbind();
}

void Model::createEmpty()
{
    name = "modelName";
    meshMode = false;
    shouldNormalize = false;
    shouldUseCommonShader = true;
    if(shouldUseCommonShader) shader = commonShader;
    mode = GL_TRIANGLES;
    vao.create();
    int vType = Vertex::vertexType;

    BufferLayout layout;

    layout.addAttrib<float>(3);
    // layout.addAttrib<float>(3);
    // layout.addAttrib<float>(3);
    // layout.addAttrib<float>(2);

    vao.addLayout(layout);

    vao.unbind();
    vbo.create();
    vio.create();




    vbo.bufferData(vertices);
    vio.bufferData(indices);


    vbo.unbind();
    vio.unbind();
}


void Model::normalizeData()
{
    assert(vertices.size());

    glm::vec3 minv = vertices[0].position;

    glm::vec3 maxv = vertices[0].position;
    
    auto f = [](float &amin,float &amax,float val){
        if(val<amin) amin = val;
        if(val>=amax) amax = val;
    };
    
    for(uint i=0;i<vertices.size();i++)
    {
        f(minv.x,maxv.x,vertices[i].position.x);
        f(minv.y,maxv.y,vertices[i].position.y);
        f(minv.z,maxv.z,vertices[i].position.z);
    }

    for(uint i=0;i<vertices.size();i++)
    {
        if(maxv.x!=minv.x)
           vertices[i].position.x = (vertices[i].position.x - minv.x)/(maxv.x-minv.x);
        if(maxv.y!=minv.y)
           vertices[i].position.y = (vertices[i].position.y - minv.y)/(maxv.y-minv.y);
        if(maxv.z!=minv.z)
           vertices[i].position.z = (vertices[i].position.z - minv.z)/(maxv.z-minv.z);
    }
}


void Model::readModel(const string &fileName)
{
    ifstream file;
    file.open(fileName);
    string type;
    file>>type;
    if(type!="OFF")
    {
        std::cerr<<"Failed to read Model from file["<<fileName<<"]\n";
        exit(-1);
    }
    file>>noOfVertices>>noOfPolygons>>noOfEdges;
    vertices.clear();
    indices.clear();
    vertices.reserve(noOfVertices);
    indices.reserve(noOfPolygons*3);
    for(int i = 0; i<noOfVertices;i++)
    {
        Vertex a;
        file >> a;
        vertices.push_back(a);
    }
    for(int i = 0; i<noOfPolygons;i++)
    {
        int n;
        file>>n;
        int tmp;
        for(int j=0;j<n;j++)
        {
            file>>tmp;
            indices.push_back(tmp);
        }
        // handle special cases if n>4
    }
    if(shouldNormalize) normalizeData();
    //generateNormals(vertices,indices);
}

void Model::render(function<void(Shader&)> f)
{
    shader.bind();
//    vao.bind();
    vbo.bind();
    vio.bind();

    f(shader);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    if(meshMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(mode,indices.size(),GL_UNSIGNED_INT,0);
    if(meshMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    ErrorHandler();
//    vao.unbind();
    vbo.unbind();
    vio.unbind();

    shader.unbind();
}

Model::~Model()
{

}
