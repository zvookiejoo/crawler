#include "Harvester.h"
#include "UI.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <regex>

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

Harvester::Harvester()
{
}

Harvester::~Harvester()
{
}

void Harvester::grab(const char * url)
{
	if (!url)
	{
		throw std::exception("URL was null at Harvester::grab");
		return;
	}

	std::string pageContent;

	URI * uri = parseURL(url);

	get(*uri, pageContent);

	GumboOutput * output = gumbo_parse(pageContent.c_str());

	processPage(output->root);

	std::list<std::string> pageLinks;

	findPageLinks(output->root, pageLinks);

	for (auto it = pageLinks.begin(); it != pageLinks.end(); it++)
	{
		std::string link = *it;
		size_t pos = uri->path.find("?");
		uri->path = uri->path.substr(0, pos);
		uri->path += link;

		get(*uri, pageContent);
		output = gumbo_parse(pageContent.c_str());
		processPage(output->root);
	}

	return;
}

void Harvester::get(const URI & url, std::string & result)
{
	boost::asio::io_context ioc;

	tcp::resolver resolver(ioc);
	tcp::socket socket(ioc);

	auto const results = resolver.resolve(url.host, "80");

	boost::asio::connect(socket, results.begin(), results.end());

	http::request<http::string_body> req(http::verb::get, url.path, 11);
	req.set(http::field::host, url.host);
	req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

	http::write(socket, req);

	boost::beast::flat_buffer buffer;

	http::response<http::dynamic_body> res;

	http::read(socket, buffer, res);
	
	result = boost::beast::buffers_to_string(res.body().data());
	
	boost::system::error_code ec;
	socket.shutdown(tcp::socket::shutdown_both, ec);

	if (ec && ec != boost::system::errc::not_connected)
		throw boost::system::system_error(ec);
}

void Harvester::findNodeWithAttr(GumboTag tag, const char * attrName, const char * attrValue, const GumboNode * root, const GumboNode ** result)
{
	const GumboVector * attributes = &root->v.element.attributes;

	bool attrFound = false;

	for (unsigned int i = 0; i < attributes->length; i++)
	{
		GumboAttribute * attr = (GumboAttribute *)attributes->data[i];

		int a = strcmp(attr->name, attrName);
		int b = strcmp(attr->value, attrValue);

		attrFound = !a && !b;

		if (attrFound) break;
	}

	if (root->type == GUMBO_NODE_ELEMENT &&
		root->v.element.tag == tag &&
		attrFound)
	{
		*result = root;
	}
	else
	{
		const GumboVector * children = &root->v.element.children;

		for (unsigned int i = 0; i < children->length; i++)
		{
			if (*result) break;

			GumboNode * node = (GumboNode *)children->data[i];

			if (node->type != GUMBO_NODE_ELEMENT) continue;

			findNodeWithAttr(tag, attrName, attrValue, node, result);
		}
	}
}

bool Harvester::startsWith(const char * text, const char * begin)
{
	if (!text || !begin) return false;

	return strcmp(text, begin) > 0;
}

void Harvester::findAll(GumboTag tag, const GumboNode * root, const char * attrName, const char * startsWith, std::list<GumboNode *> & result)
{
	if (!root) throw std::exception("root was null at Harvester::findAll");
	if (!attrName) throw std::exception("attrName was null at Harvester::findAll");
	if (!startsWith) throw std::exception("startsWith was null at Harvester::findAll");

	const GumboVector * children = &root->v.element.children;

	for (unsigned int i = 0; i < children->length; i++)
	{
		GumboNode * node = (GumboNode *)children->data[i];

		if (node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == tag &&
			haveAttrBeginsWith(node, attrName, startsWith))
		{
			result.push_back(node);
		}
	}
}

void Harvester::findWithAttrStartsWith(GumboTag tag, const GumboNode * root, const char * name, const char * start, GumboNode ** result)
{
	if (!root) throw std::exception("root was null at Harvester::findWithAttrStartsWith");
	if (!name) throw std::exception("name was null at Harvester::findWithAttrStartsWith");
	if (!start) throw std::exception("start was null at Harvester::findWithAttrStartsWith");

	const GumboVector * children = &root->v.element.children;

	for (unsigned int i = 0; i < children->length; i++)
	{
		GumboNode * node = (GumboNode *)children->data[i];

		if (*result) break;

		if (node->type != GUMBO_NODE_ELEMENT) continue;

		if (node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == tag &&
			haveAttrBeginsWith(node, name, start))
		{
			*result = node;
			break;
		}
		else
		{
			findWithAttrStartsWith(tag, node, name, start, result);
		}
	}
}

