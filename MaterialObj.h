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

enum material_internal
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
	unsigned int matIndex;//1 channel
	unsigned int diffuse;//rgb 3 channels
	unsigned int specular;//1 channel
	unsigned int roughness;//1 channel
	unsigned int metallic;//1 channel
	unsigned int transparancy;//1 channel

	//unsigned int createTextureMap(float value);

	//this second one is used instead of first because I did not want to type the same thing 16 times
	unsigned int createTextureMap(float value, material_internal type = MATERIAL_ANY_INTERNAL);
	unsigned int createTextureMap(glm::vec3 diffuseIN, material_internal type = MATERIAL_DIFFUSE_INTERNAL);
	unsigned int createTextureMap(std::string path, material_internal type);

	void cleanupTexture(unsigned int* texture);

public:

	Material(unsigned int Index);
	Material(glm::vec3 diffuseVal, float specularVal, float roughnessVal, float metallicVal, float transparancyVal, unsigned int Index);
	Material(unsigned int diffuseTex, unsigned int specularTex, unsigned int roughnessTex, unsigned int metallicTex, unsigned int transparancyTex, unsigned int Index, bool zeromeansdefault = false);


	
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

	unsigned int getTexture(int which);		// 0  to 4 in order of textures not including matindex
					//not a good solution, very lazy




	void bind();


	unsigned int getdiffuse()
	{
		return diffuse;
	}
	unsigned int getspecular()
	{
		return specular;
	}
};




static Material Lyra_default_mat = Material(glm::vec3(1.0, 1.0, 1.0), 0.5, 0.0, 0.0,1.0, 0);	//zero index will be reserved for the default texture