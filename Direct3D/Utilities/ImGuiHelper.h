#pragma once
class ImGuiHelper
{
public:
	static void RenderImageButton(ID3D11ShaderResourceView*& srv,ImVec2 size = ImVec2(50,50));	//WithDragAndDrop
	static void RenderImageButton(class Texture** texture, ImVec2 size = ImVec2(50, 50));
	static void RenderModelSelectable(string name,function<void(struct ModelData*)> func,ImVec2 size = ImVec2(0,0));
};

