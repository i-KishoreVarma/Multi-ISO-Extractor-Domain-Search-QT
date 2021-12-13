#ifndef ISOSURFACE_H
#define ISOSURFACE_H
#include "library/MyLibrary.h"
#include<thread>
#include<queue>
#include <chrono>
using namespace std::chrono;

#define TIME_NOW std::chrono::high_resolution_clock::now()
#define TIME_DIFF(gran, start, end) std::chrono::duration_cast<milliseconds>(end - start).count()


typedef int ISODataType;

typedef pair<ISODataType, ISODataType> pTT;
//typedef pair<ISODataType, ISODataType> pTT;

typedef vector<ISODataType> vT;
typedef vector<vT> vvT;
typedef vector<vvT> vvvT;

typedef vector<pTT> vpTT;
typedef vector<vpTT> vvpTT;
typedef vector<vvpTT> vvvpTT;

class ISOSurface
{
    /* OpenGL Objects */
    VAO vao;
    VBO vbo;
    IBO vio;
    
    /* Dimensions Of Model */
    int x,y,z;

    vvvT *volume = 0;

    int isoSkipValue=4;
    
    /* Domain Search Tree */
    vector<vvvpTT> *minmaxData = 0,*minmaxDataSample = 0;

    /* Active Cells Info */
    int cellsCou = 0;

    /* ISOValue */

    float ISOValue;

    int opacity;
    int colorMap;

    vector<Vertex> vertices;
    vector<uint> indices;

    /* Mesh Mode or not */

    bool meshMode;

    int inr_count = 1;

    vector<glm::vec3> adjacentIndices = {
        {-inr_count, 0, 0},
        {inr_count, 0, 0},
        {0, -inr_count, 0},
        {0, inr_count, 0},
        {0, 0, -inr_count},
        {0, 0, inr_count}
    };





    /* Checks for validity of access */

