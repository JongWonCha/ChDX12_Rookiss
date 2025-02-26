#include "pch.h"
#include "AssimpManager.h"
#include "Resources.h"
#include "Mesh.h"

AssimpManager::AssimpManager()
{

}

void AssimpManager::Import(string name, string file)
{
	_name = name;

	_importer = new Assimp::Importer();

	_scene = _importer->ReadFile(file, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality);

	if (_scene == nullptr) return;

	aiMesh* mesh = _scene->mMeshes[0];
	vector<Vertex> vertices(mesh->mNumVertices);

	for (uint32 i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex v;
		vertices[i].pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		vertices[i].normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		vertices[i].tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };

		if (mesh->mTextureCoords[0])
			vertices[i].uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		else
			vertices[i].uv = { 0.0f, 0.0f };
	}

	vector<uint32> indices;
	for (uint32 i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace& face = mesh->mFaces[i];
		for (uint32 j = 0; j < face.mNumIndices; ++j)
		{
			indices.emplace_back(face.mIndices[j]);
		}
	}
	shared_ptr<Mesh> m = make_shared<Mesh>();
	m->Init(vertices, indices);
	GET_SINGLE(Resources)->Add<Mesh>(L"ass", m);
	//assert(_scene != nullptr);
}
