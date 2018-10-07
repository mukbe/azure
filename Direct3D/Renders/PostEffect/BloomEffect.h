#pragma once
#include "Renders/ShaderBuffer.h"
//bloom Effect Pass
//1) DownScale 
//		-firstPass , secondPass
//2) BloomEffect 3
//3) Blur 
//		-horizental,vertival
//4) FinalRendering 

class BloomEffect : public Renderer
{
private:
	//OutClass Values ---------------------------------------
	Synthesize(ID3D11ShaderResourceView *, deferredRenderingSRV, DeferredSRV)
	//-------------------------------------------------------

	//Sahder -------------------------------------------------
	class ComputeShader*	downScaleFirstShader;
	class ComputeShader*	downScaleSecondShader;
	class ComputeShader*	bloomShader;
	class ComputeShader*	horizentalBlurShader;
	class ComputeShader*	verticalBlurShader;
	class Shader*			finalRenderingShader;
	//--------------------------------------------------------

	//Buffer -------------------------------------------------
	class DonwScaleBuffer*  downScaleBuffer;
	class FinalRenderingBuffer* finalRenderingBuffer;

	class CResource1D*		downScale1DBuffer;
	class CResource1D*		averageValuesBuffer;
	class CResource1D*		prevAvgLumBuffer;

	class CResource2D*		bloomTextureBuffer;
	class CResource2D*		downTextureBuffer;

	class CResource2D*		temporaryTexture0;
	class CResource2D*		temporaryTexture1;

	//--------------------------------------------------------

	//Values --------------------------------------------------
	UINT					width;
	UINT					height;
	UINT					downScaleGroup;
	float					middleGrey;		//휘도설정 임의 값
	float					white;			//일정 이상의 휘도를 잘라내기 위한 Factor값
	float					adaption;		//TODO 추 후에 노출효과 구현할 때 사용
	float					bloomThreshold;
	float					bloomScale;
	//---------------------------------------------------------

public:
	BloomEffect();
	~BloomEffect();

	virtual void Render();
	virtual void SetRTV();

	void UIRender();
private:
	void InitailizeValue();
	void CreateShaders();
	void CreateBuffers();

	void BindConstantBuffer();
	void DonwScale();
	void Bloom();
	void Blur();
	void FinalRendering();
};

class DonwScaleBuffer : public ShaderBuffer
{
public:
	struct Struct
	{
		UINT width;
		UINT height;
		UINT totalPixels;
		UINT groupSize;
		float adaptation;
		float bloomThreshold;
		D3DXVECTOR2 padding;
	}data;

	DonwScaleBuffer()
		:ShaderBuffer(&data, sizeof Struct)
	{

	}
};

class FinalRenderingBuffer : public ShaderBuffer
{
public:
	struct Struct
	{
		float middleGrey;
		float lumWhiteSqr;
		float bloomScale;
		int hdrActive;
		int bloomActive;
		D3DXVECTOR3 padding;
	}data;

	FinalRenderingBuffer()
		:ShaderBuffer(&data, sizeof Struct) {
		data.hdrActive = 1;
		data.bloomActive = 1;
	}

};