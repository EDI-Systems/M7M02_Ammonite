#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class Shmem{
public:
	Shmem(wxXmlNode* root);
	Shmem(const wxString& _Name, const wxString& _Attribute);

	const wxString toString(const wxString& n = wxEmptyString)const;
	wxString GetName() { return Name.Upper(); }

	void AddInfo(wxXmlNode*& root);


	wxString Name;
	wxString Attribute;

};

