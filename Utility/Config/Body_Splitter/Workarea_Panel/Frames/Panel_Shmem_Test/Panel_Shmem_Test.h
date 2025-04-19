#pragma once
#include <wx/wx.h>
#include <wx/notebook.h>
#include <vector>

#include "../PanelBase/PanelBase.h"
#include "../Units/Mem_Unit/Mem_Unit.h"
#include "../Unit_Manager_Test/Unit_Manager_Test.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Memory/Memory.h"

class Panel_Shmem_Test
	:public PanelBase {
public:
	Panel_Shmem_Test(
		wxWindow* parent,
		wxWindowID id = -1,
		const wxString& errorloc = wxEmptyString,
		const bool& makeGUI = false,
		std::function<void()>func = nullptr);
	void SetPanelShmemTest();
	bool SaveShmem();
	bool IsLegal()override;
	void ClearError()override;
private:
	Unit_Manager_Test<Mem_Unit, Memory>* shmem;

	void OnRefresh(const wxCommandEvent& event)override;
	void OnPreview(const wxCommandEvent& event)override;
	void OnConfirm(const wxCommandEvent& event)override;


};

