#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
class Extmem {
public:
	Extmem();
	void SetExtmem(wxXmlNode* _root);
	void AddInfo(wxXmlNode*& root);
	void ClearInfo();
	const wxString toString()const;

	wxString Base;
	wxString Size;
	wxString Type;
	wxString Attribute;
};

