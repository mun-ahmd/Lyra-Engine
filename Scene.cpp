#include "Scene.h"
#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <ctime>
#include <stack>

using namespace std;




struct NodeTypeCaster
{
	Node* node;

	NodeTypeCaster(Node* nodeIN)
	{
		node = nodeIN;
	}
	operator ModelNode*()
	{
		if (node->getNodeType() != MODEL_NODE)
		{
			std::cout << "WRONG CONVERSION ATTEMPTED THE NODE: " << node->name << " IS NOT A MODEL NODE! NULLPTR RETURNED";
			return nullptr;
		}

		return (static_cast<ModelNode*>(node));
	}

	operator DirLightNode* ()
	{
		if (node->getNodeType() != DIR_LIGHT_NODE)
		{
			std::cout << "WRONG CONVERSION ATTEMPTED THE NODE: " << node->name << " IS NOT A DIRLIGHT NODE! NULLPTR RETURNED";
			return nullptr;
		}

		return (static_cast<DirLightNode*>(node));
	}

	operator PointLightNode* ()
	{
		if (node->getNodeType() != POINT_LIGHT_NODE)
		{
			std::cout << "WRONG CONVERSION ATTEMPTED THE NODE: " << node->name << " IS NOT A POINT LIGHT NODE! NULLPTR RETURNED";
			return nullptr;
		}

		return (static_cast<PointLightNode*>(node));
	}

	operator SpotLightNode* ()
	{
		if (node->getNodeType() != SPOT_LIGHT_NODE)
		{
			std::cout << "WRONG CONVERSION ATTEMPTED THE NODE: " << node->name << " IS NOT A SPOTLIGHT NODE! NULLPTR RETURNED";
			return nullptr;
		}

		return (static_cast<SpotLightNode*>(node));
	}

	operator Scene* ()
	{
		if (node->getNodeType() != SCENE_NODE)
			//can also check if the parent is a nullptr
		{
			std::cout << "WRONG CONVERSION ATTEMPTED THE NODE: " << node->name << " IS NOT A SCENE NODE! NULLPTR RETURNED";
			return nullptr;
		}

		return (static_cast<Scene*>(node));
	}
};


Transform::Transform()
{
	translation = glm::vec3(0);
	rotation = glm::vec4(0);
	scaleSize = glm::vec3(0);
}

glm::mat4 Transform::getModelMatrix()
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, this->translation);
	model = glm::scale(model, this->scaleSize);
	model = glm::rotate(model, this->rotation.w, glm::vec3(this->rotation));
	return model;
}


Scene* getRootScene(Node* node)
{
	return (Scene*)NodeTypeCaster(node->getRootParent());
}







int copiesCount = 0;

void printNodeIdentity(Node* node) {
	cout << node->internal_name << " <--";
}

void doJobOnAllNodesBelow(Node* root)
{
	if (root == nullptr)
		return;

	// Standard level order traversal code 
	// using queue 
	queue<Node*> q;  // Create a queue 
	q.push(root); // Enqueue root  
	while (!q.empty())
	{
		int n = q.size();

		// If this node has children 
		while (n > 0)
		{
			//So it goes from the top and leaves nothing left
			// Dequeue a node, tell it to do its job and add its children to the queue
			Node* p = q.front();
			std::cout << p->internal_name;
			for (int i = 0; i < p->getChildren().size(); i++)
				q.push(p->getChildren()[i]);
			q.pop();


			n--;
		}

		cout << endl; // Print new line between two levels 
	}
}

//Brute forcing for example takes 0.0035638s(avg) and requires hardcoding for different depths.
//0.0024476s-0.004s is time taken by this queue method.

//start of member functions of Node obj

Node::Node() 
{
	//this class is a template
}

Node::~Node()
{
	if (this->parent != nullptr)
	{
		this->parent->removeChild(this);
	}
	for (int i = 0; i < this->children.size(); ++i)
	{
		this->children[i]->~Node();
	}
	//return
}


void Node::changeParent_only(Node* newParentNode)
{
	this->parent = newParentNode;
}

void Node::addChildren(Node* nodeToAdd)
{
	this->children.push_back(nodeToAdd);
	nodeToAdd->changeParent_only(this);
}

