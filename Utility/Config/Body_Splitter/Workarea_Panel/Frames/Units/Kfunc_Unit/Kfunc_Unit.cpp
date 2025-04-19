#include "Kfunc_Unit.h"
#include "../../../../../Func/func.h"
Kfunc_Unit::Kfunc_Unit(
    wxWindow* parent, 
    const wxString& _errorloc,
    std::function<void()>func)
    :UnitBase(parent, _errorloc, func) {

    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* stbox_kfunc = new wxStaticBoxSizer(wxVERTICAL, this, "title");
    //wxStaticText* txt_name = new wxStaticText(this, wxID_ANY, "Name");
    //tcr_name = new wxTextCtrl(this, wxID_ANY);

    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc, func);

    k_begin = new BaseSlider(this, "Begin", wxEmptyString, errorloc, func);

    k_end = new BaseSlider(this, "End", wxEmptyString, errorloc, func,
        true, "该值应该大于等于Begin");

    stbox_kfunc->Add(base_name, 0, wxALL | wxEXPAND, 5);
    stbox_kfunc->Add(0, 5);
    stbox_kfunc->Add(k_begin, 0, wxALL | wxEXPAND, 5);
    stbox_kfunc->Add(0, 5);
    stbox_kfunc->Add(k_end, 0, wxALL | wxEXPAND, 5);



    box->Add(stbox_kfunc, 0, wxALL | wxEXPAND, 5);
    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);
    SetSizer(box);
    this->FitInside();
}

void Kfunc_Unit::SetUnit(const std::unique_ptr<Kfunc>& ptr) {

    base_name->SetValue(ptr->Name);
    k_begin->SetValue(ptr->Begin);
    k_end->SetValue(ptr->End);

}
//
//wxXmlNode* Kfunc_Unit::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Begin", k_begin->GetValue());
//    myfunc::SetNodeCont(root, "End", k_end->GetValue());
//    return root;
//}

bool Kfunc_Unit::IsLegal() {
    bool j1 = base_name->IsLegal();
    bool j2 = k_begin->IsLegal();
    bool j3 = k_end->IsLegal();
    bool j4 = myfunc::GEhex(k_begin->GetValue(), k_end->GetValue());
    if (!j4)
        k_end->IncorrectSet("增大结束值 该值不能比起始值低");
    else
        k_end->CorrectSet();
    return j1 && j2 && j3 && j4;
}

void Kfunc_Unit::ClearError() {
    base_name->CorrectSet();
    k_begin->CorrectSet();
    k_end->CorrectSet();
}

std::unique_ptr<Kfunc>Kfunc_Unit::GetUnit() {
    return std::make_unique<Kfunc>(base_name->GetValue(), k_begin->GetValue(), k_end->GetValue());
}