#pragma once

#include "stdafx.h"

class Writer
{
public:
	Writer();
	~Writer();

	void write(const std::vector<Lot> & data, const wchar_t * fileName);
};

