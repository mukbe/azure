#pragma once

class DepthVis
{
public:
	DepthVis(ID3D11ShaderResourceView* depthView);
	~DepthVis();
private:
	ComputeShader * shader;
	CResource2D* depth;

	
};

