#pragma once
#include "MaterialObj.h"
#include <vector>

class Material_Manager
{
private:
	std::vector<Material*> materials;
	Material createDefaultMat(unsigned int index);
public:
	Material_Manager();
	Material_Manager(unsigned int size);
	
	void addMaterial(Material* Mat);
	void removeMaterial(unsigned int index);

	unsigned int numMaterials()
	{
		return materials.size();
	}

	Material* operator[] (unsigned int index);

};
