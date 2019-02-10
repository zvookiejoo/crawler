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
#include "gumbo.h"

typedef struct {
	std::string host;
	std::string path;
} URI;

class Harvester
{
public:
	Harvester();
	~Harvester();

	void grab(const char * url, ProductList & data);
private:
	std::string hostname;

	void get(const URI & url, std::string & result);
	void findNodeWithAttr(GumboTag tag, const char * attrName, const char * attrValue, const GumboNode * root, const GumboNode ** result);
	bool startsWith(const char * text, const char * begin);
	void findAll(GumboTag tag, const GumboNode * root, const char * attrName, const char * startsWith, std::list<GumboNode *> & result);
	void findWithAttrStartsWith(GumboTag tag, const GumboNode * root, const char * name, const char * start, GumboNode ** result);
	bool haveAttrBeginsWith(const GumboNode * node, const char * name, const char * begin);
	void processPage(GumboNode * root, ProductList & data);
	URI * parseURL(const char * url);
	void findPageLinks(GumboNode * root, std::list<std::string> & result);
	void findCondition(const char * url, std::wstring & result);
};
