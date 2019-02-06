#include "Application.h"
#include "UI.h"

Application::Application()
{
	ui = &(UI::getInstance());
	data = nullptr;
}

Application::~Application()
{
}

void Application::run()
{
	try
	{
		ui->create();
	}
	catch (std::exception e)
	{
		ui->showError(e.what());
		return;
	}

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	/*
	*/
}

const std::vector<Lot> & Application::runHarvesting()
{
	try
	{
		harv.grab("http://directlot.ru/user.php?id=6460&f=tovary");
	}
	catch (std::exception e)
	{
		ui->showError(e.what());
	}

	data = (std::vector<Lot> *)&harv.getResults();

	return harv.getResults();
}

const std::vector<Lot>& Application::getData()
{
	return *data;
}

void Application::clearData()
{
	if (data)
	{
		data->clear();
	}
}

bool Application::haveData()
{
	return (data) ? data->size() > 0 : false;
}

Application & Application::getInstance()
{
	static Application app;
	return app;
}
