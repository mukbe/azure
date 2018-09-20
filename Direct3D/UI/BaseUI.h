#pragma once
#include "./Renders/ShaderBuffer.h"


class BaseUI
{
protected:
	class Shader* shader;
	class BaseUI* pParent;
	class OrthoWindow* orthoWindow;
	D3DXVECTOR2 position;
	D3DXVECTOR2 size;
public:
	BaseUI();
	BaseUI(D3DXVECTOR2 pos, D3DXVECTOR2 size);
	virtual ~BaseUI();

	void SetPosition(D3DXVECTOR2 pos);
	void SetSize(D3DXVECTOR2 size);

	virtual void Update() = 0;
	virtual void Render() = 0;
};

