#include "Node.h"
#include <iostream>



void printNodeIdentity(Node* node) {
	std::cout << node->internal_name << " <--";
}

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
	std::cout << std::endl;
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
