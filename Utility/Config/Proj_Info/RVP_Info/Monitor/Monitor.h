#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class Monitor{
public:
	Monitor(){}
	void SetMonitor(wxXmlNode* _root);
	void AddInfo(wxXmlNode*& root);
	void ClaerInfo();
	wxString toString();

	wxString Code_Size;
	wxString Data_Size;
	wxString Init_Stack_Size;
	wxString Sftd_Stack_Size;
	wxString Vmmd_Stack_Size;
	wxString Extra_Captbl;
	wxString Virt_Prio;
	wxString Virt_Event;
	wxString Virt_Map;
	wxString Buildsystem;
	wxString Toolchain;
	wxString Optimization;
	wxString Project_Output;
	wxString Project_Overwrite;
	wxString Linker_Output;
	wxString Config_Header_Output;
	wxString Boot_Header_Output;
	wxString Boot_Source_Output;
	wxString Hook_Source_Output;
	wxString Hook_Source_Overwrite;

	//wxXmlNode* root;
};