void Node::addChildren(Node* childrenNodes, int number_of_children)
{
	for (int i = 0; i < number_of_children; ++i)
	{
		this->children.push_back(childrenNodes + i);
		(childrenNodes + i)->changeParent_only(this);
	}
}

void Node::addChildren(std::vector<Node*> childrenNodes)
{
	for (int i = 0; i < children.size(); ++i)
	{
		this->children.push_back(childrenNodes[i]);
		childrenNodes[i]->changeParent_only(this);
	}
}

void Node::removeChild(Node* child) {
	for (int i = 0; i < this->children.size(); ++i)
	{
		if (this->children[i] == child)
		{
			this->children.erase(this->children.begin() + i);
			return;
		}
	}
}

void Node::printDirectory() {
	this->doInParentPath(printNodeIdentity);
	cout << endl;
}

std::vector<Node*> Node::getChildren() {
	return this->children;
}

void Node::doInParentPath(void func(Node*)) {
	Node* currentNode = this;
	while (currentNode->parent != nullptr) {
		func(currentNode);
		currentNode = currentNode->parent;
	}
}

Node* Node::getRootParent()
{
	Node* currentNode = this;
	while (currentNode->parent != nullptr) {
		currentNode = currentNode->parent;
	}
	return currentNode;
}


NODE_TYPE Node::getNodeType()
{
	return this->typeOfNode;
}


//End of memeber functions of Node obj


//Node types' definitions
int meshCount = 0;
int emptyCount = 0;
int ModelCount = 0;
int LightCount = 0;




ModelNode::ModelNode(Node* parentIN,Model* model_in) 
{
	this->typeOfNode = MODEL_NODE;
	model = model_in;
	ModelNode::internal_name = "Model" + std::to_string(ModelCount);
	++ModelCount;
	parentIN->addChildren(this);
	getRootScene(parentIN)->addModel(this);
};

void ModelNode::cleanupCall(Shader& shader)
{
	((Model*)model)->Cleanup(shader);
	delete(((Model*)model));
}

void ModelNode::drawCall(Shader& shader) 
{
	shader.addUniformMat4("model", &this->transform.getModelMatrix());
	((Model*)model)->Draw(shader);
}




void splitStringOnce(std::string& stringToSplit, std::string& secondPart, char delimiter)
{
	//it will put part before delimiter in first arguement and the part after it in the second arguement]
	std::string partOne;
	secondPart.clear();

	bool notSplitOnce = true;
	for (int index = 0; index < stringToSplit.size();++index)
	{
		if (notSplitOnce && stringToSplit[index] == delimiter)
		{
			notSplitOnce = false;
			continue;
		}
		if (notSplitOnce)
		{
			partOne += stringToSplit[index];
		}
		else
		{
			secondPart += stringToSplit[index];
		}
	}
	stringToSplit = partOne;
}
int dirLightCount = 0;
DirLightNode::DirLightNode(Node* parentIN, DirectionalLight* dirLightIN)
{
	this->typeOfNode = DIR_LIGHT_NODE;
	this->dirLight = ((DirectionalLight*)dirLightIN);
	DirLightNode::internal_name = "dirLight" + std::to_string(dirLightCount);
	++dirLightCount;
	parentIN->addChildren(this);
	getRootScene(parentIN)->addLight(this);
}

void DirLightNode::cleanupCall(Shader& shader)
{
	delete(((DirectionalLight*)dirLight));
}

void DirLightNode::drawCall(Shader& shader)
{
	//CONVENTION!
	//now, directional light will be named as dirLight0, dirLight1, etc... 
	//multiple such uniforms will be present in the shader
	((DirectionalLight*)dirLight)->addToShader(shader, internal_name);
}

void DirLightNode::setupShadows()
{
	((DirectionalLight*)dirLight)->setupShadowGeneration();
}

void DirLightNode::shadowDrawCall(Shader& shader, unsigned int squareVAO)
{
	((DirectionalLight*)dirLight)->generateShadow(shader, squareVAO);
}

