#pragma once
#include <string>
#include <vector>
#include "meshObj.h"
class Mesh_Importer
{
public:
	void operator()(std::string file_path, unsigned int offset_from_beginning)
	{
		
		std::ifstream file;
		file.open(file_path, std::ios::binary | std::ios::in);
		file.seekg(offset_from_beginning, file.beg);
		
		int no_of_vertices,no_of_indices;
		file.read((char*)&no_of_vertices, sizeof(int));
		file.read((char*)&no_of_indices, sizeof(int));

		std::vector<Vertex> vertices(no_of_vertices);
		std::vector<unsigned int> indices(no_of_indices);

		file.read((char*)&vertices[0], no_of_vertices * sizeof(Vertex));
		file.read((char*)&indices[0], no_of_indices* sizeof(unsigned int));

		unsigned int mat_index;
		file.read((char*)&mat_index, sizeof(unsigned int));
		
		file.close();

		mesh = Mesh(vertices, indices, mat_index);

		
	}
	Mesh mesh;
};

