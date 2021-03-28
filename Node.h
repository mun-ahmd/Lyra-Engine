#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <string>


class Transform
{
private:
	//outdated:
	//todo use quaternions on cpu and send matrices to gpu
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