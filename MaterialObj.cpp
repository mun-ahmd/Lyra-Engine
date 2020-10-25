#include "MaterialObj.h"
#include "stb_image.h"
#include <iostream>
#include "ManyFunctions.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glad/glad.h>






unsigned int Material::createTextureMap(float value, MATERIAL_TYPE_INTERNAL type)
{
    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrComponents;
    float data[1] = { value };

    nrComponents = 1;
    width = 1; height = 1;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, data);

    //glGenerateMipmap(GL_TEXTURE_2D);  //no use in creating mipmaps for a 1x1 texture

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;

}



unsigned int Material::createTextureMap(glm::vec3 diffuseIN, MATERIAL_TYPE_INTERNAL type)
{
    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrComponents;
    float data[3] = { diffuseIN.x , diffuseIN.y , diffuseIN.z };

    nrComponents = 1;
    width = 1; height = 1;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, data);

    //glGenerateMipmap(GL_TEXTURE_2D);  //no use in creating mipmaps for a 1x1 texture

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;

}


unsigned int Material::createTextureMap(std::string path, MATERIAL_TYPE_INTERNAL type)
{

    unsigned int textureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);


    int width, height, nrComponents;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format;
        GLenum outFormat;
        //this will cause problems when I need to send alpha values as well to the texture obj

        if (type == MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL)
        {
            if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            outFormat = GL_RGB;
        }

        else if (type == MATERIAL_TYPE_INTERNAL::MATERIAL_TRANSPARANCY_INTERNAL)
        {
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 2)
                format = GL_RG;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            else
            {
                std::cout << "INVALID TEXTURE, NOT LOADED";
                //TODO make it default or smthn
            }
            outFormat = GL_ALPHA;
        }
        else
        {
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 2)
                format = GL_RG;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            else
            {
                std::cout << "INVALID TEXTURE, NOT LOADED";
                format = NULL;
                //TODO make it default or smthn
            }
            outFormat = GL_RGB;
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
        std::cout << "Texture failed to load at path: " << path << std::endl << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}


unsigned int Material::defaultTransparant()
{
    unsigned int texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    float data[1] = { 1.0f };

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED, GL_FLOAT, data);

    //glGenerateMipmap(GL_TEXTURE_2D);  //no use in creating mipmaps for a 1x1 texture

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}


void Material::cleanupTexture(unsigned int* Texture)
{
    glDeleteTextures(1, Texture);
    *Texture = NULL;
    //will check if texture is null before sending
    //if it is null, make sure default is made
}


Material::Material(int Index)
{
	matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(glm::vec3(0.5), MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(0.5, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(0.25, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(0.5, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}

Material::Material(glm::vec3 diffuseIN, float specularIN, float roughnessIN, float metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(std::string diffuseIN, std::string specularIN, std::string roughnessIN, std::string metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN,MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN,MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}

Material::Material(std::string diffuseIN, std::string specularIN, std::string roughnessIN, float metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(std::string diffuseIN, std::string specularIN, float roughnessIN, std::string metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(std::string diffuseIN, float specularIN, std::string roughnessIN, std::string metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(glm::vec3 diffuseIN, std::string specularIN, std::string roughnessIN, std::string metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}

Material::Material(glm::vec3 diffuseIN, float specularIN, std::string roughnessIN, std::string metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(glm::vec3 diffuseIN, std::string specularIN, float roughnessIN, std::string metallicIN, int Index)
{   
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(glm::vec3 diffuseIN, std::string specularIN, std::string roughnessIN, float metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(std::string diffuseIN, float specularIN, float roughnessIN, std::string metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(std::string diffuseIN, std::string specularIN, float roughnessIN, float metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(std::string diffuseIN, float specularIN, std::string roughnessIN, float metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}

Material::Material(glm::vec3 diffuseIN, float specularIN, float roughnessIN, std::string metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(glm::vec3 diffuseIN, float specularIN, std::string roughnessIN, float metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(glm::vec3 diffuseIN, std::string specularIN, float roughnessIN, float metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
Material::Material(std::string diffuseIN, float specularIN, float roughnessIN, float metallicIN, int Index)
{
    matIndex = Index;

    transparancy = defaultTransparant();
    diffuse = createTextureMap(diffuseIN, MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
    specular = createTextureMap(specularIN, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
    metallic = createTextureMap(metallicIN, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
    roughness = createTextureMap(roughnessIN, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}



void Material::change_diffuse(glm::vec3 new_diffuse)
{
    cleanupTexture(&this->diffuse);
    this->diffuse = createTextureMap(new_diffuse);
}
void Material::change_diffuse(std::string new_diffuse_path)
{
    cleanupTexture(&this->diffuse);
    this->diffuse = createTextureMap(new_diffuse_path,MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
}
void Material::change_specular(float new_specular)
{
    cleanupTexture(&this->specular);
    this->specular = createTextureMap(new_specular);
}
void Material::change_specular(std::string new_specular_path) 
{
    cleanupTexture(&this->specular);
    this->specular = createTextureMap(new_specular_path, MATERIAL_TYPE_INTERNAL::MATERIAL_SPECULAR_INTERNAL);
}
void Material::change_metallic(float new_metallic)
{
    cleanupTexture(&this->metallic);
    this->metallic= createTextureMap(new_metallic);
}
void Material::change_metallic(std::string new_metallic_path)
{
    cleanupTexture(&this->metallic);
    this->metallic = createTextureMap(new_metallic_path, MATERIAL_TYPE_INTERNAL::MATERIAL_METALLIC_INTERNAL);
}
void Material::change_roughness(float new_roughness)
{
    cleanupTexture(&this->roughness);
    this->roughness = createTextureMap(new_roughness);
}
void Material::change_roughness(std::string new_roughness_path)
{
    cleanupTexture(&this->roughness);
    this->roughness = createTextureMap(new_roughness_path, MATERIAL_TYPE_INTERNAL::MATERIAL_ROUGHNESS_INTERNAL);
}
void Material::change_transparent(glm::vec3 new_transparent)
{
    cleanupTexture(&this->transparancy);
    this->transparancy = createTextureMap(new_transparent);
}
void Material::change_transparent(std::string new_transparent_path)
{
    cleanupTexture(&this->transparancy);
    this->transparancy = createTextureMap(new_transparent_path,MATERIAL_TYPE_INTERNAL::MATERIAL_TRANSPARANCY_INTERNAL);
}
