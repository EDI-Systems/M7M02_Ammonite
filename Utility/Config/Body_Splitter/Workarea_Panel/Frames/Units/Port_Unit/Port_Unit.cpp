#include "Port_Unit.h"
#include "../../../../../Func/func.h"

Port_Unit::Port_Unit(
    wxWindow* parent,
    const wxString& _errorloc,
    std::function<void()>func)
    :UnitBase(parent, _errorloc, func) {

    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* stbox = new wxStaticBoxSizer(wxVERTICAL, this, "title");
    //wxStaticText* txt1 = new wxStaticText(this, wxID_ANY, "Name");
    //this->tcr_name = new wxTextCtrl(this, wxID_ANY);    

    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc, func);

    p_process = new SimpleText(this, "Process", wxEmptyString, wxEmptyString, 0L, errorloc, func);
    stbox->Add(base_name, 0, wxEXPAND | wxALL, 5);
    stbox->Add(0, 5);
    stbox->Add(p_process, 0, wxEXPAND | wxALL, 5);

    box->Add(stbox, 0, wxALL | wxEXPAND, 5);
    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
    this->SetSizer(box);
    this->FitInside();
}

void Port_Unit::SetUnit(const std::unique_ptr<Port>& ptr) {
    base_name->SetValue(ptr->Name);
    p_process->SetValue(ptr->Process);
}

//wxXmlNode* Port_Unit::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Process", p_process->GetValue());
//    return root;
//}

bool Port_Unit::IsLegal() {
    bool j1 = base_name->IsLegal();
    bool j2 = p_process->IsLegal();
    return j1 && j2;
}

void Port_Unit::ClearError() {
    base_name->CorrectSet();
    p_process->CorrectSet();
}

wxString Port_Unit::GetName() {
    if (base_name && p_process)
        return base_name->GetValue().Upper() + p_process->GetValue().Upper();
    return "Invalid";
}

std::unique_ptr<Port> Port_Unit::GetUnit() {
    return std::make_unique<Port>(base_name->GetValue(), p_process->GetValue());
}