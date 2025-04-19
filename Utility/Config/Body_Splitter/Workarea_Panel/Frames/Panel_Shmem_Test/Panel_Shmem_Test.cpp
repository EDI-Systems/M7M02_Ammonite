#include "Panel_Shmem_Test.h"
#include "../../../../main.h"

Panel_Shmem_Test::Panel_Shmem_Test(
	wxWindow* parent,
	wxWindowID id,
	const wxString& _errorloc,
	const bool& _makeGUI,
	std::function<void()>_func)
	:PanelBase(parent, wxID_ANY, _errorloc, _makeGUI, _func) {

	shmem = new Unit_Manager_Test<Mem_Unit, Memory>(swindow, "S", errorloc, func);
	box->Add(shmem, 1, wxALL | wxEXPAND, 5);

	swindow->SetSizer(box);
	swindow->FitInside();

}

void Panel_Shmem_Test::SetPanelShmemTest() {
	shmem->SetGUI(Main::proj_info->rvp->shmem->memory);
	makeGUI = true;
}

bool Panel_Shmem_Test::IsLegal() {
	if (!makeGUI) {
		SetPanelShmemTest();
	}
	// �ж�ÿ��Mem_Unit�Ƿ�Ϸ�
	bool j1 = shmem->IsLegal();

	return j1;
}


bool Panel_Shmem_Test::SaveShmem() {
	if (!IsLegal()) {
		wxMessageBox("����");
		return false;
	}
	shmem->ConfirmFunc(Main::proj_info->rvp->shmem->memory);
	return true;
}

void Panel_Shmem_Test::ClearError() {
	shmem->ClearError();
}

void Panel_Shmem_Test::OnConfirm(const wxCommandEvent& event) {
	SaveShmem();
}

void Panel_Shmem_Test::OnPreview(const wxCommandEvent& event) {
	myfunc::PreviewHelper(Main::proj_info->rvp->shmem->toString(), "�����ڴ��");
}

void Panel_Shmem_Test::OnRefresh(const wxCommandEvent& event) {
	SetPanelShmemTest();
}

//void Panel_Shmem_Test::ClearGUI() {
//	shmem->ClearAll();
//}

//
//
//void Panel_Shmem_Test::SetPanelShmemTest() {
//	pages->Clear();
//	for (const auto& shmem_ptr : Main::proj_info->rvp->shmem->memory) {
//		if (shmem_ptr) {
//			pages->Append(wxString::Format("%d", shmem_ptr->key));
//		}
//	}
//	if (pages->GetCount() > 0) {
//		// Ĭ��0��
//		pages->SetSelection(0);
//
//		// Ĭ�����0��
//		shmem->SetUnit(Main::proj_info->rvp->shmem->memory[0]);
//	}
//	else {
//		wxMessageBox("�޹����ڴ��");
//	}
//}
//
//void Panel_Shmem_Test::OnChoiceSelected(const wxCommandEvent& event) {
//	wxString selected = pages->GetString(pages->GetSelection());
//	wxMessageBox("You selected: " + selected, "Selection", wxOK | wxICON_INFORMATION);
//}
//
//
//void Panel_Shmem_Test::OnAddButtonClicked(const wxCommandEvent& event) {
//	// ��������ż�
//	pages->Append(wxString::Format("%d", Main::proj_info->rvp->shmem->max_count));
//
//
//	// ά�����
//	Main::proj_info->rvp->shmem->max_count++;
//}
//
//
//void Panel_Shmem_Test::OnDeleteButtonClicked(const wxCommandEvent& event) {
//	// ȷ��
//	if (wxMessageBox("ȷ��ɾ����ǰ�����ڴ��", "ȷ��ɾ��", wxYES_NO | wxICON_QUESTION) != wxYES)
//		return;
//	// ��ģ�Ͳ���ɾ��
//	const int tar = wxAtoi(pages->GetString(pages->GetSelection()));
//	if (Main::proj_info->rvp->shmem->ReMoveMemory(tar)) {
//		wxMessageBox("ɾ���ɹ�");
//	}
//
//	// ��յ�ǰҳ��
//}