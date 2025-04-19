#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class Thread{
public:
	Thread(wxXmlNode* root);
	Thread(const wxString& _Name, const wxString& _Stack_Size, const wxString& _Parameter, const wxString& _Priority);

	const wxString toString(const wxString& n = wxEmptyString)const;
	wxString GetName() { return Name.Upper(); }

	void AddInfo(wxXmlNode*& root);

	wxString Name;
	wxString Stack_Size;
	wxString Parameter;
	wxString Priority;

};

