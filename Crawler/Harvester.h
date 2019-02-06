#pragma once
#include <string>
#include <vector>
#include <list>
#include "gumbo.h"
#include "Lot.h"

typedef struct {
	std::string host;
	std::string path;
} URI;

class Harvester
{
public:
	Harvester();
	~Harvester();

	void grab(const char * url);
	const std::vector<Lot> & getResults();
private:
	std::vector<Lot> lotCards;
	std::string hostname;

	void get(const URI & url, std::string & result);
	void findNodeWithAttr(GumboTag tag, const char * attrName, const char * attrValue, const GumboNode * root, const GumboNode ** result);
	bool startsWith(const char * text, const char * begin);
	void findAll(GumboTag tag, const GumboNode * root, const char * attrName, const char * startsWith, std::list<GumboNode *> & result);
	void findWithAttrStartsWith(GumboTag tag, const GumboNode * root, const char * name, const char * start, GumboNode ** result);
	bool haveAttrBeginsWith(const GumboNode * node, const char * name, const char * begin);
	void processPage(GumboNode * root);
	URI * parseURL(const char * url);
	void findPageLinks(GumboNode * root, std::list<std::string> & result);
};