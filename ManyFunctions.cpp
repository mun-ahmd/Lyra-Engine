#define STB_IMAGE_IMPLEMENTATION


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <map>
#include <array>
#include <random>
#include <fstream>
#include "ManyFunctions.h"
//Math library is included in shader class header file
//including my shader class
#include "shaderObj.h"
//including camera class
#include "cameraObj.h"
//include light class
#include "lightObj.h"
//include mesh class
#include "meshObj.h"

#include "Scene.h"
//for glm::length2()
#include "glm/gtx/norm.hpp"
using namespace std;

//void mouseMovedCall(GLFWwindow* window, double xPos, double yPos);	

//WINDOW INFO
int WINDOW_WIDTH = 1920;
int WINDOW_HEIGHT = 1080;

//Shadow Info
const int SHADOW_WIDTH = 2048*2, SHADOW_HEIGHT = 2048*2;



//SENSITIVITY
float keyboardMovementSensitivity = 2.0;
double mouseSensitivity = 0.05f;


vector<glm::vec3> reflectionFronts = {
	glm::vec3(1,0,0),
	glm::vec3(-1,0,0),
	glm::vec3(0,1,0),
	glm::vec3(0,-1,0),
	glm::vec3(0,0,1),
	glm::vec3(1,0,0)
};


//CAMERA
Camera rearCam = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
CameraCubemap reflectionCam = CameraCubemap();

//MOUSE INPUT
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;

//PROJECTION MATRIX
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;



float cubeVertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

float planeVertices[] = {
	// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
	 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};

float reflectionPlane[]{
	//positions			  //normals				//texcoords
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

//translations for our 10 cubes
glm::vec3 cubePositions[] = {
glm::vec3(0.0f,  0.0f,  0.0f),
glm::vec3(2.0f,  5.0f, -15.0f),
glm::vec3(-1.5f, -2.2f, -2.5f),
glm::vec3(-3.8f, -2.0f, -12.3f),
glm::vec3(2.4f, -0.4f, -3.5f),
glm::vec3(-1.7f,  3.0f, -7.5f),
glm::vec3(1.3f, -2.0f, -2.5f),
glm::vec3(1.5f,  2.0f, -2.5f),
glm::vec3(1.5f,  0.2f, -1.5f),
glm::vec3(-1.3f,  1.0f, -1.5f)
};




float skyboxCubeVertices[]{
	//positions			  //normals
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};





float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}



GLFWwindow* openWindow() //opens a window and returns it as an object using glfw
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //want modern opengl
	
	// Open a window and create its OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "window", NULL, NULL);
	glfwMakeContextCurrent(window); // 

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		glfwTerminate();
		return window;
	}
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return window;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	return window;
}

/*
bool sortTransparentObjects(glm::vec3 transPos, glm::vec3 transPos2) {
	return (glm::length2(transPos - firstCam.position) > glm::length2(transPos2 - firstCam.position));
}
*/

/*
unsigned int makeCubeMapFromTex(unsigned int* textures, int size) 
{

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	unsigned int cubemap;
	glGenTextures(1, &cubemap);

	for (int i = 0; i < 6; ++i) 
	{
		
	}
}
*/


void createFrameBuffer(unsigned int& FBO , unsigned int& texColorBuffer) {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);	//data is initially set to NULL 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);	//framebuffer target, color attachment , texture target , texture , mipmap level

	
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 768);	// 24 bits for depth and 8 bits for stencil

	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		cout << "created framebuffer" << endl;
	}
	else {
		cout << glCheckFramebufferStatus(GL_FRAMEBUFFER);
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

}



unsigned int framebuffersFromArraysForCubemap(unsigned int* framebuffers, int size) 
{
	glGenFramebuffers(6, framebuffers);

	int CUBEMAP_SIZE = WINDOW_HEIGHT - 40;
	unsigned int renderbuffers[6] = {0,0,0,0,0,0};

	unsigned int cubemap;
	glGenTextures(1, &cubemap);


	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, CUBEMAP_SIZE, CUBEMAP_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}


	for (int i = 0;i < 6;i++) 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i]);


		glGenRenderbuffers(1, &renderbuffers[i]);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffers[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CUBEMAP_SIZE, CUBEMAP_SIZE);


		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffers[i]);



		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
			cout << "created framebuffer" << endl;
		}
		else {
			cout << glCheckFramebufferStatus(GL_FRAMEBUFFER);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return cubemap;

}



