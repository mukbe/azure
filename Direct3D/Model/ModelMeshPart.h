#pragma once
#include "../Interfaces/ICloneable.h"

class ModelMeshPart : public ICloneable
{
	VertexType(VertexTextureBlendNT)
public:
	friend class Model;
	friend class ModelMesh;
	friend class Models;
	friend class ObjectData;
	friend class ObjectInstance;
public:
	virtual void Clone(void** clone);
private:
	ModelMeshPart();
	~ModelMeshPart();

	void Binding();
	void Render(class Shader* shader);

	ID3D11Buffer* GetVertexBuffer()const { return vertexBuffer; }
	ID3D11Buffer* GetIndexBUffer()const { return indexBuffer; }
	UINT GetIndexCount()const { return indices.size(); }
	UINT GetVertexCount()const { return vertices.size(); }
	class Material* GetMaterial()const { return material; }
private:
	class Material * material;
	wstring materialName;

	class ModelMesh* parent;

	vector<VertexType> vertices;
	vector<UINT> indices;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
};
