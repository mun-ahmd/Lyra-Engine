#pragma once
#include <string>
#include "Scene.h"
#include "Material_Manager.h"

class Lyra_Project
{
private:
	std::string projectDir;
	std::string projectFile;

	std::vector<Scene*> project_scenes;

	//.lyra
	void createProject();
	void loadProject();
	void saveProject();

	void drawScenes();


public:
	Lyra_Project(std::string filepath);
	
	//TODO delete this when the .lyra format is implemented:
	Lyra_Project();

};