void getFrustumCorners(std::vector<glm::vec3>& corners, const glm::mat4& projection, glm::mat4 model = glm::mat4(1))
{
	//warning: this will empty any vector sent to the function!
	corners.clear();

	// homogeneous corner coords
	glm::vec4 hcorners[8];
	// near
	hcorners[0] = glm::vec4(-1, 1, 1, 1);
	hcorners[1] = glm::vec4(1, 1, 1, 1);
	hcorners[2] = glm::vec4(1, -1, 1, 1);
	hcorners[3] = glm::vec4(-1, -1, 1, 1);
	// far
	hcorners[4] = glm::vec4(-1, 1, -1, 1);
	hcorners[5] = glm::vec4(1, 1, -1, 1);
	hcorners[6] = glm::vec4(1, -1, -1, 1);
	hcorners[7] = glm::vec4(-1, -1, -1, 1);

	glm::mat4 inverseProj = glm::inverse(projection);
	glm::mat4 inverseModel = glm::inverse(model);
	for (int i = 0; i < 8; i++) {
		hcorners[i] = inverseModel *inverseProj * hcorners[i];
		hcorners[i] /= hcorners[i].w;

		corners.push_back(glm::vec3(hcorners[i]));
	}
}

float munzAbsMax(float a, float b)
{
	if (abs(a) > abs(b))
		return a;
	else
		return b;
}


void formProjectionForViewFrustum(glm::mat4& projectionToForm, const glm::mat4& model, const glm::mat4& viewProjection)
{
	std::vector<glm::vec3> corners;
	getFrustumCorners(corners, viewProjection, model);
	glm::vec3 left = glm::vec3(0), right = glm::vec3(0), top = glm::vec3(0), bottom = glm::vec3(0), farthest = glm::vec3(0), nearest= glm::vec3(0);
	for (int i = 0; i < corners.size();++i)
	{
		if (corners[i].x < left.x)
			left = corners[i];
		else if (corners[i].x > right.x)
			right = corners[i];

		if (corners[i].y < bottom.y)
			bottom = corners[i];
		else if (corners[i].y > top.y)
			top = corners[i];

		if (corners[i].z < nearest.z)
			nearest = corners[i];
		else if (corners[i].z > farthest.z)
			farthest = corners[i];
	}

	//farthest.z = 100 - std::abs(nearest.z); right.x = 100 - std::abs(left.x); top.y = 100 - std::abs(bottom.y);

	float l; float b; float n;
	l = abs(munzAbsMax(left.x, right.x)); b =abs( munzAbsMax(top.y, bottom.y)); n = abs(munzAbsMax(nearest.z, farthest.z));

	//projectionToForm = glm::ortho(-l,l,-b,b,-n,n
	glm::mat4 proj = glm::ortho((float)-SHADOW_WIDTH / 2, (float)SHADOW_WIDTH / 2, (float)-SHADOW_HEIGHT/ 2, (float)SHADOW_HEIGHT / 2, nearest.z, farthest.z);
	glm::mat4 direction = model;
	float scale_x = 1.0, scale_y = 1.0;
	glm::mat4 view = glm::scale(glm::mat4(1.0), glm::vec3(scale_x, scale_y, 1.0)) * direction;
	glm::vec4 spos = view * glm::vec4(0,0,0,1);
	glm::vec2 off = -glm::round(glm::vec2(spos));
	glm::mat4 offset = glm::translate(glm::mat4(1.0), glm::vec3(off, 0.0));
	projectionToForm = proj * offset * view;
 
}


void textureDrawSetup( unsigned int *texDrawVao)
{

	float squareVerts[] = {
    // positions          //texture coords
     1.f,  1.f, 0.0f,   1.0f, 1.0f,   // top right
     1.f, -1.f, 0.0f,   1.0f, 0.0f,   // bottom right
    -1.f, -1.f, 0.0f,	  0.0f, 0.0f,   // bottom left
	-1.f, -1.f, 0.0f,	  0.0f, 0.0f,   // bottom left
    -1.f,  1.f, 0.0f,   0.0f, 1.0f,    // top left 
	 1.f,  1.f, 0.0f,   1.0f, 1.0f   // top right

	};
	glCreateVertexArrays(1,texDrawVao);
	unsigned int squareBuffer;
	glCreateBuffers(1, &squareBuffer);
	glBindVertexArray(*texDrawVao);
	glBindBuffer(GL_ARRAY_BUFFER, squareBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVerts), squareVerts, GL_STATIC_DRAW); 

	glEnableVertexArrayAttrib(*texDrawVao, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexArrayAttrib(*texDrawVao, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));

}

