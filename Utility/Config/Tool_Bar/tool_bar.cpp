
#include <wx/artprov.h>     // 提供默认图标

#include "tool_bar.h"
#include "../Image/test.xpm"
#include "../Func/func.h"
#include "../main.h"

Tool_Bar::Tool_Bar(class wxWindow* Parent)
	:wxToolBar(Parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT | wxTB_NODIVIDER) {
	select_frame = new Frame_Select(this);

	 // 加载 .bmp 图像
	wxBitmap bmp1(wxT("Image\\toolbar_button1_2.bmp"), wxBITMAP_TYPE_BMP);
	import_source = AddTool(wxID_ANY, "资源文件夹", bmp1, "选择合法的Platform文件夹所在的位置");


	wxBitmap bmp2(wxT("Image\\toolbar_button2_2.bmp"), wxBITMAP_TYPE_BMP);
	rva_preveiw = AddTool(wxID_ANY, "RVA", bmp2, "平台描述文件预览");


	wxBitmap bmp3(wxT("Image\\toolbar_button3_2.bmp"), wxBITMAP_TYPE_BMP);
	rvc_preveiw = AddTool(wxID_ANY, "RVC", bmp3, "芯片描述文件预览");


	// 激活工具栏
	Realize();
	// 绑定事件
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
		myfunc::PreviewHelper(str, "源码预览 .rva文件");
	}
}

void Tool_Bar::OnRvcPreview(const wxCommandEvent& event) {
	if (Main::proj_info->rvc->xml_root) {
		wxString str;
		myfunc::DFS(Main::proj_info->rvc->xml_root, str);
		myfunc::PreviewHelper(str, "源码预览 .rvc文件");
	}
}