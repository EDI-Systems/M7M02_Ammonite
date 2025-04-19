#include "Invocation_Unit.h"
#include "../../../../../Func/func.h"

Invocation_Unit::Invocation_Unit(
    wxWindow* parent,
    const wxString& _errorloc,
    std::function<void()>func)
    :UnitBase(parent, _errorloc, func) {

    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* stbox = new wxStaticBoxSizer(wxVERTICAL, this, "title");
    //wxStaticText* txt1 = new wxStaticText(this, wxID_ANY, "Name");
    //this->tcr_name = new wxTextCtrl(this, wxID_ANY);

    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc, func);

    invo_stack_size = new SizeSlider(this, "Stack_Size", wxEmptyString, errorloc, func);

    stbox->Add(base_name, 0, wxEXPAND | wxALL, 5);
    stbox->Add(0, 5);
    stbox->Add(invo_stack_size, 0, wxEXPAND | wxALL, 5);

    box->Add(stbox, 0, wxALL | wxEXPAND, 5);
    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
    this->SetSizer(box);
    this->FitInside();
}

void Invocation_Unit::SetUnit(const std::unique_ptr<Invocation>& ptr) {
    base_name->SetValue(ptr->Name);
    invo_stack_size->SetValue(ptr->Stack_Size);
}

//wxXmlNode* Invocation_Unit::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Stack_Size", in_stack_size->GetValue());
//    return root;
//}

std::unique_ptr<Invocation> Invocation_Unit::GetUnit() {
    return std::make_unique<Invocation>(base_name->GetValue(), invo_stack_size->GetValue());
}

bool Invocation_Unit::IsLegal() {
    bool j1 = base_name->IsLegal();
    bool j2 = invo_stack_size->IsLegal();
    return j1 && j2;
}

void Invocation_Unit::ClearError() {
    base_name->CorrectSet();
    invo_stack_size->CorrectSet();
}

