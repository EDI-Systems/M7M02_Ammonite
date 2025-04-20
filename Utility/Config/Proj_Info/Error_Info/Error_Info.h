#pragma once
#include <wx/wx.h>
#include <map>

#include "ErrorUnitInfo/ErrorUnitInfo.h"
class Error_Info{
public:
	Error_Info();
	void AddInfo(const int& id, const wxString& loc, const wxString& cont, std::function<void()>func);
	void RemoveInfo(const int& id);
	void RemoveAll();
	std::map<int, std::unique_ptr<ErrorUnitInfo>>arr;
};

