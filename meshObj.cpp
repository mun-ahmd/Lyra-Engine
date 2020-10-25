#include "meshObj.h"
#include <string>
#include <iostream>
#include <fstream>


//memory layout of Vertex struct would be like:
// = [0.2f, 0.4f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f];
//so sizeof and offsetof make it easy to setup buffer

void meshToLyraStore(Mesh& mesh, std::ostream* data_file) {
    std::ostream_iterator<float> vertex_data_out(*data_file, ",");
    std::ostream_iterator<unsigned int> index_data_out(*data_file, ",");
    std::ostream_iterator<std::string> tex_data_out(*data_file, ",");
    for (int i = 0; i < mesh.vertices.size();++i)
    {
        Vertex vertex = mesh.vertices[i];
        std::vector<float> vertexDataToStore = { vertex.Position.x , vertex.Position.y , vertex.Position.z, vertex.Normal.x, vertex.Normal.y , vertex.Normal.z , vertex.TexCoords.x, vertex.TexCoords.y };
        std::copy(vertexDataToStore.begin(), vertexDataToStore.end(), vertex_data_out);
        //*data_file << ";";
    }
    *data_file << "\t";
    std::copy(mesh.indices.begin(), mesh.indices.end(), index_data_out);
    *data_file << "\t";
    std::vector<std::string> texturesLoc;
    for (int z = 0; z < mesh.textures.size(); ++z) 
    {
        texturesLoc.push_back(mesh.textures[z].filePath);
    }
    std::copy(texturesLoc.begin(), texturesLoc.end(), tex_data_out);
    *data_file << "\t" << mesh.shininess;
    *data_file << "\n";

}

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
void create1by1Tex(Texture& texture ,aiColor4D color) {
    glGenTextures(1, &texture.id);
    std::cout << color.r;
    glm::vec4 colorUsable(color.r, color.g, color.b, color.a);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(false);
    float data[4] = { colorUsable.x , colorUsable.y , colorUsable.z,colorUsable.w };

    texture.filePath = R"(FastLoading\texture)" + std::to_string(texturesCounter) + ".lyratex";
    std::ofstream fileToStore; fileToStore.open(texture.filePath, std::ios::out);
    for (int i = 0;i < 4;++i) {
        fileToStore << data[i];
    }
    fileToStore.close();

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
void Mesh::setupMesh()
{

    if (textures.size() == 0) {
        Texture defaultTex;
        defaultTex.type = "texture_diffuse";
        defaultTex.id = TextureFromFile("default_white.png", "Images");
        textures.push_back(defaultTex);        
        defaultTex.type = "texture_specular";
        textures.push_back(defaultTex);
    }


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

int Mesh::getL3D_size()
{
    int totalSizeOfMesh = sizeof(this->vertices) + sizeof(this->indices);
    for (int i = 0; i < textures.size(); ++i)
    {
        textures[i].filePath.shrink_to_fit();
        totalSizeOfMesh += sizeof(textures[i].filePath);
    }
    return totalSizeOfMesh;
}

void Mesh::writeToL3D(std::ofstream& file)
{
    //Expect that file will be opened as binary


    int verticesSize = this->vertices.size() * sizeof(Vertex) ;
    int indicesSize = this->indices.size() * sizeof(unsigned int);
    int texturesSize = 0;
    for (int i = 0; i < this->textures.size(); ++i)
    {
        texturesSize += sizeof(textures[i].filePath);
    }

    file.write((char*)&verticesSize, sizeof(int));
    file.write((char*)&indicesSize, sizeof(int));
    file.write((char*)&texturesSize, sizeof(int));


    file.write((char*)&this->vertices[0], verticesSize);
    file.write((char*)&this->indices[0], indicesSize);

    //TODO AFTER CREATING THE MATERIAL SYSTEM ADD THE MATERIAL NUMBER HERE AND REMOVE THE CURRENT THING WITH TEXTURES AND SHININESS

    for (int i = 0; i < this->textures.size(); ++i)
    {
        const char* texPath = textures[i].filePath.c_str();
        file.write((char*)texPath, sizeof(*texPath));
    }

    file.write((char*)&this->shininess, sizeof(int));
}



void Mesh::draw(Shader& shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++) {
        //reserve first texture unit for shadows
        glActiveTexture(GL_TEXTURE1 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        }
        else if (name == "texture_specular") {
            number = std::to_string(specularNr++);
        }
        else {
            std::cout << "Texture type is unidentified" << std::endl;
            continue;
        }

        shader.addUniform1I(("material." + name + number).c_str(), i+1);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    shader.addUniform1F("material.shininess", Mesh::shininess);


    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::drawGeometryPass(Shader& shader)
{
    //Ensure that proper gBuffer setup has been done before this!
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0].id);
    shader.addUniform1I("texture_diffuse1", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1].id);
    shader.addUniform1I("texture_specular1", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, float shininess)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;  
    this->shininess = shininess+0.001;
    setupMesh();
    //vertices.clear();
    //vertices.shrink_to_fit();
}

