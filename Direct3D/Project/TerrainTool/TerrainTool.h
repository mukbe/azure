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

	void Render();
};

