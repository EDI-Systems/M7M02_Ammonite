#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class Kernel{
public:
	Kernel() {}
	void SetKernel(wxXmlNode* _root);
	void AddInfo(wxXmlNode*& root);
	void ClearInfo();
	wxString toString();

	wxString Code_Base;
	wxString Code_Size;
	wxString Data_Base;
	wxString Data_Size;
	wxString Stack_Size;
	wxString Extra_Kom;
	wxString Kom_Order;
	wxString Kern_Prio;
	wxString Buildsystem;
	wxString Toolchain;
	wxString Optimization;
	wxString Full_Image;
	wxString Project_Output;
	wxString Project_Overwrite;
	wxString Linker_Output;
	wxString Config_Header_Output;
	wxString Boot_Header_Output;
	wxString Boot_Source_Output;
	wxString Hook_Source_Output;
	wxString Hook_Source_Overwrite;
	wxString Handler_Source_Output;
	wxString Handler_Source_Overwrite;

	//wxXmlNode* root;
};