Mesh::Mesh()
{
    shininess = 0.0f;
    material_index = 0;
}
Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures, unsigned int materialIndex)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
    this->material_index = materialIndex;
}

void Mesh::cleanup(Shader& shader) {
    for (int i = 0; i < textures.size();++i) {
        glDeleteTextures(1, &textures[i].id);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shader.program);
}


//Model Functions

//Draws all meshes in the model
void Model::Draw(Shader& shader) {
    for (unsigned int i = 0;i < meshes.size();i++) {
        meshes[i].draw(shader);
    }
}

void Model::drawDeferredFirstPass(Shader& shader)
{
    for (unsigned int i = 0;i < meshes.size();i++) {
        meshes[i].drawGeometryPass(shader);
    }
}

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
    int no_of_meshes;
    file.read((char*)&no_of_meshes, sizeof(int));
    for (int i = 0; i < no_of_meshes; ++i)
    {
        Mesh newMesh = Mesh()
    }
}

void Model::loadModel(std::string path)
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
    fileName = path.substr(path.find_last_of(R"(\)", path.find_last_of(".")));
    //directory is used to get texture locations for the model
    processNode(scene->mRootNode, scene);

    modelLoaded = true;
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
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
        processNode(node->mChildren[i], scene);
    }
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;


    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].filePath.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), R"(3DModelData\rocks\)",typeName);//, this->directory);
            texture.type = typeName;
            texture.filePath = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
        }
    }

    if (mat->GetTextureCount(type) == 0) {
        if (type == aiTextureType_DIFFUSE) {
            aiColor4D Color;
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, Color);
            Texture texture;
            texture.type = typeName;
            create1by1Tex(texture,Color);
            textures.push_back(texture);
        }
        else if (type == aiTextureType_SPECULAR) {
            aiColor4D Color;
            mat->Get(AI_MATKEY_COLOR_SPECULAR, Color);
            Texture texture;
            texture.type = typeName;
            create1by1Tex(texture,Color);
            textures.push_back(texture);
        }
    }

    return textures;
}


Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    float shininessValue = 32;

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
    // process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material,
            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        //get shininess for specular lighting
        material->Get(AI_MATKEY_SHININESS, shininessValue);
    }
    std::cout << "og shininess" << shininessValue;
    
    return Mesh(vertices, indices, textures, shininessValue);
}


void Model::Cleanup(Shader& shader) {
    for (unsigned int i = 0;i < meshes.size(); i++) {
        meshes[i].cleanup(shader);
    }
}

void Model::storeToFastLoad() {
    if (modelLoaded != true) {
        std::cout << "Model Not Loaded Yet";
        return;
    }

    std::ofstream fastLoadFile;
    fastLoadFile.open(R"(FastLoading\test2.lyra)", std::ios::out);
    for (int i = 0; i < Model::meshes.size(); ++i) 
    {
        meshToLyraStore(Model::meshes[i], &fastLoadFile);
    }
}


void Model::storeModelToL3D(std::string filePath)
{
    std::ofstream file;
    file.open(filePath, std::ios::binary | std::ios::out);

    const char signature[2] = {'L','Y'};
    file.write(&signature[0], 2);

    int no_of_meshes = this->meshes.size();
    file.write((char*)&no_of_meshes, sizeof(int));

    for (int i = 0; i < this->meshes.size(); ++i)
    {
        int meshSize = this->meshes[i].getL3D_size();
        file.write((char*)&meshSize, sizeof(int));
    }

    for (int i = 0; i < this->meshes.size(); ++i)
    {
        this->meshes[i].writeToL3D(file);
    }

    file.close();
}