int spotLightCount = 0;
SpotLightNode::SpotLightNode(Node* parentIN, Spotlight* spotLightIN)
{
	this->typeOfNode = SPOT_LIGHT_NODE;
	spotlight = ((Spotlight*)spotLightIN);
	SpotLightNode::internal_name = "spotLight" + std::to_string(spotLightCount);
	++spotLightCount;
	parentIN->addChildren(this);
	getRootScene(parentIN)->addLight(this);
}
void SpotLightNode::drawCall(Shader& shader)
{
	//CONVENTION!
	//now, spotlights will be named as spotLight0, spotLight1, etc... 
	//multiple such uniforms will be present in the shader
	((Spotlight*)spotlight)->addToShader(shader, internal_name);
}

int pointLightCount = 0;
PointLightNode::PointLightNode(Node* parentIN, PointLight* pointLight)
{
	this->typeOfNode = POINT_LIGHT_NODE;
	pointLight = ((PointLight*)pointLight);
	PointLightNode::internal_name = "pointLight" + std::to_string(pointLightCount);
	++pointLightCount;
	parentIN->addChildren(this);
	getRootScene(parentIN)->addLight(this);
}

void PointLightNode::drawCall(Shader& shader)
{
	//CONVENTION!
	//now, spotlights will be named as spotLight0, spotLight1, etc... 
	//multiple such uniforms will be present in the shader
	((PointLight*)pointLight)->addToShader(shader, internal_name);
}


DirLightNode createDirLightNode
(std::vector<std::string>::const_iterator tokenValueBegin,
	std::vector<std::string>::const_iterator tokenValueEnd,
	Node* parent)
{
	glm::vec3 color(1);glm::vec3 direction(1);
	std::vector<std::string> tokenValuePairs(tokenValueBegin + 2, tokenValueEnd);
	for (int index = 0; index < tokenValuePairs.size(); ++index)
	{
		std::string token = tokenValuePairs[index]; std::string value;
		splitStringOnce(token, value, '=');
		if (token == "color")
		{
			int currCommaIndex = value.find(',');
			std::string currSubStr = value.substr(1, currCommaIndex);
			color.x = std::stof(currSubStr);

			int lastCommaIndex = currCommaIndex;
			currCommaIndex = value.find(',', (lastCommaIndex + 1));
			currSubStr = value.substr((lastCommaIndex + 1), currCommaIndex);
			color.y = std::stof(currSubStr);

			lastCommaIndex = currCommaIndex;
			currCommaIndex = value.find(',', (lastCommaIndex + 1));
			currSubStr = value.substr((lastCommaIndex + 1), currCommaIndex);
			color.z = std::stof(currSubStr);
		}
		else if (token == "direction")
		{
			int currCommaIndex = value.find(',');
			std::string currSubStr = value.substr(1, currCommaIndex);
			direction.x = std::stof(currSubStr);

			int lastCommaIndex = currCommaIndex;
			currCommaIndex = value.find(',', (lastCommaIndex + 1));
			currSubStr = value.substr((lastCommaIndex + 1), currCommaIndex);
			direction.y = std::stof(currSubStr);

			lastCommaIndex = currCommaIndex;
			currCommaIndex = value.find(',', (lastCommaIndex + 1));
			currSubStr = value.substr((lastCommaIndex + 1), currCommaIndex);
			direction.z = std::stof(currSubStr);
		}
	}
	DirectionalLight* dirlight = new DirectionalLight(direction, color);
	DirLightNode* dirLightNodeToReturn = new DirLightNode(parent, dirlight);
	dirLightNodeToReturn->name = *(tokenValueBegin + 1);


	return *dirLightNodeToReturn;

}


ModelNode createModelNode
(std::vector<std::string>::const_iterator tokenValueBegin,
	std::vector<std::string>::const_iterator tokenValueEnd,
	Node* parent)
{
	std::string filepath;
	std::vector<std::string> tokenValuePairs(tokenValueBegin + 2, tokenValueEnd);
	for (int index = 0; index < tokenValuePairs.size(); ++index)
	{
		std::string token = tokenValuePairs[index]; std::string value;
		splitStringOnce(token, value, '=');
		if (token == "location")
		{
			filepath = value;
		}

	}
	Model* model = new Model(filepath);
	ModelNode* modelNodeToReturn = new ModelNode(parent, model);
	modelNodeToReturn->name = *(tokenValueBegin + 1);
	return *modelNodeToReturn;
}

