#include "Native_Child_BaseInfo.h"
#include "../../../../../Func/func.h"
#include "../../../../../main.h"
Native_Child_BaseInfo::Native_Child_BaseInfo(
    wxWindow* parent,
    const int& _id,
    const wxString& _errorloc,
    std::function<void()>func)
    :wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE), 
    id(_id), errorloc(_errorloc) {

    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    this->title = new wxStaticText(this, wxID_ANY, wxString::Format("ԭ������ %d", _id));
    this->SetScrollRate(10, 10);  // ���ù������� (��λ������)

    wxStaticBoxSizer* stbox_base_info = new wxStaticBoxSizer(wxVERTICAL, this, "Base_info");
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    //wxStaticText* txt_name = new  wxStaticText(this, wxID_ANY, "Name");
    name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc, func,
        true, "�����Ʊ�����һ���Ϸ��ı�ʶ�������Һ�ϵͳ�е��������̡�����������Ʋ����ظ�");
    extra_captbl = new MyZ_GE_Zero(this, "Extra Captbl", wxEmptyString, errorloc, func,
        true, "ֻ�����û�Ҫ�ֶ�����ں˶��󵽸ý��̵�Ȩ�ܱ�ʱ����Ҫ����Ϊ��Ϊ0������");
    hbox1->Add(name, 1);
    hbox1->Add(extra_captbl, 1);


    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    coprocessor = new MyCoprocessor(this, "Coprocessor", wxEmptyString, errorloc, func);
    coprocessor->FillAllItem(Main::proj_info->rvc->coprocessor);

    buildsys = new SimpleText(this, "Buildsystem", wxEmptyString, wxEmptyString, wxTE_READONLY);
    toolchain = new SimpleText(this, "Toolchain", wxEmptyString, wxEmptyString, wxTE_READONLY);

    optimi = new MyChoice(this, "Optimization");
    optimi->FillChoice(wxArrayString(6, new wxString[6]{ "O0","O1","O2","O3","Of","Os" }));
    optimi->SetSelection(0);

    hbox2->Add(buildsys, 1);
    hbox2->Add(toolchain, 1);
    hbox2->Add(optimi, 1);


    stbox_base_info->Add(hbox1, 0, wxALL | wxEXPAND, 5);
    stbox_base_info->Add(hbox2, 0, wxALL | wxEXPAND, 5);
    stbox_base_info->Add(coprocessor, 0, wxALL | wxEXPAND, 5);


    wxStaticBoxSizer* stbox_path = new wxStaticBoxSizer(wxVERTICAL, this, "Output");
    project_output = new SimpleText(this, "Project Output", wxEmptyString, wxEmptyString, wxTE_READONLY);
    project_overw = new BoolBox(this, "Project Overwrite");
    linker_output = new SimpleText(this, "Linker Output", wxEmptyString, "./", wxTE_READONLY);
    main_hdr_output = new SimpleText(this, "Main Header Output", wxEmptyString, "../Include/", wxTE_READONLY);
    main_src_output = new SimpleText(this, "Main Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
    entry_src_output = new SimpleText(this, "Entry Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
    entry_src_overw = new BoolBox(this, "Entry Source Overwrite");

    stbox_path->Add(project_output, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(project_overw, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(linker_output, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(main_hdr_output, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(main_src_output, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(entry_src_output, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(entry_src_overw, 0, wxALL | wxEXPAND, 5);

    //wxButton* btn_confirm = new wxButton(this, wxID_ANY, "ȷ��");
    wxButton* btn_refresh = new wxButton(this, wxID_ANY, "ˢ��");

    box->Add(title, 0, wxALL | wxALIGN_LEFT, 5);
    box->Add(stbox_base_info, 0, wxALL | wxEXPAND, 5);
    box->Add(stbox_path, 0, wxALL | wxEXPAND, 5);
    //box->Add(btn_confirm, 0, wxALL | wxALIGN_RIGHT, 5);
    box->Add(btn_refresh, 0, wxALL | wxALIGN_RIGHT, 5);
    SetSizer(box);

    //Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Native_Child_BaseInfo::OnConfirm, this, btn_confirm->GetId());
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Native_Child_BaseInfo::OnRefresh, this, btn_refresh->GetId());

}

void Native_Child_BaseInfo::SetNativeBaseInfo(const std::unique_ptr<Native_BaseInfo>& ptr) {
    // ϣ����N0
    // ��proj_info��Ӧ����һ�µģ�id��0��ʼ��������
    // ����ɾ������1��3���ٵ��ô˺��������

    name->SetValue(ptr->Name);
    extra_captbl->SetValue(ptr->Extra_Captbl);

    coprocessor->SetValue(ptr->GetCoprocessor());

    //buildsys->SetValue(ptr->Buildsystem);
    //toolchain->SetValue(ptr->Toolchain);
    buildsys->SetValue(Main::proj_info->rvp->GetBuildsys());
    toolchain->SetValue(Main::proj_info->rvp->GetToolchain());
    optimi->SetSelection(optimi->FindValue(ptr->Optimization));
    project_output->SetValue(ptr->Project_Output);
    project_overw->SetValue(ptr->Project_Overwrite);
    linker_output->SetValue(ptr->Linker_Output);
    main_hdr_output->SetValue(ptr->Main_Header_Output);
    main_src_output->SetValue(ptr->Main_Source_Output);
    entry_src_output->SetValue(ptr->Entry_Source_Output);
    entry_src_overw->SetValue(ptr->Entry_Source_Overwrite);
}

bool Native_Child_BaseInfo::IsLegal() {
    // ������Ҫʱ�Ϸ���C��ʶ��
    bool j1 = name->IsLegal();
    bool j2 = Main::body_splitter->workarea_panel->IsUniqueNativeName(id, name);
    bool j3 = coprocessor->IsLegal();
    bool j4 = extra_captbl->IsLegal();
    return j1 && j2 && j3 && j4;
}

//void Native_Child_BaseInfo::ConfirmFunc() {
//    SaveNativeBaseInfo();
//}

void Native_Child_BaseInfo::OnRefresh(const wxCommandEvent& event) {
    RefreshFunc();
}

//void Native_Child_BaseInfo::OnConfirm(const wxCommandEvent& event) {
//    ConfirmFunc();
//}

void Native_Child_BaseInfo::RefreshFunc() {
    this->buildsys->SetValue(Main::proj_info->rvp->GetBuildsys());
    this->toolchain->SetValue(Main::proj_info->rvp->GetToolchain());
}

void Native_Child_BaseInfo::ConfirmFunc(std::unique_ptr<Native_BaseInfo>& ptr) {
    if (!IsLegal()) {
        wxMessageBox("����", "ԭ�����̻�����Ϣ");
        return;
    }

    buildsys->SetValue(Main::proj_info->rvp->GetBuildsys());
    toolchain->SetValue(Main::proj_info->rvp->GetToolchain());
    project_output->SetValue(wxString::Format("./%s/Project/", name->GetValue()));

    ptr->Name = name->GetValue();
    ptr->Extra_Captbl = extra_captbl->GetValue();
    ptr->Coprocessor = coprocessor->GetValue();
    ptr->Buildsystem = buildsys->GetValue();
    ptr->Toolchain = toolchain->GetValue();
    ptr->Optimization = optimi->GetStringSelection();
    ptr->Project_Output = project_output->GetValue();
    ptr->Project_Overwrite = project_overw->GetStringValue();
    ptr->Linker_Output = linker_output->GetValue();
    ptr->Main_Header_Output = main_hdr_output->GetValue();
    ptr->Main_Source_Output = main_src_output->GetValue();
    ptr->Entry_Source_Output = entry_src_output->GetValue();
    ptr->Entry_Source_Overwrite = entry_src_overw->GetStringValue();
}

void Native_Child_BaseInfo::ClearError() {
    name->CorrectSet();
    coprocessor->CorrectSet();
    extra_captbl->CorrectSet();
}