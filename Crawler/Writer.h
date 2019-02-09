#pragma once

class Writer
{
public:
	Writer();
	~Writer();

	void write(const ProductList & data, const wchar_t * fileName);
};
