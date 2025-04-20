#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class Send{
public:
	Send(wxXmlNode* root);
	Send(const wxString& _Name, const wxString& _Process);

	const wxString toString(const wxString& n = wxEmptyString)const;
	wxString GetName() { return Name.Upper(); }
	void AddInfo(wxXmlNode*& root);

	wxString Name;
	wxString Process;
};

