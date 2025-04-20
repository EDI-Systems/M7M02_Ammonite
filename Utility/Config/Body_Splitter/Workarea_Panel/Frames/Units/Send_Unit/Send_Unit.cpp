#include "Send_Unit.h"
#include "../../../../../Func/func.h"

Send_Unit::Send_Unit(
    wxWindow* parent, 
    const wxString& _errorloc,
    std::function<void()>func)
    :UnitBase(parent, _errorloc, func) {

    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* stbox_send = new wxStaticBoxSizer(wxVERTICAL, this);

    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc, func);

    sd_process = new SimpleText(this, "Process", wxEmptyString, wxEmptyString, 0L, errorloc, func);

    stbox_send->Add(base_name, 0, wxALL | wxEXPAND, 5);
    stbox_send->Add(0, 5);
    stbox_send->Add(sd_process, 0, wxALL | wxEXPAND, 5);


    box->Add(stbox_send, 0, wxALL | wxEXPAND, 5);
    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
    this->SetSizer(box);

    this->FitInside();
}

void Send_Unit::SetUnit(const std::unique_ptr<Send>& ptr) {
    base_name->SetValue(ptr->Name);
    sd_process->SetValue(ptr->Process);
}

//wxXmlNode* Send_Unit::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Process", sd_process->GetValue());
//    return root;
//}

bool Send_Unit::IsLegal() {
    bool j1 = base_name->IsLegal();
    bool j2 = sd_process->IsLegal();
    return j1 && j2;
}

void Send_Unit::ClearError() {
    base_name->CorrectSet();
    sd_process->CorrectSet();
}

wxString Send_Unit::GetName() {
    if (base_name && sd_process)
        return base_name->GetValue().Upper() + sd_process->GetValue().Upper();
    return "Invalid";
}

std::unique_ptr<Send> Send_Unit::GetUnit() {
    return std::make_unique<Send>(base_name->GetValue(), sd_process->GetValue());
}