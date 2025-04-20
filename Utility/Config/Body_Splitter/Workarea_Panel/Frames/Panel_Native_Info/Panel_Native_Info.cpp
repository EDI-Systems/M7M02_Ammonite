#include "Panel_Native_Info.h"
#include "../../../../main.h"



Panel_Native_Info::Panel_Native_Info(wxWindow* parent)
	:wxPanel(parent, wxID_ANY) {
	SetBackgroundColour(*wxWHITE);
	wxStaticBoxSizer* box = new wxStaticBoxSizer(wxVERTICAL, this, "原生线程信息");
	count = new wxStaticText(this, wxID_ANY, wxEmptyString);

	box->Add(count, 1, wxALL | wxEXPAND, 10);

	SetSizer(box);
}

void Panel_Native_Info::SetPanelNativeInfo() {
	count->SetLabel(wxString::Format("当前有效的原生进程信息数:\t%d", int(Main::proj_info->rvp->native_manager->native.size())));
	Layout();
}
