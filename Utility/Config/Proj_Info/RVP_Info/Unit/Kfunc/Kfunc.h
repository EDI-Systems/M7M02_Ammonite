#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class Kfunc{
public:
	Kfunc(wxXmlNode* root);
	Kfunc(const wxString& _Name, const wxString& _Begin, const wxString& _End);

	wxString GetName() { return Name.Upper(); }
	const wxString toString(const wxString& n = wxEmptyString)const;

	void AddInfo(wxXmlNode*& root);

	wxString Name;
	wxString Begin;
	wxString End;
};

