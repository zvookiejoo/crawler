#pragma once
#include "stdafx.h"
#include "Harvester.h"

class UI;

class Application
{
public:

	void run();
	const std::vector<Lot> & runHarvesting();
	const std::vector<Lot> & getData();
	void clearData();
	bool haveData();
	static Application & getInstance();
private:
	UI * ui;
	Harvester harv;

	std::vector<Lot> * data;

	Application();
	~Application();
	Application(const Application &) = delete;
	const Application & operator=(const Application &) = delete;
};

