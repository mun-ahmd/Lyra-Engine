#pragma once
#include <string>
#include "meshObj.h"
class Mesh_Importer
{
public:
	void operator()(std::string file_path, unsigned int offset_from_beginning)
	{
		std::ifstream file;
		file.open(file_path, std::ios::binary | std::ios::in);
		file.seekg(offset_from_beginning, file.beg);

	}
	Mesh mesh;
};

