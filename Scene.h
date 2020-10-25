#pragma once
#include "meshObj.h"
#include "cameraObj.h"
#include "lightObj.h"
#include "Material_Manager.h"
#include <sstream>


class Transform
{
private:
	glm::vec3 translation;
	glm::vec3 scaleSize;
	glm::vec4 rotation; //the first three values are the axis of rotation, the last is the angle rotated in radians
public:
	Transform();
	void translate(glm::vec3 translationIN);
	void scale(glm::vec3 scaleIN);
	void rotate(glm::vec3 rotationAxis, float radians);

	glm::mat4 getModelMatrix();
};

enum NODE_TYPE
{
	MODEL_NODE,
	DIR_LIGHT_NODE,
	POINT_LIGHT_NODE,
	SPOT_LIGHT_NODE,
	SCENE_NODE
};


class Node {
protected:
	Node();
	Node* parent;
	std::vector<Node*> children;
	NODE_TYPE typeOfNode;
	void doInParentPath(void func(Node*));
public:
	std::string name;
	std::string internal_name;

	Transform transform;



	~Node();

	void changeParent_only(Node* newParentNode);
	//please remember this only changes the parent, NOTHING ELSE
	void addChildren(Node* nodeToAdd);
	void addChildren(Node* childrenNodes, int number_of_children);
	void addChildren(std::vector<Node*> childrenNodes);

	void removeChild(Node* childNode);

	std::vector<Node*> getChildren();

	NODE_TYPE getNodeType();
	Node* getRootParent();

	void printDirectory();

	
};




class ModelNode : public Node {
private:
	Model* model;
public:
	
	ModelNode(Node* parentIN, Model* model);
	void drawCall(Shader& shader);
	void cleanupCall(Shader& shader);
};


class DirLightNode : public Node {
private:
	DirectionalLight* dirLight;
	bool shadowsON = false;
	void setupShadows();
public:
	DirLightNode(Node* parentIN, DirectionalLight* dirLight);
	void cleanupCall(Shader& shader);
	void shadowDrawCall(Shader& shader, unsigned int squareVAO);
	void drawCall(Shader& shader);
	void shadowSetupCall();
};

class SpotLightNode : public Node {
private:
	Spotlight* spotlight;
public:
	SpotLightNode(Node* parentIN, Spotlight* spotLight);
	void drawCall(Shader& shader);
	void cleanupCall(Shader& shader) {};


};

class PointLightNode : public Node {
private:
	PointLight* pointLight;
public:
	PointLightNode(Node* parentIN, PointLight* pointLight);
	void drawCall(Shader& shader);
	void cleanupCall(Shader& shader) {};
};




class SCENE_LIGHTS
{
public:

	std::vector<SpotLightNode*> spotLightNodes;
	std::vector<PointLightNode*> pointLightNodes;
	std::vector<DirLightNode*> dirLightNodes;
	SCENE_LIGHTS() = default;
	SCENE_LIGHTS(std::vector<SpotLightNode*> sptLtIN, std::vector<PointLightNode*> ptLtIN, std::vector<DirLightNode*> dirLts);
	
};

class SCENE_MODELS
{
public:
	Material_Manager scene_materials;
	std::vector<ModelNode*> modelNodes;
	SCENE_MODELS() = default;
};



class Scene : public Node {
private:
	unsigned int squareVAO;
	SCENE_LIGHTS lights;
	SCENE_MODELS models;
	std::vector<std::string> loadedModelPaths;
	Camera* mainCamera;
	double mouseLastX, mouseLastY;
	bool hasMouseMovedOnce = false;
	int SCR_WIDTH, SCR_HEIGHT;
	GLFWwindow* currentWindow;
	unsigned int gPosition, gNormal, gAlbedoSpec;
	unsigned int gBuffer;
	unsigned int colorTexture, brightTexture;
	unsigned int colorBuffer;
	bool gaussianBlurState = false;
	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffer[2];
	void reGetWindowDimensions()
	{
		glfwGetWindowSize(this->currentWindow, &this->SCR_WIDTH, &this->SCR_HEIGHT);
	}
public:
	Scene(GLFWwindow* windowUsing, Camera* camera) {
		this->typeOfNode = SCENE_NODE;
		parent = nullptr;
		internal_name = "root_node";
		currentWindow = windowUsing;
		mainCamera = camera;
		reGetWindowDimensions();
	}
	Scene(GLFWwindow* windowUsing, std::string sceneFileName);
	void setMainCamera(Camera* camera);
	
	void addLight(Node* node);
	void addModel(Node* node);

	Camera* getMainCamera()
	{
		return mainCamera;
	}
	unsigned int getFinalImage();
	unsigned int getgNormal();
	void setCurrentWindow(GLFWwindow* window);
	void setGaussianBlurState(bool gsbState);
	void moveAround(float deltaTime);
	void lookAround();
	void setupDeferredShading();
	void clean(Shader& shader);

	/*
	ModelNode* getModelNodeByName(std::string nameIN)
	{
		for (int i = 0; i < meshDrawList.size();++i)
		{
			if (meshDrawList[i]->name == nameIN)
			{
				return (ModelNode*)meshDrawList[i];
			}
		}
	}
	*/


	void drawCall(Shader& shader) {}
	void draw(Shader& shader);
	void drawFinal(Shader& shader, Shader& gaussianBlurShader, Shader& additiveMixingShader);
	void ShadowsPass(Shader& shader);
	void GeometryPass(Shader& shader);
	void LightingPass(Shader& shader);
	void reloadScene();
};

void doJobOnAllNodesBelow(Node* root);

Scene* getRootScene(Node* node);