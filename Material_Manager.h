#pragma once
#include "MaterialObj.h"
#include <vector>

class Material_Manager
{
private:
	std::vector<Material*> materials;
public:
	Material_Manager() = default;
	Material_Manager(unsigned int size);
	
	void addMaterial(Material* Mat);
	void removeMaterial(unsigned int index);

	Material* operator[] (unsigned int index);

};

