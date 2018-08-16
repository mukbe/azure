#pragma once
#include "../Interfaces/ICloneable.h"
class Material : public ICloneable
{
private:
	Synthesize(UINT,number,Number)
	Synthesize(wstring,name,Name)
	Synthesize(class MaterialBuffer*, buffer,Buffer)
	Synthesize(ID3D11ShaderResourceView*,diffuseMap,DiffuseMap)
	Synthesize(ID3D11ShaderResourceView*,specularMap,SpecularMap)
	Synthesize(ID3D11ShaderResourceView*,emissiveMap,EmissiveMap)
	Synthesize(ID3D11ShaderResourceView*,normalMap,NormalMap)
	Synthesize(ID3D11ShaderResourceView*,detailMap,DetailMap)
	Synthesize(D3DXCOLOR, diffuseColor,DiffuseColor)
	Synthesize(D3DXCOLOR, specColor, SpecColor)
	Synthesize(D3DXCOLOR , emissiveColor,EmissiveColor)
	Synthesize(float, shiness,Shiness)
	Synthesize(float,detailFactor,DetailFactor)
public:
	Material();
	~Material();

	void Clone(void** clone)override;
	void BindBuffer();
	void UnBindBuffer();
};

