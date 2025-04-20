#include "Mem_Unit.h"
#include "../../../../../Func/func.h"
#include "../../../../../main.h"

Mem_Unit::Mem_Unit(
    wxWindow* parent,
    const wxString& _errorloc,
    std::function<void()>func)
    :UnitBase(parent, _errorloc, func) {
    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);


    mem_base = new BaseSlider(this, "Base", wxEmptyString, errorloc, func);
    mem_size = new SizeSlider(this, "Size", wxEmptyString, errorloc, func);
    box->Add(mem_base, 0, wxALL | wxEXPAND, 5);
    box->Add(mem_size, 0, wxALL | wxEXPAND, 5);


    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    //wxStaticBoxSizer* other = new wxStaticBoxSizer(wxVERTICAL, this);
    base_name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc, func);
    mem_align = new MyAlign(this, "Align", wxEmptyString, errorloc, func,
        true, "当段基址是固定值时，该配置项必须填写 Auto");
    hbox1->Add(base_name, 1, wxALL, 5);
    hbox1->Add(mem_align, 1, wxALL, 5);


    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    mem_type = new MyChoice(this, "Type");
    mem_type->FillChoice(wxArrayString(3, new wxString[3]{ "Code","Data","Device" }));
    mem_attribute = new MyAttribute(this, wxID_ANY, "Attribute", wxEmptyString, errorloc, func);
    hbox2->Add(mem_type, 1, wxALL, 5);
    hbox2->Add(mem_attribute, 1, wxALL, 5);

    //other->Add(mem_align, 0, wxALL | wxEXPAND, 5);
    //other->Add(0, 5);
    //other->Add(mem_type, 0, wxALL | wxEXPAND, 5);
    //other->Add(0, 5);
    //other->Add(mem_attribute, 0, wxALL | wxEXPAND, 5);
    
    box->Add(hbox1, 0, wxEXPAND);
    box->Add(hbox2, 0, wxEXPAND);
    box->Add(delete_unit, 0, wxALL | wxALIGN_RIGHT, 5);

    this->SetSizer(box);
    this->FitInside();
}

void Mem_Unit::SetUnit(const std::unique_ptr<Memory>& ptr) {
    base_name->SetValue(ptr->Name);
    mem_base->SetValue(ptr->Base);
    mem_size->SetValue(ptr->Size);
    mem_align->SetValue(ptr->Align);
    mem_type->SetSelection(mem_type->FindValue(ptr->Type));
    mem_attribute->SetValue(ptr->Attribute);
}

std::unique_ptr<Memory> Mem_Unit::GetUnit() {
    return std::make_unique<Memory>(base_name->GetValue(), mem_base->GetValue(), mem_size->GetValue(),
        mem_align->GetValue(), mem_type->GetStringSelection(), mem_attribute->GetValue());
}

//wxXmlNode* Mem_Unit::MakeXmlUnit(const wxString& name) {
//    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//    myfunc::SetNodeCont(root, "Name", base_name->GetValue());
//    myfunc::SetNodeCont(root, "Base", mem_base->GetValue());
//    myfunc::SetNodeCont(root, "Size", mem_size->GetValue());
//    myfunc::SetNodeCont(root, "Align", mem_align->GetValue());
//    myfunc::SetNodeCont(root, "Type", mem_type->GetStringSelection());
//    myfunc::SetNodeCont(root, "Attribute", mem_attribute->GetValue());
//    return root;
//}

bool Mem_Unit::IsLegal() {
    bool j1 = base_name->IsLegal();
    bool j2 = mem_base->IsLegal();
    bool j3 = mem_size->IsLegal();
    bool j4 = mem_align->IsLegal(mem_base->GetValue());
    bool j5 = mem_attribute->IsLegal();
    
    Main::log_panel2->RefreshErrorLog();

    return j1 && j2 && j3 && j4 && j5;
}

void Mem_Unit::ClearError() {
    base_name->CorrectSet();
    mem_base->CorrectSet();
    mem_size->CorrectSet();
    mem_align->CorrectSet();
    mem_attribute->CorrectSet();
}

