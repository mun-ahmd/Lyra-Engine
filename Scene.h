#pragma once
#include "meshObj.h"
#include "cameraObj.h"
#include "lightObj.h"
#include "Material_Manager.h"
#include <sstream>
#include "Node.h"





class ModelNode : public Node {
private:
	Model* model;
public:
	
	ModelNode(Node* parentIN, Model* model);
	void drawCall(Shader& shader);
	void cleanupCall(Shader& shader);
	std::vector<Mesh*>* getMeshes();
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
	//Lights will be stored in an interleaved format in both buffer and cpu
	//in the order of point, spot, dir
	unsigned int lightBuffer;
	SCENE_LIGHTS();
	SCENE_LIGHTS(std::vector<SpotLightNode*> sptLtIN, std::vector<PointLightNode*> ptLtIN, std::vector<DirLightNode*> dirLts);
	//todo a-lot
	void addPointLight(PointLightNode* ptLight);
	void addSpotLight(SpotLightNode* sptLight);
	void addDirLight(DirLightNode* dirLight);
	
private:
	struct lightGroup
	{
		PointLightNode* point;
		SpotLightNode* spot;
		DirLightNode* dir;
	};
	std::vector<lightGroup> lights;
	unsigned int num_ptlights;
	unsigned int num_sptlights;
	unsigned int num_dirlights;
	void setupLightBuffer();
	void updateLightBuffer();
};


class SCENE_MODELS
{
public:
	Material_Manager scene_materials;
	std::vector<Mesh*> meshes;
	std::vector<Node*> nodes;
	SCENE_MODELS()
	{
		meshes.reserve(10);
		scene_materials = Material_Manager();
	}
	void sortMeshes();
	void newDraw();
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
	Material_Manager matmanager;
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
	
	void redesignDrawPrototype(Shader& shader);

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

	//TODO NO DEFINATIONS RIGHT NOW
	void drawCall(Shader& shader) {}
	void draw(Shader& shader) {}
	void drawFinal(Shader& shader, Shader& gaussianBlurShader, Shader& additiveMixingShader) {}
	void ShadowsPass(Shader& shader) {}
	void GeometryPass(Shader& shader) {}
	void LightingPass(Shader& shader) {}
	void reloadScene();

	void newDraw(Shader& shader);
};

void doJobOnAllNodesBelow(Node* root);

Scene* getRootScene(Node* node);