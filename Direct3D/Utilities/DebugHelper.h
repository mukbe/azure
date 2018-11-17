#pragma once

struct DebugData
{
	float worldTime;
	string log;
};

class DebugHelper
{
private:
	static vector<DebugData> debugData;
public:
	DebugHelper();
	~DebugHelper();

	static void UIRender();
	static void Delete();
	static void Log(string log);
};

