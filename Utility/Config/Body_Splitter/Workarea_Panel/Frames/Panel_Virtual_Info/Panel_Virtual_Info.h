#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
class Panel_Virtual_Info
	:public wxPanel {
public:
	Panel_Virtual_Info(wxWindow* parent);
	void SetPanelVirtualInfo();
private:
	wxStaticText* count;
};

