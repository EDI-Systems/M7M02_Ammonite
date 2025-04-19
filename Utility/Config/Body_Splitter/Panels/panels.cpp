//#include "panels.h"
//#include "../../Func/func.h"
//#include "../../main.h"
//#include "../../RVP/rvp.h"
//#include "../File_Tree/Tree_Item_Data/tree_item_data.h"
//#include <wx/image.h>
//#include <wx/treectrl.h>
//
//Base_Unit::Base_Unit(wxWindow* parent, const wxString& _errorloc)
//    :wxScrolledWindow(parent, wxID_ANY) {
//    errorloc = _errorloc;
//
//    this->SetScrollRate(10, 10);
//
//    delete_unit = new wxButton(this, wxID_ANY, "删除");
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Base_Unit::OnDelete, this, delete_unit->GetId());
//}
//
//void Base_Unit::OnDelete(const wxCommandEvent& event) {
//    //wxMessageBox("基类func");
//    wxNotebook* notebook = (wxNotebook*)this->GetParent();  // 父窗口必须是UnitM_Manager提供的notebook
//    int response = wxMessageBox("确认删除当前页信息", "确认删除", wxYES_NO | wxICON_QUESTION);
//    if (response != wxYES)
//        return; // 如果用户选择 "否"，则直接返回
//    for (int i = 0; i < notebook->GetPageCount(); i++) {
//        Base_Unit* cur = (Base_Unit*)notebook->GetPage(i);
//        //Mem_Unit_Test* cur = (Mem_Unit_Test*)notebook->GetPage(i);
//        if (cur->FindWindow(this->delete_unit->GetId())) {
//            cur->ClearError();
//            notebook->DeletePage(i);
//            //cur->ClearError();
//            //cur->ClearError();
//            break;
//        }
//    }
//    return;
//}
//wxString Base_Unit::GetName() {
//    // “不分大小写地不同”，用Upper处理
//    if (base_name)
//        return base_name->GetValue().Upper();
//    return "Invalid";
//}
//
//void Base_Unit::NameCorrectSet() {
//    base_name->CorrectSet();
//}
//void Base_Unit::NameIncorrectSet(const wxString& str) {
//    base_name->IncorrectSet(str);
//}
//
///************************************************
//*   虚拟机（原生进程）管理 Mem unit
//************************************************/
//Mem_Unit_Test::Mem_Unit_Test(wxWindow* parent, const wxString& _errorloc)
//    :Base_Unit(parent, _errorloc) {
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//
//    //wxPanel* bck_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
//    //wxBoxSizer* bck_box = new wxBoxSizer(wxVERTICAL);
//    wxStaticBoxSizer* bod_mem = new wxStaticBoxSizer(wxVERTICAL, this);
//    mem_base = new BaseSlider(this, "Base", wxEmptyString, errorloc);
//    mem_base->SetDefaultColor(*wxWHITE);
//    mem_size = new SizeSlider(this, "Size", wxEmptyString, errorloc);
//    mem_size->SetDefaultColor(*wxWHITE);
//    bod_mem->Add(mem_base, 0, wxALL | wxEXPAND, 5);
//    bod_mem->Add(mem_size, 0, wxALL | wxEXPAND, 5);
//    box->Add(bod_mem, 0, wxALL | wxEXPAND, 10);
//
//    //bck_box->Add(bod_mem, 0, wxALL | wxEXPAND, 5);
//    //bck_panel->SetSizer(bck_box);
//
//    //wxPanel* bck_panel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
//    //wxBoxSizer* bck_box2 = new wxBoxSizer(wxVERTICAL);
//    wxStaticBoxSizer* other = new wxStaticBoxSizer(wxVERTICAL, this);
//    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc);
//    base_name->SetDefaultColor(*wxWHITE);
//    mem_align = new Align(this, "Align", wxEmptyString, errorloc);
//    mem_type = new MyChoice(this, "Type");
//    mem_type->SetDefaultColor(*wxWHITE);
//    wxArrayString arr = wxArrayString(3, new wxString[3]{ "Code","Data","Device" });
//    mem_type->FillChoice(arr);
//    mem_attribute = new MyAttribute(this, wxID_ANY, "Attribute", wxEmptyString, errorloc);
//    other->Add(base_name, 0, wxALL | wxEXPAND, 5);
//    other->Add(0, 5);
//    other->Add(mem_align, 0, wxALL | wxEXPAND, 5);
//    other->Add(0, 5);
//    other->Add(mem_type, 0, wxALL | wxEXPAND, 5);
//    other->Add(0, 5);
//    other->Add(mem_attribute, 0, wxALL | wxEXPAND, 5);
//
//    box->Add(other, 0, wxALL | wxEXPAND, 10);
//    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->SetSizer(box);
//    this->FitInside();
//}
//
//void Mem_Unit_Test::SetUnit(wxXmlNode* root) {
//    std::map<wxString, wxXmlNode*>* mp = new std::map<wxString, wxXmlNode*>;
//    for (wxXmlNode* son = root->GetChildren(); son; son = son->GetNext()) {
//        mp->insert(std::make_pair(son->GetName(), son));
//    }
//    this->base_name->SetValue(myfunc::FindorMake(root, "Name", mp)->GetNodeContent());
//    this->mem_base->SetValue(myfunc::FindorMake(root, "Base", mp)->GetNodeContent());
//    this->mem_size->SetValue(myfunc::FindorMake(root, "Size", mp)->GetNodeContent());
//    this->mem_align->SetValue(myfunc::FindorMake(root, "Align", mp)->GetNodeContent());
//
//    mem_type->SetSelection(mem_type->FindValue(myfunc::FindorMake(root, "Type", mp)->GetNodeContent()));
//    this->mem_attribute->SetValue(myfunc::FindorMake(root, "Attribute", mp)->GetNodeContent());
//}
//
//wxXmlNode* Mem_Unit_Test::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Base", mem_base->GetValue());
//    myfunc::SetNodeCont(root, "Size", mem_size->GetValue());
//    myfunc::SetNodeCont(root, "Align", mem_align->GetValue());
//    myfunc::SetNodeCont(root, "Type", mem_type->GetStringSelection());
//    myfunc::SetNodeCont(root, "Attribute", mem_attribute->GetValue());
//    return root;
//}
//
//bool Mem_Unit_Test::IsLegal() {
//    bool j1 = base_name->IsLegal();
//    bool j2 = mem_base->IsLegal();
//    bool j3 = mem_size->IsLegal();
//    bool j4 = mem_align->IsLegal(mem_base->GetValue());
//    bool j5 = mem_attribute->IsLegal();
//
//    return j1 && j2 && j3 && j4 && j5;
//    //bool judge = true;
//    //judge = judge &&
//    //    mem_base->IsLegal() &&
//    //    mem_size->IsLegal() &&
//    //    mem_align->IsLegal(mem_base->GetValue());
//    // 如果用这种写法，当其中某个值为false之后，相与后judge就确定为false，后续的IsLegal函数就不再执行了
//    //Layout();
//    //return judge;  // 各个IsLegal相与即可
//}
//
//void Mem_Unit_Test::ClearError() {
//    base_name->CorrectSet();
//    mem_base->CorrectSet();
//    mem_size->CorrectSet();
//    mem_align->CorrectSet();
//    mem_attribute->CorrectSet();
//}
//
///************************************************
//*    虚拟机（原生进程）管理   Shmem unit
//************************************************/
//Shmem_Unit_Test::Shmem_Unit_Test(wxWindow* parent, const wxString& _errorloc)
//    :Base_Unit(parent, _errorloc) {
//    //errorloc = _errorlc;
//
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//    wxStaticBoxSizer* stbox_shmem = new wxStaticBoxSizer(wxVERTICAL, this);
//
//    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc);
//    base_name->SetDefaultColor(*wxWHITE);
//
//    sh_attribute = new MyAttribute(this, wxID_ANY, "Attribute", wxEmptyString, errorloc);
//
//    stbox_shmem->Add(base_name, 0, wxALL | wxEXPAND, 5);
//    stbox_shmem->Add(0, 5);
//    stbox_shmem->Add(sh_attribute, 0, wxALL | wxEXPAND, 5);
//
//    box->Add(stbox_shmem, 0, wxALL | wxEXPAND, 5);
//    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->SetSizer(box);
//    this->FitInside();
//}
//
//
//void Shmem_Unit_Test::SetUnit(wxXmlNode* root) {
//    base_name->SetValue(myfunc::FindorMake(root, "Name")->GetNodeContent());
//    sh_attribute->SetValue(myfunc::FindorMake(root, "Attribute")->GetNodeContent());
//}
//
//wxXmlNode* Shmem_Unit_Test::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Attribute", sh_attribute->GetValue());
//    return root;
//}
//
//bool Shmem_Unit_Test::IsLegal() {
//    bool j1 = base_name->IsLegal();
//    bool j2 = sh_attribute->IsLegal();
//    return j1 && j2;
//}
//
//void Shmem_Unit_Test::ClearError() {
//    base_name->CorrectSet();
//    sh_attribute->CorrectSet();
//}
///************************************************
//*   虚拟机（原生进程）管理   Send unit
//************************************************/
//Send_Unit_Test::Send_Unit_Test(wxWindow* parent, const wxString& _errorloc)
//    :Base_Unit(parent, _errorloc) {
//
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//    wxStaticBoxSizer* stbox_send = new wxStaticBoxSizer(wxVERTICAL, this);
//
//    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc);
//    base_name->SetDefaultColor(*wxWHITE);
//
//    sd_process = new SimpleText(this, "Process", wxEmptyString, wxEmptyString, 0L, errorloc);
//
//    stbox_send->Add(base_name, 0, wxALL | wxEXPAND, 5);
//    stbox_send->Add(0, 5);
//    stbox_send->Add(sd_process, 0, wxALL | wxEXPAND, 5);
//
//
//    box->Add(stbox_send, 0, wxALL | wxEXPAND, 5);
//    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->SetSizer(box);
//
//    this->FitInside();
//}
//
//void Send_Unit_Test::SetUnit(wxXmlNode* root) {
//    base_name->SetValue(myfunc::FindorMake(root, "Name")->GetNodeContent());
//    sd_process->SetValue(myfunc::FindorMake(root, "Process")->GetNodeContent());
//}
//
//wxXmlNode* Send_Unit_Test::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Process", sd_process->GetValue());
//    return root;
//}
//
//bool Send_Unit_Test::IsLegal() {
//    bool j1 = base_name->IsLegal();
//    bool j2 = sd_process->IsLegal();
//    return j1 && j2;
//}
//
//void Send_Unit_Test::ClearError() {
//    base_name->CorrectSet();
//    sd_process->CorrectSet();
//}
//
//wxString Send_Unit_Test::GetName() {
//    if (base_name && sd_process)
//        return base_name->GetValue().Upper() + sd_process->GetValue().Upper();
//    return "Invalid";
//}
///************************************************
//*   虚拟机（原生进程）管理   Kfunc
//************************************************/
//Kfunc_Unit_Test::Kfunc_Unit_Test(wxWindow* parent, const wxString& _errorloc)
//    :Base_Unit(parent, _errorloc) {
//
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//    wxStaticBoxSizer* stbox_kfunc = new wxStaticBoxSizer(wxVERTICAL, this, "title");
//    //wxStaticText* txt_name = new wxStaticText(this, wxID_ANY, "Name");
//    //tcr_name = new wxTextCtrl(this, wxID_ANY);
//
//    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc);
//    base_name->SetDefaultColor(*wxWHITE);
//
//    k_begin = new BaseSlider(this, "Begain", wxEmptyString, errorloc);
//
//    k_end = new BaseSlider(this, "End", wxEmptyString, errorloc);
//
//    stbox_kfunc->Add(base_name, 0, wxALL | wxEXPAND, 5);
//    stbox_kfunc->Add(0, 5);
//    stbox_kfunc->Add(k_begin, 0, wxALL | wxEXPAND, 5);
//    stbox_kfunc->Add(0, 5);
//    stbox_kfunc->Add(k_end, 0, wxALL | wxEXPAND, 5);
//
//
//
//    box->Add(stbox_kfunc, 0, wxALL | wxEXPAND, 5);
//    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
//    SetSizer(box);
//    this->FitInside();
//}
//
//void Kfunc_Unit_Test::SetUnit(wxXmlNode* root) {
//    base_name->SetValue(myfunc::FindorMake(root, "Name")->GetNodeContent());
//    k_begin->SetValue(myfunc::FindorMake(root, "Begin")->GetNodeContent());
//    k_end->SetValue(myfunc::FindorMake(root, "End")->GetNodeContent());
//
//}
//
//wxXmlNode* Kfunc_Unit_Test::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Begin", k_begin->GetValue());
//    myfunc::SetNodeCont(root, "End", k_end->GetValue());
//    return root;
//}
//bool Kfunc_Unit_Test::IsLegal() {
//    bool j1 = base_name->IsLegal();
//    bool j2 = k_begin->IsLegal();
//    bool j3 = k_end->IsLegal();
//    bool j4 = myfunc::LaterGEHex(k_begin->GetValue(), k_end->GetValue());
//    if (!j4) 
//        k_end->IncorrectSet("增大结束值 该值不能比起始值低");
//    else 
//        k_end->CorrectSet();
//    return j1 && j2 && j3 && j4;
//}
//void Kfunc_Unit_Test::ClearError() {
//    base_name->CorrectSet();
//    k_begin->CorrectSet();
//    k_end->CorrectSet();
//}
///************************************************
//*   原生进程管理  Thread
//************************************************/
//Thread_Unit_Test::Thread_Unit_Test(wxWindow* parent, const wxString& _errorloc)
//    :Base_Unit(parent, _errorloc){
//
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//    wxStaticBoxSizer* stbox = new wxStaticBoxSizer(wxVERTICAL, this, "title");
//    //wxStaticText* txt1 = new wxStaticText(this, wxID_ANY, "Name");
//    //tcr_name = new wxTextCtrl(this, wxID_ANY);
//
//    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc);
//    base_name->SetDefaultColor(*wxWHITE);
//    t_stack_size = new SizeSlider(this, "Stack_Size", wxEmptyString, errorloc);
//    t_stack_size->SetDefaultColor(*wxWHITE);
//    t_parameter = new MyParameter(this, "Parameter", wxEmptyString, errorloc);
//    t_parameter->SetDefaultColor(*wxWHITE);
//    t_priority = new MyThreadPrio(this, "Priority", wxEmptyString, errorloc);
//    t_priority->SetCheckTip("该值的范围限制[5, Kernel Priority - 2]，见 内核配置->Kernrl Priority");
//    t_priority->SetDefaultColor(*wxWHITE);
//
//    stbox->Add(base_name, 0, wxEXPAND | wxALL, 5);
//    stbox->Add(0, 5);
//    stbox->Add(t_stack_size, 0, wxEXPAND | wxALL, 5);
//    stbox->Add(0, 5);
//    stbox->Add(t_parameter, 0, wxEXPAND | wxALL, 5);
//    stbox->Add(0, 5);
//    stbox->Add(t_priority, 0, wxEXPAND | wxALL, 5);
//
//    box->Add(stbox, 0, wxALL | wxEXPAND, 5);
//    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->SetSizer(box);
//    this->FitInside();
//}
//
//
//void Thread_Unit_Test::SetUnit(wxXmlNode* root) {
//    this->base_name->SetValue(myfunc::FindorMake(root, "Name")->GetNodeContent());
//    this->t_stack_size->SetValue(myfunc::FindorMake(root, "Stack_Size")->GetNodeContent());
//    this->t_parameter->SetValue(myfunc::FindorMake(root, "Parameter")->GetNodeContent());
//    this->t_priority->SetValue(myfunc::FindorMake(root, "Priority")->GetNodeContent());
//}
//
//wxXmlNode* Thread_Unit_Test::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Stack_Size", t_stack_size->GetValue());
//    myfunc::SetNodeCont(root, "Parameter", t_parameter->GetValue());
//    myfunc::SetNodeCont(root, "Priority", t_priority->GetValue());
//    return root;
//}
//
//void Thread_Unit_Test::OnDelete(const wxCommandEvent& event) {
//    wxNotebook* notebook = (wxNotebook*)this->GetParent();  // 父窗口必须是UnitM_Manager提供的notebook
//    int response = wxMessageBox("确认删除当前页信息", "确认删除", wxYES_NO | wxICON_QUESTION);
//    if (response != wxYES)
//        return; // 如果用户选择 "否"，则直接返回
//    int count = notebook->GetPageCount();
//    if (count <= 1) {
//        wxMessageBox("每一个进程中至少要有一个线程存在", "删除失败");
//        return;
//    }
//    for (int i = 0; i < notebook->GetPageCount(); i++) {
//        Thread_Unit_Test* cur = (Thread_Unit_Test*)notebook->GetPage(i);
//        if (cur->FindWindow(this->delete_unit->GetId())) {
//            cur->ClearError();
//            notebook->DeletePage(i);
//            break;
//        }
//    }
//    return;
//}
//
//bool Thread_Unit_Test::IsLegal() {
//    bool j1 = base_name->IsLegal();
//    bool j2 = t_stack_size->IsLegal();
//    bool j3 = t_parameter->IsLegal();
//    bool j4 = t_priority->IsLegal(Main::rvp->GetKernPrio());
//    return j1 && j2 && j3 && j4;
//}
//
//void Thread_Unit_Test::ClearError() {
//    base_name->CorrectSet();
//    t_stack_size->CorrectSet();
//    t_parameter->CorrectSet();
//    t_priority->CorrectSet();
//}
//
///************************************************
//*   原生进程管理  Invocation
//************************************************/
//Invocation_Unit_Test::Invocation_Unit_Test(wxWindow* parent, const wxString& _errorloc)
//    :Base_Unit(parent, _errorloc) {
//
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//    wxStaticBoxSizer* stbox = new wxStaticBoxSizer(wxVERTICAL, this, "title");
//    //wxStaticText* txt1 = new wxStaticText(this, wxID_ANY, "Name");
//    //this->tcr_name = new wxTextCtrl(this, wxID_ANY);
//
//    base_name = new SimpleText(this, "Name",wxEmptyString,wxEmptyString,0L,errorloc);
//    base_name->SetDefaultColor(*wxWHITE);
//
//    in_stack_size = new SizeSlider(this, "Stack_Size", wxEmptyString, errorloc);
//    in_stack_size->SetDefaultColor(*wxWHITE);
//
//    stbox->Add(base_name, 0, wxEXPAND | wxALL, 5);
//    stbox->Add(0, 5);
//    stbox->Add(in_stack_size, 0, wxEXPAND | wxALL, 5);
//
//    box->Add(stbox, 0, wxALL | wxEXPAND, 5);
//    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->SetSizer(box);
//    this->FitInside();
//}
//
//void Invocation_Unit_Test::SetUnit(wxXmlNode* root) {
//    this->base_name->SetValue(myfunc::FindorMake(root, "Name")->GetNodeContent());
//    this->in_stack_size->SetValue(myfunc::FindorMake(root, "Stack_Size")->GetNodeContent());
//}
//
//wxXmlNode* Invocation_Unit_Test::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Stack_Size", in_stack_size->GetValue());
//    return root;
//}
//
//bool Invocation_Unit_Test::IsLegal() {
//    bool j1 = base_name->IsLegal();
//    bool j2 = in_stack_size->IsLegal();
//    return j1 && j2;
//}
//
//void Invocation_Unit_Test::ClearError() {
//    base_name->CorrectSet();
//    in_stack_size->CorrectSet();
//}
//
///************************************************
//*   原生进程管理  Port
//************************************************/
//Port_Unit_Test::Port_Unit_Test(wxWindow* parent, const wxString& _errorloc)
//    :Base_Unit(parent, _errorloc) {
//
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//    wxStaticBoxSizer* stbox = new wxStaticBoxSizer(wxVERTICAL, this, "title");
//    //wxStaticText* txt1 = new wxStaticText(this, wxID_ANY, "Name");
//    //this->tcr_name = new wxTextCtrl(this, wxID_ANY);    
//
//    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc);
//    base_name->SetDefaultColor(*wxWHITE);
//
//    p_process = new SimpleText(this, "Process", wxEmptyString, wxEmptyString, 0L, errorloc);
//    p_process->SetDefaultColor(*wxWHITE);
//    stbox->Add(base_name, 0, wxEXPAND | wxALL, 5);
//    stbox->Add(0, 5);
//    stbox->Add(p_process, 0, wxEXPAND | wxALL, 5);
//
//    box->Add(stbox, 0, wxALL | wxEXPAND, 5);
//    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->SetSizer(box);
//    this->FitInside();
//}
//
//void Port_Unit_Test::SetUnit(wxXmlNode* root) {
//    this->base_name->SetValue(myfunc::FindorMake(root, "Name")->GetNodeContent());
//    this->p_process->SetValue(myfunc::FindorMake(root, "Process")->GetNodeContent());
//}
//
//wxXmlNode* Port_Unit_Test::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Process", p_process->GetValue());
//    return root;
//}
//
//bool Port_Unit_Test::IsLegal() {
//    bool j1 = base_name->IsLegal();
//    bool j2 = p_process->IsLegal();
//    return j1 && j2;
//}
//
//void Port_Unit_Test::ClearError() {
//    base_name->CorrectSet();
//    p_process->CorrectSet();
//}
//
//wxString Port_Unit_Test::GetName() {
//    if (base_name && p_process)
//        return base_name->GetValue().Upper() + p_process->GetValue().Upper();
//    return "Invalid";
//}
//
///************************************************
//*   原生进程管理  Receive
//************************************************/
//Receive_Unit_Test::Receive_Unit_Test(wxWindow* parent, const wxString& _errorloc)
//    :Base_Unit(parent, _errorloc){
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//    wxStaticBoxSizer* stbox = new wxStaticBoxSizer(wxVERTICAL, this, "title");
//    //wxStaticText* txt1 = new wxStaticText(this, wxID_ANY, "Name");
//    //this->tcr_name = new wxTextCtrl(this, wxID_ANY);
//
//    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc);;
//    base_name->SetDefaultColor(*wxWHITE);
//
//    stbox->Add(base_name, 0, wxEXPAND | wxALL, 5);
//
//    box->Add(stbox, 0, wxALL | wxEXPAND, 5);
//    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->SetSizer(box);
//    this->FitInside();
//}
//
//void Receive_Unit_Test::SetUnit(wxXmlNode* root) {
//    this->base_name->SetValue(myfunc::FindorMake(root, "Name")->GetNodeContent());
//}
//
//wxXmlNode* Receive_Unit_Test::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    return root;
//}
//
//bool Receive_Unit_Test::IsLegal() {
//    bool j1 = base_name->IsLegal();
//    return j1;
//}
//
//void Receive_Unit_Test::ClearError() {
//    base_name->CorrectSet();
//}
///************************************************
//*   原生进程管理  Vector
//************************************************/
//Vector_Unit_Test::Vector_Unit_Test(wxWindow* parent, const wxString& _errorloc)
//    :Base_Unit(parent, _errorloc) {
//
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//    wxStaticBoxSizer* stbox = new wxStaticBoxSizer(wxVERTICAL, this);
//    //wxStaticText* txt1 = new wxStaticText(this, wxID_ANY, "Name");
//    //tcr_name = new wxTextCtrl(this, wxID_ANY);
//    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc);
//    base_name->SetDefaultColor(*wxWHITE);
//
//    v_number = new MyZ_GE_Zero(this, "Number", wxEmptyString, errorloc);
//    v_number->SetDefaultColor(*wxWHITE);
//
//    stbox->Add(base_name, 0, wxEXPAND | wxALL, 5);
//    stbox->Add(0, 5);
//    stbox->Add(v_number, 0, wxEXPAND | wxALL, 5);
//
//    box->Add(stbox, 0, wxALL | wxEXPAND, 5);
//    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->SetSizer(box);
//    this->FitInside();
//}
//
//void Vector_Unit_Test::SetUnit(wxXmlNode* root) {
//    this->base_name->SetValue(myfunc::FindorMake(root, "Name")->GetNodeContent());
//    this->v_number->SetValue(myfunc::FindorMake(root, "Number")->GetNodeContent());
//}
//
//wxXmlNode* Vector_Unit_Test::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Number", v_number->GetValue());
//    return root;
//}
//
//bool Vector_Unit_Test::IsLegal() {
//    bool j1 = base_name->IsLegal();
//    bool j2 = v_number->IsLegal();
//    return j1 && j2;
//}
//
//
///************************************************
//*   PROJECT
//************************************************/
//Panel_Project::Panel_Project(wxWindow* parent)
//    :wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE) {
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//
//    wxStaticBoxSizer* stbox_preview = new wxStaticBoxSizer(wxVERTICAL, this, "预览");
//    wxButton* btn_preview = new wxButton(this, wxID_ANY, "项目描述文件");
//    stbox_preview->Add(btn_preview, 0, wxALL | wxALIGN_LEFT, 5);
//
//    box->Add(stbox_preview, 1, wxEXPAND | wxALL, 5);
//
//    this->SetSizer(box);
//
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Panel_Project::PreviewFunc, this, btn_preview->GetId());
//}
//
//void Panel_Project::PreviewFunc(wxCommandEvent& event) {
//    if (Main::rvp->root) {
//        wxString str;
//        myfunc::DFS(Main::rvp->root, str);
//        myfunc::PreviewHelper(str, "源码预览 项目描述文件");
//    }
//}
//
///************************************************
//*   BASE INFOMATION
//************************************************/
//Panel_BaseInfo::Panel_BaseInfo(wxWindow* parent)
//    :PanelBase(parent, wxID_ANY) {
//    errorloc = "基本信息->";
//    // 设置滚动速率 (单位是像素)
//
//    bs_name = new SimpleText(swindow, "Name", "工程生成器不关心该配置项的值", wxEmptyString, 0L, errorloc);
//    bs_name->SetDefaultColor(RGB(240, 240, 240));
//    bs_version = new MyVersion(swindow, "Version", "工程生成器不关心该配置项的值", errorloc);
//    bs_version->SetDefaultColor(RGB(240, 240, 240));
//    assert_e = new BoolBox(swindow, "Assert Enable", "是否对代码中的断言进行检查");
//    debug_e = new BoolBox(swindow, "Debug Log Enable", "是否启用RVM_DBG_开头的调试打印宏");
//    pagetble_e = new BoolBox(swindow, "Pagetable Raw Enable", "是否启用RVM_DBG_是否启用固定的用户态MPU元数据");
//
//    sys_tc_tp = new MyGrid(swindow, "Buildsystem Toolchain Type", "可用的合法组合", errorloc);
//    
//    wow = new BoolBox(swindow, "Workspace Overwrite", "重新生成工程时是否覆盖工作空间文件");
//
//    // 控件布局
//    box->Add(bs_name, 0, wxEXPAND | wxALL, 5);
//    box->Add(bs_version, 0, wxEXPAND | wxALL, 5);
//    box->Add(assert_e, 0, wxEXPAND | wxALL, 5);
//    box->Add(debug_e, 0, wxEXPAND | wxALL, 5);
//    box->Add(pagetble_e, 0, wxEXPAND | wxALL, 5);
//    box->Add(sys_tc_tp, 0, wxALL, 10);
//    box->Add(wow, 0, wxEXPAND | wxALL, 5);
//    swindow->SetSizer(box);
//    swindow->FitInside();
//
//    // 绑定 wxEVT_LEFT_DOWN 事件来捕获点击事件
//   // bs_name->Bind(wxEVT_LEFT_DOWN, &Panel_BaseInfo::OnChange, this);
//}
//
//
//void Panel_BaseInfo::SetBaseInfo(wxXmlNode* r) {
//    // 恢复需要恢复默认的
//    sys_tc_tp->Clear();
//    //stbox_buildsys->Clear(true);
//
//    // 记录到类中
//    name = myfunc::FindorMake(r, "Name");
//    version = myfunc::FindorMake(r, "Version");
//    assert_enable = myfunc::FindorMake(r, "Assert_Enable");
//    debug_log_enable = myfunc::FindorMake(r, "Debug_Log_Enable");
//    pagetable_enable = myfunc::FindorMake(r, "Pgtbl_Raw_Enable");
//    buildsys = myfunc::FindorMake(r, "Buildsystem");
//    workspace_overview = myfunc::FindorMake(r, "Workspace_Overwrite");
//
//    // 填充当前的
//    bs_name->SetValue(name->GetNodeContent());
//    bs_version->SetValue(version->GetNodeContent());
//    assert_e->SetValue(assert_enable->GetNodeContent());
//    debug_e->SetValue(debug_log_enable->GetNodeContent());
//    pagetble_e->SetValue(pagetable_enable->GetNodeContent());
//    wow->SetValue(workspace_overview->GetNodeContent());
//
//    // 从rvc文件中找到合法的组合
//    //for (auto i : Main::rva->GetCompatible()) {
//    //    sys_tc_tp->AppendOneRows(i);
//    //}
//    if (Main::work_mode == MODE_IMPORT) {
//        // 找到当前文件中的合法组合
//        sys_tc_tp->MatchFirst2nd(Main::rvp->GetSysArr());
//    }
//    else if (Main::work_mode == MODE_NEWFILE) {
//        ;
//    }
//
//
//    swindow->FitInside();
//}
//
//bool Panel_BaseInfo::IsLegal() {
//    bool j1 = bs_name->IsLegal();
//    bool j2 = bs_version->IsLegal();
//    bool j3 = sys_tc_tp->IsLegal();
//    return j1 && j2 && j3;
//}
//
//void Panel_BaseInfo::OnConfirm(const wxCommandEvent& event) {
//    SaveBaseInfo();
//}
//
//
//void Panel_BaseInfo::SaveBaseInfo() {
//    // 检查是否合法
//    if (!IsLegal()) {
//        wxMessageBox("错误", "基本信息");
//        return;
//    }
//    wxArrayString cur_arr = sys_tc_tp->GetValue();
//    name->GetChildren()->SetContent(bs_name->GetValue());
//    version->GetChildren()->SetContent(bs_version->GetValue());
//    assert_enable->GetChildren()->SetContent(assert_e->GetStringValue());
//    debug_log_enable->GetChildren()->SetContent(debug_e->GetStringValue());
//    pagetable_enable->GetChildren()->SetContent(pagetble_e->GetStringValue());
//    buildsys->GetChildren()->SetContent(cur_arr[0]);   // 填写本页中的buildsys
//    workspace_overview->GetChildren()->SetContent(wow->GetStringValue());
//    Main::rvp->cur_buildsys = cur_arr[0];
//    Main::rvp->cur_toolchain = cur_arr[1];
//    Main::rvp->cur_guest = cur_arr[2];
//    Main::body_splitter->file_tree->SavedBaseInfo();
//    return;
//}
//
///************************************************
//*   CHIP
//************************************************/
//Panel_Chip::Panel_Chip(wxWindow* parent)
//    :PanelBase(parent, wxID_ANY) {
//    // 创建控件
//    c_platform = new SimpleText(swindow, "Platform", "工程的架构平台", wxEmptyString, wxTE_READONLY);
//    c_platform->SetDefaultColor(RGB(240, 240, 240));
//    c_class = new SimpleText(swindow, "Class", "具体的芯片类别", wxEmptyString, wxTE_READONLY);
//    c_class->SetDefaultColor(RGB(240, 240, 240));
//    c_name = new MyChoice(swindow, "Name", "芯片的全称");
//    c_coprocessor = new MyCoprocessor(swindow, "Coprocessor", "项目中需要启用的协处理器");
//    c_coprocessor->SetDefaultColor(RGB(240, 240, 240));
//
//    stbox_config = new wxStaticBoxSizer(wxVERTICAL, swindow, "Config");
//
//    box->Add(c_platform, 0, wxALL | wxEXPAND, 5);
//    box->Add(c_class, 0, wxALL | wxEXPAND, 5);
//    box->Add(c_name, 0, wxALL | wxEXPAND, 5);
//    box->Add(c_coprocessor, 0, wxALL | wxEXPAND, 5);
//    box->Add(stbox_config, 0, wxALL | wxEXPAND, 15);
//
//    swindow->SetSizer(box);    
//    swindow->FitInside();
//}
//
//void Panel_Chip::SetChip(wxXmlNode* root) {
//    //this->chip = root;
//
//    //stbox_config->Clear(true);
//    //configs.clear();
//
//    //wxString text_platform = myfunc::FindorMake(chip, "Platform")->GetNodeContent();
//    //wxString text_class = myfunc::FindorMake(chip, "Class")->GetNodeContent();
//    //if (text_platform == wxEmptyString)
//    //    text_platform = Main::rva->GetName();
//    //if (text_class == wxEmptyString)
//    //    text_class = Main::rvc->GetName();
//
//    //c_platform->SetValue(text_platform);
//    //c_class->SetValue(text_class);
//
//    //wxString cur_chip_name = myfunc::FindorMake(chip, "Name")->GetNodeContent();
//    //wxArrayString chip_names = myfunc::Separater(Main::rvc->FindContent("#Chip#Compatible"), ",");
//    //chip_names.push_back("None");
//    //c_name->FillChoice(chip_names);
//    //c_name->SetSelection(c_name->FindValue(cur_chip_name));
//
//
//    //wxString str = myfunc::FindorMake(chip, "Coprocessor")->GetNodeContent();
//    //wxArrayString cur_coprocessers = myfunc::Separater(str, ",");
//    //wxArrayString coprocessers = myfunc::Separater(Main::rvc->FindContent("#Chip#Coprocessor"), ",");
//    //coprocessers.push_back("None");   // 对应未配置协处理器的情况
//    //c_coprocessor->FillAllItem(coprocessers);
//    //c_coprocessor->SetValue(cur_coprocessers);
//
//    //// Platform中声明的参数
//    //wxXmlNode* root_config = myfunc::FindorMake(Main::rva->GetRoot(), "Config");
//    //wxXmlNode* chip_config = myfunc::FindorMake(chip, "Config");
//    //for (wxXmlNode* c = root_config->GetChildren(); c; c = c->GetNext()) {
//    //    wxString name = myfunc::FindorMake(c, "Name")->GetNodeContent();
//    //    SimpleText* config_item = new SimpleText(swindow, name, "items");
//    //    // 读入文件中的属性填入
//    //    config_item->SetValue(myfunc::FindorMake(chip_config, name)->GetNodeContent());
//    //    stbox_config->Add(config_item, 0, wxALL | wxEXPAND, 5);
//    //    configs.insert(std::make_pair(name, config_item));
//    //}
//    //swindow->FitInside();
//}
//
//
//void Panel_Chip::SaveChip() {
//    // 先检查
//    if (!IsLegal()) {
//        wxMessageBox("错误");
//        return;
//    }
//
//    myfunc::SetNodeCont(chip, "Platform", c_platform->GetValue());
//    myfunc::SetNodeCont(chip, "Class", c_class->GetValue());
//    myfunc::SetNodeCont(chip, "Name",c_name->GetStringSelection());
//    myfunc::SetNodeCont(chip, "Coprocessor", c_coprocessor->GetValue()); 
//    wxXmlNode* chip_config = myfunc::FindorMake(chip, "Config");
//    for (auto i : configs) {
//        myfunc::SetNodeCont(chip_config, i.first, i.second->GetValue());
//    }
//    Main::body_splitter->file_tree->SavedChip();
//}
//
//void Panel_Chip::OnPreview(const wxCommandEvent& event) {
//    if (this->chip) {
//        wxString str;
//        myfunc::DFS(this->chip, str);
//        myfunc::PreviewHelper(str, "源码预览 芯片配置");
//    }
//}
//
//bool Panel_Chip::IsLegal() {
//    bool j1 = c_coprocessor->IsLegal();
//    bool j2 = c_class->IsLegal();
//    bool j3 = c_platform->IsLegal();
//    bool j4 = true;
//    // chip config没有给检查的标准
//    //for (auto i : configs) {
//    //    if (!i.second->IsLegal()) {
//    //        j4 = false;
//    //    }
//    //}
//    return j1 && j2 && j3 && j4;
//}
//void Panel_Chip::OnConfirm(const wxCommandEvent& event) {
//    SaveChip();
//}
//
///************************************************
//*   SHMEM
//************************************************/
//Shmem::Shmem(wxWindow* parent)
//    :PanelBase(parent, wxID_ANY) {
//    errorloc = "共享存储块->";
//    //box = new wxBoxSizer(wxVERTICAL);
//
//    this->mem_units = new Unit_Manager<Mem_Unit_Test>(swindow, errorloc, false);
//    this->mem_units->SetUnitSign("S");
//
//    box->Add(mem_units, 1, wxEXPAND);
//
//    swindow->SetSizer(box);
//    swindow->FitInside();
//    //this->SetSizer(box);
//
//}
//
//
//void Shmem::SetShmem(wxXmlNode* root) {
//    this->shmem = root;
//    this->mem_units->SetAllUnit(shmem);
//}
//
//
//void Shmem::OnPreview(const wxCommandEvent& event) {
//    mem_units->PreviewFunc();
//}
//void Shmem::OnConfirm(const wxCommandEvent& event) {
//    // 为什么下面的这个大括号不能省略？
//    if (mem_units->ConfirmFunc()) {
//        Main::body_splitter->file_tree->SavedShmem();
//    }
//}
///************************************************
//*   KERNEL
//************************************************/
//Panel_Kernel::Panel_Kernel(wxWindow* parent)
//    :PanelBase(parent, wxID_ANY) {
//    errorloc = "内核配置";
//    wxStaticBoxSizer* stbox_m = new wxStaticBoxSizer(wxVERTICAL, swindow, "Memory Manage");
//
//    wxStaticBoxSizer* code_box = new wxStaticBoxSizer(wxVERTICAL, swindow);
//    this->code_base = new BaseSlider(swindow, "Code Base", "内核所使用的代码段的基址", errorloc);
//    this->code_size = new SizeSlider(swindow, "Code Size", "内核所使用的代码段的长度", errorloc);
//    code_base->SetDefaultColor(RGB(240, 240, 240));
//    code_size->SetDefaultColor(RGB(240, 240, 240));
//    code_box->Add(code_base, 0, wxEXPAND);
//    code_box->Add(code_size, 0, wxEXPAND);
//
//    wxStaticBoxSizer* data_box = new wxStaticBoxSizer(wxVERTICAL, swindow);
//    this->data_base = new BaseSlider(swindow, "Data Base", "内核所使用的数据段的基址", errorloc);
//    this->data_size = new SizeSlider(swindow, "Data Size", "内核所使用的数据段的长度", errorloc);
//    data_size->SetDefaultColor(RGB(240, 240, 240));
//    data_size->SetDefaultColor(RGB(240, 240, 240));
//    data_box->Add(data_base, 0, wxEXPAND);
//    data_box->Add(data_size, 0, wxEXPAND);
//
//    stack_size = new SizeSlider(swindow, "Stack Size", "内核栈段的长度（1kB即可满足大多数需求）", errorloc);
//    stack_size->SetDefaultColor(RGB(240, 240, 240));
//    extra_kom = new SizeSlider(swindow, "Extro Kernel Object Memory", "额外内核内存的长度", errorloc);
//    extra_kom->SetDefaultColor(RGB(240, 240, 240));
//
//    stbox_m->Add(code_box, 0, wxALL | wxEXPAND, 5);
//    stbox_m->Add(0, 10);
//    stbox_m->Add(data_box, 0, wxALL | wxEXPAND, 5);
//    stbox_m->Add(0, 10);
//    stbox_m->Add(stack_size, 0, wxALL | wxEXPAND, 5);
//    stbox_m->Add(0, 10);
//    stbox_m->Add(extra_kom, 0, wxALL | wxEXPAND, 5);
//
//    wxStaticBoxSizer* order_prio_box = new wxStaticBoxSizer(wxVERTICAL, swindow);
//    
//    wxArrayString kmo_arr = wxArrayString(8, new wxString[8]{ "2","4","8","16","32","64","128","256" });
//    choice_kom_order = new MyChoice(swindow, "Kernel Object Memory Order", "内核内存分配粒度");
//    choice_kom_order->FillChoice(kmo_arr);
//
//
//    wxArrayString kern_prio_arr = wxArrayString(8, new wxString[8]{ "2","4","8","16","32","64","128","256" });
//    kern_prio = new MyChoice(swindow, "Kernrl Priority", "内核态优先级的数量", errorloc);
//    kern_prio->FillChoice(kern_prio_arr);
//    
//    order_prio_box->Add(choice_kom_order, 1, wxEXPAND);
//    order_prio_box->Add(kern_prio, 1, wxEXPAND);
//
//    wxStaticBoxSizer* sys_tc_optim_box = new wxStaticBoxSizer(wxVERTICAL, swindow);
//    tcr_buildsys = new SimpleText(swindow, "Buildsystem", "构建系统", wxEmptyString, wxTE_READONLY);
//    tcr_toolchain = new SimpleText(swindow, "Toolchain", "构建工具链", wxEmptyString, wxTE_READONLY);
//    k_optim = new MyChoice(swindow, "Optimization", "内核工程所使用的优化等级");
//    wxArrayString arr = wxArrayString(6, new wxString[6]{ "O0","O1","O2","O3","Of","Os" });
//    k_optim->FillChoice(arr);
//    
//    sys_tc_optim_box->Add(tcr_buildsys, 1, wxALL | wxEXPAND, 2);
//    sys_tc_optim_box->Add(tcr_toolchain, 1, wxALL | wxEXPAND, 2);
//    sys_tc_optim_box->Add(k_optim, 1, wxALL | wxEXPAND, 2);
//
//    wxStaticBoxSizer* stbox_path = new wxStaticBoxSizer(wxVERTICAL, swindow, "Path");
//    full_image = new BoolBox(swindow,"Full Image");
//    project_output = new SimpleText(swindow, "Project Output", "内核工程的输出目录", "./Kernel/Project/", wxTE_READONLY);
//    project_overw = new BoolBox(swindow, "Project Overwrite", "重新生成时是否覆盖之前生成的内核工程文件");
//    linker_output = new SimpleText(swindow, "Linker Output", "内核工程链接器脚本的输出目录", "./", wxTE_READONLY);
//    conf_hdr_output = new SimpleText(swindow, "Config Header Output", "两个内核配置头文件的输出目录", "../Include/", wxTE_READONLY);
//    boot_hdr_output = new SimpleText(swindow, "Boot Header Output", "头文件的输出目录", "../Include/", wxTE_READONLY);
//    boot_src_output = new SimpleText(swindow, "Boot Source Output", "源文件的输出目录", "../Source/", wxTE_READONLY);
//    hook_src_output = new SimpleText(swindow, "Hook Source Output", "钩子源文件的输出目录", "../Source/", wxTE_READONLY);
//    hook_src_overw = new BoolBox(swindow, "Hook Source Overwrite", "在重新生成时是否覆盖之前生成的内核钩子源文件");
//    handler_src_output = new SimpleText(swindow, "Handler Source Output", "中断向量源文件的输出目录", "../Source/", wxTE_READONLY);
//    handler_scr_overw = new BoolBox(swindow, "Handler Source Overwrite", "重新生成时是否覆盖之前生成的中断向量源文件");
//
//    stbox_path->Add(full_image, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(project_output, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(project_overw, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(linker_output, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(conf_hdr_output, 0,wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(boot_hdr_output, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(boot_src_output, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(hook_src_output, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(hook_src_overw, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(handler_src_output, 0,  wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(handler_scr_overw, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//
//    box->Add(stbox_m, 0, wxALL | wxEXPAND, 5);
//    box->Add(order_prio_box, 0, wxALL | wxEXPAND, 5);
//    box->Add(sys_tc_optim_box, 0, wxALL | wxEXPAND, 5);
//    box->Add(stbox_path, 0, wxALL | wxEXPAND, 5);
//
//    swindow->SetSizer(box);
//    swindow->FitInside();
//}
//
//void Panel_Kernel::SetKernel(wxXmlNode* root) {
//    kernel = root;
//    std::map<wxString, wxXmlNode*>* mp = new std::map<wxString, wxXmlNode*>;
//    for (wxXmlNode* child = kernel->GetChildren(); child; child = child->GetNext()) {
//        mp->insert(std::make_pair(child->GetName(), child));
//    }
//
//    code_base->SetValue(myfunc::FindorMake(kernel, "Code_Base", mp)->GetNodeContent());
//    code_base->SetPosition(0);
//    code_size->SetValue(myfunc::FindorMake(kernel, "Code_Size", mp)->GetNodeContent());
//    code_size->SetPosition(0);
//    data_base->SetValue(myfunc::FindorMake(kernel, "Data_Base", mp)->GetNodeContent());
//    data_size->SetValue(myfunc::FindorMake(kernel, "Data_Size", mp)->GetNodeContent());
//    stack_size->SetValue(myfunc::FindorMake(kernel, "Stack_Size", mp)->GetNodeContent());
//    extra_kom->SetValue(myfunc::FindorMake(kernel, "Extra_Kom", mp)->GetNodeContent());
//    int kom_id = choice_kom_order->FindValue(myfunc::FindorMake(kernel, "Kom_Order", mp)->GetNodeContent());
//    choice_kom_order->SetSelection(kom_id);
//
//    int kern_prio_id = kern_prio->FindValue(myfunc::FindorMake(kernel, "Kern_Prio", mp)->GetNodeContent());
//    this->kern_prio->SetSelection(kern_prio_id);
//
//    this->tcr_buildsys->SetValue(myfunc::FindorMake(kernel, "Buildsystem", mp)->GetNodeContent());
//    this->tcr_toolchain->SetValue(myfunc::FindorMake(kernel, "Toolchain", mp)->GetNodeContent());
//    int optim_id = k_optim->FindValue(myfunc::FindorMake(kernel, "Optimization", mp)->GetNodeContent());
//    k_optim->SetSelection(optim_id);
//    full_image->SetValue(myfunc::FindorMake(kernel, "Full_Image", mp)->GetNodeContent());
//    project_output->SetValue(myfunc::FindorMake(kernel, "Project_Output", mp)->GetNodeContent());
//    project_overw->SetValue(myfunc::FindorMake(kernel, "Project_Overwrite", mp)->GetNodeContent());
//
//    linker_output->SetValue(myfunc::FindorMake(kernel, "Linker_Output", mp)->GetNodeContent());
//    conf_hdr_output->SetValue(myfunc::FindorMake(kernel, "Config_Header_Output", mp)->GetNodeContent());
//    boot_hdr_output->SetValue(myfunc::FindorMake(kernel, "Boot_Header_Output", mp)->GetNodeContent());
//    boot_src_output->SetValue(myfunc::FindorMake(kernel, "Boot_Source_Output", mp)->GetNodeContent());
//    hook_src_output->SetValue(myfunc::FindorMake(kernel, "Hook_Source_Output", mp)->GetNodeContent());
//    hook_src_overw->SetValue(myfunc::FindorMake(kernel, "Hook_Source_Overwrite", mp)->GetNodeContent());
//    handler_src_output->SetValue(myfunc::FindorMake(kernel, "Handler_Source_Output", mp)->GetNodeContent());
//    handler_scr_overw->SetValue(myfunc::FindorMake(kernel, "Handler_Source_Overwrite", mp)->GetNodeContent());
//    Main::rvp->SetKernPrio(wxAtoi(kern_prio->GetStringSelection()));
//}
//
//void Panel_Kernel::RefreshFunc() {
//    this->tcr_buildsys->SetValue(Main::rvp->cur_buildsys);
//    this->tcr_toolchain->SetValue(Main::rvp->cur_toolchain);
//
//}
//bool Panel_Kernel::IsLegal() {
//    bool j1 = code_base->IsLegal();
//    bool j2 = code_size->IsLegal();
//    bool j3 = data_base->IsLegal();
//    bool j4 = data_size->IsLegal();
//    bool j5 = stack_size->IsLegal();
//    bool j6 = extra_kom->IsLegal();
//    bool j7 = kern_prio->IsLegal();
//    return j1 && j2 && j3 && j4 && j5 && j6 && j7;
//}
//void Panel_Kernel::OnRefresh(const wxCommandEvent& event) {
//    RefreshFunc();
//}
//
//void Panel_Kernel::OnPreview(const wxCommandEvent& event) {
//    if (kernel) {
//        wxString str;
//        myfunc::DFS(kernel, str);
//        myfunc::PreviewHelper(str, "源码预览 内核配置");
//    }
//}
//void Panel_Kernel::OnConfirm(const wxCommandEvent& event) {
//
//    // 检查todo
//    SaveKernel();
//}
//
//void Panel_Kernel::SaveKernel() {
//    if (!IsLegal()) {
//        wxMessageBox("错误", "芯片配置");
//        return;
//    }
//    myfunc::SetNodeCont(kernel, "Code_Base", code_base->GetValue());
//    myfunc::SetNodeCont(kernel, "Code_Size", code_size->GetValue());
//    myfunc::SetNodeCont(kernel, "Data_Base", data_base->GetValue());
//    myfunc::SetNodeCont(kernel, "Data_Size", data_size->GetValue());
//    myfunc::SetNodeCont(kernel, "Stack_Size", stack_size->GetValue());
//    myfunc::SetNodeCont(kernel, "Extra_Kom", extra_kom->GetValue());
//    myfunc::SetNodeCont(kernel, "Kom_Order", choice_kom_order->GetStringSelection());
//    myfunc::SetNodeCont(kernel, "Kern_Prio", kern_prio->GetStringSelection());
//    myfunc::SetNodeCont(kernel, "Buildsystem", Main::rvp->GetSysArr()[0]);
//    myfunc::SetNodeCont(kernel, "Toolchain", Main::rvp->GetSysArr()[1]);
//    myfunc::SetNodeCont(kernel, "Optimization", k_optim->GetStringSelection());
//    myfunc::SetNodeCont(kernel, "Full_Image", full_image->GetStringValue());
//    myfunc::SetNodeCont(kernel, "Project_Output", project_output->GetValue());
//    myfunc::SetNodeCont(kernel, "Project_Overwrite", project_overw->GetStringValue());
//    myfunc::SetNodeCont(kernel, "Linker_Output", linker_output->GetValue());
//    myfunc::SetNodeCont(kernel, "Config_Header_Output", conf_hdr_output->GetValue());
//    myfunc::SetNodeCont(kernel, "Boot_Header_Output", boot_hdr_output->GetValue());
//    myfunc::SetNodeCont(kernel, "Boot_Source_Output", boot_src_output->GetValue());
//    myfunc::SetNodeCont(kernel, "Hook_Source_Output", hook_src_output->GetValue());
//    myfunc::SetNodeCont(kernel, "Hook_Source_Overwrite", hook_src_overw->GetStringValue());
//    myfunc::SetNodeCont(kernel, "Handler_Source_Output", handler_src_output->GetValue());
//    myfunc::SetNodeCont(kernel, "Handler_Source_Overwrite", handler_scr_overw->GetStringValue());
//    Main::rvp->SetKernPrio(wxAtoi(kern_prio->GetStringSelection()));
//    Main::body_splitter->file_tree->SavedKernel();
//    return;
//}
//
//
///************************************************
//*   MONITOR
//************************************************/
//Monitor::Monitor(wxWindow* parent)
//    :PanelBase(parent, wxID_ANY) {
//    errorloc = "监视器配置->";
//    wxStaticBoxSizer* stbox_m = new wxStaticBoxSizer(wxVERTICAL, swindow, "Memory Manage");
//    wxStaticBoxSizer* cd_size_box = new wxStaticBoxSizer(wxVERTICAL, swindow);
//    code_size = new SizeSlider(swindow, "Code Size", "代码段长度", errorloc);
//    data_size = new SizeSlider(swindow, "Data Size", "数据段长度", errorloc);
//    code_size->SetDefaultColor(RGB(240, 240, 240));
//    data_size->SetDefaultColor(RGB(240, 240, 240));
//    cd_size_box->Add(code_size, 0, wxALL | wxEXPAND, 5);
//    cd_size_box->Add(data_size, 0, wxALL | wxEXPAND, 5);
//
//    init_stack_size = new SizeSlider(swindow, "Initial Stack Size", "启动守护线程INIT的栈长度（推荐大小 256B）", errorloc);
//    init_stack_size->SetDefaultColor(RGB(240, 240, 240));
//    sftd_stack_size = new SizeSlider(swindow, "SFTD Stack Size", "安全守护线程SFTD的栈长度（推荐大小 256B）", errorloc);
//    sftd_stack_size->SetDefaultColor(RGB(240, 240, 240));
//    vmmd_stack_size = new SizeSlider(swindow, "VMMD Stack Size", "了虚拟机守护线程VMMD的栈长度（推荐大小 512B）", errorloc);
//    vmmd_stack_size->SetDefaultColor(RGB(240, 240, 240));
//
//    extra_captbl = new MyZ_GE_Zero(swindow, "Extra Capability", "额外权能表槽位数", errorloc);
//
//    wxStaticBoxSizer* other_box = new wxStaticBoxSizer(wxVERTICAL, swindow);
//
//    //wxArrayString virt_prio_arr = { "1","2","4","8","16","32","64","128","256","512","1024","0" };
//    virt_prio = new MyVirtPrio(swindow, "Virtual Priority", "虚拟化优先级数", errorloc);
//    //virt_prio->SetCheckTip("使用虚拟机时该值不为0");
//    //virt_prio->FillChoice(virt_prio_arr);
//
//    //wxArrayString virt_event_arr = { "1","2","4","8","16","32","64","128","256","512","1024","0" };
//    virt_event = new MyVirtPrio(swindow, "Virtual Event", "虚拟机事件源数", errorloc, true);
//    //virt_event->SetCheckTip("使用虚拟机时该值不为0");
//    //virt_event->FillChoice(virt_event_arr);
//
//    virt_map = new MyVirtMap(swindow, "Virtual Map", "虚拟机向量映射数", errorloc);
//    virt_map->SetCheckTip("使用虚拟机时该值不为0，该值需要大于等于Virtual Event");
//    virt_prio->SetDefaultColor(RGB(240, 240, 240));
//    virt_event->SetDefaultColor(RGB(240, 240, 240));
//    virt_map->SetDefaultColor(RGB(240, 240, 240));
//
//    other_box->Add(virt_prio, 0, wxEXPAND);
//    other_box->Add(virt_event, 0, wxEXPAND);
//    other_box->Add(virt_map, 0, wxEXPAND);
//
//
//    stbox_m->Add(cd_size_box, 0, wxALL | wxEXPAND, 5);
//    stbox_m->Add(0, 5);
//    stbox_m->Add(init_stack_size, 0, wxALL | wxEXPAND, 5);
//    stbox_m->Add(0, 5);
//    stbox_m->Add(sftd_stack_size, 0, wxALL | wxEXPAND, 5);
//    stbox_m->Add(0, 5);
//    stbox_m->Add(vmmd_stack_size, 0, wxALL | wxEXPAND, 5);
//    stbox_m->Add(0, 5);
//    stbox_m->Add(extra_captbl, 0, wxALL | wxEXPAND, 5);
//    stbox_m->Add(0, 5);
//    stbox_m->Add(other_box, 0, wxALL | wxEXPAND, 5);
//    stbox_m->Add(0, 5);
//
//    wxStaticBoxSizer* bto_box = new wxStaticBoxSizer(wxVERTICAL, swindow);
//    c_buildsys = new SimpleText(swindow, "Buildsystem", "构建系统", wxEmptyString, wxTE_READONLY);
//    c_toolchain = new SimpleText(swindow, "Toolchain", "构建工具链", wxEmptyString, wxTE_READONLY);
//    c_optim = new MyChoice(swindow, "Optimization", "内核工程所使用的优化等级");
//    //wxArrayString arr = { "O0","O1","O2","O3","Of","Os" };
//    //c_optim->FillChoice(arr);
//
//    bto_box->Add(c_buildsys, 0, wxEXPAND);
//    bto_box->Add(c_toolchain, 0, wxEXPAND);
//    bto_box->Add(c_optim, 0, wxEXPAND);
//
//    wxStaticBoxSizer* stbox_path = new wxStaticBoxSizer(wxVERTICAL, swindow, "Path");
//    project_output = new SimpleText(swindow, "Project Output", wxEmptyString, "./Monitor/Project/", wxTE_READONLY);
//    project_overw = new BoolBox(swindow, "Project Overwrite");
//    linker_output = new SimpleText(swindow, "Linker Output", wxEmptyString, "./", wxTE_READONLY);
//    conf_hdr_output = new SimpleText(swindow, "Config Header Output", wxEmptyString, "../Include", wxTE_READONLY);
//    boot_hdr_output = new SimpleText(swindow, "Boot Header Output", wxEmptyString, "../Include/", wxTE_READONLY);
//    boot_src_output = new SimpleText(swindow, "Boot Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
//    hook_src_output = new SimpleText(swindow, "Hook Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
//    hook_src_overw = new BoolBox(swindow, "Hook Source Overwrite");
//
//    stbox_path->Add(project_output, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(project_overw, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(linker_output, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(conf_hdr_output, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(boot_hdr_output, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(boot_src_output, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(hook_src_output, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(hook_src_overw, 0, wxEXPAND);
//    stbox_path->Add(0, 5);
//
//    box->Add(stbox_m, 0, wxALL | wxEXPAND, 5);
//    box->Add(bto_box, 0, wxALL | wxEXPAND, 5);
//    box->Add(stbox_path, 0, wxALL | wxEXPAND, 5);
//    swindow->SetSizer(box);
//    swindow->FitInside();
//    SetSizer(highbox);
//}
//
//void Monitor::RefreshFunc() {
//    c_buildsys->SetValue(Main::rvp->cur_buildsys);
//    c_toolchain->SetValue(Main::rvp->cur_toolchain);
//    virt_prio->SetVmCount(Main::rvp->GetVmCount());
//}
//
//bool Monitor::IsLegal() {
//    bool j1 = code_size->IsLegal();
//    bool j2 = data_size->IsLegal();
//    bool j3 = init_stack_size->IsLegal();
//    bool j4 = sftd_stack_size->IsLegal();
//    bool j5 = vmmd_stack_size->IsLegal();
//    bool j6 = virt_prio->IsLegal();
//    bool j7 = virt_event->IsLegal();
//    // j8的检查理论上应该用xml层上的值，但是如果virt_event在此之前检查合法了
//    // 是否可以用边界层上的值
//    bool j8 = virt_map->IsLegal(wxAtoi(virt_event->GetStringSelection()));
//    bool j9 = extra_captbl->IsLegal();
//    return j1 && j2 && j3 && j4 && j5 && j6 && j7 && j8 && j9;
//}
//
//void Monitor::OnRefresh(const wxCommandEvent& event) {
//    RefreshFunc();
//}
//
//void Monitor::OnPreview(const wxCommandEvent& event) {
//    if (monitor) {
//        wxString str;
//        myfunc::DFS(monitor, str);
//        myfunc::PreviewHelper(str, "源码预览 监视器配置");
//    }
//}
//
//void Monitor::OnConfirm(const wxCommandEvent& event) {
//    SaveMonitor();
//}
//
//void Monitor::SetMonitor(wxXmlNode* root) {
//    monitor = root;
//    std::map<wxString, wxXmlNode*>* mp = new std::map<wxString, wxXmlNode*>;
//    for (auto child = monitor->GetChildren(); child; child = child->GetNext()) {
//        mp->insert(std::make_pair(child->GetName(), child));
//    }
//    code_size->SetValue(myfunc::FindorMake(monitor, "Code_Size", mp)->GetNodeContent());
//    data_size->SetValue(myfunc::FindorMake(monitor, "Data_Size", mp)->GetNodeContent());
//    init_stack_size->SetValue(myfunc::FindorMake(monitor, "Init_Stack_Size", mp)->GetNodeContent());
//    sftd_stack_size->SetValue(myfunc::FindorMake(monitor, "Sftd_Stack_Size", mp)->GetNodeContent());
//    vmmd_stack_size->SetValue(myfunc::FindorMake(monitor, "Vmmd_Stack_Size", mp)->GetNodeContent());
//    extra_captbl->SetValue(myfunc::FindorMake(monitor, "Extra_Captbl", mp)->GetNodeContent());
//
//    int virt_prio_id = virt_prio->FindValue(myfunc::FindorMake(monitor, "Virt_Prio", mp)->GetNodeContent());
//    virt_prio->SetSelection(virt_prio_id);
//
//    int virt_event_id = virt_event->FindValue(myfunc::FindorMake(monitor, "Virt_Event", mp)->GetNodeContent());
//    virt_event->SetSelection(virt_event_id);
//
//    virt_map->SetValue(myfunc::FindorMake(monitor, "Virt_Map", mp)->GetNodeContent());
//
//    c_buildsys->SetValue(myfunc::FindorMake(monitor, "Buildsystem", mp)->GetNodeContent());
//    c_toolchain->SetValue(myfunc::FindorMake(monitor, "Toolchain", mp)->GetNodeContent());
//
//    int optim_id = c_optim->FindValue(myfunc::FindorMake(monitor, "Optimization", mp)->GetNodeContent());
//    c_optim->SetSelection(optim_id);
//
//    project_output->SetValue(myfunc::FindorMake(monitor, "Project_Output", mp)->GetNodeContent());
//    project_overw->SetValue(myfunc::FindorMake(monitor, "Project_Overwrite", mp)->GetNodeContent());
//    linker_output->SetValue(myfunc::FindorMake(monitor, "Linker_Output", mp)->GetNodeContent());
//    conf_hdr_output->SetValue(myfunc::FindorMake(monitor, "Config_Header_Output", mp)->GetNodeContent());
//    boot_hdr_output->SetValue(myfunc::FindorMake(monitor, "Boot_Header_Output", mp)->GetNodeContent());
//    boot_src_output->SetValue(myfunc::FindorMake(monitor, "Boot_Source_Output", mp)->GetNodeContent());
//    hook_src_output->SetValue(myfunc::FindorMake(monitor, "Hook_Source_Output", mp)->GetNodeContent());
//    hook_src_overw->SetValue(myfunc::FindorMake(monitor, "Hook_Source_Overwrite", mp)->GetNodeContent());
//    Main::rvp->SetVirtPrio(wxAtoi(virt_prio->GetStringSelection()));
//}
//
//void Monitor::SaveMonitor() {
//    if (!IsLegal()) {
//        wxMessageBox("错误", "监视器设置");
//        return;
//    }
//    myfunc::SetNodeCont(monitor, "Code_Size", code_size->GetValue());
//    myfunc::SetNodeCont(monitor, "Data_Size", data_size->GetValue());
//    myfunc::SetNodeCont(monitor, "Init_Stack_Size", init_stack_size->GetValue());
//    myfunc::SetNodeCont(monitor, "Sftd_Stack_Size", sftd_stack_size->GetValue());
//    myfunc::SetNodeCont(monitor, "Vmmd_Stack_Size", vmmd_stack_size->GetValue());
//    myfunc::SetNodeCont(monitor, "Extra_Captbl", extra_captbl->GetValue());
//    myfunc::SetNodeCont(monitor, "Virt_Prio", virt_prio->GetStringSelection());
//    myfunc::SetNodeCont(monitor, "Virt_Event", virt_event->GetStringSelection());
//    myfunc::SetNodeCont(monitor, "Virt_Map", virt_map->GetValue());
//    myfunc::SetNodeCont(monitor, "Buildsystem", Main::rvp->GetSysArr()[0]);
//    myfunc::SetNodeCont(monitor, "Toolchain", Main::rvp->GetSysArr()[1]);
//    myfunc::SetNodeCont(monitor, "Optimization", c_optim->GetStringSelection());
//    myfunc::SetNodeCont(monitor, "Project_Output", project_output->GetValue());
//    myfunc::SetNodeCont(monitor, "Project_Overwrite", project_overw->GetStringValue());
//    myfunc::SetNodeCont(monitor, "Linker_Output", linker_output->GetValue());
//    myfunc::SetNodeCont(monitor, "Config_Header_Output", conf_hdr_output->GetValue());
//    myfunc::SetNodeCont(monitor, "Boot_Header_Output", boot_hdr_output->GetValue());
//    myfunc::SetNodeCont(monitor, "Boot_Source_Output", boot_src_output->GetValue());
//    myfunc::SetNodeCont(monitor, "Hook_Source_Output", hook_src_output->GetValue());
//    myfunc::SetNodeCont(monitor, "Hook_Source_Overwrite", hook_src_overw->GetStringValue());
//    Main::rvp->SetVirtPrio(wxAtoi(virt_prio->GetStringSelection()));
//    Main::body_splitter->file_tree->SavedMonitor();
//    return;
//}
//
//
///************************************************
//*   Native
//************************************************/
//Panel_Native::Panel_Native(wxWindow* parent)
//    :wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE) {
//    //this->vm_count = 0; // 虚拟机计数
//    this->box = new wxBoxSizer(wxVERTICAL);
//    this->SetScrollRate(10, 10);  // 设置滚动速率 (单位是像素)
//
//    wxStaticBoxSizer* stbox_preview = new wxStaticBoxSizer(wxVERTICAL, this, "预览");
//    this->btn_preview = new wxButton(this, wxID_ANY, "原生进程配置源码预览");
//    stbox_preview->Add(this->btn_preview, 0, wxALL, 5);
//
//    this->box->Add(stbox_preview, 0, wxEXPAND | wxALL, 5);
//    this->SetSizer(this->box);
//
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Panel_Native::PreviewFunc, this, this->btn_preview->GetId());
//}
//
//void Panel_Native::SetNative(wxXmlNode* root) {
//    native = root;
//    Main::body_splitter->file_tree->AddNative(root);
//}
//
//void Panel_Native::PreviewFunc(wxCommandEvent& event) {
//    if (native) {
//        wxString str;
//        myfunc::DFS(native, str);
//        myfunc::PreviewHelper(str, "原生进程配置源码预览");
//    }
//}
//
///************************************************
//*   NATIVE  base_info
//************************************************/
//Native_Base_Info::Native_Base_Info(wxWindow* parent, const int& _counter, const wxString& _errorloc)
//    :wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE) {
//    errorloc = _errorloc;
//
//    this->counter = _counter;
//    this->box = new wxBoxSizer(wxVERTICAL);
//    this->title = new wxStaticText(this, wxID_ANY, wxString::Format("虚拟机 %d", this->counter));
//    this->SetScrollRate(10, 10);  // 设置滚动速率 (单位是像素)
//
//    wxStaticBoxSizer* stbox_base_info = new wxStaticBoxSizer(wxVERTICAL, this, "Base_info");
//    //wxStaticText* txt_name = new  wxStaticText(this, wxID_ANY, "Name");
//    n_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc);
//    n_extra_captbl = new SimpleText(this, "Extra Captbl", wxEmptyString, wxEmptyString, 0L, errorloc);
//    n_coprocessor = new MyCoprocessor(this, "Coprocessor", wxEmptyString, errorloc);
//    n_buildsys = new SimpleText(this, "Buildsystem", wxEmptyString, wxEmptyString, wxTE_READONLY);
//    n_toolchain = new SimpleText(this, "Toolchain", wxEmptyString, wxEmptyString, wxTE_READONLY);
//    n_optimi = new MyChoice(this, "Optimization");
//    //wxArrayString arr_optimi = { "O0","O1","O2","O3","Of","Os" };
//    //n_optimi->FillChoice(arr_optimi);
//    //n_optimi->SetSelection(0);
//
//    stbox_base_info->Add(n_name, 0, wxALL | wxEXPAND, 5);
//    stbox_base_info->Add(0, 5);
//    stbox_base_info->Add(n_extra_captbl, 0, wxALL | wxEXPAND, 5);
//    stbox_base_info->Add(0, 5);
//    stbox_base_info->Add(n_coprocessor, 0, wxALL | wxEXPAND, 5);
//    stbox_base_info->Add(0, 5);
//    stbox_base_info->Add(n_buildsys, 0, wxALL | wxEXPAND, 5);
//    stbox_base_info->Add(0, 5);
//    stbox_base_info->Add(n_toolchain, 0, wxALL | wxEXPAND, 5);
//    stbox_base_info->Add(0, 5);
//    stbox_base_info->Add(n_optimi, 0, wxALL | wxEXPAND, 5);
//
//    wxStaticBoxSizer* stbox_path = new wxStaticBoxSizer(wxVERTICAL, this, "Path");
//    project_output = new SimpleText(this, "Project Output", wxEmptyString, wxEmptyString, wxTE_READONLY);
//    project_overw = new BoolBox(this, "Project Overwrite");
//    linker_output = new SimpleText(this, "Linker Output", wxEmptyString, "./", wxTE_READONLY);
//    main_hdr_output = new SimpleText(this, "Main Header Output", wxEmptyString, "../Include/", wxTE_READONLY);
//    main_src_output = new SimpleText(this, "Main Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
//    entry_src_output = new SimpleText(this, "Entry Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
//    entry_src_overw = new BoolBox(this, "Entry Source Overwrite");
//
//    stbox_path->Add(project_output, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(project_overw, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(linker_output, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(main_hdr_output, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(main_src_output, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(entry_src_output, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(entry_src_overw, 0, wxALL | wxEXPAND, 5);
//
//    wxButton* btn_confirm = new wxButton(this, wxID_ANY, "确认");
//    wxButton* btn_refresh = new wxButton(this, wxID_ANY, "刷新");
//
//    this->box->Add(title, 0, wxALL | wxALIGN_LEFT, 5);
//    this->box->Add(stbox_base_info, 0, wxALL | wxEXPAND, 5);
//    this->box->Add(stbox_path, 0, wxALL | wxEXPAND, 5);
//    this->box->Add(btn_confirm, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->box->Add(btn_refresh, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->SetSizer(this->box);
//
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Native_Base_Info::OnConfirm, this, btn_confirm->GetId());
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Native_Base_Info::OnRefresh, this, btn_refresh->GetId());
//
//}
//
//void Native_Base_Info::SetNativeBaseInfo(wxXmlNode* _node) {
//    // 希望到N0
//    this->node = _node;
//    std::map<wxString, wxXmlNode*>* new_mp = new std::map<wxString, wxXmlNode*>;
//    for (wxXmlNode* son = node->GetChildren(); son; son = son->GetNext()) {
//        new_mp->insert(std::make_pair(son->GetName(), son));
//    }
//    // 如果root有对应的子节点，则通过GetNodeContent得到该子节点的值
//    // 如果root没有对应的子节点，则为之新建特定名称的子节点，GetNodeContent返回一个wxEmptyString
//    wxString native_name = myfunc::FindorMake(node, "Name", new_mp)->GetNodeContent();
//    n_name->SetValue(native_name);
//
//    n_extra_captbl->SetValue(myfunc::FindorMake(node, "Extra_Captbl", new_mp)->GetNodeContent());
//
//    wxString str = myfunc::FindorMake(node, "Coprocessor", new_mp)->GetNodeContent();
//    wxArrayString cur_coprocessers = myfunc::Separater(str, ",");
//    //wxArrayString coprocessers = myfunc::Separater(Main::rvc->FindContent("#Chip#Coprocessor"), ",");
//    //coprocessers.push_back("None");   // 对应未配置协处理器的情况
//    //n_coprocessor->FillAllItem(coprocessers);
//    n_coprocessor->SetValue(cur_coprocessers);
//
//
//    n_buildsys->SetValue(myfunc::FindorMake(node, "Buildsystem", new_mp)->GetNodeContent());
//    n_toolchain->SetValue(myfunc::FindorMake(node, "Toolchain", new_mp)->GetNodeContent());
//
//    wxString cur_optim = myfunc::FindorMake(node, "Optimization", new_mp)->GetNodeContent();
//    n_optimi->SetSelection(n_optimi->FindValue(cur_optim));
//
//    project_output->SetValue(myfunc::FindorMake(node, "Project_Output", new_mp)->GetNodeContent());
//    project_overw->SetValue(myfunc::FindorMake(node, "Project_Overwrite", new_mp)->GetNodeContent());
//
//    linker_output->SetValue(myfunc::FindorMake(node, "Linker_Output", new_mp)->GetNodeContent());
//    main_hdr_output->SetValue(myfunc::FindorMake(node, "Main_Header_Output", new_mp)->GetNodeContent());
//    main_src_output->SetValue(myfunc::FindorMake(node, "Main_Source_Output", new_mp)->GetNodeContent());
//    entry_src_output->SetValue(myfunc::FindorMake(node, "Entry_Source_Output", new_mp)->GetNodeContent());
//    entry_src_overw->SetValue(myfunc::FindorMake(node, "Entry_Source_Overwrite", new_mp)->GetNodeContent());
//}
//
//bool Native_Base_Info::IsLegal() {
//    bool j1 = n_name->IsLegal();
//    bool j2 = Main::body_splitter->workarea_panel->IsUniqueNameNative(this, n_name->GetValue());
//    if (!j2)
//        n_name->IncorrectSet("命名不合法 命名重复(检查其他 原生进程 和 虚拟机)");
//    bool j3 = n_coprocessor->IsLegal();
//    return j1 && j2 && j3;
//}
//
//void Native_Base_Info::ConfirmFunc() {
//    SaveNativeBaseInfo();
//}
//
//void Native_Base_Info::OnRefresh(const wxCommandEvent& event) {
//    RefreshFunc();
//}
//
//void Native_Base_Info::OnConfirm(const wxCommandEvent& event) {
//    ConfirmFunc();
//}
//
//void Native_Base_Info::RefreshFunc() {
//    this->n_buildsys->SetValue(Main::rvp->cur_buildsys);
//    this->n_toolchain->SetValue(Main::rvp->cur_toolchain);
//}
//
//void Native_Base_Info::SaveNativeBaseInfo() {
//    if (!IsLegal()) {
//        wxMessageBox("错误", "原生进程基础信息");
//        return;
//    }
//    project_output->SetValue(wxString::Format("./%s/Project/", n_name->GetValue()));
//    myfunc::SetNodeCont(node, "Name", n_name->GetValue());
//    myfunc::SetNodeCont(node, "Extra_Captbl", n_extra_captbl->GetValue());
//    myfunc::SetNodeCont(node, "Coprocessor", n_coprocessor->GetValue());
//    myfunc::SetNodeCont(node, "Buildsystem", n_buildsys->GetValue());
//    myfunc::SetNodeCont(node, "Toolchain", n_toolchain->GetValue());
//    myfunc::SetNodeCont(node, "Optimization", n_optimi->GetStringSelection());
//    myfunc::SetNodeCont(node, "Project_Output", project_output->GetValue());
//    myfunc::SetNodeCont(node, "Project_Overwrite", project_overw->GetStringValue());
//    myfunc::SetNodeCont(node, "Linker_Output", linker_output->GetValue());
//    myfunc::SetNodeCont(node, "Main_Header_Output", main_hdr_output->GetValue());
//    myfunc::SetNodeCont(node, "Main_Source_Output", main_src_output->GetValue());
//    myfunc::SetNodeCont(node, "Entry_Source_Output", entry_src_output->GetValue());
//    myfunc::SetNodeCont(node, "Entry_Source_Overwrite", entry_src_overw->GetStringValue());
//}
//
//void Native_Base_Info::ClearError() {
//    n_name->CorrectSet();
//    n_coprocessor->CorrectSet();
//}
//
///************************************************
//*   NATIVE  Frame
//************************************************/
//Native_Frame::Native_Frame(wxWindow* parent, wxXmlNode* _root, wxTreeItemId _tree_item_id, const wxString& _errorloc)
//    :wxFrame(parent, wxID_ANY, "原生进程管理", wxDefaultPosition, wxSize(700, 650), wxDEFAULT_FRAME_STYLE),
//    errorloc(_errorloc), root(_root), tree_item_id(_tree_item_id) {
//    //errorloc = _errorloc;
//    //root = _root;
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//    nv_notebook = new wxNotebook(this, wxID_ANY);
//
//    // baseinfo直接给root就行，因为baseinfo里的信息全都是root的直接子节点
//    base_info = new Native_Base_Info(nv_notebook, 0, errorloc + "base info->");
//    base_info->SetNativeBaseInfo(_root);
//    // 以Memory为例
//    // 读取时，找到mem_node节点后将其信息读到各个mem_unit中
//    // 新建时，新建的mem_node节点没有子节点，但是mem_node会被记录在mem_unit中
//    // 等到Memory页面点击“确定”时，再将新建页面的信息挂到mem_node上
//    mem_unit = new Unit_Manager<Mem_Unit_Test>(nv_notebook, errorloc + "Memory->");
//    mem_unit->SetAllUnit(myfunc::FindorMake(_root, "Memory"));
//    mem_unit->SetUnitSign("M");
//
//    shmem_unit = new Unit_Manager<Shmem_Unit_Test>(nv_notebook, errorloc + "Shmem->");
//    shmem_unit->SetAllUnit(myfunc::FindorMake(_root, "Shmem"));
//    shmem_unit->SetUnitSign("S");
//
//    thread_unit = new Unit_Manager<Thread_Unit_Test>(nv_notebook, errorloc + "Thead->");
//    wxXmlNode* thread_root = myfunc::FindorMake(_root, "Thread");
//    if (!myfunc::HaveElementChild(thread_root))
//        thread_root->AddChild(new wxXmlNode(wxXML_ELEMENT_NODE, "T0"));   // 有进程就至少要有一个线程
//    thread_unit->SetAllUnit(myfunc::FindorMake(_root, "Thread"));
//    thread_unit->SetUnitSign("T");
//
//    invocation_unit = new Unit_Manager<Invocation_Unit_Test>(nv_notebook, errorloc + "Invocation->");
//    invocation_unit->SetAllUnit(myfunc::FindorMake(_root, "Invocation"));
//    invocation_unit->SetUnitSign("I");
//
//    port_unit = new Unit_Manager<Port_Unit_Test>(nv_notebook, errorloc + "Port->");
//    port_unit->SetAllUnit(myfunc::FindorMake(_root, "Port"));
//    port_unit->SetUnitSign("P");
//
//    receive_unit = new Unit_Manager<Receive_Unit_Test>(nv_notebook, errorloc + "Receive->");
//    receive_unit->SetAllUnit(myfunc::FindorMake(_root, "Receive"));
//    receive_unit->SetUnitSign("R");
//
//    send_unit = new Unit_Manager<Send_Unit_Test>(nv_notebook, errorloc + "Send->");
//    send_unit->SetAllUnit(myfunc::FindorMake(_root, "Send"));
//    send_unit->SetUnitSign("S");
//
//    kfunc_unit = new Unit_Manager<Kfunc_Unit_Test>(nv_notebook, errorloc + "Kfunc->");
//    kfunc_unit->SetAllUnit(myfunc::FindorMake(_root, "Kfunc"));
//    kfunc_unit->SetUnitSign("K");
//
//    vector_unit = new Unit_Manager<Vector_Unit_Test>(nv_notebook, errorloc + "Vector->");
//    vector_unit->SetAllUnit(myfunc::FindorMake(_root, "Vector"));
//    vector_unit->SetUnitSign("V");
//
//    wxPanel* page_green = new wxPanel(nv_notebook, wxID_ANY);
//    page_green->SetBackgroundColour(*wxGREEN);
//
//    nv_notebook->AddPage(base_info, "Base Info", true);
//    nv_notebook->AddPage(mem_unit, "Memory");
//    nv_notebook->AddPage(shmem_unit, "Shmem");
//    nv_notebook->AddPage(thread_unit, "Thread");
//    nv_notebook->AddPage(invocation_unit, "Invocation");
//    nv_notebook->AddPage(port_unit, "Port");
//    nv_notebook->AddPage(receive_unit, "Receive");
//    nv_notebook->AddPage(send_unit, "Send");
//    nv_notebook->AddPage(kfunc_unit, "Kfunc");
//    nv_notebook->AddPage(vector_unit, "Vector");
//    nv_notebook->AddPage(page_green, "Green");
//
//    wxPanel* confirm_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
//    wxBoxSizer* confirm_box = new wxBoxSizer(wxVERTICAL);
//    wxButton* tot_confirm = new wxButton(confirm_panel, wxID_ANY, "保存原生进程设置");
//    confirm_box->Add(tot_confirm, 0, wxALIGN_RIGHT | wxALL, 5);
//    confirm_panel->SetSizer(confirm_box);
//
//    box->Add(nv_notebook, 1, wxEXPAND);
//    box->Add(confirm_panel, 0, wxEXPAND);
//
//    this->SetSizer(box);
//    Bind(wxEVT_CLOSE_WINDOW, &Native_Frame::OnClose, this);
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Native_Frame::OnTotConfirm, this, tot_confirm->GetId());
//}
//void Native_Frame::ClearError() {
//    base_info->ClearError();
//    mem_unit->ClearError();
//    shmem_unit->ClearError();
//    thread_unit->ClearError();
//    invocation_unit->ClearError();
//    port_unit->ClearError();
//    receive_unit->ClearError();
//    send_unit->ClearError();
//    vector_unit->ClearError();
//    kfunc_unit->ClearError();
//}
//
//void Native_Frame::OnClose(const wxCloseEvent& event) {
//    this->Hide();
//}
//
//void Native_Frame::TotConfirmFunc() {
//    base_info->ConfirmFunc();
//    mem_unit->ConfirmFunc();
//    shmem_unit->ConfirmFunc();
//    thread_unit->ConfirmFunc();
//    invocation_unit->ConfirmFunc();
//    port_unit->ConfirmFunc();
//    receive_unit->ConfirmFunc();
//    send_unit->ConfirmFunc();
//    vector_unit->ConfirmFunc();
//    kfunc_unit->ConfirmFunc();
//    Main::body_splitter->file_tree->RemoveStar(tree_item_id);
//}
//void Native_Frame::OnTotConfirm(const wxCommandEvent& event) {
//    TotConfirmFunc();
//}
//
///************************************************
//*   VIRTUAL
//************************************************/
//Virtual_Machine::Virtual_Machine(wxWindow* parent)
//    :wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE) {
//    //this->vm_count = 0; // 虚拟机计数
//    this->box = new wxBoxSizer(wxVERTICAL);
//    this->SetScrollRate(10, 10);  // 设置滚动速率 (单位是像素)
//
//    wxStaticBoxSizer* stbox_preview = new wxStaticBoxSizer(wxVERTICAL, this, "预览");
//    this->btn_preview = new wxButton(this, wxID_ANY, "虚拟机配置源码预览");
//    stbox_preview->Add(this->btn_preview, 0, wxALL, 5);
//
//    this->box->Add(stbox_preview, 0, wxEXPAND | wxALL, 5);
//    this->SetSizer(this->box);
//
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Virtual_Machine::PreviewFunc, this, this->btn_preview->GetId());
//}
//
//void Virtual_Machine::PreviewFunc(wxCommandEvent& event) {
//    if (virtual_machine) {
//        wxString str;
//        myfunc::DFS(virtual_machine, str);
//        myfunc::PreviewHelper(str, "虚拟机配置源码预览");
//    }
//}
//
//void Virtual_Machine::SetVirtualMachine(wxXmlNode* root) {
//    virtual_machine = root;
//    Main::body_splitter->file_tree->AddVm(root);
//}
//
///************************************************
//*   VIRTUAL base_info
//************************************************/
//Virtual_Machine_Base_Info::Virtual_Machine_Base_Info(wxWindow* parent, const int& _counter, const wxString& _errorloc)
//    :wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE) {
//    errorloc = _errorloc;
//    this->counter = _counter;
//    this->box = new wxBoxSizer(wxVERTICAL);
//    this->title = new wxStaticText(this, wxID_ANY, wxString::Format("虚拟机 %d", this->counter));
//    this->SetScrollRate(10, 10);  // 设置滚动速率 (单位是像素)
//
//    wxStaticBoxSizer* stbox_base_info = new wxStaticBoxSizer(wxVERTICAL, this, "Base_info");
//    //wxStaticText* txt_name = new  wxStaticText(this, wxID_ANY, "Name");
//    v_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc);
//    v_extra_captbl = new MyZ_GE_Zero(this, "Extra Captbl", wxEmptyString, errorloc);
//    v_coprocessor = new MyCoprocessor(this, "Coprocessor");
//
//    stbox_base_info->Add(v_name, 0, wxALL | wxEXPAND, 5);
//    stbox_base_info->Add(0, 5);
//    stbox_base_info->Add(v_extra_captbl, 0, wxALL | wxEXPAND, 5);
//    stbox_base_info->Add(0, 5);
//    stbox_base_info->Add(v_coprocessor, 0, wxALL | wxEXPAND, 5);
//
//    wxStaticBoxSizer* stbox_memory = new wxStaticBoxSizer(wxVERTICAL, this, "Memory");
//    v_vs_size = new SizeSlider(this, "Vector Stack_ ize", wxEmptyString, errorloc);
//    v_us_size = new SizeSlider(this, "User Stack Size", wxEmptyString, errorloc);
//    stbox_memory->Add(v_vs_size, 0, wxALL | wxEXPAND, 5);
//    stbox_memory->Add(0, 5);
//    stbox_memory->Add(v_us_size, 0, wxALL | wxEXPAND, 5);
//
//
//    wxStaticBoxSizer* stbox_others = new wxStaticBoxSizer(wxVERTICAL, this, "Others");
//
//    v_priority = new MyVirtualPrio(this, "Priority", wxEmptyString, errorloc);
//    v_priority->SetCheckTip("该值的范围限制[0, Virtual Priority - 1]，见 监视器配置->Virtual Priority");
//
//    v_timesslice = new MyZ_G_Zero(this, "Timeslice", wxEmptyString, errorloc);
//    v_period = new MyZ_G_Zero(this, "Period", wxEmptyString, errorloc);
//    v_wachdog = new MyZ_GE_Zero(this, "Watchdog", wxEmptyString, errorloc);
//    v_vnum = new MyZ_GE_Zero(this, "Vector_Num", wxEmptyString, errorloc);
//
//    v_gtype = new SimpleText(this, "Guest_Type", wxEmptyString, wxEmptyString, wxTE_READONLY);
//    v_buildsys = new SimpleText(this, "Buildsystem", wxEmptyString, wxEmptyString, wxTE_READONLY);
//    v_toolchain = new SimpleText(this, "Toolchain", wxEmptyString, wxEmptyString, wxTE_READONLY);
//
//    v_optimi = new MyChoice(this, "Optimization");
//    //wxArrayString arr_optimi = { "O0","O1","O2","O3","Of","Os" };
//    //v_optimi->FillChoice(arr_optimi);
//    //v_optimi->SetSelection(0);
//
//    stbox_memory->Add(v_priority, 0, wxALL | wxEXPAND, 5);
//    stbox_memory->Add(0, 5);
//    stbox_memory->Add(v_timesslice, 0, wxALL | wxEXPAND, 5);
//    stbox_memory->Add(0, 5);
//    stbox_memory->Add(v_period, 0, wxALL | wxEXPAND, 5);
//    stbox_memory->Add(0, 5);
//    stbox_memory->Add(v_wachdog, 0, wxALL | wxEXPAND, 5);
//    stbox_memory->Add(0, 5);
//    stbox_memory->Add(v_vnum, 0, wxALL | wxEXPAND, 5);
//    stbox_memory->Add(0, 5);
//    stbox_memory->Add(v_gtype, 0, wxALL | wxEXPAND, 5);
//    stbox_memory->Add(0, 5);
//    stbox_memory->Add(v_buildsys, 0, wxALL | wxEXPAND, 5);
//    stbox_memory->Add(0, 5);
//    stbox_memory->Add(v_toolchain, 0, wxALL | wxEXPAND, 5);
//    stbox_memory->Add(0, 5);
//    stbox_memory->Add(v_optimi, 0, wxALL | wxEXPAND, 5);
//
//    wxStaticBoxSizer* stbox_path = new wxStaticBoxSizer(wxVERTICAL, this, "Path");
//    project_output = new SimpleText(this, "Project_Output", wxEmptyString, wxEmptyString, wxTE_READONLY);
//
//    project_overw = new BoolBox(this, "Project Overwrite");
//
//    linker_output = new SimpleText(this, "Linker Output", wxEmptyString, "./", wxTE_READONLY);
//    main_hdr_output = new SimpleText(this, "Main Header Output", wxEmptyString, "../Include/", wxTE_READONLY);
//    main_src_output = new SimpleText(this, "Main Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
//    vir_hdr_output = new SimpleText(this, "Virtual Header Output", wxEmptyString, "../Include/", wxTE_READONLY);
//    vir_hdr_overw = new BoolBox(this, "Virtual Header Overwrite");
//    vir_src_output = new SimpleText(this, "Virtual Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
//    vir_src_overw = new BoolBox(this, "Virtual_Source_Overwrite");
//
//    stbox_path->Add(project_output, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(project_overw, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(linker_output, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(main_hdr_output, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(main_src_output, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(vir_hdr_output, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(vir_hdr_overw, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(vir_src_output, 0, wxALL | wxEXPAND, 5);
//    stbox_path->Add(0, 5);
//    stbox_path->Add(vir_src_overw, 0, wxALL | wxEXPAND, 5);
//
//    wxButton* btn_confirm = new wxButton(this, wxID_ANY, "确认");
//    wxButton* btn_refresh = new wxButton(this, wxID_ANY, "刷新");
//
//    this->box->Add(title, 0, wxALL | wxALIGN_LEFT, 5);
//    this->box->Add(stbox_base_info, 0, wxALL | wxEXPAND, 5);
//    this->box->Add(stbox_memory, 0, wxALL | wxEXPAND, 5);
//    this->box->Add(stbox_others, 0, wxALL | wxEXPAND, 5);
//    this->box->Add(stbox_path, 0, wxALL | wxEXPAND, 5);
//    this->box->Add(btn_confirm, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->box->Add(btn_refresh, 0, wxALL | wxALIGN_RIGHT, 5);
//    this->SetSizer(this->box);
//    
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Virtual_Machine_Base_Info::OnConfirm, this, btn_confirm->GetId());
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Virtual_Machine_Base_Info::OnRefresh, this, btn_refresh->GetId());
//
//}
//
//void Virtual_Machine_Base_Info::SetVirMachineBaseInfo(wxXmlNode* _node) {
//    this->node = _node;
//    std::map<wxString, wxXmlNode*>* mp = new std::map<wxString, wxXmlNode*>;
//    for (wxXmlNode* son = node->GetChildren(); son; son = son->GetNext()) {
//        mp->insert(std::make_pair(son->GetName(), son));
//    }
//    v_name->SetValue(myfunc::FindorMake(node, "Name", mp)->GetNodeContent());
//    v_extra_captbl->SetValue(myfunc::FindorMake(node, "Extra_Captbl", mp)->GetNodeContent());
//
//
//    wxString str = myfunc::FindorMake(node, "Coprocessor", mp)->GetNodeContent();
//    wxArrayString cur_coprocessers = myfunc::Separater(str, ",");
//    //wxArrayString coprocessers = myfunc::Separater(Main::rvc->FindContent("#Chip#Coprocessor"), ",");
//    //coprocessers.push_back("None");   // 对应未配置协处理器的情况
//    //v_coprocessor->FillAllItem(coprocessers);
//    v_coprocessor->SetValue(cur_coprocessers);
//
//
//    v_vs_size->SetValue(myfunc::FindorMake(node, "Vector_Stack_Size", mp)->GetNodeContent());
//    v_us_size->SetValue(myfunc::FindorMake(node, "User_Stack_Size", mp)->GetNodeContent());
//
//    v_priority->SetValue(myfunc::FindorMake(node, "Priority", mp)->GetNodeContent());
//    v_timesslice->SetValue(myfunc::FindorMake(node, "Timeslice", mp)->GetNodeContent());
//
//    v_period->SetValue(myfunc::FindorMake(node, "Period", mp)->GetNodeContent());
//    v_wachdog->SetValue(myfunc::FindorMake(node, "Watchdog", mp)->GetNodeContent());
//    v_vnum->SetValue(myfunc::FindorMake(node, "Vector_Num", mp)->GetNodeContent());
//    v_gtype->SetValue(myfunc::FindorMake(node, "Guest_Type", mp)->GetNodeContent());
//    v_buildsys->SetValue(myfunc::FindorMake(node, "Buildsystem", mp)->GetNodeContent());
//    v_toolchain->SetValue(myfunc::FindorMake(node, "Toolchain", mp)->GetNodeContent());
//
//    wxString cur_optim = myfunc::FindorMake(node, "Optimization", mp)->GetNodeContent();
//    v_optimi->SetSelection(v_optimi->FindValue(cur_optim));
//
//    project_output->SetValue(myfunc::FindorMake(node, "Project_Output", mp)->GetNodeContent());
//    project_overw->SetValue(myfunc::FindorMake(node, "Project_Overwrite", mp)->GetNodeContent());
//    linker_output->SetValue(myfunc::FindorMake(node, "Linker_Output", mp)->GetNodeContent());
//    main_hdr_output->SetValue(myfunc::FindorMake(node, "Main_Header_Output", mp)->GetNodeContent());
//    main_src_output->SetValue(myfunc::FindorMake(node, "Main_Source_Output", mp)->GetNodeContent());
//    vir_hdr_output->SetValue(myfunc::FindorMake(node, "Virtual_Header_Output", mp)->GetNodeContent());
//    vir_hdr_overw->SetValue(myfunc::FindorMake(node, "Virtual_Header_Overwrite", mp)->GetNodeContent());
//    
//    vir_src_output->SetValue(myfunc::FindorMake(node, "Virtual_Source_Output", mp)->GetNodeContent());
//    vir_src_overw->SetValue(myfunc::FindorMake(node, "Virtual_Source_Overwrite", mp)->GetNodeContent());
//}
//
//void Virtual_Machine_Base_Info::RefreshFunc() {
//    this->v_gtype->SetValue(Main::rvp->cur_guest);
//    this->v_buildsys->SetValue(Main::rvp->cur_buildsys);
//    this->v_toolchain->SetValue(Main::rvp->cur_toolchain);
//}
//bool Virtual_Machine_Base_Info::IsLegal() {
//    bool j1 = v_name->IsLegal();
//    bool j2 = Main::body_splitter->workarea_panel->IsUniqueNameVm(this, v_name->GetValue());
//    if (!j2)
//        v_name->IncorrectSet("命名不合法 命名重复(检查其他 原生进程 和 虚拟机)");
//    bool j3 = v_extra_captbl->IsLegal();
//    bool j4 = v_vs_size->IsLegal();
//    bool j5 = v_us_size->IsLegal();
//    bool j6 = v_priority->IsLegal(Main::rvp->GetVirtPrio());
//    bool j7 = v_timesslice->IsLegal();
//    bool j8 = v_period->IsLegal();
//    bool j9 = v_wachdog->IsLegal();
//    bool j10 = v_vnum->IsLegal();
//    bool j11 = v_coprocessor->IsLegal();
//
//    return j1 && j2 && j3 && j4 && j5 && j6 && j7 && j8 && j9 && j10 && j11;
//}
//
//void Virtual_Machine_Base_Info::ClearError() {
//    v_name->CorrectSet();
//    v_extra_captbl->CorrectSet();
//    v_vs_size->CorrectSet();
//    v_us_size->CorrectSet();
//    v_priority->CorrectSet();
//    v_timesslice->CorrectSet();
//    v_period->CorrectSet();
//    v_wachdog->CorrectSet();
//    v_vnum->CorrectSet();
//    v_coprocessor->CorrectSet();
//}
//
//void Virtual_Machine_Base_Info::OnConfirm(const wxCommandEvent& event) {
//    ConfirmFunc();
//}
//
//void Virtual_Machine_Base_Info::OnRefresh(const wxCommandEvent& event) {
//    RefreshFunc();
//}
//
//void Virtual_Machine_Base_Info::SaveVirMachineBaseInfo() {
//    myfunc::SetNodeCont(node, "Name", v_name->GetValue());
//    myfunc::SetNodeCont(node, "Extra_Captbl", v_extra_captbl->GetValue());
//    myfunc::SetNodeCont(node, "Coprocessor", v_coprocessor->GetValue());
//
//    myfunc::SetNodeCont(node, "Vector_Stack_Size", v_vs_size->GetValue());
//    myfunc::SetNodeCont(node, "User_Stack_Size", v_us_size->GetValue());
//
//    myfunc::SetNodeCont(node, "Priority", v_priority->GetValue());
//    myfunc::SetNodeCont(node, "Timeslice", v_timesslice->GetValue());
//
//    myfunc::SetNodeCont(node, "Period", v_period->GetValue());
//    myfunc::SetNodeCont(node, "Watchdog", v_wachdog->GetValue());
//    myfunc::SetNodeCont(node, "Vector_Num", v_vnum->GetValue());
//    myfunc::SetNodeCont(node, "Guest_Type", v_gtype->GetValue());
//    myfunc::SetNodeCont(node, "Buildsystem", v_buildsys->GetValue());
//    myfunc::SetNodeCont(node, "Toolchain", v_toolchain->GetValue());
//    myfunc::SetNodeCont(node, "Optimization", v_optimi->GetStringSelection());
//
//    myfunc::SetNodeCont(node, "Project_Output", project_output->GetValue());
//    myfunc::SetNodeCont(node, "Project_Overwrite", project_overw->GetStringValue());
//    myfunc::SetNodeCont(node, "Linker_Output", linker_output->GetValue());
//    myfunc::SetNodeCont(node, "Main_Header_Output", main_hdr_output->GetValue());
//    myfunc::SetNodeCont(node, "Main_Source_Output", main_src_output->GetValue());
//
//    myfunc::SetNodeCont(node, "Virtual_Header_Output", vir_hdr_output->GetValue());
//    myfunc::SetNodeCont(node, "Virtual_Header_Overwrite", vir_hdr_overw->GetStringValue());
//
//    myfunc::SetNodeCont(node, "Virtual_Source_Output", vir_src_output->GetValue());
//    myfunc::SetNodeCont(node, "Virtual_Source_Overwrite", vir_src_overw->GetStringValue());
//}
//
//
//void Virtual_Machine_Base_Info::ConfirmFunc() {
//    if (!IsLegal()) {
//        wxMessageBox("错误", "虚拟机基础信息");
//        return;
//    }
//    project_output->SetValue(wxString::Format("./%s/Project/", v_name->GetValue()));
//    SaveVirMachineBaseInfo();
//}
//
///************************************************
//*   VIRTUAL Frame
//************************************************/
//Virtual_Machine_Frame::Virtual_Machine_Frame(wxWindow* parent, wxXmlNode* _root, const wxString& _errorloc)
//    :wxFrame(parent, wxID_ANY, "虚拟机管理", wxDefaultPosition, wxSize(700, 650), wxDEFAULT_FRAME_STYLE),
//    errorloc(_errorloc), root(_root) {
//
//    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//    vm_notebook = new wxNotebook(this, wxID_ANY);
//
//    base_info = new Virtual_Machine_Base_Info(vm_notebook, 0, errorloc + "base info->");
//    base_info->SetVirMachineBaseInfo(_root);
//
//    mem_unit = new Unit_Manager<Mem_Unit_Test>(vm_notebook, errorloc + "Memory->");
//    mem_unit->SetAllUnit(myfunc::FindorMake(_root, "Memory"));
//    mem_unit->SetUnitSign("M");
//
//    shmem_unit = new Unit_Manager<Shmem_Unit_Test>(vm_notebook, errorloc + "Shmem->");
//    shmem_unit->SetAllUnit(myfunc::FindorMake(_root, "Shmem"));
//    shmem_unit->SetUnitSign("S");
//
//    send_unit = new Unit_Manager<Send_Unit_Test>(vm_notebook, errorloc + "Send->");
//    send_unit->SetAllUnit(myfunc::FindorMake(_root, "Send"));
//    send_unit->SetUnitSign("S");
//
//    kfunc_unit = new Unit_Manager<Kfunc_Unit_Test>(vm_notebook, errorloc + "Kfunc->");
//    kfunc_unit->SetAllUnit(myfunc::FindorMake(_root, "Kfunc"));
//    kfunc_unit->SetUnitSign("K");
//
//    wxPanel* page_green = new wxPanel(vm_notebook, wxID_ANY);
//    page_green->SetBackgroundColour(*wxGREEN);
//
//    vm_notebook->AddPage(base_info, "Base Info", true);
//    vm_notebook->AddPage(mem_unit, "Mem");
//    vm_notebook->AddPage(shmem_unit, "Shmem");
//    vm_notebook->AddPage(send_unit, "Send");
//    vm_notebook->AddPage(kfunc_unit, "Kfunc");
//    vm_notebook->AddPage(page_green, "Green");
//
//    wxPanel* confirm_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
//    wxBoxSizer* confirm_box = new wxBoxSizer(wxVERTICAL);
//    wxButton* tot_confirm = new wxButton(confirm_panel, wxID_ANY, "保存原生进程设置");
//    confirm_box->Add(tot_confirm, 0, wxALIGN_RIGHT | wxALL, 5);
//    confirm_panel->SetSizer(confirm_box);
//
//    box->Add(vm_notebook, 1, wxEXPAND);
//    box->Add(confirm_panel, 0, wxEXPAND);
//    this->SetSizer(box);
//    Bind(wxEVT_CLOSE_WINDOW, &Virtual_Machine_Frame::OnClose, this);
//    //Bind(wxEVT_SHOW, &Virtual_Machine_Frame::OnShow, this);
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Virtual_Machine_Frame::OnTotConfirm, this, tot_confirm->GetId());
//}
//
//void Virtual_Machine_Frame::OnClose(const wxCloseEvent& event) {
//    this->Hide();
//}
//
//
//void Virtual_Machine_Frame::ClearError() {
//    base_info->ClearError();
//    mem_unit->ClearError();
//    shmem_unit->ClearError();
//    send_unit->ClearError();
//    kfunc_unit->ClearError();
//}
//
//void Virtual_Machine_Frame::OnTotConfirm(const wxCommandEvent& event) {
//    TotConfirmFunc();
//}
//
//void Virtual_Machine_Frame::TotConfirmFunc() {
//    base_info->ConfirmFunc();
//    mem_unit->ConfirmFunc();
//    shmem_unit->ConfirmFunc();
//    send_unit->ConfirmFunc();
//    kfunc_unit->ConfirmFunc();
//
//}
//
//// 加载页面
//LoadingFrame::LoadingFrame(wxWindow* parent)
//    : wxFrame(parent, wxID_ANY, "加载中...", wxDefaultPosition, wxSize(300, 100), wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP | wxCAPTION) {
//    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
//    wxStaticText* label = new wxStaticText(this, wxID_ANY, "请稍候，正在加载...");
//    sizer->Add(label, 1, wxALL | wxALIGN_CENTER, 10);
//    this->SetSizer(sizer);
//    this->Centre();
//}
//
//
///*******************************************************************
//*   Base 选择栏
//*******************************************************************/
//BaseSlider::BaseSlider(wxWindow* parent, const wxString& name, const wxString& describe, const wxString& errorloc)
//    :SimpleBase(parent, name, describe, errorloc) {
//
//    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
//    content = new wxTextCtrl(this, wxID_ANY, "0x1");
//    hex_content = "0x1";
//    wxButton* fill_auto = new wxButton(this, wxID_ANY, "自动");
//    wxButton* fill_info = new wxButton(this, wxID_ANY, "填入");
//    hbox1->Add(content, 1, wxLEFT, 15);
//    hbox1->Add(fill_auto, 0, wxLEFT, 3);
//    hbox1->Add(fill_info, 0, wxLEFT, 3);
//    hbox1->Add(15, 0);
//    box->Add(hbox1, 0, wxEXPAND);
//
//    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
//    // 创建一个滑块控件，范围从 0 到 3 (对应四个选项)
//    slider = new wxSlider(this, wxID_ANY, 0, 0, 39, wxPoint(0, 0), wxDefaultSize, wxSL_AUTOTICKS);
//    slider->SetTickFreq(2);// 设置每10个单位显示一个刻度
//
//    displayText = new wxStaticText(this, wxID_ANY, "0x1");
//    hbox2->Add(slider, 2, wxLEFT, 10);
//    hbox2->Add(displayText, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, 15);
//
//    box->Add(hbox2, 0, wxEXPAND);
//
//    slider->Bind(wxEVT_SLIDER, &BaseSlider::OnSliderUpdate, this);
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &BaseSlider::OnFillIn, this, fill_info->GetId());
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &BaseSlider::OnAuto, this, fill_auto->GetId());
//
//}
//
//void BaseSlider::SetPosition(const int& pos) {
//    slider->SetValue(pos);
//}
//
//bool BaseSlider::IsLegal() {
//    wxString str = content->GetValue();
//    if (str == "Auto") {
//        CorrectSet();
//        return true;
//    }
//    str = str.Lower();
//    wxRegEx regex("^0x[0-9a-fA-F]+$|^[0-9a-fA-F]+$");
//    if (!regex.Matches(str)) {
//        IncorrectSet("空字符串 或 不是合法的十六进制数");
//        return false;
//    }
//    if (str.starts_with("0x"))
//        str = "0x" + str.SubString(2, str.Length() - 1).Upper();
//    else
//        str = "0x" + str.Upper();
//    SetValue(str);
//    CorrectSet();
//    return true;
//}
//
//void BaseSlider::OnFillIn(const wxCommandEvent& event) {
//    this->content->SetValue(this->hex_content);
//}
//
//void BaseSlider::OnAuto(const wxCommandEvent& event) {
//    this->content->SetValue("Auto");
//}
//
//
//void BaseSlider::OnSliderUpdate(const wxCommandEvent& event) {
//    unsigned int value = slider->GetValue();
//    unsigned int the_decade = value / 10;
//    unsigned int the_unit = value % 10;
//    wxString size_unit;
//    switch (the_decade) {
//    case 0:
//        size_unit = "B";
//        break;
//    case 1:
//        size_unit = "KB";
//        break;
//    case 2:
//        size_unit = "MB";
//        break;
//    case 3:
//        size_unit = "GB";
//        break;
//    default:
//        break;
//    }
//    value = 1 << the_unit;
//    this->hex_content = myfunc::toHex(value, size_unit);
//    displayText->SetLabel(wxString::Format("%s", hex_content)); // 更新文本显示
//}
//
///********************************************************************************
//*   Size 选择栏
//********************************************************************************/
//SizeSlider::SizeSlider(wxWindow* parent, const wxString& name, const wxString& describe, const wxString& errorloc)
//    :SimpleBase(parent, name, describe, errorloc) {
//    //this->SetBackgroundColour(*wxGREEN);
//
//    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
//    content = new wxTextCtrl(this, wxID_ANY, "0x1");
//    wxButton* fill_info = new wxButton(this, wxID_ANY, "填入");
//    hex_content = "0x1";
//    hbox1->Add(content, 1, wxLEFT, 15);
//    hbox1->Add(fill_info, 0, wxLEFT, 2);
//    hbox1->Add(15, 0);
//    box->Add(hbox1, 0, wxEXPAND);
//
//    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
//    // 创建一个滑块控件，范围从 0 到 3 (对应四个选项)
//    slider = new wxSlider(this, wxID_ANY, 0, 0, 39 , wxPoint(0, 0), wxDefaultSize, wxSL_AUTOTICKS);
//    slider->SetTickFreq(2);// 设置每10个单位显示一个刻度
//    //slider->SetBackgroundColour(*wxRED);
//    //box->Add(slider, 0, wxEXPAND);
//    displayText = new wxStaticText(this, wxID_ANY, "1 B(0x1)");
//
//    hbox2->Add(slider, 2, wxLEFT, 10);
//    hbox2->Add(displayText, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, 15);
//
//    box->Add(hbox2, 0, wxEXPAND);
//
//    slider->Bind(wxEVT_SLIDER, &SizeSlider::OnSliderUpdate, this);
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &SizeSlider::OnFillIn, this, fill_info->GetId());
//}
//
//void SizeSlider::SetPosition(const int& pos) {
//    slider->SetValue(pos);
//}
//
//bool SizeSlider::IsLegal() {
//    wxString str = content->GetValue();
//    str = str.Lower();
//    wxRegEx regex("^0x[0-9a-fA-F]+$|^[0-9a-fA-F]+$");
//    if (!regex.Matches(str)) {
//        IncorrectSet("空字符串 或 不是合法的十六进制数");
//        return false;
//    }
//    if (str.starts_with("0x"))
//        str = "0x" + str.SubString(2, str.Length() - 1).Upper();
//    else
//        str = "0x" + str.Upper();
//    SetValue(str);
//    CorrectSet();
//    return true;        
//}
//
//void SizeSlider::OnFillIn(const wxCommandEvent& event) {
//    this->content->SetValue(this->hex_content);
//}
//
//void SizeSlider::OnSliderUpdate(wxCommandEvent& event) {
//    unsigned int value = slider->GetValue();
//    unsigned int the_decade = value / 10;
//    unsigned int the_unit = value % 10;
//    wxString size_unit;
//    switch (the_decade){
//    case 0:
//        size_unit = "B";
//        break;
//    case 1:
//        size_unit = "KB";
//        break;
//    case 2:
//        size_unit = "MB";
//        break;
//    case 3:
//        size_unit = "GB";
//        break;
//    default:
//        break;
//    }
//    value = 1 << the_unit;
//    this->hex_content = myfunc::toHex(value, size_unit);
//    displayText->SetLabel(wxString::Format("%d %s(%s)", value, size_unit, hex_content)); // 更新文本显示
//}
//
///*********************************************************************
//*   简单的文本框
//*********************************************************************/
//SimpleText::SimpleText(wxWindow* parent, wxString _title, wxString _describe, wxString _content, long _style, wxString _errorloc)
//    :SimpleBase(parent, _title, _describe, _errorloc) {
//    default_content = _content;
//    text = new wxTextCtrl(this, wxID_ANY, _content, wxDefaultPosition, wxDefaultSize, _style);
//    box->Add(text, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 15);
//    //SetSizer(box);
//}
//
//bool SimpleText::IsLegal() {
//    // 首先不能为空
//    wxString tar = text->GetValue();
//    tar = tar.Upper(); 
//    wxRegEx regex("^[a-zA-Z_][a-zA-Z0-9_]*$");
//    if (!regex.Matches(tar)) {
//        IncorrectSet("空字符串 或 不是一个合法C标识符");
//        return false;
//    }
//    CorrectSet();
//    return true;
//}
//
///**********************************************************************
//*   Bool 选择栏
//**********************************************************************/
//BoolBox::BoolBox(wxWindow* parent, wxString _title, wxString _intr)
//    :SimpleBase(parent, _title, _intr) {
//
//    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
//    yes = new wxRadioButton(this, wxID_ANY, "是", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);  // 按钮组的起始
//    no = new wxRadioButton(this, wxID_ANY, "否");
//    hbox->Add(10, 0);
//    hbox->Add(yes, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 5);
//    hbox->Add(no, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 5);
//
//    box->Add(hbox, 0, wxEXPAND);
//    SetSizer(box);
//    box->Layout();
//}
//
//
///********************************************************************
//*   Align
//********************************************************************/
//Align::Align(wxWindow* parent, wxString _title, wxString _intr, wxString _errorloc)
//    :SimpleBase(parent, _title, _intr, _errorloc) {
//
//    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
//    text = new wxTextCtrl(this, wxID_ANY);
//    wxButton* fill_auto = new wxButton(this, wxID_ANY, "自动");
//    hbox->Add(text, 1, wxLEFT, 15);
//    hbox->Add(2, 0);
//    hbox->Add(fill_auto, 0, wxRIGHT, 15);
//
//    box->Add(hbox, 0, wxEXPAND);
//
//    SetSizer(box);
//    Layout();
//
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Align::OnFillAuto, this, fill_auto->GetId());
//}
//
//void Align::OnFillAuto(const wxCommandEvent& event) {
//    text->SetValue("Auto");
//}
//
//bool Align::IsLegal(const wxString& base) {
//    wxString str = text->GetValue();
//    // align必须是合法的十进制数或者 Auto【首先要满足】
//    wxRegEx regex("^-?\\d+(\\.\\d+)?$");
//    if (!regex.Matches(str) && str != "Auto") {
//        IncorrectSet("Align不合法，该项可以填写 Auto或者一个合法的十进制数");
//        return false;
//    }
//    
//    // 段基址为Auto时，对align没有限制【其次要满足】
//    if (base == "Auto") {
//        CorrectSet();
//        return true;
//    }
//    else {
//        if (str != "Auto") {
//            IncorrectSet("段基址是固定值，Align项必须填写 Auto");
//            return false;
//        }
//    }
//    CorrectSet();
//    return true;
//}
//
//
///***********************************************************************************
//*   SimpleBase  自编控件继承的基类
//***********************************************************************************/
//SimpleBase::SimpleBase(wxWindow* parent, wxString _title, wxString _intr, wxString _errorloc)
//    :wxPanel(parent, wxID_ANY) {
//    uni_id = std::hash<SimpleBase*>()(this);
//    default_color = *wxWHITE;
//    errorloc = _errorloc + _title;
//
//    box = new wxBoxSizer(wxVERTICAL);
//    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
//
//    light = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(8, 8));
//    title = new wxStaticText(this, wxID_ANY, _title);
//    wxFont title_font = title->GetFont();
//    title_font.SetPointSize(10);
//    title->SetFont(title_font);
//    wxStaticText* introduction = new wxStaticText(this, wxID_ANY, _intr);
//    introduction->SetForegroundColour(RGB(128, 128, 128));
//    wxFont intr_font = introduction->GetFont();
//    intr_font.SetPointSize(8);
//    
//    introduction->SetFont(intr_font);
//
//    hbox->Add(light, 0, wxLEFT| wxALIGN_CENTER_VERTICAL, 2);
//    hbox->Add(title, 0, wxLEFT | wxALIGN_BOTTOM, 5);
//    hbox->Add(introduction, 0, wxLEFT | wxTOP | wxRIGHT | wxALIGN_BOTTOM, 5);
//    box->Add(hbox, 0, wxTOP, 5);
//    //box->Add(introduction, 0, wxALL | wxALIGN_LEFT, 2);
//
//    if (_title == wxEmptyString)
//        title->Hide();
//    if (_intr == wxEmptyString)
//        introduction->Hide();
//
//    SetSizer(box);
//    box->Layout();
//
//
//}
//
//
//void SimpleBase::CorrectSet() {
//    // 关灯
//    light->SetBackgroundColour(default_color);
//    // 关闭ToolTip
//    light->SetToolTip(nullptr);
//    // 从错误消息提示中移除
//    Main::log_panel->DeleteLog(uni_id);
//    // 刷新页面
//    Refresh();
//}
//
//void SimpleBase::IncorrectSet(const wxString& info) {
//    // 开灯
//    light->SetBackgroundColour(*wxRED);
//    // 设置ToolTip
//    light->SetToolTip(wxString::Format("%s[%d]", info, uni_id));
//    // 添加到错误消息提示中
//    Main::log_panel->AddLog(uni_id, info, errorloc);
//    // 刷新页面
//    Refresh();
//}
//
//void SimpleBase::SetCheckTip(const wxString& tip) {
//    title->SetToolTip(tip);
//    title->SetForegroundColour(wxColor(225, 183, 37)); 
//    title->Refresh();
//}
//
///***********************************************************************************
//*   MyChoice
//***********************************************************************************/
//MyChoice::MyChoice(wxWindow* parent,
//    const wxString& title,
//    const wxString& describe,
//    const wxString& errorloc)
//    :SimpleBase(parent, title, describe, errorloc) {
//    choice = new wxChoice(this, wxID_ANY);
//    box->Add(choice, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
//}
//
//void MyChoice::FillChoice(wxArrayString arr) {
//    choice->Clear();
//    choice->Append(arr);
//    choice->SetSelection(0);
//}
//
//int MyChoice::FindValue(const wxString& tar) {
//    int size = choice->GetCount();
//    for (int i = 0; i < size; i++) {
//        if (choice->GetString(i) == tar)
//            return i;
//    }
//    //wxMessageBox("未找到预设选项");
//    return 0;
//}
//
///***********************************************************************************
//*   MyAttribute
//***********************************************************************************/
//MyAttribute::MyAttribute(wxWindow* parent, wxWindowID id, const wxString& title, const wxString& describe, const wxString& _errorloc)
//    :SimpleBase(parent, title, describe, _errorloc) {
//    
//    mp = new std::unordered_map<char, wxCheckBox*>;
//    text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
//
//    Readable = new wxCheckBox(this, wxID_ANY, "Readable");
//    Writable = new wxCheckBox(this, wxID_ANY, "Writable");
//    Executable = new wxCheckBox(this, wxID_ANY, "Executable");
//    Bufferable = new wxCheckBox(this, wxID_ANY, "Bufferable");
//    Cacheable = new wxCheckBox(this, wxID_ANY, "Cacheable");
//    Static = new wxCheckBox(this, wxID_ANY, "Static");
//    mp->insert(std::make_pair('R', Readable));
//    mp->insert(std::make_pair('W', Writable));
//    mp->insert(std::make_pair('X', Executable));
//    mp->insert(std::make_pair('B', Bufferable));
//    mp->insert(std::make_pair('C', Cacheable));
//    mp->insert(std::make_pair('S', Static));
//
//    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
//    box->Add(0,3);
//    box->Add(Readable, 0, wxLEFT, 15);
//    box->Add(0, 3);
//    box->Add(Writable, 0, wxLEFT, 15);
//    box->Add(0, 3);
//    box->Add(Executable, 0, wxLEFT, 15);
//    box->Add(0, 3);
//    box->Add(Bufferable, 0, wxLEFT, 15);
//    box->Add(0, 3);
//    box->Add(Cacheable, 0, wxLEFT, 15);
//    box->Add(0, 3);
//    box->Add(Static, 0, wxLEFT, 15);
//
//    Readable->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
//    Writable->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
//    Executable->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
//    Bufferable->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
//    Cacheable->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
//    Static->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
//}
//
//void MyAttribute::SetValue(const wxString& str) {
//    text->SetValue(str);
//    for (auto i : str)
//        mp->find(i)->second->SetValue(true);
//}
//
//bool MyAttribute::IsLegal() {
//    if (text->GetValue() == wxEmptyString) {
//        IncorrectSet("内存段属性设置不能为空");
//        return false;
//    }
//    CorrectSet();
//    return true;
//}
//
//void MyAttribute::OnRadioButtonClicked(const wxCommandEvent& event) {
//    wxString ans;
//    wxCheckBox* curButton = dynamic_cast<wxCheckBox*>(event.GetEventObject());
//    if (curButton) {
//        wxString label = curButton->GetLabel();
//        //wxLogMessage("You clicked the radio button: %s", label);
//        for (auto i : *mp) {
//            if (i.second->GetValue()) {
//                ans.append(i.first);
//            }
//        }
//        text->SetValue(ans);
//    }
//}
//
///***********************************************************************************
//*   MyCoprocessor
//***********************************************************************************/
//MyCoprocessor::MyCoprocessor(wxWindow* parent,
//    const wxString& title,
//    const wxString& describe,
//    const wxString& errorloc)
//    :SimpleBase(parent, title, describe, errorloc) {
//
//    checkbox = new wxBoxSizer(wxVERTICAL);
//    box->Add(checkbox, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
//    mp = new std::unordered_map<wxString, wxCheckBox*>;
//}
//
//void MyCoprocessor::FillAllItem(wxArrayString arr) {
//    mp->clear();
//    checkbox->Clear(true);
//    arr = myfunc::MakeUnique(arr);  // 去重
//    for (auto i : arr) {
//        wxCheckBox* wxcheckbox = new wxCheckBox(this, wxID_ANY, i);
//        checkbox->Add(wxcheckbox, 0, wxALL, 3);
//        mp->insert(std::make_pair(i, wxcheckbox));
//    }
//    Refresh();
//}
//
//void MyCoprocessor::SetValue(const wxArrayString& arr) {
//    for (auto i : arr) {
//        if (mp->find(i) != mp->end()) {
//            mp->find(i)->second->SetValue(true);
//        }
//    }
//}
//
//wxString MyCoprocessor::GetValue() {
//    wxString ans;
//    for (auto i : *mp) {
//        if (i.second->GetValue()) {
//            ans += (i.first + ",");
//        }
//    }
//    return ans.SubString(0, ans.length() - 2);
//}
//
//bool MyCoprocessor::IsLegal() {
//    // 起码得选一个
//    bool select_one = false;
//    for (auto i : *mp)
//        select_one = i.second->GetValue() || select_one;
//    if (!select_one) {
//        IncorrectSet("至少选择一项 不启用协处理器则选择 None");
//        return false;
//    }
//    // 选了None不能选其他
//    if (mp->find("None") != mp->end() && mp->find("None")->second->GetValue()) {
//        for (auto i : *mp) {
//            if (i.first == "None")
//                continue;
//            if (i.second->GetValue()) {
//                IncorrectSet("选择 None选项则不能选择其他");
//                return false;
//            }
//        }
//    }
//    CorrectSet();
//    return true;
//}
//
///***********************************************************************************
//*   MyPrio
//***********************************************************************************/
////MyPrio::MyPrio(wxWindow* parent, const wxString& title, const wxString& describe, const wxString& errorlc)
////    :SimpleBase(parent, title, describe, errorlc) {
////    text = new wxTextCtrl(this, wxID_ANY);
////    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
////}
////
////bool MyPrio::IsLegal() {
////    wxString str = text->GetValue();
////    wxRegEx regex("^\\d+$");
////    if (!regex.Matches(str)) {
////        IncorrectSet("不是合法的十进制整数");
////        return false;
////    }
////    CorrectSet();
////    return true;
////}
//
///***********************************************************************************
//*   MyVirtPrio
//***********************************************************************************/
//MyVirtPrio::MyVirtPrio(wxWindow* parent, 
//    const wxString& title,
//    const wxString& describe, 
//    const wxString& errorloc,
//    const bool& hide_virt_mes)
//    :SimpleBase(parent, title, describe, errorloc) {
//
//    choice = new wxChoice(this, wxID_ANY);
//    box->Add(choice, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
//
//    text_vm_count = new wxStaticText(this, wxID_ANY, "当前虚拟机数 0");
//    wxFont font = text_vm_count->GetFont();
//    font.SetPointSize(7);
//    text_vm_count->SetFont(font);
//    box->Add(text_vm_count, 0, wxALIGN_RIGHT | wxRIGHT, 15);
//    if (hide_virt_mes)
//        box->Hide(text_vm_count);
//
//    SetSizer(box);
//    Layout();
//}
//
//void MyVirtPrio::FillChoice(wxArrayString arr) {
//    choice->Clear();
//    choice->Append(arr);
//    choice->SetSelection(0);
//}
//
//int MyVirtPrio::FindValue(const wxString& tar) {
//    int size = choice->GetCount();
//    for (int i = 0; i < size; i++) {
//        if (choice->GetString(i) == tar)
//            return i;
//    }
//    //wxMessageBox("未找到预设选项");
//    return 0;
//}
//
//bool MyVirtPrio::IsLegal() {
//    // 合法的十进制数
//    int value = wxAtoi(choice->GetStringSelection());
//    if (value == 0 && Main::rvp->GetVmCount() != 0) {
//        IncorrectSet("在使用虚拟机的情况下 该值不能为0");
//        return false;
//    }
//    CorrectSet();
//    return true;
//}
//
//
///***********************************************************************************
//*   MyVirtEvent
//***********************************************************************************/
////MyVirtEvent::MyVirtEvent(wxWindow* parent, const wxString& title, const wxString& describe, const wxString& errorloc)
////    :SimpleBase(parent, title, describe, errorloc) {
////
////    text = new wxTextCtrl(this, wxID_ANY);
////    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
////
////    SetSizer(box);
////    Layout();
////}
////
////bool MyVirtEvent::IsLegal() {
////    // 合法的十进制数
////    wxString str = text->GetValue();
////    int value = wxAtoi(str);
////    wxRegEx regex("^\\d+$");
////    if (!regex.Matches(str)) {
////        IncorrectSet("不是合法的十进制非负整数");
////        return false;
////    }
////    else {
////        // 最大限制
////        if (value > 1024) {
////            IncorrectSet("事件源的数量不能超过1024的硬性限制");
////            return false;
////        }
////        else if (value == 0 && Main::rvp->GetVmCount() != 0) {
////            IncorrectSet("在使用虚拟机的情况下 该值不能为0");
////            return false;
////        }
////    }
////    CorrectSet();
////    return true;
////}
//
//
///***********************************************************************************
//*   MyVirtMap
//***********************************************************************************/
//MyVirtMap::MyVirtMap(wxWindow* parent, const wxString& title, const wxString& describe, const wxString& errorloc)
//    :SimpleBase(parent, title, describe, errorloc) {
//
//    text = new wxTextCtrl(this, wxID_ANY);
//    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
//
//    SetSizer(box);
//    Layout();
//}
//
//bool MyVirtMap::IsLegal(const int& virt_event) {
//    // 合法的十进制数
//    wxString str = text->GetValue();
//    int value = wxAtoi(str);
//    wxRegEx regex("^\\d+$");
//    if (!regex.Matches(str)) {
//        IncorrectSet("不是合法的十进制非负整数");
//        return false;
//    }
//    else {
//        // 最大限制
//        if (value < virt_event) {
//            IncorrectSet("虚拟机向量映射数必须大于事件源的数量");
//            return false;
//        }
//        else if (value == 0 && Main::rvp->GetVmCount() != 0) {
//            IncorrectSet("在使用虚拟机的情况下 该值不能为0");
//            return false;
//        }
//    }
//    CorrectSet();
//    return true;
//}
//
//
//
///***********************************************************************************
//*   MyVersion
//***********************************************************************************/
//MyVersion::MyVersion(wxWindow* parent, const wxString& title, const wxString& describe, const wxString& errorloc)
//    :SimpleBase(parent, title, describe, errorloc) {
//
//    text = new wxTextCtrl(this, wxID_ANY);
//    box->Add(text, 0, wxLEFT | wxRIGHT | wxEXPAND, 15);
//
//    SetSizer(box);
//    Layout();
//}
//
//bool MyVersion::IsLegal() {
//    // 版本号需要是一个非负数
//    wxString str = text->GetValue();
//    int value = wxAtoi(str);
//    wxRegEx regex("^\\d+(\\.\\d+)?$");
//    if (!regex.Matches(str)) {
//        IncorrectSet("版本号不合法");
//        return false;
//    }
//    CorrectSet();
//    return true;
//}
//
//
///***********************************************************************************
//*   MyParameter
//***********************************************************************************/
//MyParameter::MyParameter(wxWindow* parent, const wxString& title, const wxString& describe, const wxString& _errorloc)
//    :SimpleBase(parent, title, describe, _errorloc) {
//
//    text = new wxTextCtrl(this, wxID_ANY);
//    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
//
//    SetSizer(box);
//    Layout();
//}
//
//bool MyParameter::IsLegal() {
//    // 版本号需要是一个非负数
//    wxString str = text->GetValue();
//    if (str == wxEmptyString) {
//        text->SetValue("0x0");
//        CorrectSet();
//        return true;
//    }
//
//    str = str.Lower();
//    wxRegEx regex("^0x[0-9a-fA-F]+$|^[0-9a-fA-F]+$");
//    if (!regex.Matches(str)) {
//        IncorrectSet("不是合法的十六进制数");
//        return false;
//    }
//    if (str.starts_with("0x"))
//        str = "0x" + str.SubString(2, str.Length() - 1).Upper();
//    else
//        str = "0x" + str.Upper();;
//    SetValue(str);
//    CorrectSet();
//    return true;
//}
//
///***********************************************************************************
//*   MyThreadPrio
//***********************************************************************************/
//MyThreadPrio::MyThreadPrio(wxWindow* parent, const wxString& title, const wxString& describe, const wxString& _errorloc)
//    :SimpleBase(parent, title, describe, _errorloc) {
//
//    text = new wxTextCtrl(this, wxID_ANY);
//    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
//
//    SetSizer(box);
//    Layout();
//}
//
//bool MyThreadPrio::IsLegal(const int& kern_prio) {
//    wxString str = text->GetValue();
//    wxRegEx regex("^\\d+$");
//    if (!regex.Matches(str)) {
//        IncorrectSet("不是合法的十进制非负整数");
//        return false;
//    }
//    int value = wxAtoi(str);
//    if (!(5 <= value && value <= kern_prio - 2)) {
//        IncorrectSet("优先级不在规定范围内");
//        return false;
//    }
//    CorrectSet();
//    return true;
//}
//
///***********************************************************************************
//*   MyVirtualPrio
//***********************************************************************************/
//MyVirtualPrio::MyVirtualPrio(wxWindow* parent, 
//    const wxString& title, 
//    const wxString& describe,
//    const wxString& _errorloc)
//    :SimpleBase(parent, title, describe) {
//
//    text = new wxTextCtrl(this, wxID_ANY);
//    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
//
//    SetSizer(box);
//    Layout();
//}
//
//bool MyVirtualPrio::IsLegal(const int& virt_prio) {
//    wxString str = text->GetValue();
//    wxRegEx regex("^\\d+$");
//    if (!regex.Matches(str)) {
//        IncorrectSet("不是合法的十进制数");
//        return false;
//    }
//    int value = wxAtoi(str);
//    if (!(0 <= value && value <= virt_prio - 1)) {
//        IncorrectSet("优先级不在规定范围内");
//        return false;
//    }
//    CorrectSet();
//    return true;
//}
//
///***********************************************************************************
//*   MyZ_G_Zero
//***********************************************************************************/
//MyZ_G_Zero::MyZ_G_Zero(wxWindow* parent,
//    const wxString& title,
//    const wxString& describe,
//    const wxString& errorloc)
//    :SimpleBase(parent, title, describe, errorloc) {
//
//    text = new wxTextCtrl(this, wxID_ANY);
//    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
//
//    SetSizer(box);
//    Layout();
//}
//
//bool MyZ_G_Zero::IsLegal() {
//    wxString str = text->GetValue();
//    wxRegEx regex("^[1-9]\\d*$");  // 创建正则表达式对象
//    if (!regex.Matches(str)) {
//        IncorrectSet("不是合法的正整数");
//        return false;
//    }
//
//    CorrectSet();
//    return true;
//}
//
///***********************************************************************************
//*   MyZ_GE_Zero
//***********************************************************************************/
//MyZ_GE_Zero::MyZ_GE_Zero(wxWindow* parent,
//    const wxString& title,
//    const wxString& describe,
//    const wxString& errorloc)
//    :SimpleBase(parent, title, describe, errorloc) {
//
//    text = new wxTextCtrl(this, wxID_ANY);
//    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
//
//    SetSizer(box);
//    Layout();
//}
//
//bool MyZ_GE_Zero::IsLegal() {
//    wxString str = text->GetValue();
//    wxRegEx regex("^\\d+$");
//    if (!regex.Matches(str)) {
//        IncorrectSet("不是合法的非负整数");
//        return false;
//    }
//    CorrectSet();
//    return true;
//}
///***********************************************************************************
//*   MyGrid
//***********************************************************************************/
//
////MyGrid::MyGrid(wxWindow* parent, wxWindowID id)
////    :wxGrid(parent, id) {
////
////    CreateGrid(1, 4);
////    SetColSize(0, 30);
////    SetCellValue(0, 1, "Buildsystem");
////    SetCellValue(0, 2, "ToolChain");
////    SetCellValue(0, 3, "Type");
////    SetMargins(0, 0);
////    SetGridLineColour(wxColour(175, 175, 175));
////    EnableDragGridSize(false);
////
////    SetRowLabelSize(2);
////    SetColLabelSize(2);
////
////    Bind(wxEVT_GRID_CELL_CHANGED, &MyGrid::OnCellValueChanged, this);
////
////
////}
//
//MyGrid::MyGrid(wxWindow* parent,
//    const wxString& title,
//    const wxString& describe,
//    const wxString& errorloc)
//    :SimpleBase(parent, title, describe, errorloc) {
//
//    grid = new wxGrid(this, wxID_ANY);
//    grid->CreateGrid(1, 4);
//    grid->SetColSize(0, 30);
//    grid->SetCellValue(0, 1, "Buildsystem");
//    grid->SetCellValue(0, 2, "ToolChain");
//    grid->SetCellValue(0, 3, "Type");
//    grid->SetMargins(0, 0);
//    grid->SetGridLineColour(wxColour(175, 175, 175));
//    grid->EnableDragGridSize(false);
//
//    grid->SetRowLabelSize(2);
//    grid->SetColLabelSize(2);
//
//    box->Add(grid, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
//
//    SetSizer(box);
//    Layout();
//
//    Bind(wxEVT_GRID_CELL_CHANGED, &MyGrid::OnCellValueChanged, this);
//
//
//}
//void MyGrid::AppendOneRows(const wxArrayString& arr) {
//    grid->AppendRows(1);
//    grid->SetCellEditor(max_row, 0, new wxGridCellBoolEditor());
//    grid->SetCellRenderer(max_row, 0, new wxGridCellBoolRenderer());
//    for (int i = 1; i < 4; i++) {
//        grid->SetCellValue(max_row, i, arr[i - 1]);
//    }
//    ++max_row;
//}
//
//void MyGrid::OnCellValueChanged(const wxGridEvent& event){
//    // 获取事件中的行和列
//    int row = event.GetRow();
//    int col = event.GetCol();
//    //wxMessageBox(wxString::Format("message %d %d", row, col));
//    if (grid->GetCellValue(row, col) != wxEmptyString) {
//        for (int i = 1; i < max_row; i++) {
//            if (row == i)
//                continue;
//            grid->SetCellValue(i, 0, wxEmptyString);
//        }
//    }
//    else {
//        // 选中之后不能再选取消
//        grid->SetCellValue(row, col, "1");
//    }
//}
//
//bool MyGrid::IsLegal() {
//    for (int i = 1; i < max_row; i++) {
//        if (grid->GetCellValue(i, 0) != wxEmptyString) {
//            CorrectSet();
//            return true;
//        }
//    }
//    IncorrectSet("至少选择一个合法的组合");
//    return false;
//}
//
//void MyGrid::MatchFirst2nd(const wxArrayString& arr) {
//    for (int row = 1; row < max_row; row++) {
//        if (grid->GetCellValue(row, 1) == arr[0] &&
//            grid->GetCellValue(row, 2) == arr[1] &&
//            grid->GetCellValue(row, 3) == arr[2]) {
//            grid->SetCellValue(row, 0, "1");
//            return;
//        }
//    }
//    wxLogError("未找到匹配项 错误在%s", __func__);
//    return;
//}
//
//wxArrayString MyGrid::GetValue() {
//    wxArrayString ans;
//    for (int row = 1; row < max_row; row++) {
//        if (grid->GetCellValue(row, 0) != wxEmptyString) {
//            ans.push_back(grid->GetCellValue(row, 1));
//            ans.push_back(grid->GetCellValue(row, 2));
//            ans.push_back(grid->GetCellValue(row, 3));
//        }
//    }
//    return ans;
//}
//
//void MyGrid::Clear() {
//    if (max_row >= 2) {
//        grid->DeleteRows(1, max_row - 1);
//        max_row = 1;
//    }
//}
//
///***********************************************************************************
//*   PanelBase
//***********************************************************************************/
//PanelBase::PanelBase(wxWindow* parent, wxWindowID id)
//    :wxPanel(parent, id) {
//    //this->SetBackgroundColour(*wxYELLOW);
//    highbox = new wxBoxSizer(wxVERTICAL);
//    box = new wxBoxSizer(wxVERTICAL);
//
//    swindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
//    swindow->SetScrollRate(10, 10);
//
//    wxPanel* btn_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
//    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
//    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
//    wxButton* btn_preview = new wxButton(btn_panel, wxID_ANY, "预览");
//    wxButton* btn_confirm = new wxButton(btn_panel, wxID_ANY, "确认");
//    // 刷新
//    wxButton* btn_refresh = new wxButton(btn_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(25, 25));
//    wxImage image;
//    if (image.LoadFile("Image\\preview2.bmp")) {
//        btn_refresh->SetBitmap(wxBitmap(image));
//        btn_refresh->SetBitmapPosition(wxLEFT);
//    }
//    hbox->Add(btn_preview, 0, wxALL, 3);
//    hbox->Add(btn_confirm, 0, wxALL, 3);
//    hbox->Add(btn_refresh, 0, wxALL, 3);
//    vbox->Add(hbox, 0, wxALIGN_LEFT);
//    btn_panel->SetSizer(vbox);
//
//    highbox->Add(swindow, 1, wxEXPAND);
//    highbox->Add(btn_panel, 0, wxEXPAND);
//    SetSizer(highbox);
//
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PanelBase::OnPreview, this, btn_preview->GetId());
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PanelBase::OnConfirm, this, btn_confirm->GetId());
//    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PanelBase::OnRefresh, this, btn_refresh->GetId());
//}
