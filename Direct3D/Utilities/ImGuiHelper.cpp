#include "stdafx.h"
#include "ImGuiHelper.h"



void ImGuiHelper::RenderImageButton(ID3D11ShaderResourceView *& srv,ImVec2 size)
{
	ImGui::ImageButton(srv, size);
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = new ImGuiPayload;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TextureMove"))
		{
			const Texture* tex = reinterpret_cast<const Texture*>(payload->Data);
			srv = (const_cast<Texture*>(tex))->GetSRV();
		}
		ImGui::EndDragDropTarget();
	}
}

void ImGuiHelper::RenderImageButton(Texture ** lpTexture, ImVec2 size)
{
	if(*lpTexture != nullptr)
	{
		ImGui::ImageButton((*lpTexture)->GetSRV(), size);
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = new ImGuiPayload;
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TextureMove"))
			{
				const Texture* tex = reinterpret_cast<const Texture*>(payload->Data);
				(*lpTexture) = const_cast<Texture*>(tex);
			}
			ImGui::EndDragDropTarget();
		}
	}
}

void ImGuiHelper::RenderModelSelectable(string name, function<void(struct ModelData*)> func,ImVec2 size)
{
	static bool b;
	ImGui::Selectable(name.c_str(),&b,0, size);
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = new ImGuiPayload;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ModelMove"))
		{
			UINT p;
			memcpy(&p, &payload->Data, sizeof(UINT));
			const ModelData* pData = reinterpret_cast<const ModelData*>(payload->Data);
			func(const_cast<ModelData*>(pData));
		}
		ImGui::EndDragDropTarget();
	}
}
