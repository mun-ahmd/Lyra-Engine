#include "Material_Manager.h"
#include <vector>

Material Material_Manager::createDefaultMat(unsigned int index)
{
	return Material(index);
}

Material_Manager::Material_Manager()
{
	this->materials = std::vector<Material*>();
	Material* defaultMat = new Material(0);
	this->materials.push_back(defaultMat);
}

Material_Manager::Material_Manager(unsigned int size)
{
	this->materials = std::vector<Material*>();
	this->materials.reserve(size);
	Material* defaultMat = new Material(0);
	this->materials.push_back(defaultMat);
}

void Material_Manager::addMaterial(Material* mat)
{
	this->materials.push_back(mat);
}

void Material_Manager::removeMaterial(unsigned int index)
{
	std::vector<Material*> new_materials(this->materials.size() - 1);
	
	for (int i = 0; i < index; ++i)
	{
		new_materials.push_back(materials[i]);
	}
	for (int i = index + 1; i < materials.size(); ++i)
	{
		new_materials.push_back(materials[i]);
	}
	this->materials = new_materials;
}

Material* Material_Manager::operator[](unsigned int index)
{
	return this->materials[index];
}
