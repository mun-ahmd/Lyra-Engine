	#include "shaderObj.h"
#include <cstring>



	std::string Shader::readShaderFile(std::string fileName) //stores shader code file as a string, takes file location/name(if in same folder) as parameter
	{
		std::ifstream ifs(fileName);
		if (ifs)
		{
			std::string shaderCode((std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>()));
			return shaderCode;
		}
		else
		{
			std::cout << "No file found/ file invalid at " << fileName << std::endl;
			return "";
		}
	};

	unsigned int Shader::compileShader(unsigned int type, const std::string& source)//type tells whether fragment or vertex shader
	{
		unsigned int _shader;
		const char* src = source.c_str();
		_shader = glCreateShader(type);
		glShaderSource(_shader, 1, &src, nullptr); // READ MORE @ DOCS.GL
		glCompileShader(_shader);

		//ERROR HANDLING(in shader code) BELOW:
		GLint isCompiled = 0;
		glGetShaderiv(_shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)//some error in compiling
		{
			GLint maxLength = 0;
			glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(_shader, maxLength, &maxLength, &errorLog[0]);
			std::cout << "Compiling Error \n"/*+source*/ << std::endl << &errorLog[0] << std::endl;  //prints the error log.. very useful
			// Provide the infolog in whatever manor you deem best.
			// Exit with failure
			glDeleteShader(_shader); // Don't leak the shader.
			unsigned int r = 0;
			return r;
		}
		//return shader
		return _shader;
	}


	void Shader::linkShader(unsigned int _Shader, unsigned int program)     //GLuint and other types starting with GL can be found on:
																	   //https://www.khronos.org/opengl/wiki/OpenGL_Type
	{
		glAttachShader(program, _Shader);
		glLinkProgram(program);

		//we can now delete the shader since it has been linked to program and now the program has their data in binary
		glDeleteShader(_Shader);
		//ERROR HANDLING FOR LINKING :
		GLint link_ok = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
		//Error handling for linking probs
		if (link_ok == 0)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);
			std::cout << "Linking Error" << std::endl << &errorLog[0] << std::endl;
			std::cout << location + "\n";
			// Provide the infolog in whatever manor you deem best.
			// Exit with failure.
			glDeleteProgram(program); // Don't leak the shader
		}
		int infologlen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologlen);
		if (infologlen > 0) { std::cout << "ERROR" << std::endl; }
		//return unsigned int program

	}




	Shader::Shader(std::string vertexShaderLocation, std::string fragmentShaderLocation) {
		program = glCreateProgram();
		location = vertexShaderLocation;
		linkShader(Shader::compileShader(GL_VERTEX_SHADER, readShaderFile(vertexShaderLocation)), program);							//compiles and links vertex shader to program
		linkShader(Shader::compileShader(GL_FRAGMENT_SHADER, readShaderFile(fragmentShaderLocation)), program);						//compiles and links fragment shader 
	}

	std::string Shader::getVertexShaderSource(int sizeToRead)
	{
		GLuint fragShader[1]; int a = 1;
		glGetAttachedShaders(program, 1, &a, fragShader);
		GLchar sourceCodeChar[500];GLsizei lenSrc;
		glGetShaderSource(fragShader[0], sizeToRead, &lenSrc, sourceCodeChar);
		std::string stringToReturn(sourceCodeChar);
		return stringToReturn;
	}

	std::string Shader::getFragmentShaderSource(int sizeToRead)
	{
		GLuint fragShader[2]; int a = 2;
		glGetAttachedShaders(program, 2, &a, fragShader);
		GLchar sourceCodeChar[500];GLsizei lenSrc;
		glGetShaderSource(fragShader[1], sizeToRead, &lenSrc, sourceCodeChar);
		std::string stringToReturn(sourceCodeChar);
		return stringToReturn;
	}


	void Shader::addUniformMat4(const char* uniformName, glm::mat4* mat4ToSend) {
		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program,uniformName), 1, GL_FALSE, glm::value_ptr(*mat4ToSend));
	}

	void Shader::addUniformVec3(const char* uniformName, float x, float y, float z) {
		glUseProgram(program);
		float send[3] = { x,y,z };
		glUniform3fv(glGetUniformLocation(program, uniformName),1, send);
	}

	void Shader::addUniformVec3(const char* uniformName, glm::vec3 vecToSend) {
		glUseProgram(program);
		float send[3] = { vecToSend.x,vecToSend.y,vecToSend.z };
		glUniform3fv(glGetUniformLocation(program, uniformName), 1, send);
	}

	void Shader::addUniform1I(const char* uniformName, int intToSend) {
		glUseProgram(program);
		glUniform1i(glGetUniformLocation(program, uniformName), intToSend);
	}

	void Shader::addUniform1F(const char* uniformName, float floatToSend) {
		glUseProgram(program);
		glUniform1f(glGetUniformLocation(program, uniformName), floatToSend);
	}