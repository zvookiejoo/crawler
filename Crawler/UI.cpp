#include "UI.h"
#include "Application.h"
#include "Lot.h"

#include <boost/format.hpp>
#include <exception>
#include <CommCtrl.h>
#include <commdlg.h>
#pragma comment (lib, "comctl32.lib")

#include "Writer.h"

LRESULT UI::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		try
		{
			createControls(hWnd);
		}
		catch (std::exception e)
		{
			showError(e.what());
			PostQuitMessage(1);
		}
		break;
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED)
		{
			runAction((HWND)lParam);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}

void harvest()
{
	Application & app = Application::getInstance();
	UI & ui = UI::getInstance();

	try
	{
		const std::vector<Lot> & lots = app.runHarvesting();

		if (lots.size() > 0)
		{
			for (auto it = lots.begin(); it != lots.end(); it++)
			{
				Lot lot = *it;
				ui.appendLot(&lot);
			}
		}
	}
	catch (std::exception e)
	{
		ui.showError(e.what());
	}
}

void clear()
{
	Application::getInstance().clearData();
	UI & ui = UI::getInstance();
	ui.clearList();
}

void save()
{
	UI::getInstance().saveAs();
}

void UI::createControls(HWND parent)
{
	HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	HINSTANCE inst = (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE);

	btnGet = CreateWindow(L"BUTTON", L"Втянуть", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		10, 10, 64, 24, parent, NULL, inst, NULL);
	btnClear = CreateWindow(L"BUTTON", L"Очистить", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		84, 10, 64, 24, parent, NULL, inst, NULL);
	btnSave = CreateWindow(L"BUTTON", L"Сохранить как...", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		158, 10, 96, 24, parent, NULL, inst, NULL);

	if (!btnGet || !btnClear || !btnSave) throw std::exception("Failed to create UI controls.");

	SendMessage(btnGet, WM_SETFONT, (WPARAM)font, true);
	SendMessage(btnClear, WM_SETFONT, (WPARAM)font, true);
	SendMessage(btnSave, WM_SETFONT, (WPARAM)font, true);

	INITCOMMONCONTROLSEX icex;

	icex.dwICC |= ICC_LISTVIEW_CLASSES;
	icex.dwSize = sizeof(icex);

	InitCommonControlsEx(&icex);

	RECT rc;

	GetClientRect(parent, &rc);

	int x = rc.left + 10;
	int y = rc.top + 48;
	int w = rc.right - rc.left - 20;
	int h = rc.bottom - rc.top - 58;

	list = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE | LVS_REPORT, 
		x, y, w, h, parent, NULL, inst, NULL);

	if (!list) throw std::exception("Could not create list window.");

	LVCOLUMN lvc;

	lvc.fmt = LVCFMT_LEFT;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText = (LPWSTR)L"Наименование";
	lvc.cx = 600;

	SendMessage(list, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

	lvc.iSubItem = 1;
	lvc.pszText = (LPWSTR)L"Количество";
	lvc.cx = 80;
	
	SendMessage(list, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc);

	lvc.iSubItem = 2;
	lvc.pszText = (LPWSTR)L"Цена";
	lvc.cx = 80;

	SendMessage(list, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc);

	SendMessage(list, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	status = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 264, 10, 200, 16, parent, NULL, inst, NULL);

	if (!status) throw std::exception("Could not create status text.");

	SendMessage(status, WM_SETFONT, (WPARAM)font, true);

	actions.insert(std::make_pair(btnGet, &harvest));
	actions.insert(std::make_pair(btnClear, &clear));
	actions.insert(std::make_pair(btnSave, &save));
}

void UI::runAction(HWND hWnd)
{
	if (hWnd)
	{
		auto it = actions.find(hWnd);

		if (it != actions.end())
		{
			void(*action)() = it->second;

			if (action)
			{
				try
				{
					action();
				}
				catch (std::exception e)
				{
					showError(e.what());
				}
			}
		}
	}
}

void UI::updateStatus(int n)
{
	int size = Application::getInstance().getData().size();
	std::wstring text = boost::str(boost::wformat(L"Обработано %d строк из %d") % n % size);

	SetWindowText(status, text.c_str());
}

void UI::clearList()
{
	SendMessage(list, LVM_DELETEALLITEMS, 0, 0);
}

void UI::saveAs()
{
	if (!Application::getInstance().haveData()) return;

	OPENFILENAME ofn = { 0 };

	wchar_t buf[MAX_PATH] = L"";

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = handle;
	ofn.lpstrFilter = L"Excel 2007 books (*.xlsx)";
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags |= OFN_LONGNAMES;
	ofn.lpstrDefExt = L"xlsx";
	ofn.lpstrFile = buf;

	if (GetSaveFileName(&ofn))
	{
		Writer w;

		w.write(Application::getInstance().getData(), buf);
	}
}

UI::UI()
{
	handle = NULL;
	btnGet = NULL;
	btnClear = NULL;
	title = L"DirectLot Crawler";
	className = L"CrawlerWindowClass";
	hInstance = GetModuleHandle(NULL);
	itemIndex = 0;
}

UI::~UI()
{
}

UI & UI::getInstance()
{
	static UI ui;

	return ui;
}

void UI::create()
{
	WNDCLASSEX wc = { 0 };

	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(wc);
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = UI::wndProc;
	wc.lpszClassName = className;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc)) throw std::exception("Failed to register window class.");

	handle = CreateWindowEx(WS_EX_CLIENTEDGE, className, title, 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, 
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

	if (!handle) throw std::exception("Failed to create application window.");
}

LRESULT CALLBACK UI::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return UI::getInstance().windowProc(hWnd, uMsg, wParam, lParam);
}

void UI::appendLot(Lot * lot)
{
	if (!lot) return;

	int result = 0;

	LVITEM item;

	item.mask = LVIF_TEXT;
	item.pszText = (LPWSTR)lot->getName().c_str();
	item.iItem = itemIndex;
	item.iSubItem = 0;

	result = SendMessage(list, LVM_INSERTITEM, 0, (LPARAM)&item);
	
	int pcs = lot->getInStock();
	std::wstring p = std::to_wstring(pcs);

	item.pszText = (LPWSTR)p.c_str();
	item.iItem = itemIndex;
	item.iSubItem = 1;

	result = SendMessage(list, LVM_SETITEM, 0, (LPARAM)&item);

	double price = lot->getPrice();
	std::wstring pr = boost::str(boost::wformat(L"%.2f") % price);
	item.pszText = (LPWSTR)pr.c_str();
	item.iItem = itemIndex;
	item.iSubItem = 2;

	result = SendMessage(list, LVM_SETITEM, 0, (LPARAM)&item);
	itemIndex++;
}

void UI::showError(const char * text)
{
	// convert multi-byte string to wide-character
	int length = MultiByteToWideChar(CP_ACP, 0, text, -1, NULL, 0);
	wchar_t * message = new wchar_t[length];
	memset(message, 0, length);
	MultiByteToWideChar(CP_ACP, 0, text, length, message, length);

	// show error message
	showError(message);

	// delete intermediate string
	delete[] message;
}

void UI::showError(const wchar_t * text)
{
	if (text)
	{
		MessageBox(handle, text, title, MB_ICONERROR | MB_OK);
	}
}
