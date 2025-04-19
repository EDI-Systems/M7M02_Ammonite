#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class Invocation{
public:
	Invocation(wxXmlNode* root);
	Invocation(const wxString& _Name, const wxString& _Stack_Size);

	wxString GetName() { return Name.Upper(); }
	const wxString toString(const wxString& n = wxEmptyString)const;

	void AddInfo(wxXmlNode*& root);

	wxString Name;
	wxString Stack_Size;

};

