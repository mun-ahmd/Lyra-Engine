#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shaderObj.h"
class Light
{
public:
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	Light() {};
	Light(glm::vec3 lightPos, glm::vec3 lightColor, float ambVal, float diffVal, float specVal);
	void setIntensity(float intensity)
	{
		this->ambient *= 0.25 * intensity;
		this->diffuse *= 0.4 * intensity;
		this->specular *= 0.15 * intensity;
	}
	virtual void addToShader(Shader& shader, std::string nameInUniform) = 0;

};

class DirectionalLight: public Light
{
public:
	glm::vec3 direction;

	DirectionalLight(glm::vec3 lightDir, glm::vec3 lightColor, float ambVal, float diffVal, float specVal);
	DirectionalLight(glm::vec3 lightDir, glm::vec3 lightColor);
	void addToShader(Shader& shader, std::string nameInUniform);
};

class PointLight: public Light
{
public:
	float constant;
	float linear;
	float quadratic;

	PointLight(glm::vec3 lightPos, glm::vec3 lightColor, float ambVal, float diffVal, float specVal, float attenConstant, float attenLinear, float attenQuadratic);
	PointLight(glm::vec3 lightPos, glm::vec3 lightColor, float ambVal, float diffVal, float specVal);
	PointLight(glm::vec3 lightPos, glm::vec3 lightColor);
	void addToShader(Shader& shader, std::string nameInUniform);

};

class Spotlight: public Light
{
public:
	glm::vec3 direction;
	
	float cutoff;
	float outerCutoff;

	float constant;
	float linear;
	float quadratic;

    Spotlight(glm::vec3 lightPos, glm::vec3 lightDir, glm::vec3 lightColor, float cutoffInDegrees, float outerCutoffInDegrees, float ambVal, float diffVal , float specVal, float constant,float linear, float quadratic);
	Spotlight(glm::vec3 lightPos, glm::vec3 lightDir, glm::vec3 lightColor, float cutoffInDegrees, float outerCutoffInDegrees, float ambVal, float diffVal, float specVal);
	Spotlight(glm::vec3 lightPos, glm::vec3 lightDir, glm::vec3 lightColor, float cutoffInDegrees, float outerCutoffInDegrees);
	void addToShader(Shader& shader, std::string nameInUniform);

};


