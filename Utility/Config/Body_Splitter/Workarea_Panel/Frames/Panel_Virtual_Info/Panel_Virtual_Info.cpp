#include "Panel_Virtual_Info.h"
#include "../../../../main.h"



Panel_Virtual_Info::Panel_Virtual_Info(wxWindow* parent)
	:wxPanel(parent, wxID_ANY) {
	SetBackgroundColour(*wxWHITE);
	wxStaticBoxSizer* box = new wxStaticBoxSizer(wxVERTICAL, this, "�������Ϣ");
	count = new wxStaticText(this, wxID_ANY, wxEmptyString);

	box->Add(count, 1, wxALL | wxEXPAND, 10);

	SetSizer(box);
}

void Panel_Virtual_Info::SetPanelVirtualInfo() {
	count->SetLabel(wxString::Format("��ǰ��Ч���������Ϣ��:\t%d", int(Main::proj_info->rvp->vm_manager->virtual_machine.size())));
	Layout();
}
