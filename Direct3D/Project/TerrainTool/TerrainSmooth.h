#pragma once
class TerrainSmooth
{
public:
	TerrainSmooth();
	~TerrainSmooth();
	
	void CopyHeight();
	void EditHeight();
private:
	ComputeShader * computeSmooth;
	ComputeShader* computeALL;

};

