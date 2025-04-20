#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class Port{
public:
	Port(wxXmlNode* root);
	Port(const wxString& _Name, const wxString& _Process);

	wxString GetName() { return Name.Upper(); }
	const wxString toString(const wxString& n = wxEmptyString)const;

	void AddInfo(wxXmlNode*& root);

	wxString Name;
	wxString Process;

};