    void  PolygoniseTri(vector<Vertex> &vertices,int iso,int v0,int v1,int v2,int v3,glm::vec3 &pos,int inr_count)
    {
        int ntri = 0;
        int triindex;
        vector<glm::vec3> txyz = {
                    {0,0-inr_count,0-inr_count},
                    {0,0,0-inr_count},
                    {0,0,0},
                    {0,0-inr_count,0},
                    {0-inr_count,0-inr_count,0-inr_count},
                    {0-inr_count,0,0-inr_count},
                    {0-inr_count,0,0},
                    {0-inr_count,0-inr_count,0}
        };
        /*
            Determine which of the 16 cases we have given which vertices
            are above or below the isosurface
        */
        triindex = 0;

        glm::vec3 pos0 = pos + txyz[v0];
        glm::vec3 pos1 = pos + txyz[v1];
        glm::vec3 pos2 = pos + txyz[v2];
        glm::vec3 pos3 = pos + txyz[v3];

        int val0 = (*volume)[pos0.z][pos0.y][pos0.x];
        int val1 = (*volume)[pos1.z][pos1.y][pos1.x];
        int val2 = (*volume)[pos2.z][pos2.y][pos2.x];
        int val3 = (*volume)[pos3.z][pos3.y][pos3.x];
        
        if ( val0 < iso) triindex |= 1;
        if ( val1 < iso) triindex |= 2;
        if ( val2 < iso) triindex |= 4;
        if ( val3 < iso) triindex |= 8;

        vector<glm::vec3> ans;
        vector<glm::vec3> gradients;
        glm::vec3 pos0Gradient = glm::vec3((getScalarFieldValue(pos0.x - 1, pos0.y, pos0.z) - getScalarFieldValue(pos0.x + 1, pos0.y, pos0.z)) / 2.0,
                                           (getScalarFieldValue(pos0.x, pos0.y - 1, pos0.z) - getScalarFieldValue(pos0.x, pos0.y + 1, pos0.z)) / 2.0,
                                           (getScalarFieldValue(pos0.x, pos0.y, pos0.z - 1) - getScalarFieldValue(pos0.x + 1, pos0.y, pos0.z + 1)) / 2.0);

        glm::vec3 pos1Gradient = glm::vec3((getScalarFieldValue(pos1.x - 1, pos1.y, pos1.z) - getScalarFieldValue(pos1.x + 1, pos1.y, pos1.z)) / 2.0,
                                           (getScalarFieldValue(pos1.x, pos1.y - 1, pos1.z) - getScalarFieldValue(pos1.x, pos1.y + 1, pos1.z)) / 2.0,
                                           (getScalarFieldValue(pos1.x, pos1.y, pos1.z - 1) - getScalarFieldValue(pos1.x + 1, pos1.y, pos1.z + 1)) / 2.0);

        glm::vec3 pos2Gradient = glm::vec3((getScalarFieldValue(pos2.x - 1, pos2.y, pos2.z) - getScalarFieldValue(pos2.x + 1, pos2.y, pos2.z)) / 2.0,
                                           (getScalarFieldValue(pos2.x, pos2.y - 1, pos2.z) - getScalarFieldValue(pos2.x, pos2.y + 1, pos2.z)) / 2.0,
                                           (getScalarFieldValue(pos2.x, pos2.y, pos2.z - 1) - getScalarFieldValue(pos2.x + 1, pos2.y, pos2.z + 1)) / 2.0);

        glm::vec3 pos3Gradient = glm::vec3((getScalarFieldValue(pos3.x - 1, pos3.y, pos3.z) - getScalarFieldValue(pos3.x + 1, pos3.y, pos3.z)) / 2.0,
                                           (getScalarFieldValue(pos3.x, pos3.y - 1, pos3.z) - getScalarFieldValue(pos3.x, pos3.y + 1, pos3.z)) / 2.0,
                                           (getScalarFieldValue(pos3.x, pos3.y, pos3.z - 1) - getScalarFieldValue(pos3.x + 1, pos3.y, pos3.z + 1)) / 2.0);

        /* Form the vertices of the triangles for each case */
        switch (triindex) {
            case 0x00:
            case 0x0F:
                break;
            case 0x0E:
            case 0x01:
                ans.push_back( VertexInterp(iso,pos0,pos1,val0,val1) );
                gradients.push_back((pos0Gradient + pos1Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos0,pos2,val0,val2) );
                gradients.push_back((pos0Gradient + pos2Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos0,pos3,val0,val3) );
                gradients.push_back((pos0Gradient + pos3Gradient) / 2.0f);

                ntri++;
                break;
            case 0x0D:
            case 0x02:
                ans.push_back( VertexInterp(iso,pos1,pos0,val1,val0) );
                gradients.push_back((pos1Gradient + pos0Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos1,pos3,val1,val3) );
                gradients.push_back((pos1Gradient + pos3Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos1,pos2,val1,val2) );
                gradients.push_back((pos1Gradient + pos2Gradient) / 2.0f);

                ntri++;
                break;
            case 0x0C:
            case 0x03:
                ans.push_back( VertexInterp(iso,pos0,pos3,val0,val3) );
                gradients.push_back((pos0Gradient + pos3Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos0,pos2,val0,val2) );
                gradients.push_back((pos0Gradient + pos2Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos1,pos3,val1,val3) );
                gradients.push_back((pos1Gradient + pos3Gradient) / 2.0f);

                ntri++;
                ans.push_back( VertexInterp(iso,pos1,pos3,val1,val3) );
                gradients.push_back((pos1Gradient + pos3Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos1,pos2,val1,val2) );
                gradients.push_back((pos1Gradient + pos2Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos0,pos2,val0,val2) );
                gradients.push_back((pos0Gradient + pos2Gradient) / 2.0f);

                ntri++;
                break;
            case 0x0B:
            case 0x04:
                ans.push_back( VertexInterp(iso,pos2,pos0,val2,val0) );
                gradients.push_back((pos2Gradient + pos0Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos2,pos1,val2,val1) );
                gradients.push_back((pos2Gradient + pos1Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos2,pos3,val2,val3) );
                gradients.push_back((pos2Gradient + pos3Gradient) / 2.0f);

                ntri++;
                break;
            case 0x0A:
            case 0x05:
                ans.push_back( VertexInterp(iso,pos0,pos1,val0,val1) );
                gradients.push_back((pos0Gradient + pos1Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos2,pos3,val2,val3) );
                gradients.push_back((pos2Gradient + pos3Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos0,pos3,val0,val3) );
                gradients.push_back((pos0Gradient + pos3Gradient) / 2.0f);

                ntri++;
                ans.push_back( VertexInterp(iso,pos0,pos1,val0,val1) );
                gradients.push_back((pos0Gradient + pos1Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos1,pos2,val1,val2) );
                gradients.push_back((pos1Gradient + pos2Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos2,pos3,val2,val3) );
                gradients.push_back((pos2Gradient + pos3Gradient) / 2.0f);

                ntri++;
                break;
            case 0x09:
            case 0x06:
                ans.push_back( VertexInterp(iso,pos0,pos1,val0,val1) );
                gradients.push_back((pos0Gradient + pos1Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos1,pos3,val1,val3) );
                gradients.push_back((pos1Gradient + pos3Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos2,pos3,val2,val3) );
                gradients.push_back((pos2Gradient + pos3Gradient) / 2.0f);

                ntri++;
                ans.push_back( VertexInterp(iso,pos0,pos1,val0,val1) );
                gradients.push_back((pos0Gradient + pos1Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos0,pos2,val0,val2) );
                gradients.push_back((pos0Gradient + pos2Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos2,pos3,val2,val3) );
                gradients.push_back((pos2Gradient + pos3Gradient) / 2.0f);

                ntri++;
                break;
            case 0x07:
            case 0x08:
                ans.push_back( VertexInterp(iso,pos3,pos0,val3,val0) );
                gradients.push_back((pos3Gradient + pos0Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos3,pos2,val3,val2) );
                gradients.push_back((pos3Gradient + pos2Gradient) / 2.0f);

                ans.push_back( VertexInterp(iso,pos3,pos1,val3,val1) );
                gradients.push_back((pos3Gradient + pos1Gradient) / 2.0f);

                ntri++;
                break;
        }
        int index = 0;
        for(auto &tripos:ans)
        {
            vertices.emplace_back(Vertex(glm::vec3(tripos.x/x,tripos.y/y,tripos.z/z), glm::vec3(1.0f), gradients[index]));
            index++;
        }
    }

