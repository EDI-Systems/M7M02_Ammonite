#include <wx/wx.h>
#include <wx/notebook.h>

#include "Thread_Unit.h"
#include "../../../../../Func/func.h"
#include "../../../../../main.h"

Thread_Unit::Thread_Unit(
    wxWindow* parent, 
    const wxString& _errorloc,
    std::function<void()>func)
    :UnitBase(parent, _errorloc, func) {

    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* stbox = new wxStaticBoxSizer(wxVERTICAL, this, "title");
    //wxStaticText* txt1 = new wxStaticText(this, wxID_ANY, "Name");
    //tcr_name = new wxTextCtrl(this, wxID_ANY);

    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc, func);
    t_stack_size = new SizeSlider(this, "Stack_Size", wxEmptyString, errorloc, func);
    t_parameter = new MyParameter(this, "Parameter", wxEmptyString, errorloc, func);
    t_priority = new MyThreadPrio(this, "Priority", wxEmptyString, errorloc, func,
        true, "该值应大于等于5, 且小于等于 Kernel Priority - 2（Kernrl Priority 在 内核配置 中）");

    stbox->Add(base_name, 0, wxEXPAND | wxALL, 5);
    stbox->Add(0, 5);
    stbox->Add(t_stack_size, 0, wxEXPAND | wxALL, 5);
    stbox->Add(0, 5);
    stbox->Add(t_parameter, 0, wxEXPAND | wxALL, 5);
    stbox->Add(0, 5);
    stbox->Add(t_priority, 0, wxEXPAND | wxALL, 5);

    box->Add(stbox, 0, wxALL | wxEXPAND, 5);
    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
    this->SetSizer(box);
    this->FitInside();
}


void Thread_Unit::SetUnit(const std::unique_ptr<Thread>& ptr) {
    this->base_name->SetValue(ptr->Name);
    this->t_stack_size->SetValue(ptr->Stack_Size);
    this->t_parameter->SetValue(ptr->Parameter);
    this->t_priority->SetValue(ptr->Priority);
}

//wxXmlNode* Thread_Unit::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Stack_Size", t_stack_size->GetValue());
//    myfunc::SetNodeCont(root, "Parameter", t_parameter->GetValue());
//    myfunc::SetNodeCont(root, "Priority", t_priority->GetValue());
//    return root;
//}

void Thread_Unit::OnDelete(const wxCommandEvent& event) {
    wxNotebook* notebook = (wxNotebook*)this->GetParent();  // 父窗口必须是UnitM_Manager提供的notebook
    int response = wxMessageBox("确认删除当前页信息", "确认删除", wxYES_NO | wxICON_QUESTION);
    if (response != wxYES)
        return; // 如果用户选择 "否"，则直接返回
    int count = notebook->GetPageCount();
    if (count <= 1) {
        wxMessageBox("每一个进程中至少要有一个线程存在", "删除失败");
        return;
    }
    for (int i = 0; i < notebook->GetPageCount(); i++) {
        Thread_Unit* cur = (Thread_Unit*)notebook->GetPage(i);
        if (cur->FindWindow(this->delete_unit->GetId())) {
            cur->ClearError();
            notebook->DeletePage(i);
            break;
        }
    }
    return;
}

bool Thread_Unit::IsLegal() {
    bool j1 = base_name->IsLegal();
    bool j2 = t_stack_size->IsLegal();
    bool j3 = t_parameter->IsLegal();
    bool j4 = t_priority->IsLegal(Main::proj_info->rvp->GetKernelPrio());
    return j1 && j2 && j3 && j4;
}

void Thread_Unit::ClearError() {
    base_name->CorrectSet();
    t_stack_size->CorrectSet();
    t_parameter->CorrectSet();
    t_priority->CorrectSet();
}

std::unique_ptr<Thread> Thread_Unit::GetUnit() {
    return std::make_unique<Thread>(base_name->GetValue(), t_stack_size->GetValue(),
        t_parameter->GetValue(), t_priority->GetValue());
}
