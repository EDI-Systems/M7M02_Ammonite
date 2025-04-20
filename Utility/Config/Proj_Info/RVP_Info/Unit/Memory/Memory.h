#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class Memory{
public:
	Memory(wxXmlNode* root);
	Memory(
		const wxString& _Name, const wxString& _Base,
		const wxString& _Size, const wxString& _Align,
		const wxString& _Type, const wxString& _Attribute);

	wxString GetName() { return Name.Upper(); }
	const wxString toString(const wxString& n = wxEmptyString)const;

	void AddInfo(wxXmlNode*& root);

	wxString Name;
	wxString Base;
	wxString Size;
	wxString Align;
	wxString Type;
	wxString Attribute;
};

