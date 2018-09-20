#include "stdafx.h"
#include "ButtonUI.h"

#include "./View/OrthoWindow.h"

ButtonUI::ButtonUI(D3DXVECTOR2 pos, D3DXVECTOR2 size,string upTextureKey, string downTextureKey)
	:BaseUI(pos,size), state(ButtonState::Normal)
{
	Texture* temp = nullptr;
	temp = AssetManager->FindTexture(upTextureKey);
	if (temp)
		this->buttonUpTexture = temp->GetSRV();
	else
		assert(false);
	temp = AssetManager->FindTexture(downTextureKey);
	if (temp)
		this->buttonDownTexture = temp->GetSRV();
	else
		assert(false);

	shader = new Shader(ShaderPath + L"006_UI.hlsl", Shader::ShaderType::Default, "Button");

	orthoWindow->UpdateBuffer();
}

ButtonUI::~ButtonUI()
{
}

void ButtonUI::Update() 
{
	switch (state)
	{
	case ButtonUI::ButtonState::Normal:
	{
		if (Mouse::Get()->Down(0))
		{
			D3DXVECTOR3 mousePos = Mouse::Get()->GetPosition();
			if (Math::IsPointInAABB(position, size, Mouse::Get()->GetMousePosInWorld()))
			{
				state = ButtonState::Down;
			}
		}
	}
		break;
	case ButtonUI::ButtonState::Down:
	{
		if (Mouse::Get()->Up(0))
		{
			state = ButtonState::Up;
		}
	}
		break;
	case ButtonUI::ButtonState::Up:
	{
		if (func)
			func();
		state = ButtonState::Normal;
	}
		break;
	}
}

void ButtonUI::Render()
{
	this->orthoWindow->Render();

	this->shader->Render();

	if (state == ButtonState::Normal || state == ButtonState::Up)
		DeviceContext->PSSetShaderResources(0, 1, &buttonUpTexture);
	else
		DeviceContext->PSSetShaderResources(0, 1, &buttonDownTexture);

	this->orthoWindow->DrawIndexed();

	ID3D11ShaderResourceView* nullView[1] = { nullptr };
	DeviceContext->PSSetShaderResources(0, 1, nullView);
}