Camera* createCameraForScene
(std::vector<std::string>::const_iterator tokenValueBegin,
	std::vector<std::string>::const_iterator tokenValueEnd)
{
	glm::vec3 position(1); glm::vec3 front(1);glm::vec3 up(1);
	std::vector<std::string> tokenValuePairs(tokenValueBegin + 1, tokenValueEnd);
	for (int index = 0; index < tokenValuePairs.size(); ++index)
	{
		std::string token = tokenValuePairs[index]; std::string value;
		splitStringOnce(token, value, '=');
		if (token == "default")
		{
			if (value == "true")
			{
				Camera* camToReturn = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0, 1, 0));
				return camToReturn;
			}
		}

	}

	Camera* camToReturn = new Camera(position, up, front);
	return camToReturn;
}


SCENE_LIGHTS::SCENE_LIGHTS(std::vector<SpotLightNode*> sptLtIN, std::vector<PointLightNode*> ptLtIN, std::vector<DirLightNode*> dirLts)
{
	this->spotLightNodes = sptLtIN;
	this->pointLightNodes = ptLtIN;
	this->dirLightNodes = dirLts;
}

void Scene::addLight(Node* node)
{
	NODE_TYPE type = node->getNodeType();
	if (type == DIR_LIGHT_NODE)
	{
		this->lights.dirLightNodes.push_back(NodeTypeCaster(node));
	}
	else if (type == SPOT_LIGHT_NODE)
	{
		this->lights.spotLightNodes.push_back(NodeTypeCaster(node));
	}
	else if (type == POINT_LIGHT_NODE)
	{
		this->lights.pointLightNodes.push_back(NodeTypeCaster(node));
	}
	else
	{
		std::cout << "NON LIGHT NODE WAS ATTEMPTED TO BE PUT IN SCENE_LIGHTS :: IGNORED\n";
	}
}

void Scene::addModel(Node* node)
{
	if (node->getNodeType() == MODEL_NODE)
	{
		this->models.modelNodes.push_back(NodeTypeCaster(node));
	}
	else
	{
		std::cout << "NON MODEL NODE WAS ATTEMPTED TO BE PUT IN SCENE_MODELS :: IGNORED\n";
	}
}

Scene::Scene(GLFWwindow* windowUsing, std::string sceneFilename)
{
	this->typeOfNode = SCENE_NODE;
	std::ifstream sceneFile;
	std::vector<std::vector<std::string>> sceneElements;
	this->parent = NULL;
	this->internal_name = "root_node";
	this->currentWindow = windowUsing;


	sceneFile.open(sceneFilename, std::ios::in);
	if (sceneFile.is_open())
	{
		std::string temp;
		while (std::getline(sceneFile, temp))
		{
			std::stringstream currLine(temp);
			std::vector<std::string> currElement;
			std::string element;
			while (currLine >> element)
			{
				if (element == "," || element.find(",") == 0 || element.find(",") == element.size() - 1)
				{
					//currElement.push_back(element);		//puts commas into the vector, just for setting up, can remove if you want
					continue;
				}
				//else if(element.find(""))
				currElement.push_back(element);
			}

			sceneElements.push_back(currElement);
		}
	}

	sceneFile.close();

	for (int i = 0;i < sceneElements.size();++i)
	{
		if (sceneElements[i][0] == "model")
		{
			createModelNode(sceneElements[i].begin(), sceneElements[i].end(), this);
		}

		if (sceneElements[i][0] == "dirLight")
		{
			createDirLightNode(sceneElements[i].begin(), sceneElements[i].end(), this);
		}
		else if (sceneElements[i][0] == "camera")
		{
			Camera* sceneCamera = createCameraForScene(sceneElements[i].begin(), sceneElements[i].end());
			setMainCamera(sceneCamera);
		}
	}


	for (int i = 0;i < sceneElements.size();++i)
	{
		for (int x = 0; x < sceneElements[i].size();++x)
		{
			std::cout << sceneElements[i][x];
		}
		std::cout << "\n";
	}
	reGetWindowDimensions();

}

