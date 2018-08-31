#pragma once

class SunBuffer;
namespace Environment
{
	class Sun
	{
	public:
		Sun();
		~Sun();

		void UpdateView();
		void Render();

	private:
		SunBuffer* sunBuffer;

		D3DXMATRIX view;
		D3DXMATRIX ortho;
		D3DXMATRIX shadowMatrix;
	};

}

