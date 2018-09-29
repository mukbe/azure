#pragma once
#include "../Interfaces/ICloneable.h"
class MaterialBuffer;
class Material : public ICloneable
{
private:
	Synthesize(UINT,number,Number)
	Synthesize(wstring,name,Name)
	Synthesize(MaterialBuffer*, buffer,Buffer)
	Synthesize(Texture*,diffuseMap,DiffuseMap)
	Synthesize(Texture*,specularMap,SpecularMap)
	Synthesize(Texture*,emissiveMap,EmissiveMap)
	Synthesize(Texture*,normalMap,NormalMap)
	Synthesize(Texture*,detailMap,DetailMap)
	Synthesize(D3DXCOLOR, ambientColor, AmbientColor)
	Synthesize(D3DXCOLOR, diffuseColor,DiffuseColor)
	Synthesize(D3DXCOLOR, specColor, SpecColor)
	Synthesize(D3DXCOLOR , emissiveColor,EmissiveColor)
	Synthesize(float, shiness,Shiness)
	Synthesize(float,detailFactor,DetailFactor)
public:
	Material();
	~Material();

	void Clone(void** clone)override;
	
	void UpdateBuffer();
	void BindBuffer();
	void UnBindBuffer();
	void UIRender();

	void SaveData(Json::Value* parent);
};

#define UseDeiffuseMap 1
#define UseNormalMap 2
#define UseSpecularMap 4
#define UseEmissiveMap 8