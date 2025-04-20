#include "Panel_BaseInfo.h"
#include "../../../../Func/func.h"
#include "../../../../main.h"

Panel_BaseInfo::Panel_BaseInfo(
    wxWindow* parent,
    wxWindowID id,
    const wxString& _errorloc,
    const bool& _makeGUI,
    std::function<void()>_func)
    :PanelBase(parent, wxID_ANY, _errorloc, _makeGUI, _func) {

    name = new SimpleText(swindow, "Name", "���������������ĸ��������ֵ", wxEmptyString, 0L, errorloc, func);
    version = new MyVersion(swindow, "Version", "���������������ĸ��������ֵ", errorloc, func);
    assert_enable = new BoolBox(swindow, "Assert Enable", "�Ƿ�Դ����еĶ��Խ��м��");
    debug_enable = new BoolBox(swindow, "Debug Log Enable", "�Ƿ�����RVM_DBG_��ͷ�ĵ��Դ�ӡ��");
    pagetble_enable = new BoolBox(swindow, "Pagetable Raw Enable", "�Ƿ�����RVM_DBG_�Ƿ����ù̶����û�̬MPUԪ����");

    compatible = new MyGrid(swindow, "Buildsystem Toolchain Type", "���õĺϷ����", errorloc, func);

    workplace_overwrite = new BoolBox(swindow, "Workspace Overwrite", "�������ɹ���ʱ�Ƿ񸲸ǹ����ռ��ļ�");

    // �ؼ�����
    box->Add(name, 0, wxEXPAND | wxALL, 5);
    box->Add(version, 0, wxEXPAND | wxALL, 5);
    box->Add(assert_enable, 0, wxEXPAND | wxALL, 5);
    box->Add(debug_enable, 0, wxEXPAND | wxALL, 5);
    box->Add(pagetble_enable, 0, wxEXPAND | wxALL, 5);
    box->Add(compatible, 0, wxALL, 5);
    box->Add(workplace_overwrite, 0, wxEXPAND | wxALL, 5);
    swindow->SetSizer(box);
    swindow->FitInside();

}

void Panel_BaseInfo::OnRefresh(const wxCommandEvent& event) {
    SetBaseInfo();
}

void Panel_BaseInfo::SetBaseInfo() {
    // �ָ���Ҫ�ָ�Ĭ�ϵ�
    compatible->Clear();

    // ��䵱ǰ��Ϣ
    name->SetValue(Main::proj_info->rvp->base_info->name);
    version->SetValue(Main::proj_info->rvp->base_info->version);
    assert_enable->SetValue(Main::proj_info->rvp->base_info->assert_enable);
    debug_enable->SetValue(Main::proj_info->rvp->base_info->debug_enbale);
    pagetble_enable->SetValue(Main::proj_info->rvp->base_info->pagetable_enable);
    workplace_overwrite->SetValue(Main::proj_info->rvp->base_info->workspace_ow);

    // ��rva�ļ����ҵ��Ϸ������
    for (auto i : Main::proj_info->rva->compatible) {
        this->compatible->AppendOneRows(i);
    }
    if (Main::work_mode == MODE_IMPORT) {
        // �ҵ���ǰ�ļ��еĺϷ����
        compatible->MatchFirst2nd(Main::proj_info->rvp->GetCompArray());
    }
    else if (Main::work_mode == MODE_NEWFILE) {
        ;
    }
    swindow->FitInside();
    makeGUI = true;
}

bool Panel_BaseInfo::IsLegal() {
    if (!makeGUI) {
        SetBaseInfo();
    }
    
    bool j1 = name->IsLegal();
    bool j2 = version->IsLegal();
    bool j3 = compatible->IsLegal();

    return j1 && j2 && j3;
}

void Panel_BaseInfo::OnConfirm(const wxCommandEvent& event) {
    SaveBaseInfo();
}

bool Panel_BaseInfo::SaveBaseInfo() {
    // ����Ƿ�Ϸ�
    if (!IsLegal()) {
        wxMessageBox("����", "������Ϣ");
        return false;
    }

    Main::proj_info->rvp->base_info->name = name->GetValue();
    Main::proj_info->rvp->base_info->version = version->GetValue();
    Main::proj_info->rvp->base_info->assert_enable = assert_enable->GetStringValue();
    Main::proj_info->rvp->base_info->debug_enbale = debug_enable->GetStringValue();
    Main::proj_info->rvp->base_info->pagetable_enable = pagetble_enable->GetStringValue();
    Main::proj_info->rvp->base_info->workspace_ow = workplace_overwrite->GetStringValue();

    wxArrayString compatible_arr = compatible->GetValue();
    Main::proj_info->rvp->SetBuildsys(compatible_arr[0]);
    Main::proj_info->rvp->SetToolchain(compatible_arr[1]);
    Main::proj_info->rvp->SetGuest(compatible_arr[2]);

    Main::proj_info->rvp->base_info->buildsys = Main::proj_info->rvp->GetBuildsys();

    return true;
}

void Panel_BaseInfo::OnPreview(const wxCommandEvent& event) {
    myfunc::PreviewHelper(Main::proj_info->rvp->base_info->toString(), "������Ϣ");
}


void Panel_BaseInfo::ClearError() {
    name->CorrectSet();
    version->CorrectSet();
    compatible->CorrectSet();
}