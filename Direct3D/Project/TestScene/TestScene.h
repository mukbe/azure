#pragma once

class TestScene : public SceneNode
{
public:
	TestScene();
	virtual~TestScene();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();

	
	void AlphaRender();

	void PreRender();
	void Render();
	void UIRender();

private:
	class CameraBase* freeCamera;
	class TestClass* temp;

	class WorldBuffer* worldBuffer;
	class Shader* shader;

	vector<VertexColor> vertex;
	vector<UINT> index;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	class Font* font;
};

