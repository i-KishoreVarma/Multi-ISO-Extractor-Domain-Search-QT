#include "isosurface.h"

#include <limits>
#include <assert.h>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>
#include<thread>
#define DEBUG 1
namespace fs = std::filesystem;
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


class RawModel
{

    private:
        // Dimensions of Volume
        int x,y,z;

        // Volume
        vvvT volume;

        // Min Max of the data
        vector<vvvpTT> minmaxData,minmaxDataSample;

        int inr_count = 1;


        /* Checks for validity of access */
        bool isvalid(int xx, int yy, int zz)
        {
            if (xx >= 0 && xx < x && yy >= 0 && yy < y && zz >= 0 && zz < z)
                return true;
            return false;
        }

        void buildMultithreaded(int inr_count,vector<vvvpTT> &minmaxDataSample)
        {
            /*
            *  If Float is preferred then use below
            *
            *  ISODataType allMin = + numeric_limits<ISODataType>::max();
            *  ISODataType allMax = - numeric_limits<ISODataType>::max();
            *
            */

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
            // clear previous contents
            minmaxDataSample.clear();

            ISODataType allMin =   INT_MAX;
            ISODataType allMax =   INT_MIN;
            //vvvpTT curMinMaxData(z - inr_count, vvpTT(y - inr_count, vpTT(x - inr_count)));
            vvvpTT curMinMaxData(ceil(z/(1.0*inr_count))-1, vvpTT(ceil(y/(1.0*inr_count))-1, vpTT(ceil(x/(1.0*inr_count))-1)));


            int noOfThreads = 4;
            int workPerThread=z/(noOfThreads);
            int curThread=0,curZ=0,lastZ=workPerThread*(noOfThreads);

            auto threadHandler = [&](int threadi,int zb,int ze){
//                zb = ceil((1.0*zb)/inr_count) * inr_count ;
                zb = ((zb+inr_count)/inr_count)*inr_count;
                for (int i = zb,ii=zb/inr_count-1; i < ze; i+=inr_count,ii++)
                {
                    for (int j = inr_count,jj=0; j < y; j+=inr_count,jj++)
                    {
                        for (int k = inr_count,kk=0; k < x; k+=inr_count,kk++)
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
                                    cur_min = min((volume)[zz][yy][xx], cur_min);
                                    cur_max = max((volume)[zz][yy][xx], cur_max);
                                }
                            }
                            curMinMaxData[ii][jj][kk] = {cur_min, cur_max};
                        }
                    }
                }
            };

            vector<std::thread> threads;

            while(curZ<lastZ)
            {
                std::thread a(threadHandler,curThread,curZ,curZ+workPerThread);
                threads.push_back(move(a));
                curZ+=workPerThread;
                curThread++;
            }
            threadHandler(curThread,lastZ,z);

            for(auto &curThread:threads)
            {
                curThread.join();
            }

            minmaxDataSample.emplace_back(vvvpTT());
            minmaxDataSample.back().swap(curMinMaxData);
            while (1)
            {
                auto &prev = minmaxDataSample.back();
                int zSize = prev.size();
                int ySize = prev[0].size();
                int xSize = prev[0][0].size();
                int newZSize = (zSize + 1) / 2;
                int newYSize = (ySize + 1) / 2;
                int newXSize = (xSize + 1) / 2;

                if (zSize == 1 && ySize == 1 && xSize == 1)
                    break;

                curMinMaxData.resize(newZSize, vvpTT(newYSize, vpTT(newXSize, make_pair(allMin, allMax))));

                if(xSize*ySize*zSize<256)
                {
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
                }
                else
                {
                    int noOfThreads = 4;
                    int workPerThread=zSize/(noOfThreads);
                    int curThread=0,curZ=0,lastZ=workPerThread*(noOfThreads);

                    auto threadHandler = [&](int threadi,int zb,int ze){
                        for (int i = zb; i < ze; i++)
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
                    };

                    vector<std::thread> threads;

                    while(curZ<lastZ)
                    {
                        std::thread a(threadHandler,curThread,curZ,curZ+workPerThread);
                        threads.push_back(move(a));
                        curZ+=workPerThread;
                        curThread++;
                    }
                    threadHandler(curThread,lastZ,zSize);

                    for(auto &curThread:threads)
                    {
                        curThread.join();
                    }
                }



                minmaxDataSample.emplace_back(vvvpTT());
                minmaxDataSample.back().swap(curMinMaxData);
            }
        }

        // Build the minmax data
        void build(int inr_count,vector<vvvpTT> &minmaxDataSample)
        {
            /*
            *  If Float is preferred then use below
            *
            *  ISODataType allMin = + numeric_limits<ISODataType>::max();
            *  ISODataType allMax = - numeric_limits<ISODataType>::max();
            *
            */

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
            // clear previous contents
            minmaxDataSample.clear();

            ISODataType allMin =   INT_MAX;
            ISODataType allMax =   INT_MIN;
            //vvvpTT curMinMaxData(z - inr_count, vvpTT(y - inr_count, vpTT(x - inr_count)));
            vvvpTT curMinMaxData(ceil(z/(1.0*inr_count))-1, vvpTT(ceil(y/(1.0*inr_count))-1, vpTT(ceil(x/(1.0*inr_count))-1)));
            for (int i = inr_count,ii=0; i < z; i+=inr_count,ii++)
            {
                for (int j = inr_count,jj=0; j < y; j+=inr_count,jj++)
                {
                    for (int k = inr_count,kk=0; k < x; k+=inr_count,kk++)
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
                                cur_min = min((volume)[zz][yy][xx], cur_min);
                                cur_max = max((volume)[zz][yy][xx], cur_max);
                            }
                        }
                        curMinMaxData[ii][jj][kk] = {cur_min, cur_max};
                    }
                }
            }
            minmaxDataSample.emplace_back(vvvpTT());
            minmaxDataSample.back().swap(curMinMaxData);
            while (1)
            {
                auto &prev = minmaxDataSample.back();
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
                minmaxDataSample.emplace_back(vvvpTT());
                minmaxDataSample.back().swap(curMinMaxData);
            }
        }

        // Model Name
        string name;

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

        void loadModel(const string &fileName)
        {
            ifstream file;

            file.open( fileName , ios::in | ios::binary );

            auto properties = parseFileName(fileName);

            getDimensions(properties["dimension"],x,y,z);

            auto &dataType = properties["dataType"];

            if(dataType=="uint8") readModel<unsigned char>(file);
            else if(dataType=="int8") readModel<char>(file);
            else if(dataType=="int16") readModel<short int>(file);
            else if(dataType=="uint16") readModel<unsigned short int>(file);
            else
            {
                cerr<< __FILE__ << " " << __LINE__  << " Add Data in File\n";
                exit(-1);
            }

            file.close();
        }

        template<typename T>
        void readModel(ifstream &file)
        {
            /*
             *  if instead of vvvf if vf is used then we can read directly
             *  copy(istream_iterator<T>(file),istream_iterator<T>(),back_inserter(volume));
             */

            volume.clear();

            T data;

            // reserve to avoid resizing

            volume.reserve(z);

            for(int k=0;k<z;k++)
            {
                vvT mat;
                volume.push_back(mat);
                mat.reserve(y);
                for(int i=0;i<y;i++)
                {
                    vT row;
                    mat.push_back(row);
                    row.reserve(x);
                    for(int j=0;j<x;j++)
                    {
                        file.read((char*)&data,sizeof (data));
                        row.push_back(data);
                    }
                    mat.back().swap(row);
                }
                volume.back().swap(mat);
            }
        }
        
        map<int,ISOSurface> isoSurfaces;

    public:
        Shader shader;
        GLenum mode;
        bool shouldUseCommonShader;
        

        // Sampling
        int isoSkipValue = 4;
        glm::mat4 World;
        glm::vec3 ambientColor, diffuseColor, specularColor, lightPosition, viewPosition;

        RawModel(const string &modelName="myModel")
        {
            mode = GL_TRIANGLES;
            name = modelName;
            shouldUseCommonShader = true;
        }
        RawModel(const string &modelName,const string &modelPath,bool useCommonShader = true)
        {
            create(modelName,modelPath,useCommonShader);
        }
        void create(const string &modelName,const string &modelPath,bool useCommonShader = true)
        {
            name = modelName;
            shader = mainShader;
            loadModel(modelPath);
            {
                auto start = high_resolution_clock::now();
                buildMultithreaded(1,minmaxData);
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(stop - start);
                cout << "Time Taken for build : " << duration.count() << endl;
            }
            {
                auto start = high_resolution_clock::now();
                buildMultithreaded(isoSkipValue,minmaxDataSample);
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(stop - start);
                cout << "Time Taken for build Sample : " << duration.count() << endl;
            }
            
            
        }

        void render(function<void(Shader&)> f=[](Shader &shader){})
        {
            shader.bind();

            f(shader);

            for(auto &surfacePair:isoSurfaces)
            {
                auto &surface = surfacePair.second;

                /*
                 *  For each property of ISOSurface That needs to be passed to mainShader
                 */

                shader.setUniform1i("opacity",surface.opacity);



                if(surface.shouldDisplay)
                    surface.render();
            }

            shader.unbind();
        }

        ISOSurface* getISOSurface(int i)
        {
            auto it = isoSurfaces.find(i);
            if(it!=isoSurfaces.end()) return &(it->second);
            return 0;
        }

        void addISOSurface(int i,float ISOValueIn = 0 ,bool useISOValueIn=false)
        {
            auto it = isoSurfaces.find(i);
            if(it!=isoSurfaces.end()) return;
            isoSurfaces[i] = ISOSurface(volume,minmaxData,minmaxDataSample,ISOValueIn,useISOValueIn,isoSkipValue);
        }

        void removeISOSurface(int i)
        {
            auto it = isoSurfaces.find(i);
            if(it==isoSurfaces.end()) return;
            isoSurfaces.erase(it);
        }

        ~RawModel()
        {

        }
};
