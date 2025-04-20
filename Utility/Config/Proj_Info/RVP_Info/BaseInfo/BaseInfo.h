#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class BaseInfo{
public:
	BaseInfo(){}

	void SetBaseInfo(wxXmlNode* root);
	void AddInfo(wxXmlNode*& root);
	void ClearInfo();
	wxString toString();

	wxString name;
	wxString version;
	wxString assert_enable;
	wxString debug_enbale;
	wxString pagetable_enable;
	wxString buildsys;
	wxString workspace_ow;
};