    glm::vec3 VertexInterp(float isolevel,glm::vec3 &p1,glm::vec3 &p2,int val1,int val2)
    {
        double mu;
        glm::vec3 p;

        if (abs(isolevel-val1) < 0.00001)
            return(p1);
        if (abs(isolevel-val2) < 0.00001)
            return(p2);
        if (abs(val1-val2) < 0.00001)
            return(p1);
        mu = (isolevel - val1) / (val2 - val1);
        p.x = p1.x + mu * (p2.x - p1.x);
        p.y = p1.y + mu * (p2.y - p1.y);
        p.z = p1.z + mu * (p2.z - p1.z);

        return p;
    }

    void getCells(unsigned int level,unsigned int z, unsigned int y,unsigned int x, float val, vector<Vertex> &vertices, vector<vvvpTT> &mnmxData,bool useSample)
    {
        if(useSample)
            inr_count = isoSkipValue;
        else
            inr_count = 1;

        if (!(level >= 0 && level < mnmxData.size()))
            return;

        
        auto &curMinMaxLevel = mnmxData[level];
        
        if(!(0<=z&&z<curMinMaxLevel.size()&&0<=y&&y<curMinMaxLevel[0].size()&&0<=x&&x<curMinMaxLevel[0][0].size()))
            return;

        // check for x,y,z validity
        bool validCell = (curMinMaxLevel[z][y][x].first <= val && val <= curMinMaxLevel[z][y][x].second);
        
        if (!validCell)
            return;
        
        if(level==0)
        {
            glm::vec3 pos;
            pos.x = (x+1)*inr_count;
            pos.y = (y+1)*inr_count;
            pos.z = (z+1)*inr_count;
            cellsCou++;
            PolygoniseTri(vertices,val,0,2,3,7,pos,inr_count);
            PolygoniseTri(vertices,val,0,2,6,7,pos,inr_count);
            PolygoniseTri(vertices,val,0,4,6,7,pos,inr_count);
            PolygoniseTri(vertices,val,0,6,1,2,pos,inr_count);
            PolygoniseTri(vertices,val,0,6,1,4,pos,inr_count);
            PolygoniseTri(vertices,val,5,6,1,4,pos,inr_count);
            return;
        }

        int nextLevel = level - 1;
        int xx, yy, zz;
        int x2 = x << 1, y2 = y << 1, z2 = z << 1;
        for (int i = 0; i < 8; i++)
        {
            xx = x2 + ((i&1)>>0); 
            yy = y2 + ((i&2)>>1); 
            zz = z2 + ((i&4)>>2);
            getCells(nextLevel,zz,yy,xx,val,vertices,mnmxData,useSample);
        }
    }

//    void computeGradients() {
//        for(Vertex &v : vertices) {
//            float gx = getScalarFieldValue()
//        }
//    }


