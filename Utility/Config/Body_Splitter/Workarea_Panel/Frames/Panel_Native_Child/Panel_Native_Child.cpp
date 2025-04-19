#include "Panel_Native_Child.h"
#include "../../../../Func/func.h"
#include "../../../../main.h"

#include "../../../../Proj_Info/RVP_Info/Native_Manager/Native/Native.h"

Panel_Native_Child::Panel_Native_Child(wxWindow* parent, const int& _id, const wxString& _errorloc)
    :wxFrame(parent, wxID_ANY, "原生进程管理", wxDefaultPosition, wxSize(700, 650), wxDEFAULT_FRAME_STYLE), 
    errorloc(_errorloc), id(_id) {
    std::function<void()>func = [=]() {this->Show(); };

    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    nv_notebook = new wxNotebook(this, wxID_ANY);

    // baseinfo直接给root就行，因为baseinfo里的信息全都是root的直接子节点
    base_info = new Native_Child_BaseInfo(nv_notebook, id, errorloc + "base info/", func);
    //base_info->SetNativeBaseInfo();
    // 以Memory为例
    // 读取时，找到mem_node节点后将其信息读到各个mem_unit中
    // 新建时，新建的mem_node节点没有子节点，但是mem_node会被记录在mem_unit中
    // 等到Memory页面点击“确定”时，再将新建页面的信息挂到mem_node上
    memUnit = new Unit_Manager_Test<Mem_Unit, Memory>(nv_notebook, "M", errorloc + "Memory/", func);
    shmemUnit = new Unit_Manager_Test<Shmem_Unit, Shmem>(nv_notebook, "S", errorloc + "Shmem/", func);

    // 有原生进程就至少要有一个线程 todo done
    threadUnit = new Unit_Manager_Test<Thread_Unit, Thread>(nv_notebook, "T", errorloc + "Thead/", func);
    threadUnit->AddFunc();
    invocationUnit = new Unit_Manager_Test<Invocation_Unit, Invocation>(nv_notebook, "I", errorloc + "Invocation/", func);
    portUnit = new Unit_Manager_Test<Port_Unit, Port>(nv_notebook, "P", errorloc + "Port/", func);
    receiveUnit = new Unit_Manager_Test<Receive_Unit, Receive>(nv_notebook, "R", errorloc + "Receive/", func);
    sendUnit = new Unit_Manager_Test<Send_Unit, Send>(nv_notebook, "S", errorloc + "Send/", func);
    kfuncUnit = new Unit_Manager_Test<Kfunc_Unit, Kfunc>(nv_notebook, "K", errorloc + "Kfunc/", func);
    vectorUnit = new Unit_Manager_Test<Vector_Unit, Vector>(nv_notebook, "V", errorloc + "Vector/", func);

    wxPanel* page_green = new wxPanel(nv_notebook, wxID_ANY);
    page_green->SetBackgroundColour(*wxGREEN);

    nv_notebook->AddPage(base_info, "Base Info", true);
    nv_notebook->AddPage(memUnit, "Memory");
    nv_notebook->AddPage(shmemUnit, "Shmem");
    nv_notebook->AddPage(threadUnit, "Thread");
    nv_notebook->AddPage(invocationUnit, "Invocation");
    nv_notebook->AddPage(portUnit, "Port");
    nv_notebook->AddPage(receiveUnit, "Receive");
    nv_notebook->AddPage(sendUnit, "Send");
    nv_notebook->AddPage(kfuncUnit, "Kfunc");
    nv_notebook->AddPage(vectorUnit, "Vector");
    nv_notebook->AddPage(page_green, "Green");

    wxPanel* confirm_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    wxBoxSizer* confirm_box = new wxBoxSizer(wxHORIZONTAL);
    wxButton* tot_confirm = new wxButton(confirm_panel, wxID_ANY, "保存原生进程设置");
    wxButton* btn_preview = new wxButton(confirm_panel, wxID_ANY, "预览");
    confirm_box->Add(tot_confirm, 0, wxALL, 5);
    confirm_box->Add(btn_preview, 0, wxALL, 5);
    confirm_panel->SetSizer(confirm_box);

    box->Add(nv_notebook, 1, wxEXPAND);
    box->Add(confirm_panel, 0, wxEXPAND);

    this->SetSizer(box);
    Bind(wxEVT_CLOSE_WINDOW, &Panel_Native_Child::OnClose, this);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Panel_Native_Child::OnPreview, this, btn_preview->GetId());
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Panel_Native_Child::OnTotConfirm, this, tot_confirm->GetId());
}
void Panel_Native_Child::ClearError() {
    base_info->ClearError();
    memUnit->ClearError();
    shmemUnit->ClearError();
    threadUnit->ClearError();
    invocationUnit->ClearError();
    portUnit->ClearError();
    receiveUnit->ClearError();
    sendUnit->ClearError();
    vectorUnit->ClearError();
    kfuncUnit->ClearError();
}

