#pragma once
#include "./Object/GameObject/GameObject.h"
#include "./Renders/ShaderBuffer.h"
class Fog : public GameObject
{
private:
	class FogBuffer : public ShaderBuffer
	{
	public :
		struct Struct
		{
			float fogStart;
			float fogEnd;
			float fogOn;
			float fogPadding;
			D3DXCOLOR fogColor;
		}data;
		
		FogBuffer()
			:ShaderBuffer(&data, sizeof Struct) {
			data.fogStart = 0.0f;
			data.fogEnd = 300.0f;
			data.fogOn = 2.0f;
			data.fogColor = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		}
	};
private:
	FogBuffer * fogBuffer;
public:
	Fog();
	~Fog();

	virtual void Render();
	virtual void UIUpdate();
	virtual void UIRender();
};

