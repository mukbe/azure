#pragma once

class TerrainTool : public SceneNode
{
public:
	TerrainTool();
	virtual~TerrainTool();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();

	void PreRender();
	void Render();
	void UIRender();
	
private:

	class Terrain* terrain;
	class FreeCamera *freeCamera;
	class Environment::Sun* sun;
	class Scattering* test;

	class ParticleEmitterBase* particle;
};

