#pragma once
#include "./Utilities/Transform.h"
#include "./Object/GameObject/GameObject.h"
class SunBuffer;
namespace Environment
{
	class Sun
	{
	public:
		Sun();
		~Sun();

		void Update();
		void UpdateView();
		void Render();

		D3DXVECTOR3 GetForward() { return dir; }
		float* GetPosPtr();
		D3DXVECTOR3 GetPos();
		void SetColor(D3DXCOLOR color);
	private:

		SunBuffer* sunBuffer;

		D3DXMATRIX view;
		D3DXMATRIX ortho;
		D3DXMATRIX shadowMatrix;

		D3DXVECTOR3 dir;

		float height;
		bool bUpdate;

		D3DXVECTOR3 preMousePos;
		D3DXVECTOR3 angle;
		D3DXVECTOR3 right;
	};

}

