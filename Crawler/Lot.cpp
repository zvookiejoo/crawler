#include "Lot.h"
#include <Windows.h>

Lot::Lot()
{
}


Lot::~Lot()
{
}

void Lot::setName(const wchar_t * name)
{
	if (name)
	{
		this->name = name;
	}
}

void Lot::setName(const char * name)
{
	int length = MultiByteToWideChar(CP_ACP, 0, name, -1, NULL, 0);
	wchar_t * text = new wchar_t[length];
	MultiByteToWideChar(CP_ACP, 0, name, length, text, length);
	this->name = text;
}

void Lot::setInStock(unsigned int count)
{
	this->inStock = count;
}

void Lot::setPrice(double price)
{
	this->price = price;
}

const std::wstring & Lot::getName()
{
	return this->name;
}

int Lot::getInStock()
{
	return this->inStock;
}

double Lot::getPrice()
{
	return this->price;
}
