#pragma once

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/notebook.h>

#include "../Unit_Manager_Test/Unit_Manager_Test.h"

#include "../Units/Mem_Unit/Mem_Unit.h"
#include "../Units/Shmem_Unit/Shmem_Unit.h"
#include "../Units/Send_Unit/Send_Unit.h"
#include "../Units/Kfunc_Unit/Kfunc_Unit.h"

#include "Virtual_BaseInfo/Virtual_BaseInfo.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Memory/Memory.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Shmem/Shmem.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Send/Send.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Kfunc/Kfunc.h"
#include "../../../../Proj_Info/RVP_Info/VM_Manager/VirtualMachine/VirtualMachine.h"

class Panel_Virtual_Child
    :public wxFrame {
public:
    Panel_Virtual_Child(wxWindow* parent, const int& _id, const wxString& _errorloc = wxEmptyString);
    void SetVmChild(const VirtualMachine* ptr);
    wxString GetBaseInfoName();
    void ClearError();
    bool TotConfirmFunc();
    bool IsLegal();
    int id;
private:
    void OnClose(const wxCloseEvent& event);
    void OnPreview(const wxCommandEvent& event);
    void OnTotConfirm(const wxCommandEvent& event);
    wxNotebook* vm_notebook;
    Virtual_BaseInfo* base_info;

    Unit_Manager_Test<Mem_Unit, Memory>* memUnit;
    Unit_Manager_Test<Shmem_Unit, Shmem>* shmemUnit;
    Unit_Manager_Test<Send_Unit, Send>* sendUnit;
    Unit_Manager_Test<Kfunc_Unit, Kfunc>* kfuncUnit;

    wxString errorloc;
};
