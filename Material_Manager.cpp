#include "Material_Manager.h"
#include <vector>

Material_Manager::Material_Manager(unsigned int size)
{
	this->materials = std::vector<Material*>(size);
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
