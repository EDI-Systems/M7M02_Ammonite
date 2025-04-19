#include "Shmem_Unit.h"
#include "../../../../../main.h"
#include "../../../../../Func/func.h"
Shmem_Unit::Shmem_Unit(
    wxWindow* parent, 
    const wxString& _errorloc,
    std::function<void()>func)
    :UnitBase(parent, _errorloc, func) {
    //errorloc = _errorlc;

    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* stbox_shmem = new wxStaticBoxSizer(wxVERTICAL, this);

    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc, func);

    sh_attribute = new MyAttribute(this, wxID_ANY, "Attribute", wxEmptyString, errorloc, func);

    stbox_shmem->Add(base_name, 0, wxALL | wxEXPAND, 5);
    stbox_shmem->Add(0, 5);
    stbox_shmem->Add(sh_attribute, 0, wxALL | wxEXPAND, 5);

    box->Add(stbox_shmem, 0, wxALL | wxEXPAND, 5);
    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
    this->SetSizer(box);
    this->FitInside();
}


void Shmem_Unit::SetUnit(const std::unique_ptr<Shmem>& ptr) {
    base_name->SetValue(ptr->Name);
    sh_attribute->SetValue(ptr->Attribute);
}

//wxXmlNode* Shmem_Unit::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Attribute", sh_attribute->GetValue());
//    return root;
//}

bool Shmem_Unit::IsLegal() {
    bool j1 = base_name->IsLegal();
    bool j2 = sh_attribute->IsLegal();
    return j1 && j2;
}

void Shmem_Unit::ClearError() {
    base_name->CorrectSet();
    sh_attribute->CorrectSet();
}


std::unique_ptr<Shmem> Shmem_Unit::GetUnit() {
    return std::make_unique<Shmem>(base_name->GetValue(), sh_attribute->GetValue());
}