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
}*LPGRASSDATA;

class GrassRenderer : public GameObject
{
private:
	Synthesize(UINT	,maxGrassCount,MaxGrassCount)
	vector<GrassData>			grassList;
	ID3D11Buffer*				grassBuffer;
	class Shader*				shader;
	vector<class Texture*>		textureList;
	ID3D11ShaderResourceView* nullView[10];
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
	virtual void UIRender();
	virtual void ShadowRender();
	virtual void UIUpdate() {}
	virtual void DebugRender() {}

public:
	void AddGrass(GrassData data);
private:
	void CreateBuffer(bool mapTool = true);
	void UpdateBuffer();
};

