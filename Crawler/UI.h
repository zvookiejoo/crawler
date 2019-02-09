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

