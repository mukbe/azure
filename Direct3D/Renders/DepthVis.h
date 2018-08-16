#pragma once

class DepthVis
{
public:
	DepthVis();
	~DepthVis();

	void CalcuDepth(ID3D11ShaderResourceView* depthView);
	ID3D11ShaderResourceView* GetSRV();

private:
	class ComputeShader * shader;
	class CResource2D* depth;

	
};

