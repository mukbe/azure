#pragma once

#include <string>
#include <map>
#include <queue>

#include "./Interfaces/IEvent.h"



//////////////////////////////////////////////////////////////////////////
///@brief 이벤트 처리를 위한 값 저장 구조체
//////////////////////////////////////////////////////////////////////////
struct EventProc
{
	std::string name;///< 이벤트 명
	IEventArgs* args;///< 이벤트 추가정보
};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 이벤트 처리
//////////////////////////////////////////////////////////////////////////
class Event
{
public:
	friend class Program;
	typedef std::multimap<std::string, IEvent*> EventMap;

	static Event* Get();
	static void Delete();

	static void AddEventListener(std::string name, IEvent* obj);
	static void RegistEvent(std::string name, IEventArgs* args);

private:
	Event(void);
	~Event(void);

	static void ExecuteEvent();

	static Event* instance;///< 싱글톤 객체
	static EventMap eventMap;///< 이벤트 맵

	static std::queue<EventProc> eventQueue;///< 이벤트 큐
};