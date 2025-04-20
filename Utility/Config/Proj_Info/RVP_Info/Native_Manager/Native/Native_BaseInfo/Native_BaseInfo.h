#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
class Native_BaseInfo{
public:
	// �½�ʱ�Ĺ��캯��
	Native_BaseInfo();
	// ����ʱ�Ĺ��캯��
	Native_BaseInfo(wxXmlNode* root);

	void AddInfo(wxXmlNode*& root);

	wxArrayString GetCoprocessor()const;
	wxString toString();

	wxString Name;
	wxString Extra_Captbl;
	wxString Coprocessor;
	wxString Buildsystem;
	wxString Toolchain;
	wxString Optimization;
	wxString Project_Output;
	wxString Project_Overwrite;
	wxString Linker_Output;
	wxString Main_Header_Output;
	wxString Main_Source_Output;
	wxString Entry_Source_Output;
	wxString Entry_Source_Overwrite;

};

