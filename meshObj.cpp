#include "meshObj.h"
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include "Model_Importer.h"
#include "Node.h"


//memory layout of Vertex struct would be like:
// = [0.2f, 0.4f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f];
//so sizeof and offsetof make it easy to setup buffer

/*
unsigned int createCubeMap(std::vector<std::string> faces, std::string directory)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);



    int width, height, nrChannels;
    int squareSide;


    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load((directory + R"(\)" + faces[i]).c_str(), &width, &height, &nrChannels, 0);
        if (width > height)
            squareSide = height;
        else
            squareSide = width;

        if (data)
        {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            else
                std::cout << directory << ": format does not match any ";

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, squareSide, squareSide, 0, format, GL_UNSIGNED_BYTE, data
                //the reason I have used width, width over here is that cubemaps require square images
                //i.e width and height must be equal
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

int texturesCounter = 0;
void create1by1Tex(Texture& texture ,aiColor4D color) 
{
    glGenTextures(1, &texture.id);
    std::cout << color.r;
    glm::vec4 colorUsable(color.r, color.g, color.b, color.a);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(false);
    float data[4] = { colorUsable.x , colorUsable.y , colorUsable.z,colorUsable.w };

    nrComponents = 4;
    width = 1; height = 1;

    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    

}



void create1by1Tex1Channel(Texture& texture, aiColor3D color)
{
    glGenTextures(1, &texture.id);
    std::cout << color.r;
    float data= 0.25f *(color.r+color.g+ color.b);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(false);

    nrComponents = 1;
    width = 1; height = 1;

    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGBA, GL_FLOAT, &data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}
*/
unsigned int TextureFromFile(const char* file, const std::string& directory , std::string type)
{
    std::string filename = std::string(file);
    filename = directory + R"(\)" + filename;

    std::cout << filename;

    unsigned int textureID = 0;
    glCreateTextures(GL_TEXTURE_2D , 1, &textureID);


    int width, height, nrComponents;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        GLenum outFormat;
        //this will cause problems when I need to send alpha values as well to the texture obj

        if (type == "texture_diffuse") {

            if (nrComponents == 1) {
                format = GL_RED;
                outFormat = GL_RED;
            }
            else if (nrComponents == 3) {
                format = GL_SRGB;
                outFormat = GL_RGB;
            }
            else if (nrComponents == 4) {
                format = GL_SRGB_ALPHA;
                outFormat = GL_RGBA;
            }
            else
                std::cout << file<< ": format does not match any ";
        }
        else {
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            else
                std::cout << file << ": format does not match any ";
            outFormat = format;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, outFormat, GL_UNSIGNED_BYTE, data);
        

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        
        glGenerateMipmap(GL_TEXTURE_2D);

    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}




using namespace std;



Mesh::Mesh()
{
    //dangerous constructor
    shininess = 0.0f;
    material_index = 0;
}

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int materialIndex)
{
    this->vertices = vertices;
    this->indices = indices;
    this->material_index = materialIndex;
    setupMesh();
}

Mesh::~Mesh()
{
    unsigned int buffers[2]= { VBO, EBO };
    glDeleteBuffers(2, &buffers[0]);
    glDeleteVertexArrays(1, &VAO);
    VAO, VBO, EBO = NULL;
}

Mesh::Mesh(const Mesh& other)
{
    VAO = other.VAO;
    VBO = other.VBO;
    EBO = other.EBO;
    this->vertices = other.vertices;
    this->indices = other.indices;
    this->material_index = other.material_index;
}

Mesh& Mesh::operator=(const Mesh& other)
{
    VAO = other.VAO;
    VBO = other.VBO;
    EBO = other.EBO;
    this->vertices = other.vertices;
    this->indices = other.indices;
    this->material_index = other.material_index;
    return *this;
}


void Mesh::setupMesh()
{


    if (indices.size() == 1) {
        indices.pop_back();
        for (unsigned int index = 0;index < vertices.size();++index) {
            indices.push_back(index);
        }
    }


    glCreateVertexArrays(1, &VAO);  //VAO = Vertex Array Object
    glCreateBuffers(1, &VBO);       //VBO = Vertex Buffer Object
    glCreateBuffers(1, &EBO);       //EBO = Element Buffer Object

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),        //indices.size(number of indices) * unsigned int size gives memory required     
        &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));       //offsetof will tellhow much data is between start of Vertex obj and Normal member
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));    //same as above



    glBindVertexArray(0);
}

void Mesh::draw()
{
    //assume material for this has already been binded
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0);
    //potentially dangerous
}

void Mesh::draw(Shader& shader)
{
    //assume material for this has already been binded
    // draw mesh
    glUseProgram(shader.program);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0);
    //potentially dangerous
}


