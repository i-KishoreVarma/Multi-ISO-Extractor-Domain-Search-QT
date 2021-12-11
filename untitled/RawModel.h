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

#define TIME_NOW std::chrono::high_resolution_clock::now()
#define TIME_DIFF(gran, start, end) std::chrono::duration_cast<gran>(end - start).count()

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
        
        glm::mat4 World;

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
            shouldUseCommonShader = useCommonShader;
            if(shouldUseCommonShader) shader = commonShader;
            loadModel(modelPath);
        }

        void render(function<void(Shader&)> f=[](Shader &shader){})
        {
            shader.bind();
            shader.setUniformMat4f("gWorld",World);
            for(auto &surfacePair:isoSurfaces)
            {
                auto &surface = surfacePair.second;
                shader.setUniform1i("opacity",surface.opacity);
                if(surface.shouldDisplay)
                    surface.render();
            }
            shader.unbind();
        }

        pTT getMinMaxValues()
        {
            return {0,255};
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
            isoSurfaces[i] = ISOSurface(volume,ISOValueIn,useISOValueIn);
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
