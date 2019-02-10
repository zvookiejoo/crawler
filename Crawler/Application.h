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

