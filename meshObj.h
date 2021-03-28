#pragma once
#include <iostream>
#include <vector>
#include "shaderObj.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
//for texture func
#include "ManyFunctions.h"
//include assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

#include "MaterialObj.h"
#include "Material_Manager.h"

#include "Node.h"


unsigned int createCubeMap(std::vector<std::string> faces, std::string directory);
unsigned int TextureFromFile(const char* path, const std::string& directory, std::string type = "");

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh
{
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    float shininess;
    unsigned int material_index = 0;
    unsigned int models_using = 0;  //maybe won't use lets see
     
    Mesh();
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,  unsigned int materialIndex);
    ~Mesh();
    Mesh(const Mesh& other);
    Mesh& operator=(const Mesh& other);

    void draw();
    void draw(Shader& shader);
    /*
    new draw func:
    assume materials have been bound
    if(modelsUsing.size() > 1
        write to uniform buffer their model matrices
        draw them instanced
        //will have to create new instanced draw shader
    else
        draw it normally
    */


    void drawGeometryPass(Shader& shader, Material_Manager *matmanager);

    void addNodeUsing(Node* node)
    {
        this->nodesUsing.push_back(node);
    }
    void removeFromNodesUsing(Node* node)
    {
        std::remove(nodesUsing.begin(), nodesUsing.end(), node);
    }

    int getL3D_size();
    void writeToL3D(std::ofstream& file);
    //TODO
    void readFromL3D(std::ofstream& file);


protected:
    //  render data
    unsigned int VAO, VBO, EBO;
    std::vector<Node*> nodesUsing;

    void setupMesh();
};



//as of now l3d loading is out of date as well as l3d storing
class Model
{
public:
    Model() = default;
    //Model(std::string pathStr, bool l3d)
    //{
    //    loadModel_L3D(pathStr);
    //}
    Model(std::string pathStr, Material_Manager* matManager)
    {
        loadModel(pathStr, matManager);
    }
    ~Model();
    Model(const Model& other);
    Model& operator=(const Model& other);
    void Draw(Shader& shader);
    void drawDeferredFirstPass(Shader& shader,Material_Manager* matManager);
    void Cleanup(Shader& shader);
    std::vector<Mesh*>* getMeshes();
    void addNodeToMeshes(Node* node);
    std::string directory;
private:
    // model data
    void loadModel(std::string path, Material_Manager* matManager);
    std::vector<Mesh*> meshes;
    void loadModel_L3D(std::string path);
};


std::string ImportModel(std::string path,Material_Manager* matmanager);

/*
class Skybox :public Mesh {
public:
    Skybox(std::vector<Vertex> argVertices,std::vector<unsigned int> argIndices,std::vector<Texture> argTextures) :Mesh(argVertices, argIndices, argTextures) {
    }
    Skybox(std::vector<Vertex> argVertices, std::vector<unsigned int> argIndices, std::vector<std::string> faces, std::string directory) :
        Mesh(argVertices, argIndices, Texture::Texture( createCubeMap(faces, directory) ) ) 
    {

    }

    void drawCubemap(Shader& shader);
};
*/