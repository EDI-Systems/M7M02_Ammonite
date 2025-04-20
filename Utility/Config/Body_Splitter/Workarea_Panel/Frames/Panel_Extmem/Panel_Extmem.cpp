#include "Panel_Extmem.h"
#include "../../../../Func/func.h"
#include ".././../../../main.h"

Panel_Extmem::Panel_Extmem(
    wxWindow* parent,
    wxWindowID id,
    const wxString& _errorloc,
    const bool& _makeGUI,
    std::function<void()>_func)
    :PanelBase(parent, wxID_ANY, _errorloc, _makeGUI, _func) {

    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    extmem_base = new BaseSlider(swindow, "Base", "�ⲿ�ڴ�ε���ʼ��ַ", errorloc, func);
    extmem_size = new SizeSlider(swindow, "Size", "�ⲿ�ڴ�εĳ���", errorloc, func);

    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    extmem_type = new MyChoice(swindow, "Type", wxEmptyString, errorloc);
    extmem_type->FillChoice(wxArrayString(3, new wxString[3]{ "Code","Data","Device" }));
    extmem_attribute = new MyAttribute(swindow, wxID_ANY, "Attribute", "�ڴ�ε�����", errorloc, func);
    hbox1->Add(extmem_attribute, 1, wxALL, 5);
    hbox1->Add(extmem_type, 1, wxALL, 5);


    box->Add(extmem_size, 0, wxALL | wxEXPAND, 10);
    box->Add(extmem_base, 0, wxALL | wxEXPAND, 10);
    box->Add(hbox1, 1, wxEXPAND | wxALL, 5);

    swindow->SetSizer(box);
    swindow->FitInside();
}


void Panel_Extmem::SetExtmem() {
    // û�пɹ���ȡ������
    makeGUI = true;

}


bool Panel_Extmem::SaveExtmem() {
    // �ȼ��
    if (!IsLegal()) {
        wxMessageBox("����");
        return false;
    }

    Main::proj_info->rvp->extmem->Base = extmem_base->GetValue();
    Main::proj_info->rvp->extmem->Size = extmem_size->GetValue();
    Main::proj_info->rvp->extmem->Type = extmem_type->GetStringSelection();
    Main::proj_info->rvp->extmem->Attribute = extmem_attribute->GetValue();
    return true;
}

void Panel_Extmem::OnPreview(const wxCommandEvent& event) {
    myfunc::PreviewHelper(Main::proj_info->rvp->extmem->toString(), "Դ��Ԥ�� �����ڴ�����");
}

bool Panel_Extmem::IsLegal() {
    if (!makeGUI) {
        SetExtmem();
    }
    bool j1 = extmem_base->IsLegal();
    bool j2 = extmem_size->IsLegal();
    bool j3 = extmem_type->IsLegal();
    bool j4 = extmem_attribute->IsLegal();
    return j1 && j2 && j3 && j4;
}

void Panel_Extmem::ClearError() {
    extmem_base->CorrectSet();
    extmem_size->CorrectSet();
    extmem_type->CorrectSet();
    extmem_attribute->CorrectSet();
}

void Panel_Extmem::OnConfirm(const wxCommandEvent& event) {
    SaveExtmem();
}

void Panel_Extmem::OnRefresh(const wxCommandEvent& event) {
    SetExtmem();
}