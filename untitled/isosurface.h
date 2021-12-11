#ifndef ISOSURFACE_H
#define ISOSURFACE_H
#include "library/MyLibrary.h"

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
    
    /* Domain Search Tree */
    vector<vvvpTT> minmaxData;

    /* Active Cells Info */
    int cellsCou = 0;

    /* ISOValue */

    float ISOValue;

    int opacity;

    vector<Vertex> vertices;
    vector<uint> indices;

    /* Mesh Mode or not */

    bool meshMode;

    int inr_count = 1;
    /* Adjacent Cells Info */
    int ttxyz[8][3] = {
            {0, 0 - inr_count, 0 - inr_count},
            {0, 0, 0 - inr_count},
            {0, 0, 0},
            {0, 0 - inr_count, 0},
            {0 - inr_count, 0 - inr_count, 0 - inr_count},
            {0 - inr_count, 0, 0 - inr_count},
            {0 - inr_count, 0, 0},
            {0 - inr_count, 0 - inr_count, 0}
    };

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

    /* Checks for validity of access */
    bool isvalid(int xx, int yy, int zz)
    {
        if (xx >= 0 && xx < x && yy >= 0 && yy < y && zz >= 0 && zz < z)
            return true;
        return false;
    }


    void build()
    {
        /*
        *  If Float is preferred then use below
        *
        *  ISODataType allMin = + numeric_limits<ISODataType>::max();
        *  ISODataType allMax = - numeric_limits<ISODataType>::max();
        *
        */

        // clear previous contents
        minmaxData.clear();

        ISODataType allMin =   INT_MAX;
        ISODataType allMax =   INT_MIN;
        vvvpTT curMinMaxData(z - 1, vvpTT(y - 1, vpTT(x - 1)));
        for (int i = 1; i < z; i++)
        {
            for (int j = 1; j < y; j++)
            {
                for (int k = 1; k < x; k++)
                {
                    ISODataType cur_min = allMin;
                    ISODataType cur_max = allMax;
                    for (auto &adjCell : ttxyz)
                    {
                        int zz = i + adjCell[0];
                        int yy = j + adjCell[1];
                        int xx = k + adjCell[2];
                        if (isvalid(xx, yy, zz))
                        {
                            cur_min = min((*volume)[zz][yy][xx], cur_min);
                            cur_max = max((*volume)[zz][yy][xx], cur_max);
                        }
                    }
                    curMinMaxData[i - 1][j - 1][k - 1] = {cur_min, cur_max};
                }
            }
        }
        minmaxData.emplace_back(vvvpTT());
        minmaxData.back().swap(curMinMaxData);
        while (1)
        {
            auto &prev = minmaxData.back();
            int zSize = prev.size();
            int ySize = prev[0].size();
            int xSize = prev[0][0].size();
            int newZSize = (zSize + 1) / 2;
            int newYSize = (ySize + 1) / 2;
            int newXSize = (xSize + 1) / 2;

            if (zSize == 1 && ySize == 1 && xSize == 1)
                break;

            curMinMaxData.resize(newZSize, vvpTT(newYSize, vpTT(newXSize, make_pair(allMin, allMax))));
            for (int i = 0; i < zSize; i++)
            {
                for (int j = 0; j < ySize; j++)
                {
                    for (int k = 0; k < xSize; k++)
                    {
                        auto &data = curMinMaxData[i >> 1][j >> 1][k >> 1];
                        data.first = min(data.first, prev[i][j][k].first);
                        data.second = max(data.second, prev[i][j][k].second);
                    }
                }
            }
            minmaxData.emplace_back(vvvpTT());
            minmaxData.back().swap(curMinMaxData);
        }
    }

    void  PolygoniseTri(vector<Vertex> &vertices,int iso,int v0,int v1,int v2,int v3,glm::vec3 &pos)
    {
        int ntri = 0;
        int triindex;

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

        /* Form the vertices of the triangles for each case */
        switch (triindex) {
            case 0x00:
            case 0x0F:
                break;
            case 0x0E:
            case 0x01:
                ans.push_back( VertexInterp(iso,pos0,pos1,val0,val1) );
                ans.push_back( VertexInterp(iso,pos0,pos2,val0,val2) );
                ans.push_back( VertexInterp(iso,pos0,pos3,val0,val3) );
                ntri++;
                break;
            case 0x0D:
            case 0x02:
                ans.push_back( VertexInterp(iso,pos1,pos0,val1,val0) );
                ans.push_back( VertexInterp(iso,pos1,pos3,val1,val3) );
                ans.push_back( VertexInterp(iso,pos1,pos2,val1,val2) );
                ntri++;
                break;
            case 0x0C:
            case 0x03:
                ans.push_back( VertexInterp(iso,pos0,pos3,val0,val3) );
                ans.push_back( VertexInterp(iso,pos0,pos2,val0,val2) );
                ans.push_back( VertexInterp(iso,pos1,pos3,val1,val3) );
                ntri++;
                ans.push_back( VertexInterp(iso,pos1,pos3,val1,val3) );
                ans.push_back( VertexInterp(iso,pos1,pos2,val1,val2) );
                ans.push_back( VertexInterp(iso,pos0,pos2,val0,val2) );
                ntri++;
                break;
            case 0x0B:
            case 0x04:
                ans.push_back( VertexInterp(iso,pos2,pos0,val2,val0) );
                ans.push_back( VertexInterp(iso,pos2,pos1,val2,val1) );
                ans.push_back( VertexInterp(iso,pos2,pos3,val2,val3) );
                ntri++;
                break;
            case 0x0A:
            case 0x05:
                ans.push_back( VertexInterp(iso,pos0,pos1,val0,val1) );
                ans.push_back( VertexInterp(iso,pos2,pos3,val2,val3) );
                ans.push_back( VertexInterp(iso,pos0,pos3,val0,val3) );
                ntri++;
                ans.push_back( VertexInterp(iso,pos0,pos1,val0,val1) );
                ans.push_back( VertexInterp(iso,pos1,pos2,val1,val2) );
                ans.push_back( VertexInterp(iso,pos2,pos3,val2,val3) );
                ntri++;
                break;
            case 0x09:
            case 0x06:
                ans.push_back( VertexInterp(iso,pos0,pos1,val0,val1) );
                ans.push_back( VertexInterp(iso,pos1,pos3,val1,val3) );
                ans.push_back( VertexInterp(iso,pos2,pos3,val2,val3) );
                ntri++;
                ans.push_back( VertexInterp(iso,pos0,pos1,val0,val1) );
                ans.push_back( VertexInterp(iso,pos0,pos2,val0,val2) );
                ans.push_back( VertexInterp(iso,pos2,pos3,val2,val3) );
                ntri++;
                break;
            case 0x07:
            case 0x08:
                ans.push_back( VertexInterp(iso,pos3,pos0,val3,val0) );
                ans.push_back( VertexInterp(iso,pos3,pos2,val3,val2) );
                ans.push_back( VertexInterp(iso,pos3,pos1,val3,val1) );
                ntri++;
                break;
        }
        for(auto &tripos:ans)
        {
            vertices.emplace_back(Vertex(glm::vec3(tripos.x/x,tripos.y/y,tripos.z/z)));
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

    void getCells(unsigned int level,unsigned int z, unsigned int y,unsigned int x, float val)
    {

        if (!(level >= 0 && level < minmaxData.size()))
            return;
        
        auto &curMinMaxLevel = minmaxData[level];
        
        if(!(0<=z&&z<curMinMaxLevel.size()&&0<=y&&y<curMinMaxLevel[0].size()&&0<=x&&x<curMinMaxLevel[0][0].size()))
            return;

        // check for x,y,z validity
        bool validCell = (curMinMaxLevel[z][y][x].first <= val && val <= curMinMaxLevel[z][y][x].second);
        
        if (!validCell)
            return;
        
        if(level==0)
        {
            glm::vec3 pos;
            pos.x = x+1;
            pos.y = y+1;
            pos.z = z+1;
            cellsCou++;
            PolygoniseTri(vertices,val,0,2,3,7,pos);
            PolygoniseTri(vertices,val,0,2,6,7,pos);
            PolygoniseTri(vertices,val,0,4,6,7,pos);
            PolygoniseTri(vertices,val,0,6,1,2,pos);
            PolygoniseTri(vertices,val,0,6,1,4,pos);
            PolygoniseTri(vertices,val,5,6,1,4,pos);
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
            getCells(nextLevel,zz,yy,xx,val); 
        }
    }

    void marchingTetrahedraDomainSearch()
    {
        // initialize cells to 0
        cellsCou = 0;

        // clear vertices
        vertices.clear();

        // get All cells corresponding to given ISO Value
        getCells(int(minmaxData.size())-1,0,0,0,ISOValue);
        
        vao.bind();
        vbo.bind();
        vbo.bufferData(vertices);
        vbo.unbind();
        vao.unbind();
    }
    
    // void marchingTetrahedra(float isovalue=10.5)
    // {
    //     vertices.clear();
    //     indices.clear();
        
    //     int noOfThreads = 4;
    //     int workPerThread=z/noOfThreads;
    //     int curThread=0,curZ=0,lastZ=workPerThread*noOfThreads;
    //     vector<Vertex> threadedVertices[noOfThreads+1];

    //     auto threadHandler = [&threadedVertices,&isovalue,this](int threadi,int zb,int ze){
    //         glm::vec3 pos;
    //         for(int k=zb;k<ze;k+=inr_count)
    //         {
    //             pos.z = k ;
    //             for(int i=inr_count;i<y;i+=inr_count)
    //             {
    //                 pos.y = i;
    //                 for(int j=inr_count;j<x;j+=inr_count)
    //                 {
    //                     pos.x = j;
    //                     PolygoniseTri(threadedVertices[threadi],isovalue,0,2,3,7,pos);
    //                     PolygoniseTri(threadedVertices[threadi],isovalue,0,2,6,7,pos);
    //                     PolygoniseTri(threadedVertices[threadi],isovalue,0,4,6,7,pos);
    //                     PolygoniseTri(threadedVertices[threadi],isovalue,0,6,1,2,pos);
    //                     PolygoniseTri(threadedVertices[threadi],isovalue,0,6,1,4,pos);
    //                     PolygoniseTri(threadedVertices[threadi],isovalue,5,6,1,4,pos);
    //                 }
    //             }
    //         }
    //     };

    //     vector<std::thread> threads;
    //     auto begin = TIME_NOW;
    //     while(curZ<lastZ)
    //     {
    //         std::thread a(threadHandler,curThread,curZ+inr_count,curZ+workPerThread);
    //         threads.push_back(move(a));
    //         curZ+=workPerThread;
    //         curThread++;
    //     }
    //     threadHandler(curThread,lastZ+inr_count,z);

    //     for(auto &curThread:threads)
    //     {
    //         curThread.join();
    //     }

    //     // reference(N, matA, matB, output_reference);
    //     auto end = TIME_NOW;
    //     cout << "Reference execution time: " << 
    //     (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";    
    //     vbo.bind();
    //     vio.bind();

    //     curNoOfVertices = 0;
    //     for(auto &vertices:threadedVertices) curNoOfVertices+=vertices.size();
        
    //     vbo.bufferData(curNoOfVertices);

    //     int curOffset = 0;

    //     for(int i=0;i<=noOfThreads;i++) 
    //     {
    //         vbo.bufferSubData(threadedVertices[i],curOffset);
    //         curOffset+=threadedVertices[i].size();
    //     }

    //     // vio.bufferData(indices);

    //     vbo.unbind();
    //     vio.unbind();
    // }

public:

    /* Should Display Surface or not */

    bool shouldDisplay = true;

    ISOSurface(){
        volume = 0;
        z = 0;
        y = 0;
        x = 0;
        opacity = 100;
        meshMode = false;
    }

    ISOSurface(vvvT &Volume,float ISOValueIn = 0,bool useISOValueIn=false) : ISOValue(ISOValueIn)
    {
        volume = &Volume;
        z = Volume.size();
        y = Volume[0].size();
        x = Volume[0][0].size();
        opacity = 100;
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

    void setISOValue(float ISOValueIn,bool useISOValueIn=true)
    {
        // build minmax tree
        build();
        if(useISOValueIn) ISOValue = ISOValueIn;
        else
        {
            auto tmpMinMax = getMinMax();
            ISOValue = (tmpMinMax.first+tmpMinMax.second)/2;
        }
        marchingTetrahedraDomainSearch();
    }

    int getOpacity()
    {
        return opacity;
    }

    void setOpacity(int opacityIn)
    {
        opacity = opacityIn;
    }

    pTT getMinMax()
    {
        auto pr = minmaxData.back().back().back().back();
        return pr;
    }
    
    void render(function<void()> f=[](){})
    {
        vbo.bind();
        vio.bind();
//        vao.bind();

        f();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        if(meshMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES,0,vertices.size());
        if(meshMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        ErrorHandler();
//        vao.unbind();
        vbo.unbind();
        vio.unbind();
    }

    friend class RawModel;
};

#endif // ISOSURFACE_H
