#pragma once
#include <functional>

class ShadowRenderer
{
private:
	ID3D11Texture2D *							directionalTexture2D;
	ID3D11DepthStencilView*						directionalDSV;
	ID3D11ShaderResourceView*					directionalSRV;

	ID3D11Texture2D *							pointTexture2D;
	ID3D11DepthStencilView*						pointDSV;
	ID3D11ShaderResourceView*					pointSRV;;

	function<void(void)>						renderFunc;

	vector<pair<D3DXMATRIX, D3DXMATRIX>>		viewProjectionDataContext;

	class ViewProjectionBuffer*					viewProjectionBuffer;
	D3D11_VIEWPORT								viewport;
private:
	void CreatePointRenderer();
	void CreateDirectionalRenderer();
	void UpdateViewProjection(UINT index);
public:
	ShadowRenderer();
	~ShadowRenderer();

	void SetRenderFunc(function<void()> func) { this->renderFunc = func; }

	void RenderDirectionalMap();
	void RenderPointMap();

	void SetViewProjection(UINT index,D3DXMATRIX view, D3DXMATRIX projection);

	ID3D11ShaderResourceView* GetDirectionalSRV()const { return this->directionalSRV; }
};

/*
viewProjectionDataContext -> fist에 view, second에 projection행렬을 담을 예정이다.(그림자 맵 촬영 때 들어간 projection행렬은 직교투영행렬이 들어간다.
DirectionalLight촬영을 할거라면 dataContext의 -> 0번째에 세팅을 해주면된다.
PointLight촬영은0 ~ 6까지 환경 맵의 6면에 대한 view 행렬과 projection행렬을 세팅 
*/
