#pragma once

#include <vector>
#include <exception>
#include <Windows.h>
#include "Lot.h"

class Writer
{
public:
	Writer();
	~Writer();

	void write(const std::vector<Lot> & data, const wchar_t * fileName);
};

