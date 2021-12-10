#include "library/MyLibrary.h"

#include <limits>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>
#include<thread>
namespace fs = std::filesystem;

#define TIME_NOW std::chrono::high_resolution_clock::now()
#define TIME_DIFF(gran, start, end) std::chrono::duration_cast<gran>(end - start).count()
typedef pair<int, int> pii;

typedef vector<int> vi;
typedef vector<vi> vvi;
typedef vector<vvi> vvvi;

typedef vector<pii> vpii;
typedef vector<vpii> vvpii;
typedef vector<vvpii> vvvpii;


template<typename T>
class RawModel
{
    private:
        // void normalizeData();

        vector<string> split(const string &s, char delim) {
            vector<string> result;
            stringstream ss(s);
            string item;

            while (std::getline (ss, item, delim)) {
                result.push_back (item);
            }

            return result;
        }


        auto parseFileName(const string &pathFileName)
        {
            fs::path curPath = pathFileName;
            if(curPath.has_extension()) curPath.replace_extension("");
            string fileName = curPath.filename();
            vector<string> data = split(fileName,'_');
            if(data.size()!=3)
            {
                std::cerr<<"Failed to read RawModel from file["<<fileName<<"]\n";
                std::cerr<<"File Name Format should be $FileName$_$X$x$Y$x$Z$_$DataType$.raw";
                exit(-1);
            }
            std::map<string,string> m;
            m["name"] = data[0];
            m["dimension"] = data[1];
            m["dataType"] = data[2];
            return m;
        }

        void getDimensions(const string &dimension,int &x,int &y,int &z)
        {
            vector<string> dimensions = split(dimension,'x');
            if(dimensions.size()!=3)
            {
                std::cerr<<"Failed in Reading Dimensions\n";
                exit(-1);
            }
            x = stoi(dimensions[0]);
            y = stoi(dimensions[1]);
            z = stoi(dimensions[2]);
        }

