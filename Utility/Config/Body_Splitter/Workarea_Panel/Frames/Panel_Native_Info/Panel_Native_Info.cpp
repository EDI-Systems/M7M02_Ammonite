#include "Panel_Native_Info.h"
#include "../../../../main.h"



Panel_Native_Info::Panel_Native_Info(wxWindow* parent)
	:wxPanel(parent, wxID_ANY) {
	SetBackgroundColour(*wxWHITE);
	wxStaticBoxSizer* box = new wxStaticBoxSizer(wxVERTICAL, this, "ԭ���߳���Ϣ");
	count = new wxStaticText(this, wxID_ANY, wxEmptyString);

	box->Add(count, 1, wxALL | wxEXPAND, 10);

	SetSizer(box);
}

void Panel_Native_Info::SetPanelNativeInfo() {
	count->SetLabel(wxString::Format("��ǰ��Ч��ԭ��������Ϣ��:\t%d", int(Main::proj_info->rvp->native_manager->native.size())));
	Layout();
}
