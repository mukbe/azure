#pragma once

#include <string>
#include <map>
#include <queue>

#include "./Interfaces/IEvent.h"



//////////////////////////////////////////////////////////////////////////
///@brief �̺�Ʈ ó���� ���� �� ���� ����ü
//////////////////////////////////////////////////////////////////////////
struct EventProc
{
	std::string name;///< �̺�Ʈ ��
	IEventArgs* args;///< �̺�Ʈ �߰�����
};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief �̺�Ʈ ó��
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

	static Event* instance;///< �̱��� ��ü
	static EventMap eventMap;///< �̺�Ʈ ��

	static std::queue<EventProc> eventQueue;///< �̺�Ʈ ť
};