int Mesh::getL3D_size()
{
    int totalSizeOfMesh = sizeof(this->vertices)*sizeof(Vertex) + sizeof(this->indices)*sizeof(int) + sizeof(int)*3;   // no_of_vertices, no_of indices, vertices, indices,  material_indez
    /*
    for (int i = 0; i < textures.size(); ++i)
    {
        textures[i].filePath.shrink_to_fit();
        totalSizeOfMesh += sizeof(textures[i].filePath);
    }
    */
    return totalSizeOfMesh;
}

void Mesh::writeToL3D(std::ofstream& file)
{
    //Expect that file will be opened as binary


    int verticesSize = this->vertices.size() * sizeof(Vertex) ;
    int indicesSize = this->indices.size() * sizeof(unsigned int);

    file.write((char*)&verticesSize, sizeof(int));
    file.write((char*)&indicesSize, sizeof(int));


    file.write((char*)&this->vertices[0], verticesSize);
    file.write((char*)&this->indices[0], indicesSize);

    //TODO AFTER CREATING THE MATERIAL SYSTEM ADD THE MATERIAL NUMBER HERE AND REMOVE THE CURRENT THING WITH TEXTURES AND SHININESS

    /*
    for (int i = 0; i < this->textures.size(); ++i)
    {
        const char* texPath = textures[i].filePath.c_str();
        file.write((char*)texPath, sizeof(*texPath));
    }
    */
    file.write((char*)&this->material_index, sizeof(int));
}

void Mesh::drawGeometryPass(Shader& shader, Material_Manager *matmanager)
{
    //Ensure that proper gBuffer setup has been done before this!
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, (*matmanager)[material_index]->getdiffuse());
    shader.addUniform1I("texture_diffuse1", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, (*matmanager)[material_index]->getspecular());
    shader.addUniform1I("texture_specular1", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



//Model Functions



//Draws all meshes in the model
void Model::Draw(Shader& shader) {
    for (unsigned int i = 0;i < meshes.size();i++) {
        meshes[i]->draw(shader);
    }
}

void Model::drawDeferredFirstPass(Shader& shader, Material_Manager *matmanager)
{
    for (unsigned int i = 0;i < meshes.size();i++) {
        meshes[i]->drawGeometryPass(shader,matmanager);
    }
}


/*
void Model::loadModel_L3D(std::string path)
{
    std::ifstream file;
    file.open(path, std::ios::binary | std::ios::in);
    char signature[2];
    file.read(&signature[0], 2);
    if (signature != "LY")
    {
        //TODO error
    }
    unsigned int no_of_meshes;
    file.read((char*)&no_of_meshes, sizeof(unsigned int));

    std::vector<unsigned int> mesh_offsets(no_of_meshes);
    file.read((char*)&mesh_offsets[0], no_of_meshes * sizeof(unsigned int));


    unsigned int header_size = 2 + mesh_offsets.size() * sizeof(unsigned int)+ sizeof(unsigned int) ;

    std::vector<std::thread> threads;
    threads.reserve(no_of_meshes);
    std::vector<Mesh_Importer > meshImporters(no_of_meshes);


    
    for (int i = 0; i < no_of_meshes; ++i)
    {
        meshImporters.push_back(Mesh_Importer());
        threads.push_back(std::thread(std::ref(meshImporters[i]), path, mesh_offsets[i]));
    }
    this->meshes.reserve(no_of_meshes);
    for (int i = 0; i < no_of_meshes; ++i)
    {
        if(threads[i].joinable())
            threads[i].join();
        this->meshes.push_back(meshImporters[i].mesh);
    }
    
}
*/



struct meshANDmat
{
    Mesh mesh;
    Material* mat;
};


std::vector<Mesh*> processNode(aiNode* node, const aiScene* scene);
std::vector<unsigned int> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
Mesh* processMesh(aiMesh* mesh, const aiScene* scene);

void Model::loadModel(std::string path, Material_Manager* matManager)
{
    Assimp::Importer import;
    //the below does most of the loading work, aside from path the rest arguements are optional postprocessing, here I tell it I want it in triangles and textures flipped on y-axis
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate);//| aiProcess_FlipUVs);

    //if there is an error in loading model, this will let you know
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of(R"(\)")); 
    //fileName = path.substr(path.find_last_of(R"(\)", path.find_last_of(".")));
    //directory is used to get texture locations for the model
    //NOT CURRENTLY^^^
    this->meshes = processNode(scene->mRootNode, scene);

    
}

