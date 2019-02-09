#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <objbase.h>
#include <CommCtrl.h>

#include <string>
#include <vector>
#include <list>
#include <exception>
#include <map>

#include <boost/format.hpp>

#pragma comment (lib, "gumbo.lib")
#pragma comment (lib, "comctl32.lib")

struct Lot {
	unsigned long sku;
	std::wstring name;
	std::wstring condition;
	unsigned int quantity;
	double price;
	std::wstring url;
};

typedef std::vector<Lot> ProductList;