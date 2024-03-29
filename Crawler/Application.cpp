/*
MIT License

Copyright (c) 2019 Kirill Makhalov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "stdafx.h"
#include "Application.h"
#include "Writer.h"

#include <process.h>

Application::Application()
{
	CoInitialize(NULL);
}

Application::~Application()
{
	CoUninitialize();
}

void Application::run()
{
	try
	{
		ui.create();
		updateState(L"������� \"�������\" ��� ��������� ������");
	}
	catch (std::exception e)
	{
		ui.showError(e.what());
		return;
	}

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Application::harvest()
{
	_beginthreadex(NULL, 0, harvesterThread, NULL, 0, NULL);
}

unsigned int __stdcall Application::harvesterThread(void * p)
{
	Application & app = Application::getInstance();

	app.clearData();

	try
	{
		app.harv.grab("http://directlot.ru/user.php?id=6460&f=tovary", app.data);
	}
	catch (std::exception e)
	{
		app.ui.showError(e.what());
	}

	app.updateState(boost::str(boost::wformat(L"�������� %d �������") % app.data.size()).c_str());
	app.showList();

	return 0;
}

unsigned int __stdcall Application::saveThread(void * p)
{
	Writer writer;
	Application & app = Application::getInstance();

	const wchar_t * fileName = nullptr;

	try
	{
		fileName = app.ui.getFilename();
	}
	catch (std::exception e)
	{
		app.ui.showError(e.what());
	}

	if (fileName)
	{
		writer.write(app.data, fileName);
	}

	app.saved = true;

	return 0;
}

void Application::showList()
{
	static int lastNumber;

	if (haveData())
	{
		int currentNumber = 0;

		for (auto it = data.begin(); it != data.end(); it++)
		{
			if (currentNumber < lastNumber)
			{
				currentNumber++;
				continue;
			}

			std::wstring text = std::to_wstring(it->sku);
			ui.addListItem(text.c_str(), 0, false);

			text = it->name;
			ui.addListItem(text.c_str(), 1, false);

			text = it->condition;
			ui.addListItem(text.c_str(), 2, false);

			text = std::to_wstring(it->quantity);
			ui.addListItem(text.c_str(), 3, false);

			text = boost::str(boost::wformat(L"%.2f") % it->price);
			ui.addListItem(text.c_str(), 4, true);

			lastNumber = ++currentNumber;
		}
	}
	else
	{
		ui.clearList();
	}
}

void Application::updateState(const wchar_t * text)
{
	if (!text) throw std::exception("text was null at Application::updateState");
	ui.setStatusText(text);
}

void Application::clearData()
{
	Application & app = Application::getInstance();

	app.saved = false;

	app.updateState(L"������� ������");
	app.data.clear();
	app.showList();
	app.updateState(L"������� \"�������\" ��� ��������� ������");
}

void Application::save()
{
	_beginthreadex(NULL, 0, saveThread, NULL, 0, NULL);
}

bool Application::haveData()
{
	return data.size() > 0;
}

Application & Application::getInstance()
{
	static Application app;
	return app;
}

bool Application::dataSaved()
{
	return saved;
}