    float getScalarFieldValue(int i, int j, int k) {
        if(!(i >= 0 and i < x) or !(j >= 0 and j < y) or !(k >= 0 and k < z)) return 0;
        return (*volume)[k][j][i];
    }

    void marchingTetrahedraDomainSearch(bool useSample)
    {

        // initialize cells to 0
        cellsCou = 0;

        // clear vertices
        vertices.clear();

        // get All cells corresponding to given ISO Value
        if(useSample)
            getCells(int((*minmaxDataSample).size())-1,0,0,0,ISOValue,vertices,*minmaxDataSample,useSample);
        else
            getCells(int((*minmaxData).size())-1,0,0,0,ISOValue,vertices,*minmaxData,useSample);

//        computeGradients();

        vao.bind();
        vbo.bind();
        vbo.bufferData(vertices);
        vbo.unbind();
        vao.unbind();
    }

    void get4Cells(unsigned int level,unsigned int z, unsigned int y,unsigned int x, float val,vector<vvvpTT> &mnmxData,bool useSample)
    {
        vector<Vertex> vertexPerThread[4];
        auto threadHandler = [&](int threadi,int z,int y,int x){
            getCells(level-1,z,y,x,val,vertexPerThread[threadi],mnmxData,useSample);
            getCells(level-1,z,y,x+1,val,vertexPerThread[threadi],mnmxData,useSample);
        };

        vector<std::thread> threads;

        queue<glm::vec3> q;

        int xx, yy, zz;
        int x2 = x<<1, y2 = y << 1, z2 = z << 1;
        int curThread = 0;
        for (int i = 0; i < 4; i++)
        {
            xx = x2;
            yy = y2 + ((i&1)>>0);
            zz = z2 + ((i&2)>>1);
            q.push({xx,yy,zz});
        }

        while(!q.empty())
        {
            auto tmp = q.front();
            q.pop();
            std::thread a(threadHandler,curThread,tmp.z,tmp.y,tmp.x);
            threads.push_back(move(a));
            curThread++;
        }


        for(auto &curThread:threads)
        {
            curThread.join();
        }

        int no_of_elements = 0;

        for(auto &tt:vertexPerThread)
        {
            no_of_elements+= tt.size();
        }

        vertices.resize(no_of_elements);


        #pragma omp parallel for
        for(int i=0;i<4;i++)
        {
            int prev_size = 0;
            for(int j=0;j<i;j++) prev_size+=vertexPerThread[j].size();
            auto &vv = vertexPerThread[i];
            for(int j=0;j<vertexPerThread[i].size();j++)
                vertices[prev_size+j] = vv[j];
        }

    }

    void marchingTetrahedraDomainSearchParallelized(bool useSample)
    {
        // initialize cells to 0
        cellsCou = 0;

        // clear vertices
        vertices.clear();

        // get All cells corresponding to given ISO Value
        if(useSample)
            get4Cells(int((*minmaxDataSample).size())-1,0,0,0,ISOValue,*minmaxDataSample,useSample);
        else
            get4Cells(int((*minmaxData).size())-1,0,0,0,ISOValue,*minmaxData,useSample);

//        computeGradients();

        vao.bind();
        vbo.bind();
        vbo.bufferData(vertices);
        vbo.unbind();
        vao.unbind();
    }
    
//     void marchingTetrahedra(float isovalue=10.5)
//     {
//         vertices.clear();
//         indices.clear();
        
//         int noOfThreads = 4;
//         int workPerThread=z/noOfThreads;
//         int curThread=0,curZ=0,lastZ=workPerThread*noOfThreads;
//         vector<Vertex> threadedVertices[noOfThreads+1];

//         auto threadHandler = [&threadedVertices,&isovalue,this](int threadi,int zb,int ze){
//             glm::vec3 pos;
//             for(int k=zb;k<ze;k+=inr_count)
//             {
//                 pos.z = k ;
//                 for(int i=inr_count;i<y;i+=inr_count)
//                 {
//                     pos.y = i;
//                     for(int j=inr_count;j<x;j+=inr_count)
//                     {
//                         pos.x = j;
//                         PolygoniseTri(threadedVertices[threadi],isovalue,0,2,3,7,pos);
//                         PolygoniseTri(threadedVertices[threadi],isovalue,0,2,6,7,pos);
//                         PolygoniseTri(threadedVertices[threadi],isovalue,0,4,6,7,pos);
//                         PolygoniseTri(threadedVertices[threadi],isovalue,0,6,1,2,pos);
//                         PolygoniseTri(threadedVertices[threadi],isovalue,0,6,1,4,pos);
//                         PolygoniseTri(threadedVertices[threadi],isovalue,5,6,1,4,pos);
//                     }
//                 }
//             }
//         };

//         vector<std::thread> threads;
//         auto begin = TIME_NOW;
//         while(curZ<lastZ)
//         {
//             std::thread a(threadHandler,curThread,curZ+inr_count,curZ+workPerThread);
//             threads.push_back(move(a));
//             curZ+=workPerThread;
//             curThread++;
//         }
//         threadHandler(curThread,lastZ+inr_count,z);

//         for(auto &curThread:threads)
//         {
//             curThread.join();
//         }

//         // reference(N, matA, matB, output_reference);
//         auto end = TIME_NOW;
//         cout << "Reference execution time: " <<
//         (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";
//         vbo.bind();
//         vio.bind();

//         curNoOfVertices = 0;
//         for(auto &vertices:threadedVertices) curNoOfVertices+=vertices.size();
        
//         vbo.bufferData(curNoOfVertices);

//         int curOffset = 0;

//         for(int i=0;i<=noOfThreads;i++)
//         {
//             vbo.bufferSubData(threadedVertices[i],curOffset);
//             curOffset+=threadedVertices[i].size();
//         }

//         // vio.bufferData(indices);

//         vbo.unbind();
//         vio.unbind();
//     }

public:

