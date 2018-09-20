#pragma once
#include "./UI/BaseUI.h"
class ButtonUI : public BaseUI
{
private:
	enum class ButtonState
	{
		Normal = 0,Down,Up,End
	};
private:
	ID3D11ShaderResourceView* buttonUpTexture;
	ID3D11ShaderResourceView* buttonDownTexture;
	ButtonState state;
	function<void()> func;
public:
	ButtonUI(D3DXVECTOR2 pos, D3DXVECTOR2 size,string upTextureKey,string downTextureKey);
	virtual ~ButtonUI();

	void SetFunction(function<void()> func) { this->func = func; }

	virtual void Update()override;
	virtual void Render()override;
};

