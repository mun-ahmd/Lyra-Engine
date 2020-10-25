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
	const int n = nameWithExtentionStr.size() + 1;
	char* nameExt = new char[n];

	nameWithExtentionStr.copy(nameExt, n);
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

DirectionalLight::DirectionalLight(glm::vec3 lightDirection, glm::vec3 lightColor, float ambVal, float diffVal, float specVal) 
{
	direction = lightDirection;
	position = lightDirection * dirLightPosDistFactor;
	color = lightColor;
	ambient = glm::vec3(ambVal);
	diffuse = glm::vec3(diffVal);
	specular = glm::vec3(specVal);
}
DirectionalLight::DirectionalLight(glm::vec3 lightDirection, glm::vec3 lightColor) 
{
	direction = lightDirection;
	position = lightDirection * dirLightPosDistFactor;
	color = lightColor;
	ambient = glm::vec3(ambValDefault);
	diffuse = glm::vec3(diffValDefault);
	specular = glm::vec3(specValDefault);
}

void DirectionalLight::setupShadowGeneration()
{
	this->shadowMapFBO = 0;
	glCreateFramebuffers(1, &this->shadowMapFBO);


	glGenTextures(1, &this->shadowMap);
	glBindTexture(GL_TEXTURE_2D, this->shadowMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		this->SHADOW_WIDTH, this->SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowMap, 0);

	/*
	glGenTextures(1, &reflectiveShadowMap);
	glBindTexture(GL_TEXTURE_2D, reflectiveShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectiveShadowMap, 0);
	*/


	//tell opengl that we don't want to read or write any color data from this framebuffer
	//not using for reflective shadow map
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	float near_plane = 1.0f, far_plane = 550.5;
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

	glm::mat4 lightView = glm::lookAt(this->position,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	this->lightSpaceMatrix = lightProjection* lightView;
}


void DirectionalLight::generateShadow(Shader& shadowShader,unsigned int squareVAO)
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glCullFace(GL_FRONT);
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(shadowShader.program);

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, this->position);
	shadowShader.addUniformMat4("model", &model);
	shadowShader.addUniformMat4("lightSpaceMatrix", &this->lightSpaceMatrix);


}

void DirectionalLight::addToShader(Shader& shader, std::string nameInUniform) 
{
	glUseProgram(shader.program);
	shader.addUniformVec3(appendExtentionToName(nameInUniform, "color"), color);
	shader.addUniformVec3(appendExtentionToName(
								nameInUniform, "direction"), direction.x, direction.y, direction.z
																								);
	shader.addUniformMat4(appendExtentionToName(nameInUniform, "lightSpaceMatrix"), &this->lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D,this->shadowMap);
	shader.addUniform1I(appendExtentionToName(nameInUniform, "shadowMap"), 4);
	glBindTexture(GL_TEXTURE_2D,0);
	glActiveTexture(GL_TEXTURE0);


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