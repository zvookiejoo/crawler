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
#include "stdafx.h"

class UI
{
public:
	static UI & getInstance();

	void create();

	static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void addListItem(const wchar_t * text, unsigned int column, bool last);
	void clearList();
	void setStatusText(const wchar_t * text);
	const wchar_t * getFilename();

	void showError(const char * text);
	void showError(const wchar_t * text);
private:
	// Properties
	HWND handle;
	HWND btnGet;
	HWND btnClear;
	HWND btnSave;
	HWND list;
	HWND status;
	const wchar_t * title;
	const wchar_t * className;
	HINSTANCE hInstance;
	int itemIndex;

	std::map<HWND, void(*)(void)> actions;

	// Methods
	LRESULT windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void createControls(HWND parent);
	void runAction(HWND hWnd);

	// Hide the constructors - we are Singleton
	UI();
	~UI();
	UI(const UI &) = delete;
	const UI & operator=(const UI &) = delete;
};

