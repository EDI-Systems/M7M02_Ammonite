
#include <wx/artprov.h>     // �ṩĬ��ͼ��

#include "tool_bar.h"
#include "../Image/test.xpm"
#include "../Func/func.h"
#include "../main.h"

Tool_Bar::Tool_Bar(class wxWindow* Parent)
	:wxToolBar(Parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT | wxTB_NODIVIDER) {
	select_frame = new Frame_Select(this);

	 // ���� .bmp ͼ��
	wxBitmap bmp1(wxT("Image\\toolbar_button1_2.bmp"), wxBITMAP_TYPE_BMP);
	import_source = AddTool(wxID_ANY, "��Դ�ļ���", bmp1, "ѡ��Ϸ���Platform�ļ������ڵ�λ��");


	wxBitmap bmp2(wxT("Image\\toolbar_button2_2.bmp"), wxBITMAP_TYPE_BMP);
	rva_preveiw = AddTool(wxID_ANY, "RVA", bmp2, "ƽ̨�����ļ�Ԥ��");


	wxBitmap bmp3(wxT("Image\\toolbar_button3_2.bmp"), wxBITMAP_TYPE_BMP);
	rvc_preveiw = AddTool(wxID_ANY, "RVC", bmp3, "оƬ�����ļ�Ԥ��");


	// �������
	Realize();
	// ���¼�
	Bind(wxEVT_TOOL, &Tool_Bar::OnImportSrc, this, import_source->GetId());
	Bind(wxEVT_TOOL, &Tool_Bar::OnRvaPreview, this, rva_preveiw->GetId());
	Bind(wxEVT_TOOL, &Tool_Bar::OnRvcPreview, this, rvc_preveiw->GetId());
}

void Tool_Bar::OnImportSrc(const wxCommandEvent& event) {
	this->select_frame->CenterOnScreen();
	this->select_frame->Show();
}


void Tool_Bar::OnRvaPreview(const wxCommandEvent& event) {
	if (Main::proj_info->rva->xml_root) {
		wxString str;
		myfunc::DFS(Main::proj_info->rva->xml_root, str);
		myfunc::PreviewHelper(str, "Դ��Ԥ�� .rva�ļ�");
	}
}

void Tool_Bar::OnRvcPreview(const wxCommandEvent& event) {
	if (Main::proj_info->rvc->xml_root) {
		wxString str;
		myfunc::DFS(Main::proj_info->rvc->xml_root, str);
		myfunc::PreviewHelper(str, "Դ��Ԥ�� .rvc�ļ�");
	}
}