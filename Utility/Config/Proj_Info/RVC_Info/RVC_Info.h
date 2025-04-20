#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class RVC_Info {
public:
	RVC_Info();

	void SetRVC(const wxString& path);

	wxString loc;

	wxString name;
	wxString platform;

	wxArrayString compatible;
	wxArrayString coprocessor;

	wxXmlNode* xml_root;
private:
	wxXmlNode* PathToNode(const wxString& path);
};
