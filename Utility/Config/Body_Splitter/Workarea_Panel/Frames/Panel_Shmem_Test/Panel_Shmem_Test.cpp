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
	// 判断每个Mem_Unit是否合法
	bool j1 = shmem->IsLegal();

	return j1;
}


bool Panel_Shmem_Test::SaveShmem() {
	if (!IsLegal()) {
		wxMessageBox("错误！");
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
	myfunc::PreviewHelper(Main::proj_info->rvp->shmem->toString(), "共享内存块");
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
//		// 默认0项
//		pages->SetSelection(0);
//
//		// 默认填充0项
//		shmem->SetUnit(Main::proj_info->rvp->shmem->memory[0]);
//	}
//	else {
//		wxMessageBox("无共享内存块");
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
//	// 最大分配序号加
//	pages->Append(wxString::Format("%d", Main::proj_info->rvp->shmem->max_count));
//
//
//	// 维护序号
//	Main::proj_info->rvp->shmem->max_count++;
//}
//
//
//void Panel_Shmem_Test::OnDeleteButtonClicked(const wxCommandEvent& event) {
//	// 确认
//	if (wxMessageBox("确认删除当前共享内存块", "确认删除", wxYES_NO | wxICON_QUESTION) != wxYES)
//		return;
//	// 从模型层上删除
//	const int tar = wxAtoi(pages->GetString(pages->GetSelection()));
//	if (Main::proj_info->rvp->shmem->ReMoveMemory(tar)) {
//		wxMessageBox("删除成功");
//	}
//
//	// 清空当前页面
//}