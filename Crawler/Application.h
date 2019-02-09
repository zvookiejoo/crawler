#pragma once
#include "Harvester.h"
#include "UI.h"

class Application
{
public:

	void run();
	static void harvest();
	static void clearData();
	static void save();
	bool haveData();
	void showList();
	void updateState(const wchar_t * text);
	static Application & getInstance();
	bool dataSaved();
private:
	UI & ui = UI::getInstance();
	Harvester harv;
	ProductList data;
	bool saved = false;

	static unsigned int __stdcall harvesterThread(void * p);
	static unsigned int __stdcall saveThread(void * p);

	Application();
	~Application();
	Application(const Application &) = delete;
	const Application & operator=(const Application &) = delete;
};

