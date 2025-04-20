#pragma once
#include <wx/wx.h>
class RVA_Config{
public:
	RVA_Config();
	wxString name;
	wxString macro;
	wxString type;
	wxArrayString range;
};