void Scene::reloadScene()
{

}
void Scene::moveAround(float deltaTime) {
	if (glfwGetKey(this->currentWindow, GLFW_KEY_W) == GLFW_PRESS)
		this->mainCamera->keyboardMovementProcess(FORWARD, deltaTime);
	if (glfwGetKey(this->currentWindow, GLFW_KEY_S) == GLFW_PRESS)
		this->mainCamera->keyboardMovementProcess(BACKWARD, deltaTime);
	if (glfwGetKey(this->currentWindow, GLFW_KEY_A) == GLFW_PRESS)
		this->mainCamera->keyboardMovementProcess(LEFT, deltaTime);
	if (glfwGetKey(this->currentWindow, GLFW_KEY_D) == GLFW_PRESS)
		this->mainCamera->keyboardMovementProcess(RIGHT, deltaTime);
}

void Scene::lookAround() {
	double xPos, yPos;
	double lastX = this->mouseLastX; double lastY = this->mouseLastY;
	glfwGetCursorPos(this->currentWindow, &xPos, &yPos);

	//if first mouse input....
	//this stops massive offset when starting due to large diff b/w last pos and curr pos of mouse
	if (this->hasMouseMovedOnce == false) {
		lastX = xPos;
		lastY = yPos;
		this->hasMouseMovedOnce = true;
	}
	//Question, would it be worth adding an if to check if mouse hasn't been moved, or would that just be a waste of performance when it is being moved?

	double xOffset = xPos - lastX;
	double yOffset = yPos - lastY;

	lastX = xPos;
	lastY = yPos;
	this->mouseLastX = lastX; this->mouseLastY = lastY;
		
	this->mainCamera->mouseLookProcess(xOffset, yOffset, true);

}

unsigned int Scene::getgNormal()
{
	return gNormal;
}

void Scene::setGaussianBlurState(bool gsbState)
{
	gaussianBlurState = gsbState;
}

void Scene::clean(Shader& shader)
{
	for (int i = 0;i < this->models.modelNodes.size();++i)
	{
		this->models.modelNodes[i]->cleanupCall(shader);
		delete(this->models.modelNodes[i]);
	}
	for (int i = 0;i < this->lights.dirLightNodes.size();++i)
	{
		this->lights.dirLightNodes[i]->cleanupCall(shader);
		delete(this->lights.dirLightNodes[i]);
	}
	for (int i = 0;i < this->lights.spotLightNodes.size();++i)
	{
		this->lights.spotLightNodes[i]->cleanupCall(shader);
		delete(this->lights.spotLightNodes[i]);
	}
	for (int i = 0;i < this->lights.pointLightNodes.size();++i)
	{
		this->lights.pointLightNodes[i]->cleanupCall(shader);
		delete(this->lights.pointLightNodes[i]);
	}
	delete(this->mainCamera);
}
bool firstDraw = true;
void Scene::draw(Shader& shader) {
	if (firstDraw)
	{
		//model = glm::scale(model, modelScale);
		firstDraw = false;
	}
	glUseProgram(shader.program);
	shader.addUniformVec3("viewPos", mainCamera->position);
	shader.addUniformMat4("view", &mainCamera->getViewMatrix());
	for (int light = 0;light<lights.dirLightNodes.size();++light) 
	{
		lights.dirLightNodes[light]->drawCall(shader);
	}
	for (int obj = 0;obj<models.modelNodes.size();++obj)
	{
		models.modelNodes[obj]->drawCall(shader);
	}
}

void DirLightNode::shadowSetupCall()
{
	((DirectionalLight*)dirLight)->setupShadowGeneration();
}
bool firstShadow = true;
void Scene::ShadowsPass(Shader& shader)
{
	if (firstShadow)
	{
		for (int i = 0; i < this->lights.dirLightNodes.size(); ++i)
		{
			this->lights.dirLightNodes[i]->shadowSetupCall();
		}
		firstShadow = false;
	}
	for (int i = 0; i < this->lights.dirLightNodes.size(); ++i)
	{
		this->lights.dirLightNodes[i]->shadowDrawCall(shader, this->squareVAO);
		this->draw(shader);
	}

	glViewport(0, 0, this->SCR_WIDTH, this->SCR_HEIGHT);
}

