#include "Project.h"
#include <iostream>
#include <fstream>


//TODO write window creation code


Lyra_Project::Lyra_Project(std::string filepath)
{
	if (filepath.find(".lyra") == std::string::npos)
	{
		std::cout << "The file is not valid, only .lyra files can be loaded";
		return;
	}

	int lastbackslash = filepath.find_last_of(R"(\)");
	this->projectFile = filepath.substr(lastbackslash, filepath.size() - lastbackslash);
	this->projectDir = filepath.substr(0, lastbackslash);

}


Lyra_Project::Lyra_Project()
{
	//Scene* scene1 = new Scene("scenefile.txt");
}


void Lyra_Project::createProject()
{
	std::ofstream new_file;
	new_file.open(this->projectDir + this->projectFile);
	if (!new_file)
	{
		std::cout << "error while creating file";
	}

	//TODO when the .lyra file format is decided, complete

	new_file.close();
}

void Lyra_Project::loadProject()
{
	std::ifstream file;
	file.open(this->projectDir + this->projectFile);
	if (!file)
	{
		std::cout << "error while creating file";
	}

	//TODO when the .lyra file format is decided, complete

	file.close();
}

void Lyra_Project::saveProject()
{
	//TODO when the .lyra file format is decided, complete
}