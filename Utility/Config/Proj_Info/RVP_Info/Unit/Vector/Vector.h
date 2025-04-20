#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class Vector {
public:
	Vector(wxXmlNode* root);
	Vector(const wxString& _Name, const wxString& _Number);

	const wxString toString(const wxString& n = wxEmptyString)const;
	wxString GetName() { return Name.Upper(); }

	void AddInfo(wxXmlNode*& root);

	wxString Name;
	wxString Number;

};

