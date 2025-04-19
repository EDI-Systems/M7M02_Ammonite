#include "Frame_Select.h"
#include "../../Func/func.h"
#include "../../main.h"

Frame_Select::Frame_Select(wxWindow* parent)
    :wxFrame(parent, wxID_ANY, _("ƽ̨ѡ��"), wxDefaultPosition, wxSize(400, 200),
        wxCAPTION | wxCLOSE_BOX | wxMINIMIZE_BOX) {
    select_done = false;

    this->SetBackgroundColour(wxColor(240, 240, 240));
    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);

    btn_select = new wxButton(this, wxID_ANY, "ѡ��Platform�ļ���");

    wxStaticBoxSizer* stbox_pac = new wxStaticBoxSizer(wxHORIZONTAL, this, "ƽ̨��оƬ");
    this->choicer_platform = new wxChoice(this, wxID_ANY);
    this->choicer_chip = new wxChoice(this, wxID_ANY);
    stbox_pac->Add(new wxStaticText(this, wxID_ANY, "ƽ̨"), 0, wxLEFT | wxALIGN_CENTRE_VERTICAL, 5);
    stbox_pac->Add(choicer_platform, 2, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
    stbox_pac->Add(10, 0);
    stbox_pac->Add(new wxStaticText(this, wxID_ANY, "оƬ"), 0, wxLEFT | wxALIGN_CENTRE_VERTICAL, 5);
    stbox_pac->Add(choicer_chip, 2, wxALL | wxALIGN_CENTRE_VERTICAL, 5);

    wxButton* btn_confirm = new wxButton(this, wxID_ANY, "ȷ��");

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
    wxDirDialog dirDialog(nullptr, _("ѡ���ļ���"), "", wxDD_DEFAULT_STYLE);
    switch (dirDialog.ShowModal()) {
    case wxID_OK:
        // Ŀ¼���Դ� ����������Ҫ��
        if (dirDialog.GetPath().ends_with("Platform")) {
            // ��¼·��
            this->dir_path = dirDialog.GetPath();
        }
        else {
            wxMessageBox("ȷ���ļ�������ΪPlatform�����ļ��ṹ��ȷ", "����", wxICON_ERROR);
            //wxLogError("ȷ���ļ�������ΪPlatform���ļ��ṹ��ȷ ������%s", __func__);
            return;
        }
        break;
    case wxID_CANCEL:
        wxMessageBox("����ȡ��");
        return;
        break;
    default:
        wxMessageBox("����");
        return;
        break;
    }

    SetArray(dir_path, this->platforms);
    SetChoicer(this->choicer_platform, this->platforms);

    // ���chipѡ��
    SetArray(dir_path + "\\" + choicer_platform->GetStringSelection() + "\\Chip", this->chips);
    SetChoicer(this->choicer_chip, this->chips);
    // ѡ��Ϸ���platform�ļ��к�ʹ֮ʧЧ
    select_done = true;
}


void Frame_Select::SetArray(const wxString& path, wxArrayString& arr) {
    arr.clear();
    wxDir dir(path);
    if (!dir.IsOpened()) {
        wxMessageBox(wxString::Format("�޷���Ŀ¼:%s", path), "����", wxICON_ERROR);
        //wxLogError("�޷���Ŀ¼:%s ������%s", path, __func__);
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
    // ���chip
    SetArray(dir_path + "\\" + choicer_platform->GetStringSelection() + "\\Chip", this->chips);
    SetChoicer(this->choicer_chip, this->chips);
}


void Frame_Select::OnOk(const wxCommandEvent& event) {
      //���platformѡ���Ƿ�Ϸ�
    Main::proj_info->rva->name = this->choicer_platform->GetStringSelection();
    Main::proj_info->rva->loc = this->dir_path + "\\" + this->choicer_platform->GetStringSelection();

    if (myfunc::GetCompletePath(Main::proj_info->rva->loc, ".rva") == "") {
        wxMessageBox(
            wxString::Format("%s�ļ����µ�ƽ̨�����ļ������ڻ�Ψһ", Main::proj_info->rva->name),
            "����", wxICON_ERROR);
        //wxLogError("%s�ļ����µ�ƽ̨�����ļ������ڻ�Ψһ ������%s", Main::proj_info->rva->name, __func__);
        return;
    }
    // �Ϸ�

    // ��ȡrva�ļ�
    Main::proj_info->rva->SetRVA(myfunc::GetCompletePath(Main::proj_info->rva->loc, ".rva"));
    Main::proj_info->rvc->name = this->choicer_chip->GetStringSelection();
    Main::proj_info->rvc->loc = Main::proj_info->rva->loc + "\\Chip\\" + this->choicer_chip->GetStringSelection();

    if (myfunc::GetCompletePath(Main::proj_info->rvc->loc, ".rvc") == "") {
        wxMessageBox(wxString::Format("%s�ļ����µ�оƬ�����ļ������ڻ�Ψһ", Main::proj_info->rvc->name),
            "����", wxICON_ERROR);
        //wxLogError("%s�ļ����µ�оƬ�����ļ������ڻ�Ψһ ������%s", Main::proj_info->rvc->name, __func__);
        return;
    }

    // ��ȡrvc�ļ�
    Main::proj_info->platform_path = dir_path;
    Main::proj_info->rvc->SetRVC(myfunc::GetCompletePath(Main::proj_info->rvc->loc, ".rvc"));

    Main::bottom_message->SetBottomMessage(Main::proj_info->rva->name + "\t" + Main::proj_info->rvc->name, *wxGREEN);

    btn_select->Enable(false);
    Main::tool_bar->EnableRvaPreview();
    Main::tool_bar->EnableRvcPreview();
    Main::menu_bar->EnableFileMenu();
    Main::body_splitter->file_tree->ExpandAll();

    // ��������wxTreeItemData������
    // ��Ϊ��Ҫ���е�ҳ����������
    Main::body_splitter->file_tree->InitItem();

    // ����Chip�е���Ϣ
    Main::proj_info->rvp->chip->InitChip();
    this->Hide();
}

void Frame_Select::OnClose(const wxCloseEvent& event) {
    this->Hide();
}