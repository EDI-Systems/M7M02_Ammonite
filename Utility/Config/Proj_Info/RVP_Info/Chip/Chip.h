#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <map>

class Chip{
public:
	Chip();

	void InitChip();
	void SetChip(wxXmlNode* _root);
	void AddInfo(wxXmlNode*& root);
	void ClearInfo();

	wxString toString();

	wxString platform;
	wxString chip_class;
	wxString name;
	wxString coprocessor;
	std::map<wxString, wxString>config;

	//wxXmlNode* root;
};

