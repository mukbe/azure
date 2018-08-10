#pragma once

class DirectionalLight
{
private:
	D3DXMATRIX view;
	D3DXMATRIX ortho;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 dir;
public:
	DirectionalLight();
	~DirectionalLight();
	
	D3DXMATRIX GetView();
	D3DXMATRIX GetProj();
	D3DXVECTOR3 GetPos() { return this->pos; }
	float* GetPosPtr() { return &pos.x; }
	void UpdateView();
	D3DXVECTOR3 GetDir() { return this->dir; }
};

