#pragma once

class Lot
{
public:
	Lot();
	~Lot();

	void setName(const char * name);
	void setName(const wchar_t * name);
	void setInStock(unsigned int count);
	void setPrice(double price);

	const std::wstring & getName();
	int getInStock();
	double getPrice();
private:
	std::wstring name;
	unsigned int inStock;
	double price;
};

