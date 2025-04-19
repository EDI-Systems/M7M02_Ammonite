#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class Receive{
public:
	Receive(wxXmlNode* root);
	Receive(const wxString& _Name);
	const wxString toString(const wxString& n = wxEmptyString)const { return "[Receive" + n + "]\n" + "\tName:\t" + Name + "\n"; }
	wxString GetName() { return Name.Upper(); }

	void AddInfo(wxXmlNode*& root);

	wxString Name;
};

