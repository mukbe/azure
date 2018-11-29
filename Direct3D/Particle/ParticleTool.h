#pragma once
#include "Particle.h"

class ParticleTool
{
public:
	ParticleTool();
	~ParticleTool();

	void Update();
	void Render();
	void UIRender();

	void LoadData(wstring file);
	void SaveData(wstring file);
private:
	
	ParticleEmitterBase * particle;

	Json::Value*  jsonRoot;
};

