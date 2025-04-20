#include "Panel_Chip.h"
#include "../../../../Func/func.h"
#include ".././../../../main.h"

Panel_Chip::Panel_Chip(
    wxWindow* parent,
    wxWindowID id,
    const wxString& _errorloc,
    const bool& _makeGUI,
    std::function<void()>_func)
    :PanelBase(parent, wxID_ANY, _errorloc, _makeGUI, _func) {

    // �����ؼ�
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    platform = new SimpleText(swindow, "Platform", "���̵ļܹ�ƽ̨", wxEmptyString, wxTE_READONLY, errorloc, func);
    chip_class = new SimpleText(swindow, "Class", "�����оƬ���", wxEmptyString, wxTE_READONLY, errorloc, func);
    hbox1->Add(platform, 1, wxALL, 5);
    hbox1->Add(chip_class, 1, wxALL, 5);

    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    name = new MyChoice(swindow, "Name", "оƬ��ȫ��");
    coprocessor = new MyCoprocessor(swindow, "Coprocessor", "��Ŀ����Ҫ���õ�Э������", errorloc, func);
    hbox2->Add(coprocessor, 1, wxALL, 5);
    hbox2->Add(name, 1, wxALL, 5);

    stbox_config = new wxStaticBoxSizer(wxVERTICAL, swindow, "Config");

    box->Add(hbox1, 0, wxEXPAND);
    box->Add(hbox2, 0, wxEXPAND);
    box->Add(stbox_config, 0, wxALL | wxEXPAND, 15);

    swindow->SetSizer(box);
    swindow->FitInside();
}

void Panel_Chip::SetChip() {
    stbox_config->Clear(true);
    configs.clear();

    if (Main::proj_info->rva->name != Main::proj_info->rvp->chip->platform &&
        Main::proj_info->rvp->chip->platform != wxEmptyString) {
        wxLogError("Name���� rva:%s chip:%s ��rva�ļ�Ϊ׼ ������%s",
            Main::proj_info->rva->name, Main::proj_info->rvp->chip->platform, __func__);
    }
    if (Main::proj_info->rvc->name != Main::proj_info->rvp->chip->chip_class &&
        Main::proj_info->rvp->chip->chip_class != wxEmptyString) {
        wxLogError("Class���� rvc%s chip:%s ��rvc�ļ�Ϊ׼ ������%s",
            Main::proj_info->rvc->name, Main::proj_info->rvp->chip->chip_class, __func__);
    }

    platform->SetValue(Main::proj_info->rva->name);
    chip_class->SetValue(Main::proj_info->rvc->name);
   
    name->FillChoice(Main::proj_info->rvc->compatible);
    name->SetSelection(name->FindValue(Main::proj_info->rvp->chip->name));

    coprocessor->FillAllItem(Main::proj_info->rvc->coprocessor);    // �ɹ�ѡ���Э������
    coprocessor->SetValue(myfunc::Separater(Main::proj_info->rvp->chip->coprocessor, ",")); // ���ļ��е�Э������

    // ����RVA��Ϣ������config�ı�����
    for (int i = 0; i < Main::proj_info->rva->configs.size(); i++) {
        wxString item_name = Main::proj_info->rva->configs[i]->name;

        // ���ñ�����
        NotEmpty* config_item = new NotEmpty(swindow, item_name, "rva�м�¼�ı�����", errorloc, func);
        stbox_config->Add(config_item, 0, wxALL | wxEXPAND, 5);
        configs.insert(std::make_pair(item_name, config_item));

        // ���������
        // ����rva����ѡ�����rvp��д�����½����ǵ���ʱ��rvp��Ȼ�ǿյ�
        if (Main::proj_info->rvp->chip->config.find(item_name) == Main::proj_info->rvp->chip->config.end()) {
            Main::proj_info->rvp->chip->config.insert(std::make_pair(item_name, wxEmptyString));
            wxMessageBox(wxString::Format("�ѽ�%s��ӵ�proj_info�� ������Ϊ��", item_name));
        }
        config_item->SetValue(Main::proj_info->rvp->chip->config.find(item_name)->second);
    }
   
    swindow->FitInside();
    makeGUI = true;
}


bool Panel_Chip::SaveChip() {
    // �ȼ��
    if (!IsLegal()) {
        wxMessageBox("����");
        return false;
    }
    Main::proj_info->rvp->chip->platform = platform->GetValue();
    Main::proj_info->rvp->chip->chip_class = chip_class->GetValue();
    Main::proj_info->rvp->chip->name = name->GetStringSelection();
    Main::proj_info->rvp->chip->coprocessor = coprocessor->GetValue();

    for (int i = 0; i < Main::proj_info->rva->configs.size(); i++) {
        wxString item_name = Main::proj_info->rva->configs[i]->name;
        std::map<wxString, wxString>::iterator iter = Main::proj_info->rvp->chip->config.find(item_name);
        if (iter == Main::proj_info->rvp->chip->config.end() ||
            this->configs.find(item_name) == this->configs.end()) {
            wxLogError("%s������� ������%s", item_name, __func__);
            break;
        }
        iter->second = configs.find(item_name)->second->GetValue();
    }
    return true;

}

void Panel_Chip::OnPreview(const wxCommandEvent& event) {
    myfunc::PreviewHelper(Main::proj_info->rvp->chip->toString(), "Դ��Ԥ�� оƬ����");
}

bool Panel_Chip::IsLegal() {
    if (!makeGUI) {
        SetChip();
    }

    bool j1 = coprocessor->IsLegal();
    bool j2 = chip_class->IsLegal();
    bool j3 = platform->IsLegal();
    bool j4 = true;
    for (auto i : configs) {
        bool temp = i.second->IsLegal();
        j4 = j4 && temp;
    }

    // 
    return j1 && j2 && j3 && j4;
}

void Panel_Chip::ClearError() {
    coprocessor->CorrectSet();
    chip_class->CorrectSet();
    platform->CorrectSet();
}

void Panel_Chip::OnConfirm(const wxCommandEvent& event) {
    SaveChip();
}


void Panel_Chip::OnRefresh(const wxCommandEvent& event) {
    SetChip();
}