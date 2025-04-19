#include "Vector_Unit.h"
#include "../../../../../Func/func.h"
Vector_Unit::Vector_Unit(
    wxWindow* parent, 
    const wxString& _errorloc,
    std::function<void()>func)
    :UnitBase(parent, _errorloc, func) {

    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* stbox = new wxStaticBoxSizer(wxVERTICAL, this);
    //wxStaticText* txt1 = new wxStaticText(this, wxID_ANY, "Name");
    //tcr_name = new wxTextCtrl(this, wxID_ANY);
    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc, func);

    vec_number = new MyZ_GE_Zero(this, "Number", wxEmptyString, errorloc, func,
        true, "该向量号必须与芯片描述文件中声明的向量号一致");

    stbox->Add(base_name, 0, wxEXPAND | wxALL, 5);
    stbox->Add(0, 5);
    stbox->Add(vec_number, 0, wxEXPAND | wxALL, 5);

    box->Add(stbox, 0, wxALL | wxEXPAND, 5);
    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
    this->SetSizer(box);
    this->FitInside();
}

void Vector_Unit::SetUnit(const std::unique_ptr<Vector>& ptr) {
    base_name->SetValue(ptr->Name);
    vec_number->SetValue(ptr->Number);
}

std::unique_ptr<Vector> Vector_Unit::GetUnit() {
    return std::make_unique<Vector>(base_name->GetValue(), vec_number->GetValue());
}

bool Vector_Unit::IsLegal() {
    bool j1 = base_name->IsLegal();
    bool j2 = vec_number->IsLegal();
    return j1 && j2;
}
