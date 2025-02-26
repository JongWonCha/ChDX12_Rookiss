#pragma once
#include "Material.h"
class AssimpManager
{
public:
	AssimpManager();
	//~AssimpManager();

	void Import(string name, string file);

//private:
//	void ReadMaterial();
//	void WriterMaterial();
//	string CreateTexture(string file);

private:
	Assimp::Importer* _importer;
	const aiScene* _scene;

	string _name;


};

