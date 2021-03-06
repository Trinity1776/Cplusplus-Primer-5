#include "TextQuery.h"
#include "QueryResult.h"
#include "Make_plural.h"
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <utility>
#include <tuple>
using std::size_t;
using std::shared_ptr;
using std::istringstream;
using std::string;
using std::getline;
using std::vector;
using std::map;
using std::set;
using std::cerr;
using std::cout;
using std::cin;
using std::ostream;
using std::endl;
using std::ifstream;
using std::ispunct;
using std::tolower;
using std::pair;
TextQuery::TextQuery(ifstream &is) : file(new std::vector<std::string>, DebugDelete()) {
	string text;
	while (getline(is, text)) {
		file->push_back(text);
		int n = file->size() - 1;
		istringstream line(text);
		string word;
		while (line >> word) {
			word = cleanup_str(word);
			auto &lines = wm[word];
			if (!lines) {
				lines.reset(new set<line_no>, DebugDelete());
			}
			lines->insert(n);
		}
	}
}

TextQuery::tup TextQuery::query(const string &sought) const {
	
	static shared_ptr<set<line_no>> nodata(new set<line_no>, DebugDelete());
	
	auto loc = wm.find(cleanup_str(sought));
	if (loc == wm.end())
		return TextQuery::tup{ sought, nodata, file };
	else
		return TextQuery::tup{ sought, loc->second, file };
}
std::string TextQuery::cleanup_str(const std::string& s) {
	string ret;
	for (auto it = s.begin(); it != s.end(); ++it) {
		if (!ispunct(*it)) {
			ret += tolower(*it);
		}
	}
	return ret;
}
std::ostream& print(std::ostream &os, TextQuery::tup& t) {
	os << std::get<0>(t) << " occurs " << std::get<1>(t)->size() << " "
		<< make_plural(std::get<1>(t)->size(), "time", "s") << std::endl;

	for (auto &line_num : *std::get<1>(t)) {
		os << "\t(line " << line_num + 1 << ") " << std::get<2>(t)->at(line_num) << std::endl;
	}
	return os;
}

std::ostream &print(std::ostream &os, const QueryResult &qr) {
	os << qr.sought << " occurs " << qr.lines->size() << " "
		<< make_plural(qr.lines->size(), "time", "s") << endl;

	for (auto &num : *qr.lines) {

		cout << "\t(line " << num + 1 << ") " << qr.file->at(num) << endl;
	}
	return os;
}