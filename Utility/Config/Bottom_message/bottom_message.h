#pragma once
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>

class Bottom_Message
	:public wxBoxSizer {
public:
	Bottom_Message(wxWindow* parent);
	void SetBottomMessage(const wxString& message, const wxColour& color);
	wxPanel* alarm;
	wxTextCtrl* message;
};
