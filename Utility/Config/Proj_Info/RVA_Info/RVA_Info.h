#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

#include <map>
#include <set>
#include <vector>

#include "RVA_Config/RVA_Config.h"


class RVA_Info{
public:
	RVA_Info();
	void SetRVA(const wxString& path);

	wxString loc;

	wxString name;
	wxArrayString buildsys;
	wxArrayString toolchain;
	wxArrayString guest;

	wxXmlNode* xml_root;

	std::vector<wxArrayString>compatible;
	std::vector<std::unique_ptr<RVA_Config>> configs;
private:
	wxXmlNode* PathToNode(const wxString& path);
};

