#include "stdafx.h"
#include "TerrainTool.h"
#include "Terrain.h"

#include "./View/FreeCamera.h"

TerrainTool::TerrainTool()
{
	RenderRequest->AddRender("TerrainToolUIRender", bind(&TerrainTool::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("TerrainToolRender", bind(&TerrainTool::Render, this), RenderType::Render);

	terrain = new Terrain;
	freeCamera = new FreeCamera;
	sun = new Environment::Sun;
}


TerrainTool::~TerrainTool()
{
	Release();
}

void TerrainTool::Init()
{
}

void TerrainTool::Release()
{
}

void TerrainTool::PreUpdate()
{
	freeCamera->Render();
	terrain->PreUpdate();
}

void TerrainTool::Update()
{
	terrain->Update();
}

void TerrainTool::PostUpdate()
{
	terrain->PostUpdate();
	freeCamera->Update();
	sun->UpdateView();
}

void TerrainTool::PreRender()
{
	terrain->PreRender();
	//camera정보를 deferred에게 언팩킹시에 필요한 정보를 보낸다
	RenderRequest->SetUnPackGBufferProp(freeCamera->GetViewMatrix(), freeCamera->GetProjection());

}

void TerrainTool::Render()
{
	//Bind Prop
	freeCamera->Render();
	sun->Render();

	//Render
	terrain->Render();
}

void TerrainTool::UIRender()
{
	terrain->UIRender();
	ImGui::Begin("Assets");
	static const char* names[6] = { "1. Adbul", "2. Alfonso", "3. Aline", "4. Amelie", "5. Anna", "6. Arthur" };
	int move_from = -1, move_to = -1;
	for (int n = 0; n < IM_ARRAYSIZE(names); n++)
	{
		ImGui::Selectable(names[n]);

		ImGuiDragDropFlags src_flags = 0;
		//src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
		//src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
		//src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip

		if (ImGui::BeginDragDropSource(src_flags))
		{
			if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
				ImGui::Text("Moving \"%s\"", names[n]);
			ImGui::SetDragDropPayload("Assets_Move", &n, sizeof(int));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Move"))
			{
				move_from = *(const int*)payload->Data;
				move_to = n;
			}
			ImGui::EndDragDropTarget();
		}
	}

	if (move_from != -1 && move_to != -1)
	{
		// Reorder items
		int copy_dst = (move_from < move_to) ? move_from : move_to + 1;
		int copy_src = (move_from < move_to) ? move_from + 1 : move_to;
		int copy_count = (move_from < move_to) ? move_to - move_from : move_from - move_to;
		const char* tmp = names[move_from];
		//printf("[%05d] move %d->%d (copy %d..%d to %d..%d)\n", ImGui::GetFrameCount(), move_from, move_to, copy_src, copy_src + copy_count - 1, copy_dst, copy_dst + copy_count - 1);
		memmove(&names[copy_dst], &names[copy_src], (size_t)copy_count * sizeof(const char*));
		names[move_to] = tmp;
		ImGui::SetDragDropPayload("Assets_Move", &move_to, sizeof(int)); // Update payload immediately so on the next frame if we move the mouse to an earlier item our index payload will be correct. This is odd and showcase how the DnD api isn't best presented in this example.
	}

	ImGui::End();

}

