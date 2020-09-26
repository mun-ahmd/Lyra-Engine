#pragma once
#include "meshObj.h"
#include "cameraObj.h"
#include "lightObj.h"
#include <sstream>


class Node {
protected:
	Node* parent;
	std::vector<Node*> children;
	void* objectContained;
	glm::mat4 model = glm::mat4(1);
	//I store scale, apply it before drawing, because it messes up group translations otherwise
	glm::vec3 modelScale = glm::vec3(1); 
	int copiesCount = 0;

	Node();
	Node(Node* parent, std::string identify);
	void propagateTranslationToChildren(glm::vec3 translation);
	void propagateRotationToChildren(float radians, glm::vec3 axis);

public:
	std::string identify;
	std::string name;

	virtual void addToLightDraw(Node*) { std::cout << "TRYING TO ADD Light TO Node NOT Scene\n"; }
	virtual void addToDraw(Node*) { std::cout << "TRYING TO ADD DrawObj TO Node NOT Scene\n"; }

	virtual void cleanupCall(Shader& shader){}

	Node* copyNode(Node*);

	void addChildren(Node* childrenIN);
	void removeChild(Node* child);

	virtual void drawCall(Shader& shader) = 0;
	void drawGroup(Shader& shader);
	
	Node* getRootParent();
	Node* getRootParent(void func(Node*));
	std::vector<Node*> getChildren();
	void printDirectory();

	void translate(glm::vec3 displacement);
	void rotate(float radians, glm::vec3 axis);
	void rotateDegrees(float degrees, glm::vec3 axis);
	void scale(float scaleFactor);
	void scale(glm::vec3 scale);
	void scale(float scaleX, float scaleY, float scaleZ = 1.0f);
	void setPosition(glm::vec3 newPosition);
	void setRotation(float newRot_inRadians, glm::vec3 axis);
	void setRotationDegrees(float newRot_inDegrees, glm::vec3 axis);
	void setScale(float newScale);

};

class Scene : public Node {
private:
	unsigned int squareVAO;
	std::vector<Node*> meshDrawList;
	std::vector<Node*> lightDrawList;
	std::vector<std::string> loadedModelPaths;
	Camera* mainCamera;
	double mouseLastX, mouseLastY;
	bool hasMouseMovedOnce = false;
	GLFWwindow* currentWindow;
	unsigned int gPosition, gNormal, gAlbedoSpec;
	unsigned int gBuffer;
	unsigned int colorTexture, brightTexture;
	unsigned int colorBuffer;
	bool gaussianBlurState = false;
	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffer[2];
public:
	Scene(GLFWwindow* windowUsing ,Camera* camera) {
		parent = NULL;
		identify = "root_node";
		currentWindow = windowUsing;
		mainCamera = camera;
	}
	Scene(GLFWwindow* windowUsing, std::string sceneFileName);
	void setMainCamera(Camera* camera);
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
	void setupDeferredShading(int SCR_WIDTH, int SCR_HEIGHT);
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

	void addToLightDraw(Node* lightNode)
	{
		this->lightDrawList.push_back(lightNode);
	} 
	void addToDraw(Node* NodeToAdd)
	{
		this->meshDrawList.push_back(NodeToAdd);
	}

	void drawCall(Shader& shader) {}
	void draw(Shader& shader);
	void drawFinal(Shader& shader, Shader& gaussianBlurShader, Shader& additiveMixingShader);
	void GeometryPass(Shader& shader);
	void LightingPass(Shader& shader);
	void reloadScene();
};

class MeshNode : public Node {
public:
	MeshNode(Node* parentIN);
	MeshNode(Node* parentIN,Mesh* mesh);

	void drawCall(Shader& shader);
};

class EmptyNode : public Node {
public:

	EmptyNode(Node* parentIN);

	void drawCall(Shader& shader);
};

class ModelNode : public Node {
private:
public:
	
	ModelNode(Node* parentIN, Model* model);
	void drawCall(Shader& shader);
	void cleanupCall(Shader& shader);
};

class LightNode : public Node {
public:
	LightNode(Node* parentIN, Light* light);

	void drawCall(Shader& shader);
};

class DirLightNode : public Node {
public:
	DirLightNode(Node* parentIN, DirectionalLight* dirLight);
	void cleanupCall(Shader& shader);
	void drawCall(Shader& shader);
};

class SpotLightNode : public Node {
public:
	SpotLightNode(Node* parentIN, Spotlight* spotLight);
	void drawCall(Shader& shader);

};

class PointLightNode : public Node {
public:
	PointLightNode(Node* parentIN, PointLight* pointLight);
	void drawCall(Shader& shader);
};

void doJobOnAllNodesBelow(Node* root);