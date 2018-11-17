#pragma once
#include "./Object/GameObject/GameObject.h"

typedef struct GrassData
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 scale;
	D3DXVECTOR3 normal;
	UINT		textureID;

	GrassData()
		:position(0.f, 0.f, 0.f), normal(0.f, 0.f, 0.f), scale(1.f, 1.f), textureID(0) {}
	GrassData(D3DXVECTOR3 pos, D3DXVECTOR2 scale, D3DXVECTOR3 normal, UINT id)
		:position(pos), scale(scale), normal(normal), textureID(id) {}
}*LPGRASSDATA;

class GrassRenderer : public GameObject
{
private:
	Synthesize(UINT	,maxGrassCount,MaxGrassCount)
	vector<GrassData>			grassList;
	ID3D11Buffer*				grassBuffer;
	class Shader*				shader;
	class Shader*				shadowShader;
	vector<class Texture*>		textureList;

	class Terrain* terrain;
private:
	class Texture* currentTexture;
	UINT currentTextureIndex;
	GrassData createData;
public:
	GrassRenderer(UINT maxGrass = 300);
	virtual ~GrassRenderer();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void PrevRender();
	virtual void Render();
	virtual void UIUpdate();
	virtual void UIRender();
	virtual void ShadowRender();
	virtual void DebugRender() {}

	virtual void SaveData(Json::Value* parent)override;
	virtual void LoadData(Json::Value* parent)override;
public:
	void AddCrossGrass(GrassData data);
	void AddGrass(GrassData data);
	void LinkTerrain();
private:
	void CreateBuffer(bool mapTool = true);
	void UpdateBuffer();
	void ResetData();
};