    /* Should Display Surface or not */

    bool shouldDisplay = true;

    ISOSurface(){
        volume = 0;
        z = 0;
        y = 0;
        x = 0;
        opacity = 100;
        colorMap = 0;
        meshMode = false;
    }

    ISOSurface(vvvT &Volume,vector<vvvpTT> &mnmxData,vector<vvvpTT> &mnmxDataSample,float ISOValueIn = 0,bool useISOValueIn=false,int isoSkipValue=4) : ISOValue(ISOValueIn)
    {
        minmaxData = &mnmxData;
        minmaxDataSample = &mnmxDataSample;

        this->isoSkipValue = isoSkipValue;

        cout<<(*minmaxData).size()<<" "<<(*minmaxDataSample).size()<<"\n";
        volume = &Volume;
        z = Volume.size();
        y = Volume[0].size();
        x = Volume[0][0].size();
        opacity = 100;
        colorMap = 0;
        meshMode = false;

        vao.create();
        vbo.create();
        vio.create();

        BufferLayout layout;
        layout.addAttrib<float>(3);
            
        vao.addLayout(layout);

        vao.unbind();
        vbo.unbind();
        vio.unbind();

        setISOValue(ISOValueIn,useISOValueIn);
    }

    float getISOvalue()
    {
        return ISOValue;
    }

    void toggleShouldDisplay()
    {
        shouldDisplay=!shouldDisplay;
    }


    void toggleColorMap() {
        colorMap = 1 - colorMap;
    }

    void setISOValue(float ISOValueIn,bool useISOValueIn=true,bool useSample=false)
    {
        // build minmax tree
        if(useISOValueIn) ISOValue = ISOValueIn;
        else
        {
            auto tmpMinMax = getMinMax(useSample);
            ISOValue = (tmpMinMax.first+tmpMinMax.second)/2;
        }
        auto start = high_resolution_clock::now();
        marchingTetrahedraDomainSearchParallelized(useSample);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout << "Time Taken for Tetrahedra Parllelized "<< ISOValue << " : " << duration.count() << endl;
    }

    int getOpacity()
    {
        return opacity;
    }

    void setOpacity(int opacityIn)
    {
        opacity = opacityIn;
    }


    int getColorMap() {
        return colorMap;
    }

    void setColorMap(int colorMapIn) {
        colorMap = colorMapIn;
    }

    pTT getMinMax(bool useSample=false)
    {
        if(useSample){
            auto pr = (*minmaxDataSample).back().back().back().back();
            return pr;
        }
        auto pr = (*minmaxData).back().back().back().back();
        return pr;
    }
    
    void render(function<void()> f=[](){})
    {
        vbo.bind();
        vio.bind();

        // VAO Binding Causing Issue
        // vao.bind();

        f();

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        //glEnableVertexAttribArray(2);

        if(meshMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES,0,vertices.size());
        if(meshMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ErrorHandler();

        // vao.unbind();
        vbo.unbind();
        vio.unbind();
    }

    friend class RawModel;
};

#endif // ISOSURFACE_H
