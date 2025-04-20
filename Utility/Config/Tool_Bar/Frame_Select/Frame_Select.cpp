#include "Frame_Select.h"
#include "../../Func/func.h"
#include "../../main.h"

Frame_Select::Frame_Select(wxWindow* parent)
    :wxFrame(parent, wxID_ANY, _("平台选择"), wxDefaultPosition, wxSize(400, 200),
        wxCAPTION | wxCLOSE_BOX | wxMINIMIZE_BOX) {
    select_done = false;

    this->SetBackgroundColour(wxColor(240, 240, 240));
    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);

    btn_select = new wxButton(this, wxID_ANY, "选择Platform文件夹");

    wxStaticBoxSizer* stbox_pac = new wxStaticBoxSizer(wxHORIZONTAL, this, "平台和芯片");
    this->choicer_platform = new wxChoice(this, wxID_ANY);
    this->choicer_chip = new wxChoice(this, wxID_ANY);
    stbox_pac->Add(new wxStaticText(this, wxID_ANY, "平台"), 0, wxLEFT | wxALIGN_CENTRE_VERTICAL, 5);
    stbox_pac->Add(choicer_platform, 2, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
    stbox_pac->Add(10, 0);
    stbox_pac->Add(new wxStaticText(this, wxID_ANY, "芯片"), 0, wxLEFT | wxALIGN_CENTRE_VERTICAL, 5);
    stbox_pac->Add(choicer_chip, 2, wxALL | wxALIGN_CENTRE_VERTICAL, 5);

    wxButton* btn_confirm = new wxButton(this, wxID_ANY, "确认");

    box->Add(btn_select, 0, wxEXPAND | wxALL, 10);
    box->Add(stbox_pac, 0, wxEXPAND | wxALL, 10);
    box->Add(btn_confirm, 0, wxRIGHT | wxALIGN_RIGHT, 10);
    SetSizer(box);

    btn_select->Bind(wxEVT_BUTTON, &Frame_Select::OnSelectPlatformFolder, this);
    btn_confirm->Bind(wxEVT_BUTTON, &Frame_Select::OnOk, this);
    choicer_platform->Bind(wxEVT_CHOICE, &Frame_Select::OnSelectPlatform, this);
    Bind(wxEVT_CLOSE_WINDOW, &Frame_Select::OnClose, this);

}

void Frame_Select::OnSelectPlatformFolder(const wxCommandEvent& event) {
    wxDirDialog dirDialog(nullptr, _("选择文件夹"), "", wxDD_DEFAULT_STYLE);
    switch (dirDialog.ShowModal()) {
    case wxID_OK:
        // 目录可以打开 且命名符合要求
        if (dirDialog.GetPath().ends_with("Platform")) {
            // 记录路径
            this->dir_path = dirDialog.GetPath();
        }
        else {
            wxMessageBox("确保文件夹命名为Platform，且文件结构正确", "错误", wxICON_ERROR);
            //wxLogError("确保文件夹命名为Platform且文件结构正确 错误在%s", __func__);
            return;
        }
        break;
    case wxID_CANCEL:
        wxMessageBox("操作取消");
        return;
        break;
    default:
        wxMessageBox("其他");
        return;
        break;
    }

    SetArray(dir_path, this->platforms);
    SetChoicer(this->choicer_platform, this->platforms);

    // 填充chip选项
    SetArray(dir_path + "\\" + choicer_platform->GetStringSelection() + "\\Chip", this->chips);
    SetChoicer(this->choicer_chip, this->chips);
    // 选择合法的platform文件夹后使之失效
    select_done = true;
}


void Frame_Select::SetArray(const wxString& path, wxArrayString& arr) {
    arr.clear();
    wxDir dir(path);
    if (!dir.IsOpened()) {
        wxMessageBox(wxString::Format("无法打开目录:%s", path), "错误", wxICON_ERROR);
        //wxLogError("无法打开目录:%s 错误在%s", path, __func__);
        return;
    }
    wxString filename;
    bool existed = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_DIRS | wxDIR_HIDDEN);
    while (existed) {
        arr.Add(filename);
        existed = dir.GetNext(&filename);
    }
    return;
}


void Frame_Select::SetChoicer(wxChoice* choicer, const wxArrayString& arr, int default_id) {
    choicer->Clear();
    for (auto i : arr)
        choicer->Append(i);
    choicer->SetSelection(default_id);
}

void Frame_Select::OnSelectPlatform(const wxCommandEvent& event) {
    // 填充chip
    SetArray(dir_path + "\\" + choicer_platform->GetStringSelection() + "\\Chip", this->chips);
    SetChoicer(this->choicer_chip, this->chips);
}


void Frame_Select::OnOk(const wxCommandEvent& event) {
      //检查platform选项是否合法
    Main::proj_info->rva->name = this->choicer_platform->GetStringSelection();
    Main::proj_info->rva->loc = this->dir_path + "\\" + this->choicer_platform->GetStringSelection();

    if (myfunc::GetCompletePath(Main::proj_info->rva->loc, ".rva") == "") {
        wxMessageBox(
            wxString::Format("%s文件夹下的平台描述文件不存在或不唯一", Main::proj_info->rva->name),
            "错误", wxICON_ERROR);
        //wxLogError("%s文件夹下的平台描述文件不存在或不唯一 错误在%s", Main::proj_info->rva->name, __func__);
        return;
    }
    // 合法

    // 读取rva文件
    Main::proj_info->rva->SetRVA(myfunc::GetCompletePath(Main::proj_info->rva->loc, ".rva"));
    Main::proj_info->rvc->name = this->choicer_chip->GetStringSelection();
    Main::proj_info->rvc->loc = Main::proj_info->rva->loc + "\\Chip\\" + this->choicer_chip->GetStringSelection();

    if (myfunc::GetCompletePath(Main::proj_info->rvc->loc, ".rvc") == "") {
        wxMessageBox(wxString::Format("%s文件夹下的芯片描述文件不存在或不唯一", Main::proj_info->rvc->name),
            "错误", wxICON_ERROR);
        //wxLogError("%s文件夹下的芯片描述文件不存在或不唯一 错误在%s", Main::proj_info->rvc->name, __func__);
        return;
    }

    // 读取rvc文件
    Main::proj_info->platform_path = dir_path;
    Main::proj_info->rvc->SetRVC(myfunc::GetCompletePath(Main::proj_info->rvc->loc, ".rvc"));

    Main::bottom_message->SetBottomMessage(Main::proj_info->rva->name + "\t" + Main::proj_info->rvc->name, *wxGREEN);

    btn_select->Enable(false);
    Main::tool_bar->EnableRvaPreview();
    Main::tool_bar->EnableRvcPreview();
    Main::menu_bar->EnableFileMenu();
    Main::body_splitter->file_tree->ExpandAll();

    // 重置所有wxTreeItemData的属性
    // 因为需要所有的页面重新设置
    Main::body_splitter->file_tree->InitItem();

    // 重置Chip中的信息
    Main::proj_info->rvp->chip->InitChip();
    this->Hide();
}

void Frame_Select::OnClose(const wxCloseEvent& event) {
    this->Hide();
}