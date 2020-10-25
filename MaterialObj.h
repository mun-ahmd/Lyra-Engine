#pragma once
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


constexpr auto NO_MATERIAL_TEXTURE_PATH = R"(\%nopath%\)";

constexpr auto MATERIAL_MAX_SHININESS = 1.0f;
constexpr auto MATERIAL_MIN_SHININESS = 0.0f;

constexpr auto MATERIAL_MAX_METALLIC = 1.0f;
constexpr auto MATERIAL_MIN_METALLIC = 0.0f;

constexpr auto MATERIAL_MAX_SPECULAR = 1.0f;
constexpr auto MATERIAL_MIN_SPECULAR = 0.0f;

enum class MATERIAL_TYPE_INTERNAL
{
	MATERIAL_DIFFUSE_INTERNAL,
	MATERIAL_TRANSPARANCY_INTERNAL,
	MATERIAL_SPECULAR_INTERNAL,
	MATERIAL_METALLIC_INTERNAL,
	MATERIAL_ROUGHNESS_INTERNAL,
	MATERIAL_ANY_INTERNAL
};

class Material
{
private:
	unsigned int matIndex;
	unsigned int diffuse;
	unsigned int specular;
	unsigned int roughness;
	unsigned int metallic;
	unsigned int transparancy;

	unsigned int defaultTransparant();

	//unsigned int createTextureMap(float value);

	//this second one is used instead of first because I did not want to type the same thing 16 times
	unsigned int createTextureMap(float value,MATERIAL_TYPE_INTERNAL type = MATERIAL_TYPE_INTERNAL::MATERIAL_ANY_INTERNAL);
	unsigned int createTextureMap(glm::vec3 diffuseIN, MATERIAL_TYPE_INTERNAL type = MATERIAL_TYPE_INTERNAL::MATERIAL_DIFFUSE_INTERNAL);
	unsigned int createTextureMap(std::string path, MATERIAL_TYPE_INTERNAL type);

	void cleanupTexture(unsigned int* texture);

public:

	Material(int Index);

	Material(glm::vec3 diffuseIN, float specularIN, float roughnessIN, float metallicIN, int Index);
	Material(std::string diffuseIN, std::string specularIN, std::string roughnessIN, std::string metallicIN, int Index);

	Material(std::string diffuseIN, std::string specularIN, std::string roughnessIN, float metallicIN, int Index);
	Material(std::string diffuseIN, std::string specularIN, float roughnessIN, std::string metallicIN, int Index);
	Material(std::string diffuseIN, float specularIN, std::string roughnessIN, std::string metallicIN, int Index);
	Material(glm::vec3 diffuseIN, std::string specularIN, std::string roughnessIN, std::string metallicIN, int Index);

	Material(glm::vec3 diffuseIN, float specularIN, std::string roughnessIN, std::string metallicIN, int Index);
	Material(glm::vec3 diffuseIN, std::string specularIN, float roughnessIN, std::string metallicIN, int Index);
	Material(glm::vec3 diffuseIN, std::string specularIN, std::string roughnessIN, float metallicIN, int Index);
	Material(std::string diffuseIN, float specularIN, float roughnessIN, std::string metallicIN, int Index);
	Material(std::string diffuseIN, std::string specularIN, float roughnessIN, float metallicIN, int Index);
	Material(std::string diffuseIN, float specularIN, std::string roughnessIN, float metallicIN, int Index);

	Material(glm::vec3 diffuseIN, float specularIN, float roughnessIN, std::string metallicIN, int Index);
	Material(glm::vec3 diffuseIN, float specularIN, std::string roughnessIN, float metallicIN, int Index);
	Material(glm::vec3 diffuseIN, std::string specularIN, float roughnessIN, float metallicIN, int Index);
	Material(std::string diffuseIN, float specularIN, float roughnessIN, float metallicIN, int Index);


	
	//TODO do not forget to delete old textures when changing these
	//good ol' memory leak that is
	void change_diffuse(glm::vec3 new_diffuse);
	void change_diffuse(std::string new_diffuse_path);

	void change_specular(float new_specular);
	void change_specular(std::string new_specular_path);

	void change_metallic(float new_metallic);
	void change_metallic(std::string new_metallic_path);

	void change_roughness(float new_roughness);
	void change_roughness(std::string new_roughness_path);

	void change_transparent(glm::vec3 new_transparent);
	void change_transparent(std::string new_transparent_path);

};

