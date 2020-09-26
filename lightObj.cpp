#include "lightObj.h"

const float ambValDefault = 0.2f;
const float diffValDefault = 0.5f;
const float specValDefault = 1.0f;

const float attenConstantDefault = 1.0f;
const float attenLinearDefault = 0.09f;
const float attenQuadraticDefault = 0.032f;

const float dirLightPosDistFactor = 10;


char* appendExtentionToName(std::string name, std::string extention) {
	std::string nameWithExtentionStr = name + '.' + extention;
	const int n = nameWithExtentionStr.size();
	char* nameExt = new char[n + 1];

	nameWithExtentionStr.copy(nameExt, n + 1);
	nameExt[nameWithExtentionStr.size()] = '\0';
	return nameExt;
}

Light::Light(glm::vec3 lightPos, glm::vec3 lightColor, float ambVal, float diffVal, float specVal) {
	position = lightPos;
	color = lightColor;
	ambient = glm::vec3(ambVal);
	diffuse = glm::vec3(diffVal);
	specular = glm::vec3(specVal);
}

DirectionalLight::DirectionalLight(glm::vec3 lightDirection, glm::vec3 lightColor, float ambVal, float diffVal, float specVal) {
	direction = lightDirection;
	position = lightDirection * dirLightPosDistFactor;
	color = lightColor;
	ambient = glm::vec3(ambVal);
	diffuse = glm::vec3(diffVal);
	specular = glm::vec3(specVal);
}
DirectionalLight::DirectionalLight(glm::vec3 lightDirection, glm::vec3 lightColor) {
	direction = lightDirection;
	position = lightDirection * dirLightPosDistFactor;
	color = lightColor;
	ambient = glm::vec3(ambValDefault);
	diffuse = glm::vec3(diffValDefault);
	specular = glm::vec3(specValDefault);
}
void DirectionalLight::addToShader(Shader& shader, std::string nameInUniform) 
{
	glUseProgram(shader.program);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "color"), color);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "direction"), direction.x, direction.y, direction.z);
}

PointLight::PointLight(glm::vec3 lightPos, glm::vec3 lightColor, float ambVal, float diffVal, float specVal, float attenConstant, float attenLinear, float attenQuadratic) {
	position = lightPos;
	color = lightColor;

	ambient = glm::vec3(ambVal);
	diffuse = glm::vec3(diffVal);
	specular = glm::vec3(specVal);

	constant = attenConstant;
	linear = attenLinear;
	quadratic = attenQuadratic;
}
PointLight::PointLight(glm::vec3 lightPos, glm::vec3 lightColor, float ambVal, float diffVal, float specVal) {
	position = lightPos;
	color = lightColor;

	ambient = glm::vec3(ambVal);
	diffuse = glm::vec3(diffVal);
	specular = glm::vec3(specVal);

	constant = attenConstantDefault;
	linear = attenLinearDefault;
	quadratic = attenQuadraticDefault;
}
PointLight::PointLight(glm::vec3 lightPos, glm::vec3 lightColor) {
	position = lightPos;
	color = lightColor;

	ambient = glm::vec3(ambValDefault);
	diffuse = glm::vec3(diffValDefault);
	specular = glm::vec3(specValDefault);

	constant = attenConstantDefault;
	linear = attenLinearDefault;
	quadratic = attenQuadraticDefault;
}
void PointLight::addToShader(Shader& shader, std::string nameInUniform) 
{
	glUseProgram(shader.program);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "ambient"), color * ambient);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "diffuse"), color * diffuse);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "specular"), color * specular);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "position"), position.x, position.y, position.z);
	shader.addUniform1F(appendExtentionToName(nameInUniform, "constant"), constant);
	shader.addUniform1F(appendExtentionToName(nameInUniform, "linear"), linear);
	shader.addUniform1F(appendExtentionToName(nameInUniform, "quadratic"), quadratic);
}




Spotlight::Spotlight(glm::vec3 lightPos, glm::vec3 lightDir, glm::vec3 lightColor, float cutoffInDegrees, float outerCutoffInDegrees, float ambVal , float diffVal , float specVal,float attenConstant, float attenLinear, float attenQuadratic) {
	position = lightPos;
	direction = lightDir;
	color = lightColor;

	cutoff = cos(glm::radians(cutoffInDegrees));
	outerCutoff = cos(glm::radians(outerCutoffInDegrees));

	ambient = glm::vec3(ambVal);
	diffuse = glm::vec3(diffVal);
	specular = glm::vec3(specVal);

	constant = attenConstant;
	linear = attenLinear;
	quadratic = attenQuadratic;
}
Spotlight::Spotlight(glm::vec3 lightPos, glm::vec3 lightDir, glm::vec3 lightColor, float cutoffInDegrees, float outerCutoffInDegrees, float ambVal, float diffVal, float specVal) {
	position = lightPos;
	direction = lightDir;
	color = lightColor;

	cutoff = cos(glm::radians(cutoffInDegrees));
	outerCutoff = cos(glm::radians(outerCutoffInDegrees));

	ambient = glm::vec3(ambVal);
	diffuse = glm::vec3(diffVal);
	specular = glm::vec3(specVal);

	constant = attenConstantDefault;
	linear = attenLinearDefault;
	quadratic = attenQuadraticDefault;
}
Spotlight::Spotlight(glm::vec3 lightPos, glm::vec3 lightDir, glm::vec3 lightColor, float cutoffInDegrees, float outerCutoffInDegrees) {
	position = lightPos;
	direction = lightDir;
	color = lightColor;

	cutoff = cos(glm::radians(cutoffInDegrees));
	outerCutoff = cos(glm::radians(outerCutoffInDegrees));

	ambient = glm::vec3(ambValDefault);
	diffuse = glm::vec3(diffValDefault);
	specular = glm::vec3(specValDefault);

	constant = attenConstantDefault;
	linear = attenLinearDefault;
	quadratic = attenQuadraticDefault;
}
void Spotlight::addToShader(Shader& shader, std::string nameInUniform)
{
	glUseProgram(shader.program);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "position"), position.x, position.y, position.z);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "ambient"), color * ambient);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "diffuse"), color * diffuse);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "specular"), color * specular);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "direction"), direction.x, direction.y, direction.z);
	shader.addUniform1F(appendExtentionToName(nameInUniform, "cutoff"), cutoff);
	shader.addUniform1F(appendExtentionToName(nameInUniform, "outerCutoff"), outerCutoff);
	shader.addUniform1F(appendExtentionToName(nameInUniform, "constant"), constant);
	shader.addUniform1F(appendExtentionToName(nameInUniform, "linear"), linear);
	shader.addUniform1F(appendExtentionToName(nameInUniform, "quadratic"), quadratic);
}







/*	shader.addUniformVec3("light.ambient", color * glm::vec3(0.2));
	shader.addUniformVec3("light.diffuse", color * glm::vec3(0.5)); // darken diffuse light a bit
	shader.addUniformVec3("light.specular", color * glm::vec3(1));
*/