std::vector<Mesh*> processNode(aiNode* node, const aiScene* scene)
{
    std::vector<Mesh*> meshes;
    meshes.reserve(node->mNumMeshes);
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));         
        //processMesh will finally turn this data into our class
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        std::vector<Mesh*> new_meshes = processNode(node->mChildren[i], scene);
        for (unsigned int i = 0; i < new_meshes.size();++i)
        {
            meshes.push_back(new_meshes[i]);
        }
    }
    return meshes;
}


std::vector<unsigned int> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    //todo
    //redo this whole thing
    std::vector<unsigned int> yes = { 0 };
    return yes;
}


Mesh* processMesh(aiMesh* mesh, const aiScene* scene)
{
    //right now defaults material to basic mat
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces*3);
    

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions
        glm::vec3 vectorA;
        vectorA.x = mesh->mVertices[i].x;
        vectorA.y = mesh->mVertices[i].y;
        vectorA.z = mesh->mVertices[i].z;
        //process vertex normals

        glm::vec3 vectorB;
        if (mesh->HasNormals())
        {
            vectorB.x = mesh->mNormals[i].x;
            vectorB.y = mesh->mNormals[i].y;
            vectorB.z = mesh->mNormals[i].z;
        }
        else
        {
            vectorB = glm::vec3(1.0, 0.0, 0.0);
        }
        //process vertex texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        vertex.Normal = vectorB;
        vertex.Position = vectorA;

        //add this vertex to the vertices structure
        vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        //assimp stores meshes as an array of faces and each face data structure has the order in which its vertices need to be drawn
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    /*
    // process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<unsigned int> diffuseMaps = loadMaterialTextures(material,
            aiTextureType_DIFFUSE, "texture_diffuse");
        //textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<unsigned int> specularMaps = loadMaterialTextures(material,
            aiTextureType_SPECULAR, "texture_specular");
        //textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        //get shininess for specular lighting
        material->Get(AI_MATKEY_SHININESS, shininessValue);
    }
    std::cout << "og shininess" << shininessValue;
    
    unsigned int spec;
    unsigned int diffuse;
    //todo no mat rn


    Material* currMat = new Material(0,0,0,0,true);
    matManager->addMaterial(currMat);
    
    std::vector<Texture> textures1 = {};
    */

    Mesh* final_mesh = new Mesh(vertices, indices, 0);  //using 0th material for now
    return final_mesh;
}

void Model::addNodeToMeshes(Node* node)
{
    for (unsigned int i = 0; i < meshes.size(); ++i)
    {
        meshes[i]->addNodeUsing(node);
    }
}

void Model::Cleanup(Shader& shader) {
    for (unsigned int i = 0;i < meshes.size(); i++) {
        meshes[i]->~Mesh();
    }
}

Model::~Model()
{
    for (unsigned int i = 0;i < meshes.size(); i++) {
        meshes[i]->~Mesh();
    }
}

std::vector<Mesh*>* Model::getMeshes()
{
    return &this->meshes;
}

/*
void Model::storeToFastLoad() {
    if (modelLoaded != true) {
        std::cout << "Model Not Loaded Yet";
        return;
    }

    std::ofstream fastLoadFile;
    fastLoadFile.open(R"(FastLoading\test2.lyra)", std::ios::out);
    for (int i = 0; i < Model::meshes.size(); ++i) 
    {
        meshToLyraStore(*Model::meshes[i], &fastLoadFile);
    }
}
*/

/*
void Model::storeModelToL3D(std::string filePath)
{
    std::ofstream file;
    file.open(filePath, std::ios::binary | std::ios::out);

    const char signature[2] = {'L','Y'};
    file.write(&signature[0], 2);

    int no_of_meshes = this->meshes.size();
    file.write((char*)&no_of_meshes, sizeof(int));

    int header_size = 2 + sizeof(int) ;
    file.write((char*)& header_size, sizeof(int));
    for (int i = 1; i < no_of_meshes; ++ i)
    {
        int mesh_size = header_size+ this->meshes[i-1]->getL3D_size();
        file.write((char*)&mesh_size, sizeof(int));
    }

    for (int i = 0; i < this->meshes.size(); ++i)
    {
        int meshSize = this->meshes[i]->getL3D_size();
        file.write((char*)&meshSize, sizeof(int));
    }

    for (int i = 0; i < this->meshes.size(); ++i)
    {
        this->meshes[i]->writeToL3D(file);
    }

    file.close();
}
*/
/*
std::string ImportModel(std::string path, Material_Manager* matmanager)
{
    Assimp::Importer import;
    //the below does most of the loading work, aside from path the rest arguements are optional postprocessing, here I tell it I want it in triangles and textures flipped on y-axis
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate);//| aiProcess_FlipUVs);

    //if there is an error in loading model, this will let you know
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    //..Process Nodes
    processNode(scene->mRootNode, scene, matmanager);
}
*/