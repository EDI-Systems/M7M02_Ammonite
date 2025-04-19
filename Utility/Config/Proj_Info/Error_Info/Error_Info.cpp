#include "Error_Info.h"

Error_Info::Error_Info(){}

void Error_Info::AddInfo(const int& id, const wxString& loc, const wxString& cont, std::function<void()>func) {
	arr.insert(std::make_pair(id, std::make_unique<ErrorUnitInfo>(loc, cont, func)));
}

void Error_Info::RemoveInfo(const int& id) {
	if (arr.find(id) != arr.end())
		arr.erase(arr.find(id));
}

void Error_Info::RemoveAll() {
	arr.clear();
}