wxString Panel_Native_Child::GetBaseInfoName() {
    return base_info->name->GetValue();
}

void Panel_Native_Child::OnClose(const wxCloseEvent& event) {
    this->Hide();
}

bool Panel_Native_Child::IsLegal() {
    bool j1 = base_info->IsLegal();
    bool j2 = memUnit->IsLegal();
    bool j3 = shmemUnit->IsLegal();
    bool j4 = threadUnit->IsLegal();
    bool j5 = invocationUnit->IsLegal();
    bool j6 = portUnit->IsLegal();
    bool j7 = receiveUnit->IsLegal();
    bool j8 = sendUnit->IsLegal();
    bool j9 = vectorUnit->IsLegal();
    bool j10 = kfuncUnit->IsLegal();
    return j1 && j2 && j3 && j4 && j5 && j6 && j7 && j8 && j9 && j10;
}

bool Panel_Native_Child::TotConfirmFunc() {
    // 检查GUI中的信息是否合法
    // 合法
    if (IsLegal()) {
        // 从rvp信息中新建并传递给本类
        Native* ptr = Main::proj_info->rvp->native_manager->GetNativeByID_Write(id);
        if (ptr) {
            base_info->ConfirmFunc(ptr->base_info);
            memUnit->ConfirmFunc(ptr->memory);
            shmemUnit->ConfirmFunc(ptr->shmem);
            threadUnit->ConfirmFunc(ptr->thread);
            invocationUnit->ConfirmFunc(ptr->invocation);
            portUnit->ConfirmFunc(ptr->port);
            receiveUnit->ConfirmFunc(ptr->receive);
            sendUnit->ConfirmFunc(ptr->send);
            vectorUnit->ConfirmFunc(ptr->vector);
            kfuncUnit->ConfirmFunc(ptr->kfunc);
            wxMessageBox(wxString::Format("原生进程检查合法【id:%d】", this->id), "保存原生进程信息");
        }
        else {
            std::unique_ptr<Native>temp = std::make_unique<Native>(this->id);
            base_info->ConfirmFunc(temp->base_info);
            memUnit->ConfirmFunc(temp->memory);
            shmemUnit->ConfirmFunc(temp->shmem);
            threadUnit->ConfirmFunc(temp->thread);
            invocationUnit->ConfirmFunc(temp->invocation);
            portUnit->ConfirmFunc(temp->port);
            receiveUnit->ConfirmFunc(temp->receive);
            sendUnit->ConfirmFunc(temp->send);
            vectorUnit->ConfirmFunc(temp->vector);
            kfuncUnit->ConfirmFunc(temp->kfunc);
            Main::proj_info->rvp->native_manager->native.push_back(std::move(temp));    
            wxMessageBox(wxString::Format("原生进程检查合法【id:%d】", this->id), "新建原生进程信息");
        }
        return true;
    }
    return false;
}

void Panel_Native_Child::OnTotConfirm(const wxCommandEvent& event) {
    TotConfirmFunc();
}

void Panel_Native_Child::OnPreview(const wxCommandEvent& event) {
    const Native* ptr = Main::proj_info->rvp->native_manager->GetNativeByID_ReadOnly(this->id);
    if (ptr) {
        myfunc::PreviewHelper(ptr->toString(), "源码预览 原生进程");
    }
    else {
        wxMessageBox(wxString::Format("请尝试保存当前原生进程的合法信息【id:%d】", this->id), "无法预览", wxICON_ERROR);
    }
}

void Panel_Native_Child::SetNativeChild(const Native* ptr) {
    base_info->SetNativeBaseInfo(ptr->base_info);
    memUnit->SetGUI(ptr->memory);
    shmemUnit->SetGUI(ptr->shmem);
    threadUnit->SetGUI(ptr->thread);
    invocationUnit->SetGUI(ptr->invocation);
    portUnit->SetGUI(ptr->port);
    receiveUnit->SetGUI(ptr->receive);
    sendUnit->SetGUI(ptr->send);
    vectorUnit->SetGUI(ptr->vector);
    kfuncUnit->SetGUI(ptr->kfunc);
}