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


unsigned int createCubeMap(std::vector<std::string> faces, std::string directory);
unsigned int TextureFromFile(const char* path, const std::string& directory, std::string type = "");

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string filePath;       //this is to check if the texture is already loaded or not if it comes up again
};

class Mesh
{
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    float shininess;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, float shininess = 32.0f);

    void draw(Shader& shader);
    void drawGeometryPass(Shader& shader);

    void cleanup(Shader& shader);
protected:
    //  render data
    unsigned int VAO, VBO, EBO , nullTexDiffuse , nullTexSpecular;

    void setupMesh();
};




class Model
{
public:
    Model(std::string pathStr)
    {
        loadModel(pathStr);
    }
    void Draw(Shader& shader);
    void drawDeferredFirstPass(Shader& shader);
    void Cleanup(Shader& shader);
    std::string directory;
    std::string fileName;
    bool modelLoaded = false;
    void storeToFastLoad();

private:
    // model data
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded; //to contain all textures already loaded
    void loadModel(std::string path);
    void fastLoadModel(std::string path);   //ALOT of work left on this

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        std::string typeName);
    
    
};

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