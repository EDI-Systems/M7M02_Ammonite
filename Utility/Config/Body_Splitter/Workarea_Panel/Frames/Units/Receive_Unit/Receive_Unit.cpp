#include "Receive_Unit.h"
#include "../../../../../Func/func.h"
Receive_Unit::Receive_Unit(
    wxWindow* parent,
    const wxString& _errorloc,
    std::function<void()>func)
    :UnitBase(parent, _errorloc, func) {
    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* stbox = new wxStaticBoxSizer(wxVERTICAL, this, "title");
    //wxStaticText* txt1 = new wxStaticText(this, wxID_ANY, "Name");
    //this->tcr_name = new wxTextCtrl(this, wxID_ANY);

    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc, func);;

    stbox->Add(base_name, 0, wxEXPAND | wxALL, 5);

    box->Add(stbox, 0, wxALL | wxEXPAND, 5);
    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
    this->SetSizer(box);
    this->FitInside();
}

void Receive_Unit::SetUnit(const std::unique_ptr<Receive>& ptr) {
    this->base_name->SetValue(ptr->Name);
}

//wxXmlNode* Receive_Unit::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    return root;
//}

bool Receive_Unit::IsLegal() {
    bool j1 = base_name->IsLegal();
    return j1;
}

void Receive_Unit::ClearError() {
    base_name->CorrectSet();
}

std::unique_ptr<Receive> Receive_Unit::GetUnit() {
    return std::make_unique<Receive>(base_name->GetValue());
}