bool firstieDraw = true;
void Scene::GeometryPass(Shader& shader)
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (firstieDraw)
	{
		firstieDraw = false;
	}
	glUseProgram(shader.program);
	shader.addUniformMat4("view", &mainCamera->getViewMatrix());
	for (int obj = 0;obj < this->models.modelNodes.size();++obj)
	{
		this->models.modelNodes[obj]->drawCall(shader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::setupDeferredShading()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// configure g-buffer framebuffer
// ------------------------------
	glCreateFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->SCR_WIDTH, this->SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->SCR_WIDTH, this->SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->SCR_WIDTH, this->SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->SCR_WIDTH, this->SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->SCR_WIDTH, this->SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Color framebuffers :
	glGenFramebuffers(1, &colorBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, colorBuffer);

	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->SCR_WIDTH, this->SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

	glGenTextures(1, &brightTexture);
	glBindTexture(GL_TEXTURE_2D, brightTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->SCR_WIDTH, this->SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightTexture, 0);

	GLenum attachments2[2] = { GL_COLOR_ATTACHMENT0 ,GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments2);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	float squareVerts[] = {
		// positions          //texture coords
		 1.f,  1.f, 0.0f,   1.0f, 1.0f,   // top right
		 1.f, -1.f, 0.0f,   1.0f, 0.0f,   // bottom right
		-1.f, -1.f, 0.0f,	  0.0f, 0.0f,   // bottom left
		-1.f, -1.f, 0.0f,	  0.0f, 0.0f,   // bottom left
		-1.f,  1.f, 0.0f,   0.0f, 1.0f,    // top left 
		 1.f,  1.f, 0.0f,   1.0f, 1.0f   // top right

	};
	glCreateVertexArrays(1, &squareVAO);
	unsigned int squareBuffer;
	glCreateBuffers(1, &squareBuffer);
	glBindVertexArray(squareVAO);
	glBindBuffer(GL_ARRAY_BUFFER, squareBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVerts), squareVerts, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(squareVAO, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexArrayAttrib(squareVAO, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));

}

void Scene::LightingPass(Shader& shader)
{
	glBindFramebuffer(GL_FRAMEBUFFER, colorBuffer);
	//glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shader.program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	shader.addUniform1I("gPosition", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	shader.addUniform1I("gNormal", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	shader.addUniform1I("gAlbedoSpec", 2);

	glActiveTexture(GL_TEXTURE0);

	shader.addUniformVec3("viewPos", this->mainCamera->position);

	for (int lightIndex = 0; lightIndex < lights.dirLightNodes.size();++lightIndex)
	{
		lights.dirLightNodes[lightIndex]->drawCall(shader);
	}

	glBindVertexArray(squareVAO);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void addGaussianBlur(unsigned int brightTex, unsigned int* pingpongFBO, unsigned int* pingpongTex, unsigned int vao, Shader& shader)
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

void mixTexturesAdditive(unsigned int tex1, unsigned int tex2, unsigned int vao, unsigned int outputFbo, Shader& shader)
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

bool firstGaussianBlur= true;
void Scene::drawFinal(Shader& shader, Shader& gaussianBlurShader, Shader& additiveMixingShader)
{
	unsigned int finalTexToDraw = colorTexture;
	if (gaussianBlurState)
	{

		if (firstGaussianBlur)
		{
			glGenFramebuffers(2, pingpongFBO);
			glGenTextures(2, pingpongBuffer);

			for (unsigned int i = 0; i < 2; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
				glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_RGBA16F, this->SCR_WIDTH, this->SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL
				);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
				);
			}
			firstGaussianBlur = false;
		}
		addGaussianBlur(brightTexture, pingpongFBO, pingpongBuffer, squareVAO, gaussianBlurShader);
		mixTexturesAdditive(pingpongBuffer[0], colorTexture, squareVAO, 0, additiveMixingShader);
		return;

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(shader.program);
	glBindVertexArray(squareVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, finalTexToDraw);
	shader.addUniform1I("texture1", 0);
	glDrawArrays(GL_TRIANGLES,0, 6);
}

unsigned int Scene::getFinalImage()
{
	return Scene::colorTexture;
}

void Scene::setMainCamera(Camera* camera)
{
	this->mainCamera = camera;
}

void Scene::setCurrentWindow(GLFWwindow* window)
{
	this->currentWindow = window;
}









