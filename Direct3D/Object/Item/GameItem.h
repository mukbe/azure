#pragma once
class GameItem
{
private:
	class Model* model;
	class WorldBuffer* worldBuffer;
	class Shader* shader;
	D3DXMATRIX* pParentMat;
	vector<D3DXMATRIX> absolutes;
public:
	GameItem(class Model* model,D3DXMATRIX* pParentMat);
	~GameItem();

	void Update();
	void Render();
};

