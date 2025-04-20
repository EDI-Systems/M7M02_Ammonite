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

    // 创建控件
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    platform = new SimpleText(swindow, "Platform", "工程的架构平台", wxEmptyString, wxTE_READONLY, errorloc, func);
    chip_class = new SimpleText(swindow, "Class", "具体的芯片类别", wxEmptyString, wxTE_READONLY, errorloc, func);
    hbox1->Add(platform, 1, wxALL, 5);
    hbox1->Add(chip_class, 1, wxALL, 5);

    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    name = new MyChoice(swindow, "Name", "芯片的全称");
    coprocessor = new MyCoprocessor(swindow, "Coprocessor", "项目中需要启用的协处理器", errorloc, func);
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
        wxLogError("Name不符 rva:%s chip:%s 以rva文件为准 错误在%s",
            Main::proj_info->rva->name, Main::proj_info->rvp->chip->platform, __func__);
    }
    if (Main::proj_info->rvc->name != Main::proj_info->rvp->chip->chip_class &&
        Main::proj_info->rvp->chip->chip_class != wxEmptyString) {
        wxLogError("Class不符 rvc%s chip:%s 以rvc文件为准 错误在%s",
            Main::proj_info->rvc->name, Main::proj_info->rvp->chip->chip_class, __func__);
    }

    platform->SetValue(Main::proj_info->rva->name);
    chip_class->SetValue(Main::proj_info->rvc->name);
   
    name->FillChoice(Main::proj_info->rvc->compatible);
    name->SetSelection(name->FindValue(Main::proj_info->rvp->chip->name));

    coprocessor->FillAllItem(Main::proj_info->rvc->coprocessor);    // 可供选择的协处理器
    coprocessor->SetValue(myfunc::Separater(Main::proj_info->rvp->chip->coprocessor, ",")); // 打开文件中的协处理器

    // 根据RVA信息，设置config的必填项
    for (int i = 0; i < Main::proj_info->rva->configs.size(); i++) {
        wxString item_name = Main::proj_info->rva->configs[i]->name;

        // 设置必填项
        NotEmpty* config_item = new NotEmpty(swindow, item_name, "rva中记录的必填项", errorloc, func);
        stbox_config->Add(config_item, 0, wxALL | wxEXPAND, 5);
        configs.insert(std::make_pair(item_name, config_item));

        // 读入必填项
        // 根据rva设置选项，根据rvp填写，当新建而非导入时，rvp自然是空的
        if (Main::proj_info->rvp->chip->config.find(item_name) == Main::proj_info->rvp->chip->config.end()) {
            Main::proj_info->rvp->chip->config.insert(std::make_pair(item_name, wxEmptyString));
            wxMessageBox(wxString::Format("已将%s添加到proj_info中 其内容为空", item_name));
        }
        config_item->SetValue(Main::proj_info->rvp->chip->config.find(item_name)->second);
    }
   
    swindow->FitInside();
    makeGUI = true;
}


bool Panel_Chip::SaveChip() {
    // 先检查
    if (!IsLegal()) {
        wxMessageBox("错误");
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
            wxLogError("%s保存错误 错误在%s", item_name, __func__);
            break;
        }
        iter->second = configs.find(item_name)->second->GetValue();
    }
    return true;

}

void Panel_Chip::OnPreview(const wxCommandEvent& event) {
    myfunc::PreviewHelper(Main::proj_info->rvp->chip->toString(), "源码预览 芯片配置");
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