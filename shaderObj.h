#ifndef SHADER_H
#define SHADER_H
#include <vector>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
//including math library to use
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
class Shader
{
private:
		std::string readShaderFile(std::string fileName);

		unsigned int compileShader(unsigned int type,const std::string &source);

		void linkShader(unsigned int _Shader, unsigned int program);

public:
	unsigned int program;
	std::string location;
	Shader(std::string vertexShaderLocation, std::string fragmentShaderLocation);
	std::string getVertexShaderSource(int sizeToRead);
	std::string getFragmentShaderSource(int sizeToRead);

	void addUniformMat4(const char *uniformName,glm::mat4 *mat4ToSend);
	void addUniform1I(const char* uniformName, int intToSend);
	void addUniformVec3(const char* uniformName, float x, float y, float z);
	void addUniformVec3(const char* uniformName, glm::vec3 vecToSend);
	void addUniform1F(const char* uniformName, float floatToSend);
};

#endif SHADER_H