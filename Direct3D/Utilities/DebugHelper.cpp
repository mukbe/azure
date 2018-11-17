#include "stdafx.h"
#include "DebugHelper.h"

vector<DebugData> DebugHelper::debugData;

DebugHelper::DebugHelper()
{
}


DebugHelper::~DebugHelper()
{
	
}

void DebugHelper::UIRender()
{
	ImGui::Begin("Debug");
	{
		if (ImGui::Button("Clear"))
			debugData.clear();

		ImGui::Separator();

		for (size_t i = 0; i < DebugHelper::debugData.size(); ++i)
		{
			string log = to_string((int)DebugHelper::debugData[i].worldTime) + " : " + DebugHelper::debugData[i].log;
			ImGui::Text(log.c_str());
		}
			
	}
	ImGui::End();
}

void DebugHelper::Delete()
{
	DebugHelper::debugData.clear();
}

void DebugHelper::Log(string log)
{
	DebugData newData;
	newData.worldTime = Time::Get()->Running();
	newData.log = log;
	DebugHelper::debugData.push_back(newData);
}