bool Harvester::haveAttrBeginsWith(const GumboNode * node, const char * name, const char * begin)
{
	if (!node || !name || !begin)
	{
		return false;
	}

	const GumboVector * attributes = &node->v.element.attributes;

	GumboAttribute * attr = gumbo_get_attribute(attributes, name);

	if (!attr)
	{
		return false;
	}

	return startsWith(attr->value, begin);
}

void Harvester::processPage(GumboNode * root)
{
	if (!root) throw std::exception("root was null at Harvester::processPage");

	const GumboNode * lotListRoot = nullptr;

	findNodeWithAttr(GUMBO_TAG_DIV, "class", "lotListAreaIn", root, &lotListRoot);

	if (!lotListRoot)
	{
		throw std::exception("Lots list root not found.");
	}

	std::list<GumboNode *> lotNodes;

	findAll(GUMBO_TAG_DIV, lotListRoot, "id", "lot", lotNodes);

	for (auto it = lotNodes.begin(); it != lotNodes.end(); it++)
	{
		GumboNode * node = nullptr;

		findWithAttrStartsWith(GUMBO_TAG_A, *it, "id", "lot", &node);

		if (!node) throw std::exception("Lot name not found");

		Lot * lot = new Lot();

		GumboNode * text = (GumboNode *)node->v.element.children.data[0];

		std::string name = text->v.text.original_text.data;
		name = name.substr(0, name.find("<"));

		lot->setName(name.c_str());

		const GumboNode * paramRoot = nullptr;

		findNodeWithAttr(GUMBO_TAG_DIV, "class", "lotListOneLotParam", *it, &paramRoot);

		std::list<GumboNode *> params;

		findAll(GUMBO_TAG_SPAN, paramRoot, "class", "lotListOneLotParam", params);

		auto pit = params.begin();

		pit++; pit++;

		GumboNode * count = *pit;

		GumboNode * instock = (GumboNode *)count->v.element.children.data[0];

		int stockCount = atoi(instock->v.text.text);

		lot->setInStock(stockCount);

		const GumboNode * priceNode = nullptr;

		findNodeWithAttr(GUMBO_TAG_TD, "class", "lotListOneLotCenaCompA", *it, &priceNode);

		if (!priceNode) throw std::exception("Price node not found");

		GumboNode * priceText = (GumboNode *)priceNode->v.element.children.data[0];

		std::string textPrice = priceText->v.text.text;

		std::regex r(" ");

		textPrice = std::regex_replace(textPrice, r, "");

		double price = atof(textPrice.c_str());

		lot->setPrice(price);

		lotCards.push_back(*lot);

		delete lot;
	}
}

URI * Harvester::parseURL(const char * url)
{
	if (!url)
	{
		throw std::exception("Harvester::get() got nullptr");
	}

	std::string host(url);
	size_t pos = host.find("http://");

	if (pos > 0)
	{
		throw std::exception("Invalid url");
	}

	host = host.substr(7);

	pos = host.find("/");

	std::string path;

	if (pos > 0 && pos < std::string::npos)
	{
		path = host.substr(pos);
		host = host.substr(0, pos);
	}
	else
	{
		path = "/";
		host = host.substr(0, host.length() - 1);
	}

	URI * uri = new URI();

	uri->host = host;
	uri->path = path;

	return uri;
}

void Harvester::findPageLinks(GumboNode * root, std::list<std::string>& result)
{
	if (!root) throw std::exception("root was null at Harvester::findPageLinks");

	const GumboNode * pagesRoot = nullptr;

	findNodeWithAttr(GUMBO_TAG_DIV, "class", "lotListPager", root, &pagesRoot);

	if (!pagesRoot) throw std::exception("Pager root node was not found");

	GumboNode * pageList = nullptr;

	for (unsigned int i = 0; i < pagesRoot->v.element.children.length; i++)
	{
		GumboNode * node = (GumboNode *)pagesRoot->v.element.children.data[i];

		if (node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_CENTER)
		{
			pageList = node;
		}
	}

	std::list<GumboNode *> links;

	findAll(GUMBO_TAG_A, pageList, "href", "?id=", links);

	for (auto it = links.begin(); it != links.end(); it++)
	{
		GumboNode * node = *it;
		GumboAttribute * attr = (GumboAttribute *)node->v.element.attributes.data[0];
		result.push_back(attr->value);
	}
	
	return;
}

const std::vector<Lot>& Harvester::getResults()
{
	return this->lotCards;
}
