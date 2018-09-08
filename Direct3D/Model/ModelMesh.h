#pragma once
#include "../Interfaces/ICloneable.h"

class ModelMesh : public ICloneable
{
public:
	friend class Model;
	friend class Models;
	friend class ObjectData;
	friend class InstanceRenderer;
public:
	virtual void Clone(void** clone);

	void SetWorld(D3DXMATRIX& world);
	void Render(class Shader* shader);
	wstring Name() { return name; }

	int ParentBoneIndex() { return parentBoneIndex; }
	class ModelBone* ParentBone() { return parentBone; }
	class WorldBuffer* GetWorldBuffer()const { return worldBuffer; }
	vector<class ModelMeshPart*> GetMeshParts()const { return meshParts; }
private:
	void Binding();

public:
	ModelMesh();
	~ModelMesh();
private:

	wstring name;

	int parentBoneIndex;
	class ModelBone* parentBone;

	class WorldBuffer* worldBuffer;
	vector<class ModelMeshPart *> meshParts;
};