#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

class VM_BaseInfo{
public:
	VM_BaseInfo();
	VM_BaseInfo(wxXmlNode* root);
	void AddInfo(wxXmlNode*& root);
	const wxString toString()const;

	wxArrayString GetCoprocessor()const;

	wxString Name;
	wxString Extra_Captbl;
	wxString Coprocessor;
	wxString Vector_Stack_Size;
	wxString User_Stack_Size;
	wxString Priority;
	wxString Timeslice;
	wxString Period;
	wxString Watchdog;
	wxString Vector_Num;
	wxString Guest_Type;
	wxString Buildsystem;
	wxString Toolchain;
	wxString Optimization;
	wxString Project_Output;
	wxString Project_Overwrite;
	wxString Linker_Output;
	wxString Main_Header_Output;
	wxString Main_Source_Output;
	wxString Virtual_Header_Output;
	wxString Virtual_Header_Overwrite;
	wxString Virtual_Source_Output;
	wxString Virtual_Source_Overwrite;


};

