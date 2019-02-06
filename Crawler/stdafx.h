#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <objbase.h>
#include <string>
#include <CommCtrl.h>
#include <vector>
#include <list>
#include <exception>
#include <map>
#include <boost/format.hpp>
#include "Lot.h"

#pragma comment (lib, "gumbo.lib")
#pragma comment (lib, "comctl32.lib")

typedef std::vector<Lot> ProductList;