#pragma once
#include <wx/wx.h>
class ErrorUnitInfo{
public:
	ErrorUnitInfo(const wxString&_loc,const wxString&_cont,std::function<void()> _func);

	wxString loc;
	wxString cont;
	std::function<void()> func;

};

