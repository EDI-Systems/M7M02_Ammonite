#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
class Panel_Native_Info
	:public wxPanel{
public:
	Panel_Native_Info(wxWindow* parent);
	void SetPanelNativeInfo();
private:
	wxStaticText* count;
};