void drawTexture(unsigned int textureToDraw, unsigned int vao, Shader& shader)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(shader.program);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textureToDraw);
	shader.addUniform1I("texture1",0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawDeferredLighting(unsigned int fbo,unsigned int vao, Shader& shader)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glUseProgram(shader.program);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawGradiantSky(unsigned int fbo, unsigned int vao, Shader& shader, glm::mat4 & view)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glUseProgram(shader.program);
	shader.addUniformMat4("view", &view);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void applyGaussianBlur(unsigned int brightTex,unsigned int* pingpongFBO,unsigned int* pingpongTex, unsigned int vao, Shader& shader)
{
	bool horizontal = true, first_iteration = true;
	int amount = 10;
	glUseProgram(shader.program);

	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		shader.addUniform1I("horizontal", horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? brightTex : pingpongTex[!horizontal]
		);
		shader.addUniform1I("image", 0);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void additiveMixTex(unsigned int tex1, unsigned int tex2,unsigned int vao,unsigned int outputFbo, Shader& shader)
{
	glBindFramebuffer(GL_FRAMEBUFFER, outputFbo);
	glUseProgram(shader.program);
	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	shader.addUniform1I("texture1", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);
	shader.addUniform1I("texture2", 1);

	shader.addUniform1F("exposure", 1.0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void shadowFBOsetup(unsigned int &shadowFBO , unsigned int &shadowMap, unsigned int &reflectiveShadowMap){
	glCreateFramebuffers(1, &shadowFBO);


	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

	glGenTextures(1, &reflectiveShadowMap);
	glBindTexture(GL_TEXTURE_2D, reflectiveShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectiveShadowMap, 0);



	//tell opengl that we don't want to read or write any color data from this framebuffer
	//not using for reflective shadow map
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 lightSpaceMatrix(glm::vec3& lightPos) {
	float near_plane = 1.0f, far_plane = 550.5;
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

	glm::mat4 lightView = glm::lookAt(lightPos,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	return (lightProjection * lightView);
}



void shadowDraw(unsigned int& shadowFBO, unsigned int& shadowMap, glm::mat4 &lightSpaceMatrix,glm::mat4 &lightModel,Shader& shader, Scene& scene) {
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glCullFace(GL_FRONT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(shader.program);
	shader.addUniformMat4("lightSpaceMatrix", &lightSpaceMatrix);
	shader.addUniformMat4("model", &lightModel);
	//RenderScene();
	//model.Draw(shader);
	scene.draw(shader);
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void imageRender(GLFWwindow* window)
{

	float vegSquare[] =
	{	// positions          // texture Coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	};

	//SHADER DIRECTORY AND GETTING SHADER CODE
	string shaderDir = R"(Shaders\)"; //folder directory where all shaders are stored


	Shader lightShader = Shader(shaderDir + "forwardRenderVertex.glsl",shaderDir + "forwardRenderFragment.glsl");

	Shader deferredFirstPass = Shader(shaderDir + "geometryPassV.glsl", shaderDir + "geometryPassF.glsl");

	Shader deferredSecondPass = Shader(shaderDir + "lightingPassV.glsl", shaderDir + "lightingPassF.glsl");

	Shader lightCubeShader = Shader(shaderDir + "lightCubeVertexShader.glsl", shaderDir + "lightCubeFragmentShader.glsl");

	Shader reflectCubeShader = Shader(shaderDir + "reflectCubeVertex.glsl", shaderDir + "reflectCubeFragment.glsl");

	Shader shadowsShader = Shader(shaderDir + "shadowsShaderVertex.glsl", shaderDir + "shadowsShaderFragment.glsl");

	Shader  texDrawShader = Shader(shaderDir + "textureDrawVertex.glsl", shaderDir + "textureDrawFragment.glsl");

	Shader gaussianBlurShader = Shader(shaderDir + "gaussianBlurVertex.glsl", shaderDir + "gaussianBlurFragment.glsl");

	Shader additiveMixTexShader = Shader(shaderDir + "additiveMixTexV.glsl", shaderDir + "additiveMixTexF.glsl");

	Shader gradientSkyShader = Shader(shaderDir + "skyGradiantV.glsl", shaderDir + "skyGradiantF.glsl");
	//Shader reflectionShader = Shader(shaderDir + "reflectionVertexShader.glsl", shaderDir + "reflectionFragmentShader.glsl");

	//Texture Naming Convention: if texture is diffuse texture give it type texture_diffuse and in fragment shader it will be passed as texture_diffuse#n where '#n' is the index of the texture among other textures

	/*
	Texture woodTex;
	woodTex.type = "texture_diffuse";
	woodTex.id = TextureFromFile("woodTexture.jpg", R"(Images)");


	Texture stoneTex;
	stoneTex.type = "texture_diffuse";
	stoneTex.id = TextureFromFile("grassFurTex.jpg", R"(Images)");

	Texture redWindowTex;
	unsigned int redWindow = TextureFromFile("red_window.png", "Images");
	redWindowTex.id = redWindow;
	redWindowTex.type = "texture_diffuse";

	Texture noneTex;
	noneTex.type = "texture_none";
	vector<Texture> texturesCube = {};
	vector<Texture> transparentTexturedCube = {redWindowTex};
	vector<unsigned int> indicesCube = {0};
	vector<Vertex> verticesCube;
	int z = 0;
	int len = sizeof(cubeVertices) / sizeof(cubeVertices[0]);

	while (z < len) {
		float b = cubeVertices[z];
		Vertex current;
		current.Position = glm::vec3(cubeVertices[z],cubeVertices[z+1],cubeVertices[z+2]);
		current.Normal = glm::vec3(cubeVertices[z + 3], cubeVertices[z + 4], cubeVertices[z + 5]);
		current.TexCoords = glm::vec2(cubeVertices[z + 6], cubeVertices[z + 7]);
		verticesCube.push_back(current);
		z += 8;
	}

	Mesh cube = Mesh(verticesCube, indicesCube, texturesCube);

	Mesh transparentCube = Mesh(verticesCube, indicesCube, transparentTexturedCube);

	vector<Texture> texturesPlane = { stoneTex };
	vector<unsigned int> indicesPlane = {0};
	vector<Vertex> verticesPlane;
	z = 0;
	len = sizeof(planeVertices) / sizeof(planeVertices[0]);
	while (z < len) {
		Vertex current2;
		current2.Position = glm::vec3(planeVertices[z], planeVertices[z + 1], planeVertices[z + 2]);
		current2.TexCoords = glm::vec2(planeVertices[z + 3], planeVertices[z + 4]);
		verticesPlane.push_back(current2);
		z += 5;
	}

	Mesh plane = Mesh(verticesPlane, indicesPlane, texturesPlane);
	*/
	unsigned int skyboxVA;
	unsigned int skyboxBuffer;
	glCreateVertexArrays(1, &skyboxVA);
	glBindVertexArray(skyboxVA);

	glCreateBuffers(1, &skyboxBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);




	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);							//Clear Color

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);					// To see outline






	/*
	PointLight ptLight = PointLight(glm::vec3(2, 2, 3), glm::vec3(1.0f));
	lightShader.addUniformPointLight(ptLight,"ptLight");
	*/
	//dirLight.addToShader(lightShader, "dirLight");

	glm::mat4 model = glm::mat4(1); glm::mat4 view; glm::mat4 projection; glm::mat4 mirrorProjection;
	//gives values to the arguements, to transform the coords to all forms in the process
	projection = glm::perspective(
		glm::radians(fov),	//field of view = 45 degrees
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,				//width over height of frustum	//I have set to window width over window height
		0.001f,					//z near
		3000.0f					// z far
	);

	//glfwSetCursorPosCallback(window, mouseMovedCall);	//this will call my mouse moved function whenever the mouse is moved
	glUseProgram(deferredFirstPass.program);

	deferredFirstPass.addUniformMat4("projection", &projection);


	glUseProgram(lightShader.program);

	lightShader.addUniformMat4("projection", &projection);


	float deltaTime = 0.0f;
	float timeSinceLastFrame = 0.0f;





	float fpsHolder = 0.0f;
	float FPS = 0.0f;






	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);



	vector<std::string> faces =
	{
			"right.png",
			"left.png",
			"top.png",
			"bottom.png",
			"front.png",
			"back.png"
	};


	glm::mat4 skyboxView = glm::mat4(1);

	//unsigned int skyboxTexId = createCubeMap(faces, R"(Images\skybox)");

	glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	unsigned int shadowFBO, shadowFBOtex, reflectiveShadowFBOtex;
	shadowFBOsetup(shadowFBO, shadowFBOtex, reflectiveShadowFBOtex);

	Camera firstCam = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), reflectionFronts[1]);
	Scene scene = Scene(window,&firstCam);


	//ModelNode vaibhavNode = ModelNode(&scene, &vaibhav);

	//Model birch = Model(R"(3DModelData\birchTree\birchTree.obj)");
	//Model cubePlayer = Model(R"(3DModelData\marsLand(nishchay\map.obj)");

	//float timePreStore = glfwGetTime();	birch.storeToFastLoad();	cout << glfwGetTime() - timePreStore;
	/*
	ModelNode birch1 = ModelNode(&birchtrees5, &birch);
	ModelNode birch2 = ModelNode(&birchtrees5, &birch); birch2.translate(glm::vec3(2.5, 0, 0));birch2.rotateDegrees(40, glm::vec3(0, 1, 0));birch2.scale(0.93);
	ModelNode birch3 = ModelNode(&birchtrees5, &birch); birch3.translate(glm::vec3(-1.75, 0, -2));birch3.rotateDegrees(79, glm::vec3(0, 1, 0));birch3.scale(1.1);
	ModelNode birch4 = ModelNode(&birchtrees5, &birch); birch4.translate(glm::vec3(-0.5, 0, -2.6));birch4.rotate(1.57 / 2, glm::vec3(0, 1, 0));birch4.scale(0.90);
	ModelNode birch5 = ModelNode(&birchtrees5, &birch); birch5.translate(glm::vec3(1.5, 0, -1.5));birch5.scale(1.05);
	ModelNode birch6 = ModelNode(&birchtrees5, &birch); birch6.translate(glm::vec3(-0.5, 0, -2.6));birch6.rotateDegrees(79, glm::vec3(0, 1, 0));birch6.scale(0.90);
	ModelNode birch7 = ModelNode(&birchtrees5, &birch); birch7.translate(glm::vec3(-0.5, 0, -2.6));birch7.rotateDegrees(9, glm::vec3(0, 1, 0));birch7.scale(0.90);
	ModelNode birch8 = ModelNode(&birchtrees5, &birch); birch8.translate(glm::vec3(-0.5, 0, -2.6));birch8.rotateDegrees(26, glm::vec3(0, 1, 0));birch8.scale(0.90);
	ModelNode birch9 = ModelNode(&birchtrees5, &birch); birch9.translate(glm::vec3(-0.5, 0, -2.6));birch9.rotateDegrees(120, glm::vec3(0, 1, 0));birch9.scale(0.90);
	ModelNode birch10 = ModelNode(&birchtrees5, &birch); birch10.translate(glm::vec3(0.5, 0, 1.6));birch10.rotateDegrees(310, glm::vec3(0, 1, 0));birch10.scale(0.90);
	ModelNode birch11 = ModelNode(&birchtrees5, &birch); birch11.translate(glm::vec3(-0.5, 0, 2.6));birch11.rotateDegrees(255, glm::vec3(0, 1, 0));birch11.scale(0.90);
	ModelNode birch12 = ModelNode(&birchtrees5, &birch); birch12.translate(glm::vec3(0.7, 0, -2.6));birch12.rotateDegrees(10, glm::vec3(0, 1, 0));birch12.scale(0.90);
	ModelNode birch13 = ModelNode(&birchtrees5, &birch); birch13.translate(glm::vec3(-0.8, 0, 2.6));birch13.rotateDegrees(160, glm::vec3(0, 1, 0));birch13.scale(0.90);
	ModelNode birch14 = ModelNode(&birchtrees5, &birch); birch14.translate(glm::vec3(0.5, 0, -3.6));birch14.rotateDegrees(37, glm::vec3(0, 1, 0));birch14.scale(0.90);
	ModelNode birch15 = ModelNode(&birchtrees5, &birch); birch15.translate(glm::vec3(-0.9, 0, -2.0));birch15.rotateDegrees(43, glm::vec3(0, 1, 0));birch15.scale(0.90);
	ModelNode birch16 = ModelNode(&birchtrees5, &birch); birch16.translate(glm::vec3(0.8, 0, -2.25));birch16.rotateDegrees(86, glm::vec3(0, 1, 0));birch16.scale(0.90);
	ModelNode birch17 = ModelNode(&birchtrees5, &birch); birch17.translate(glm::vec3(0.65, 0, -1.58));birch17.rotateDegrees(90, glm::vec3(0, 1, 0));birch17.scale(0.90);
	*/
	//cout <<"\n\n\n\n\n\n\n\n"<< birchtrees5.identify <<"\n"<< birchtrees5Copy->identify<< "\n\n\n\n\n\n\n\n\n\n";
	//birchNode.rotateDegrees(90, glm::vec3(0, 0, 1));
	
	//ModelNode playerNode = ModelNode(&scene, &cubePlayer);

	DirectionalLight dirLight = DirectionalLight(glm::vec3(0.4f, 0.2f, 0.6f), glm::vec3(1)*15.f);
	//dirLight.setIntensity(6);
	//glm::mat4 modelCube = glm::mat4(1);
	//modelCube = glm::translate(modelCube, dirLight.position);

	unsigned int texDrawVAO;
	textureDrawSetup(&texDrawVAO);

	unsigned int testTex = TextureFromFile("purple_vaibhav.png", "Images", "texture_diffuse");

	int SCR_WIDTH = WINDOW_WIDTH; int SCR_HEIGHT = WINDOW_HEIGHT;

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// configure g-buffer framebuffer
// ------------------------------
	unsigned int gBuffer;
	glCreateFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gAlbedoSpec;
	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<glm::vec3> objectPositions;
	objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));

	unsigned int framebuf, fbTex;
	createFrameBuffer(framebuf, fbTex);

	glUseProgram(deferredSecondPass.program);
	deferredSecondPass.addUniform1I("gPosition", 0);
	deferredSecondPass.addUniform1I("gNormal", 1);
	deferredSecondPass.addUniform1I("gAlbedoSpec", 2);
	deferredSecondPass.addUniform1I("shadowMap", 3);
	deferredSecondPass.addUniform1I("reflectiveShadowMap", 4);


	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffer[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
		);
	}


	unsigned int colorFramebuffer;
	unsigned int sceneColorTex, brightColorTex;

	glGenFramebuffers(1, &colorFramebuffer);
	
	glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);

	glGenTextures(1, &sceneColorTex);
	glBindTexture(GL_TEXTURE_2D, sceneColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, sceneColorTex, 0);
	
	glGenTextures(1, &brightColorTex);
	glBindTexture(GL_TEXTURE_2D, brightColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightColorTex, 0);

	GLenum attachments2[2] = { GL_COLOR_ATTACHMENT0 ,GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments2);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindTexture(GL_TEXTURE_2D,0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(lightCubeShader.program);
	lightCubeShader.addUniformMat4("projection", &projection);

	glUseProgram(gradientSkyShader.program);
	gradientSkyShader.addUniformMat4("projection", &projection);


	float* reflectionMapSamples = new float[800]();

	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> distrib1(0.0,0.3);
	std::uniform_real_distribution<> distrib2(0.3, 0.8);
	std::uniform_real_distribution<> distrib3(0.45, 0.75);


	for (int i = 0; i < 300; ++i)
	{
		reflectionMapSamples[i] = distrib1(gen);
	}
	
	for (int i = 300; i < 800; ++i)
	{
		reflectionMapSamples[i] = distrib2(gen);
	}
	/*
	for (int i = 400; i < 600; ++i)	
	{
		reflectionMapSamples[i] = distrib3(gen);
	}
	*/

	glUseProgram(deferredSecondPass.program);
	glUniform1fv(glGetUniformLocation(deferredSecondPass.program, "reflectiveSMsamples"), 800, reflectionMapSamples);

	delete[] reflectionMapSamples;

	glm::mat4 dirLightModel = glm::mat4(1);


	glm::mat4 lightView = glm::lookAt(dirLight.direction * 10.0f,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	do {



		if (fpsHolder > 1.0f) {
			cout << FPS << "(Framerate)" << endl;
			FPS = 0;
			fpsHolder = 0;
		}

		float currTime = (float)glfwGetTime();
		deltaTime = currTime - timeSinceLastFrame;
		fpsHolder += deltaTime;
		FPS++;
		timeSinceLastFrame = currTime;

		scene.moveAround(deltaTime);
		scene.lookAround();
		
		
		//glEnable(GL_DEPTH_CLAMP);
		glm::vec3 dirLightPosition = dirLight.direction*10.0f;

		glm::mat4 lightSpaceTransform = lightSpaceMatrix(dirLightPosition);

		shadowDraw(shadowFBO, shadowFBOtex, lightSpaceTransform, model ,shadowsShader, scene);
		//glDisable(GL_DEPTH_CLAMP);

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		
		glClearColor(0.3, 0.3, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		view = firstCam.getViewMatrix();

		//gonna be sending these through the scene 
		//lightShader.addUniformMat4("lightSpaceMatrix", &lightSpaceTransform);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, shadowFBOtex);
		//lightShader.addUniform1I("shadowMap",0);
		
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(deferredFirstPass.program);
		deferredFirstPass.addUniformMat4("lightSpaceTransform", &lightSpaceTransform);
		scene.GeometryPass(deferredFirstPass);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glUseProgram(deferredSecondPass.program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, shadowFBOtex);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, reflectiveShadowFBOtex);

		glActiveTexture(GL_TEXTURE0);

		deferredSecondPass.addUniformVec3("viewPos", firstCam.position);
		deferredSecondPass.addUniformVec3("dirLight.direction", dirLight.direction);
		deferredSecondPass.addUniformVec3("dirLight.color", dirLight.color);
		deferredSecondPass.addUniformMat4("lightSpaceMatrix", &lightSpaceTransform);


		drawDeferredLighting(colorFramebuffer,texDrawVAO, deferredSecondPass);
		
		applyGaussianBlur(brightColorTex,pingpongFBO, pingpongBuffer, texDrawVAO, gaussianBlurShader);

		//additiveMixTex(pingpongBuffer[0], sceneColorTex, texDrawVAO, 0, additiveMixTexShader);

		drawTexture(sceneColorTex, texDrawVAO, texDrawShader);
		
		/*
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		glBlitFramebuffer(
			0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST
		);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		
		glDepthFunc(GL_LEQUAL);
		skyboxView = glm::mat4(glm::mat3(view));			//resets w component
		drawGradiantSky(0, skyboxVA, gradientSkyShader, skyboxView);
		glDepthFunc(GL_LESS);
		*/
		/*
		//rendering skybox will be first or last(with some changes) thing to do, and it will have depth writing off so it is always in the background

	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDepthFunc(GL_LEQUAL);


		glBindVertexArray(skyboxVA);
		glUseProgram(lightCubeShader.program);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexId);
		skyboxView = glm::mat4(glm::mat3(view));			//resets w component
		lightCubeShader.addUniformMat4("view", &skyboxView);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);


		glDepthFunc(GL_LESS);
		
		
		*/


		glfwSwapBuffers(window);
		glfwPollEvents();
		


	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == false);
	//DRAWING LOOP ENDS

	//CLEANUP
	glDeleteVertexArrays(1, &skyboxVA);
	scene.clean(lightCubeShader);
	glDeleteProgram(lightShader.program);
	glDeleteProgram(deferredFirstPass.program);
	glDeleteProgram(texDrawShader.program);
	glDeleteProgram(shadowsShader.program);


	//birch.Cleanup(lightShader);
	//CLEANUP ENDS

}



void imageRender222(GLFWwindow* window)
{
	//SHADER DIRECTORY AND GETTING SHADER CODE
	string shaderDir = R"(Shaders\)"; //folder directory where all shaders are stored


	Shader lightShader = Shader(shaderDir + "forwardRenderVertex.glsl", shaderDir + "forwardRenderFragment.glsl");
	Shader deferredFirstPass = Shader(shaderDir + "geometryPassV.glsl", shaderDir + "geometryPassF.glsl");
	Shader deferredSecondPass = Shader(shaderDir + "lightingPassV.glsl", shaderDir + "lightingPassF.glsl");
	Shader texDrawShader = Shader(shaderDir + "textureDrawVertex.glsl", shaderDir + "textureDrawFragment.glsl");
	Shader gaussianBlurShader = Shader(shaderDir + "gaussianBlurVertex.glsl", shaderDir + "gaussianBlurFragment.glsl");
	Shader additiveMixTexShader = Shader(shaderDir + "additiveMixTexV.glsl", shaderDir + "additiveMixTexF.glsl");
	Shader shadowsShader = Shader(shaderDir + "shadowsShaderVertex.glsl", shaderDir + "shadowsShaderFragment.glsl");

	Shader newDrawTestShader = Shader(shaderDir + "newDrawVert.glsl", shaderDir + "newDrawFrag.glsl");

	//Shader reflectionShader = Shader(shaderDir + "reflectionVertexShader.glsl", shaderDir + "reflectionFragmentShader.glsl");

	//Texture Naming Convention: if texture is diffuse texture give it type texture_diffuse and in fragment shader it will be passed as texture_diffuse#n where '#n' is the index of the texture among other textures

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);							//Clear Color

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);					// To see outline

	glm::mat4 model = glm::mat4(1); glm::mat4 view; glm::mat4 projection;
	//gives values to the arguements, to transform the coords to all forms in the process
	projection = glm::perspective(
		glm::radians(fov),	//field of view = 45 degrees
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,				//width over height of frustum	//I have set to window width over window height
		0.001f,					//z near
		3000.0f					// z far
	);



	glUseProgram(lightShader.program);
	lightShader.addUniformMat4("projection", &projection);

	glUseProgram(deferredFirstPass.program);
	deferredFirstPass.addUniformMat4("projection", &projection);

	glUseProgram(newDrawTestShader.program);
	newDrawTestShader.addUniformMat4("projection", &projection);

	float deltaTime = 0.0f;
	float timeSinceLastFrame = 0.0f;

	float fpsHolder = 0.0f;
	float FPS = 0.0f;



	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	std::cout << sizeof(glm::mat4);
	Scene scene1 = Scene(window,"scenefile.txt");

	//Model model2(R"(C:\Users\munee\source\repos\Lyra Engine\Lyra Engine\3DModelData\birchTree\birchTree.obj)");
	//ModelNode modelNode = ModelNode(&scene1, &model2)
	//model2.storeModelToL3D("nishchay_is_not_powerful.L3D");

	//Model model_man= Model("nishchay_is_not_powerful.L3D",false);
	
	

		
	for (int i = 0; i < scene1.getChildren().size();++i)
	{
		cout << scene1.getChildren()[i]->internal_name;
	}

	std::cout << lightShader.getFragmentShaderSource(50);
	std::cout << lightShader.getVertexShaderSource(50);

	//scene1.setupDeferredShading();


	//scene1.setGaussianBlurState(true);
	glUseProgram(newDrawTestShader.program);
	newDrawTestShader.addUniform1I("diffuse", 0);
	newDrawTestShader.addUniform1I("specular", 1);
	newDrawTestShader.addUniform1I("roughness", 2);
	newDrawTestShader.addUniform1I("metallic", 3);
	newDrawTestShader.addUniform1I("transparancy", 4);





	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glClearColor(0.3, 0.3, 0.3, 1.0);

	do {



		if (fpsHolder > 1.0f) {
			cout << FPS << "(Framerate)" << endl;
			cout << deltaTime << "(Frametime)" << endl;
			FPS = 0;
			fpsHolder = 0;
		}

		float currTime = (float)glfwGetTime();
		deltaTime = currTime - timeSinceLastFrame;
		fpsHolder += deltaTime;
		FPS++;
		timeSinceLastFrame = currTime;



		scene1.moveAround(deltaTime);
		scene1.lookAround();


		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		//glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		newDrawTestShader.addUniformMat4("view", &scene1.getMainCamera()->getViewMatrix());

		//scene1.ShadowsPass(shadowsShader);

		//scene1.GeometryPass(deferredFirstPass);

		//scene1.LightingPass(deferredSecondPass);

		//scene1.drawFinal(texDrawShader,gaussianBlurShader,additiveMixTexShader);

		//drawTexture(scene1.getgNormal(), texDrawVAO, texDrawShader);

		scene1.newDraw(newDrawTestShader);

		glfwSwapBuffers(window);
		glfwPollEvents();



	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == false);
	//DRAWING LOOP ENDS

	//CLEANUP
	scene1.clean(lightShader);


	//birch.Cleanup(lightShader);
	//CLEANUP ENDS

}


void render() {
	gladLoadGL();
	glfwInit();							//initialize glfw, this is where it starts counting time, you can also change where it starts to count from using a function

	GLFWwindow* window = openWindow();	//Create a window, and refer in window object

	imageRender222(window);						//does everything 

	glfwTerminate();					//ends the process
}


