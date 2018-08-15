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
viewProjectionDataContext -> fist�� view, second�� projection����� ���� �����̴�.(�׸��� �� �Կ� �� �� projection����� ������������� ����.
DirectionalLight�Կ��� �ҰŶ�� dataContext�� -> 0��°�� ������ ���ָ�ȴ�.
PointLight�Կ���0 ~ 6���� ȯ�� ���� 6�鿡 ���� view ��İ� projection����� ���� 
*/
