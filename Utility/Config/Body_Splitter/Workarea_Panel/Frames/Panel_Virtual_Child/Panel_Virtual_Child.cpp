#include "Panel_Virtual_Child.h"
#include "../../../../Func/func.h"

#include "../../../../main.h"
#include "../../../../Proj_Info/RVP_Info/VM_Manager/VirtualMachine/VirtualMachine.h"

Panel_Virtual_Child::Panel_Virtual_Child(wxWindow* parent, const int& _id, const wxString& _errorloc)
    :wxFrame(parent, wxID_ANY, "虚拟机管理", wxDefaultPosition, wxSize(700, 650), wxDEFAULT_FRAME_STYLE),
    errorloc(_errorloc), id(_id) {

    std::function<void()>func = [=]() {this->Show(); };

    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    vm_notebook = new wxNotebook(this, wxID_ANY);

    base_info = new Virtual_BaseInfo(vm_notebook, id, errorloc + "base info/", func);
    memUnit = new Unit_Manager_Test<Mem_Unit, Memory>(vm_notebook, "M", errorloc + "Memory/", func);
    shmemUnit = new Unit_Manager_Test<Shmem_Unit, Shmem>(vm_notebook, "S", errorloc + "Shmem/", func);
    sendUnit = new Unit_Manager_Test<Send_Unit, Send>(vm_notebook, "S", errorloc + "Send/", func);
    kfuncUnit = new Unit_Manager_Test<Kfunc_Unit, Kfunc>(vm_notebook, "K", errorloc + "Kfunc/", func);

    wxPanel* page_green = new wxPanel(vm_notebook, wxID_ANY);
    page_green->SetBackgroundColour(*wxGREEN);

    vm_notebook->AddPage(base_info, "Base Info", true);
    vm_notebook->AddPage(memUnit, "Mem");
    vm_notebook->AddPage(shmemUnit, "Shmem");
    vm_notebook->AddPage(sendUnit, "Send");
    vm_notebook->AddPage(kfuncUnit, "Kfunc");
    vm_notebook->AddPage(page_green, "Green");

    wxPanel* confirm_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    wxBoxSizer* confirm_box = new wxBoxSizer(wxHORIZONTAL);
    wxButton* tot_confirm = new wxButton(confirm_panel, wxID_ANY, "保存虚拟机设置");
    wxButton* btn_preview = new wxButton(confirm_panel, wxID_ANY, "预览");
    confirm_box->Add(tot_confirm, 0, wxALL, 5);
    confirm_box->Add(btn_preview, 0, wxALL, 5);
    confirm_panel->SetSizer(confirm_box);

    box->Add(vm_notebook, 1, wxEXPAND);
    box->Add(confirm_panel, 0, wxEXPAND);
    this->SetSizer(box);
    Bind(wxEVT_CLOSE_WINDOW, &Panel_Virtual_Child::OnClose, this);
    //Bind(wxEVT_SHOW, &Panel_Virtual_Child::OnShow, this);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Panel_Virtual_Child::OnTotConfirm, this, tot_confirm->GetId());
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Panel_Virtual_Child::OnPreview, this, btn_preview->GetId());
}

void Panel_Virtual_Child::OnClose(const wxCloseEvent& event) {
    this->Hide();
}


void Panel_Virtual_Child::ClearError() {
    base_info->ClearError();
    memUnit->ClearError();
    shmemUnit->ClearError();
    sendUnit->ClearError();
    kfuncUnit->ClearError();
}

void Panel_Virtual_Child::OnPreview(const wxCommandEvent& event) {
    const VirtualMachine* ptr = Main::proj_info->rvp->vm_manager->GetVmByID_ReadOnly(this->id);

    if (ptr) {
        myfunc::PreviewHelper(ptr->toString(), "源码预览 虚拟机");
    }
    else {
        wxMessageBox(wxString::Format("请尝试保存当前虚拟机的合法信息【id:%d】", this->id), "无法预览", wxICON_ERROR);
    }
}

void Panel_Virtual_Child::OnTotConfirm(const wxCommandEvent& event) {
    TotConfirmFunc();
}


bool Panel_Virtual_Child::IsLegal() {
    bool j1 = base_info->IsLegal();
    bool j2 = memUnit->IsLegal();
    bool j3 = shmemUnit->IsLegal();
    bool j4 = sendUnit->IsLegal();
    bool j5 = kfuncUnit->IsLegal();
    return j1 && j2 && j3 && j4 && j5;
}

bool Panel_Virtual_Child::TotConfirmFunc() {
    // 检查合法性
    if (IsLegal()) {
        // 从rvp信息中新建并传递给本类
        VirtualMachine* ptr = Main::proj_info->rvp->vm_manager->GetVmByID_Write(this->id);
        if (ptr) {
            base_info->ConfirmFunc(ptr->base_info);
            memUnit->ConfirmFunc(ptr->memory);
            shmemUnit->ConfirmFunc(ptr->shmem);
            sendUnit->ConfirmFunc(ptr->send);
            kfuncUnit->ConfirmFunc(ptr->kfunc);
            wxMessageBox(wxString::Format("虚拟机检查合法【id:%d】", this->id), "保存虚拟机信息");
        }
        else {
            std::unique_ptr<VirtualMachine>temp = std::make_unique<VirtualMachine>(this->id);
            base_info->ConfirmFunc(temp->base_info);
            memUnit->ConfirmFunc(temp->memory);
            shmemUnit->ConfirmFunc(temp->shmem);
            sendUnit->ConfirmFunc(temp->send);
            kfuncUnit->ConfirmFunc(temp->kfunc);
            Main::proj_info->rvp->vm_manager->virtual_machine.push_back(std::move(temp));
            wxMessageBox(wxString::Format("虚拟机检查合法【id:%d】", this->id), "新建虚拟机信息");
        }
        return true;
    }

    return false;
}

void Panel_Virtual_Child::SetVmChild(const VirtualMachine* ptr) {

    base_info->SetVirMachineBaseInfo(ptr->base_info);
    memUnit->SetGUI(ptr->memory);
    shmemUnit->SetGUI(ptr->shmem);
    sendUnit->SetGUI(ptr->send);
    kfuncUnit->SetGUI(ptr->kfunc);
}

wxString Panel_Virtual_Child::GetBaseInfoName() {
    return base_info->name->GetValue();
}