        void readModel(const string &fileName)
        {
            ifstream file;
            file.open( fileName,ios::in | ios::binary);
            auto properties = parseFileName(fileName);
            getDimensions(properties["dimension"],x,y,z);
            int noOfNonZeros = 0;
            vector<vector<T>> matrix(y,vector<T>(x));
            for(int k=0;k<z;k++)
            {
                for(int i=0;i<y;i++)
                {
                    file.read((char *)&matrix[i][0],x*sizeof(T));
                }
                volume.push_back(matrix);
            }
            // T minval= 255,maxval = 0;
            // for(int k=0;k<z;k++)
            // {
            //     for(int i=0;i<y;i++)
            //         for(int j=0;j<x;j++)
            //         {
            //             minval = min(minval,volume[k][i][j]);
            //             maxval = max(maxval,volume[k][i][j]);
            //         }
            // }
            // cout<<"Min and max value : "<<minval<<" "<<maxval<<'\n';
            // cout<<"No of Non zeros :"<<noOfNonZeros<<'\n';
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

            int val0 =volume[pos0.z][pos0.y][pos0.x];
            int val1 =volume[pos1.z][pos1.y][pos1.x];
            int val2 =volume[pos2.z][pos2.y][pos2.x];
            int val3 =volume[pos3.z][pos3.y][pos3.x];
            
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
        int curNoOfVertices = 0;
    public:
        int x,y,z; 
        int inr_count = 1;
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
        int ttxyz[8][3] = {
            {0, 0 - inr_count, 0 - inr_count},
            {0, 0, 0 - inr_count},
            {0, 0, 0},
            {0, 0 - inr_count, 0},
            {0 - inr_count, 0 - inr_count, 0 - inr_count},
            {0 - inr_count, 0, 0 - inr_count},
            {0 - inr_count, 0, 0},
            {0 - inr_count, 0 - inr_count, 0}};
        vector<vector<vector<T>>> volume;
        vector<vvvpii> minmaxData;
        vector<vector<T>> planeSlice;
        vector<Vertex> vertices;
        vector<uint> indices;
        string name;
        VAO vao;
        VBO vbo;
        IBO vio;
        Shader shader;
        GLenum mode;
        int noOfVertices,noOfPolygons,noOfEdges;
        bool shouldUseCommonShader;
        bool meshMode;
        glm::mat4 World;

        RawModel(const string &modelName="myModel")
        {
            noOfVertices = 0,noOfEdges = 0,noOfPolygons = 0;
            mode = GL_TRIANGLES;
            meshMode = false;
            shouldUseCommonShader = true;
        }
        RawModel(const string &modelName,const string &modelPath,bool useCommonShader = true)
        {
            noOfVertices = 0,noOfEdges = 0,noOfPolygons = 0;
            create(modelName,modelPath,useCommonShader);
        }
        void create(const string &modelName,const string &modelPath,bool useCommonShader = true)
        {
            name = modelName;
            meshMode = false; 
            shouldUseCommonShader = useCommonShader;
            if(shouldUseCommonShader) shader = commonShader;
            readModel(modelPath);

            vao.create();
            vbo.create();
            vio.create();
            
            BufferLayout layout;

            // vbo.bufferData(vertices);
            // vio.bufferData(indices);

            ErrorHandler();

            int vType = Vertex::vertexType;

            layout.addAttrib<float>(3);
            // layout.addAttrib<float>(3);
            // layout.addAttrib<float>(3);
            // layout.addAttrib<float>(2);
            
            vao.addLayout(layout);

            vao.unbind();
            vbo.unbind();
            vio.unbind();

        }
        
        bool isvalid(int xx, int yy, int zz)
        {
            if (xx >= 0 && xx < x && yy >= 0 && yy < y && zz >= 0 && zz < z)
                return true;
            return false;
        }

        int cellsCou = 0;

        void getCells(int level, int z, int y, int x, float val)
        {
            // cellsCou = 0;
            if (!(level >= 0 && level < minmaxData.size()))
            {
                return;
            }
            auto &curMinMaxLevel = minmaxData[level];
            if(!(0<=z&&z<curMinMaxLevel.size()&&0<=y&&y<curMinMaxLevel[0].size()&&0<=x&&x<curMinMaxLevel[0][0].size()))
            {
                return;
            }
            // check for x,y,z validity
            bool validCell = curMinMaxLevel[z][y][x].first <= val && val <= curMinMaxLevel[z][y][x].second;
            if (!validCell)
                return;
            
            if(level==0)
            {
                //marching tetrahedra
                // cout<<z<<' '<<y<<' '<<x<<'\n';
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
        
        void build()
        {
            vvvpii curMinMaxData(z - 1, vvpii(y - 1, vpii(x - 1)));
            for (int i = 1; i < z; i++)
            {
                for (int j = 1; j < y; j++)
                {
                    for (int k = 1; k < x; k++)
                    {
                        int cur_min = INT_MAX;
                        int cur_max = INT_MIN;
                        for (auto &adjCell : ttxyz)
                        {
                            int zz = i + adjCell[0];
                            int yy = j + adjCell[1];
                            int xx = k + adjCell[2];
                            if (isvalid(xx, yy, zz))
                            {
                                cur_min = min((int)volume[zz][yy][xx], cur_min);
                                cur_max = max((int)volume[zz][yy][xx], cur_max);
                            }
                        }
                        curMinMaxData[i - 1][j - 1][k - 1] = {cur_min, cur_max};
                    }
                }
            }
            minmaxData.emplace_back(vvvpii());
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

                curMinMaxData.resize(newZSize, vvpii(newYSize, vpii(newXSize, make_pair(INT_MAX, INT_MIN))));
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
                minmaxData.emplace_back(vvvpii());
                minmaxData.back().swap(curMinMaxData);
            }
            for (auto &proj : minmaxData)
            {
                // printf("%ld %ld %ld\n", proj.size(), proj[0].size(), proj[0][0].size());
            }
        }
        
        void marchingTetrahedraDomainSearch(float isovalue=10.5)
        {
            cellsCou = 0;
            vertices.clear();
            getCells(int(minmaxData.size())-1,0,0,0,isovalue);
            vao.bind();
            vbo.bind();
            // cout<<"Vertices size : "<<vertices.size()<<'\n';
            vbo.bufferData(vertices);
            vbo.unbind();
            vao.unbind();
            curNoOfVertices = vertices.size()/3;
            // cout<<curNoOfVertices<<'\n';
            // vao.unbind();
        }
        
        void marchingTetrahedra(float isovalue=10.5)
        {
            vertices.clear();
            indices.clear();
            
            int noOfThreads = 4;
            int workPerThread=z/noOfThreads;
            int curThread=0,curZ=0,lastZ=workPerThread*noOfThreads;
            vector<Vertex> threadedVertices[noOfThreads+1];

            auto threadHandler = [&threadedVertices,&isovalue,this](int threadi,int zb,int ze){
                glm::vec3 pos;
                for(int k=zb;k<ze;k+=inr_count)
                {
                    pos.z = k ;
                    for(int i=inr_count;i<y;i+=inr_count)
                    {
                        pos.y = i;
                        for(int j=inr_count;j<x;j+=inr_count)
                        {
                            pos.x = j;
                            PolygoniseTri(threadedVertices[threadi],isovalue,0,2,3,7,pos);
                            PolygoniseTri(threadedVertices[threadi],isovalue,0,2,6,7,pos);
                            PolygoniseTri(threadedVertices[threadi],isovalue,0,4,6,7,pos);
                            PolygoniseTri(threadedVertices[threadi],isovalue,0,6,1,2,pos);
                            PolygoniseTri(threadedVertices[threadi],isovalue,0,6,1,4,pos);
                            PolygoniseTri(threadedVertices[threadi],isovalue,5,6,1,4,pos);
                        }
                    }
                }
            };

            vector<std::thread> threads;
            auto begin = TIME_NOW;
            while(curZ<lastZ)
            {
                std::thread a(threadHandler,curThread,curZ+inr_count,curZ+workPerThread);
                threads.push_back(move(a));
                curZ+=workPerThread;
                curThread++;
            }
            threadHandler(curThread,lastZ+inr_count,z);

            for(auto &curThread:threads)
            {
                curThread.join();
            }

            // reference(N, matA, matB, output_reference);
            auto end = TIME_NOW;
            cout << "Reference execution time: " << 
            (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";    
            vbo.bind();
            vio.bind();

            curNoOfVertices = 0;
            for(auto &vertices:threadedVertices) curNoOfVertices+=vertices.size();
            
            vbo.bufferData(curNoOfVertices);

            int curOffset = 0;

            for(int i=0;i<=noOfThreads;i++) 
            {
                vbo.bufferSubData(threadedVertices[i],curOffset);
                curOffset+=threadedVertices[i].size();
            }

            // vio.bufferData(indices);

            vbo.unbind();
            vio.unbind();
        }
        void render(function<void(Shader&)> f=[](Shader &shader){})
        {
            vao.bind();
            vbo.bind();
            vio.bind();
            if(shouldUseCommonShader)
            {
                shader.bind();
                shader.setUniformMat4f("gWorld",World);
            }
            else f(shader);
            if(meshMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            // glDrawElements(mode,indices.size(),GL_UNSIGNED_INT,0);
            glDrawArrays(mode,0,curNoOfVertices*3);
            if(meshMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            ErrorHandler();
            vao.unbind();
            vbo.unbind();
            vio.unbind();
        }

        ~RawModel()
        {

        }
};
