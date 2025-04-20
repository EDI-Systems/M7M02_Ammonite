#include "Virtual_BaseInfo.h"
#include "../../../../../Func/func.h"
#include "../../../../../main.h"

Virtual_BaseInfo::Virtual_BaseInfo(
    wxWindow* parent,
    const int& _id,
    const wxString& _errorloc,
    std::function<void()>func)
    :wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE),
    id(_id), errorloc(_errorloc) {
    this->box = new wxBoxSizer(wxVERTICAL);
    this->title = new wxStaticText(this, wxID_ANY, wxString::Format("虚拟机 %d", id));
    this->SetScrollRate(10, 10);  // 设置滚动速率 (单位是像素)

    wxStaticBoxSizer* stbox_base_info = new wxStaticBoxSizer(wxVERTICAL, this, "Base Information");

    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    name = new SimpleText(this, "Name", wxEmptyString, wxEmptyString, 0L, errorloc, func);
    extra_captbl = new MyZ_GE_Zero(this, "Extra Captbl", wxEmptyString, errorloc, func);
    hbox1->Add(name, 1);
    hbox1->Add(extra_captbl, 1);
    coprocessor = new MyCoprocessor(this, "Coprocessor", wxEmptyString, errorloc, func);
    coprocessor->FillAllItem(Main::proj_info->rvc->coprocessor);

    stbox_base_info->Add(hbox1, 0, wxALL | wxEXPAND, 5);
    //stbox_base_info->Add(0, 5);
    //stbox_base_info->Add(extra_captbl, 0, wxALL | wxEXPAND, 5);
    //stbox_base_info->Add(0, 5);
    stbox_base_info->Add(coprocessor, 0, wxALL | wxEXPAND, 5);

    wxStaticBoxSizer* stbox_memory = new wxStaticBoxSizer(wxVERTICAL, this);
    vs_size = new SizeSlider(this, "Vector Stack_ ize", wxEmptyString, errorloc, func);
    us_size = new SizeSlider(this, "User Stack Size", wxEmptyString, errorloc, func);

    stbox_memory->Add(vs_size, 0, wxALL | wxEXPAND, 5);
    stbox_memory->Add(0, 5);
    stbox_memory->Add(us_size, 0, wxALL | wxEXPAND, 5);


    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    priority = new MyVirtualPrio(this, "Priority", wxEmptyString, errorloc, func,
        true,"该值大于等于0, 且小于Virtual Priority（Virtual Priority的值在 监视器配置 中）");
    timeslice = new MyZ_G_Zero(this, "Timeslice", wxEmptyString, errorloc, func,
        true, "该配置项是一个正整数");
    period = new MyZ_G_Zero(this, "Period", wxEmptyString, errorloc, func,
        true, "该配置项是一个正整数");
    watchdog = new MyZ_GE_Zero(this, "Watchdog", wxEmptyString, errorloc, func,
        true, "该配置项是一个整数值，如果本配置项设置为0，那么意味着本虚拟机不启用看门狗");
    vnum = new MyZ_GE_Zero(this, "Vector_Num", wxEmptyString, errorloc, func);
    //hbox2->Add(priority, 1);
    hbox2->Add(timeslice, 1);
    hbox2->Add(period, 1);
    hbox2->Add(watchdog, 1);
    hbox2->Add(vnum, 1);

    wxBoxSizer* hbox3 = new wxBoxSizer(wxHORIZONTAL);
    gtype = new SimpleText(this, "Guest_Type", wxEmptyString, wxEmptyString, wxTE_READONLY);
    buildsys = new SimpleText(this, "Buildsystem", wxEmptyString, wxEmptyString, wxTE_READONLY);
    toolchain = new SimpleText(this, "Toolchain", wxEmptyString, wxEmptyString, wxTE_READONLY);
    optimi = new MyChoice(this, "Optimization");
    optimi->FillChoice(wxArrayString(6, new wxString[6]{ "O0","O1","O2","O3","Of","Os" }));
    optimi->SetSelection(0);
    hbox3->Add(gtype, 1);
    hbox3->Add(buildsys, 1);
    hbox3->Add(toolchain, 1);
    hbox3->Add(optimi, 1);

    stbox_memory->Add(priority, 0, wxALL | wxEXPAND, 5);
    stbox_memory->Add(hbox2, 0, wxALL | wxEXPAND, 5);
    stbox_memory->Add(hbox3, 0, wxALL | wxEXPAND, 5);

    wxStaticBoxSizer* stbox_path = new wxStaticBoxSizer(wxVERTICAL, this, "Output");
    project_output = new SimpleText(this, "Project_Output", wxEmptyString, wxEmptyString, wxTE_READONLY);

    project_overw = new BoolBox(this, "Project Overwrite");

    linker_output = new SimpleText(this, "Linker Output", wxEmptyString, "./", wxTE_READONLY);
    main_hdr_output = new SimpleText(this, "Main Header Output", wxEmptyString, "../Include/", wxTE_READONLY);
    main_src_output = new SimpleText(this, "Main Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
    vir_hdr_output = new SimpleText(this, "Virtual Header Output", wxEmptyString, "../Include/", wxTE_READONLY);
    vir_hdr_overw = new BoolBox(this, "Virtual Header Overwrite");
    vir_src_output = new SimpleText(this, "Virtual Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
    vir_src_overw = new BoolBox(this, "Virtual_Source_Overwrite");

    stbox_path->Add(project_output, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(project_overw, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(linker_output, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(main_hdr_output, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(main_src_output, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(vir_hdr_output, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(vir_hdr_overw, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(vir_src_output, 0, wxALL | wxEXPAND, 5);
    stbox_path->Add(vir_src_overw, 0, wxALL | wxEXPAND, 5);

    wxButton* btn_refresh = new wxButton(this, wxID_ANY, "刷新");

    this->box->Add(title, 0, wxALL | wxALIGN_LEFT, 5);
    this->box->Add(stbox_base_info, 0, wxALL | wxEXPAND, 5);
    this->box->Add(stbox_memory, 0, wxALL | wxEXPAND, 5);
    this->box->Add(stbox_path, 0, wxALL | wxEXPAND, 5);
    this->box->Add(btn_refresh, 0, wxALL | wxALIGN_RIGHT, 5);
    this->SetSizer(this->box);

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Virtual_BaseInfo::OnRefresh, this, btn_refresh->GetId());

}

void Virtual_BaseInfo::SetVirMachineBaseInfo(const std::unique_ptr<VM_BaseInfo>& ptr) {

    name->SetValue(ptr->Name);
    extra_captbl->SetValue(ptr->Extra_Captbl);

    coprocessor->SetValue(ptr->GetCoprocessor());

    vs_size->SetValue(ptr->Vector_Stack_Size);
    us_size->SetValue(ptr->User_Stack_Size);
    priority->SetValue(ptr->Priority);
    timeslice->SetValue(ptr->Timeslice);
    period->SetValue(ptr->Period);
    watchdog->SetValue(ptr->Watchdog);
    vnum->SetValue(ptr->Vector_Num);
    //gtype->SetValue(ptr->Guest_Type);
    //buildsys->SetValue(ptr->Buildsystem);
    //toolchain->SetValue(ptr->Toolchain);
    gtype->SetValue(Main::proj_info->rvp->GetGuest());
    buildsys->SetValue(Main::proj_info->rvp->GetBuildsys());
    toolchain->SetValue(Main::proj_info->rvp->GetToolchain());
    optimi->SetSelection(optimi->FindValue(ptr->Optimization));

    project_output->SetValue(ptr->Project_Output);
    project_overw->SetValue(ptr->Project_Overwrite);
    linker_output->SetValue(ptr->Linker_Output);
    main_hdr_output->SetValue(ptr->Main_Header_Output);
    main_src_output->SetValue(ptr->Main_Source_Output);
    vir_hdr_output->SetValue(ptr->Virtual_Header_Output);
    vir_hdr_overw->SetValue(ptr->Virtual_Header_Overwrite);
    vir_src_output->SetValue(ptr->Virtual_Source_Output);
    vir_src_overw->SetValue(ptr->Virtual_Source_Overwrite);
}

void Virtual_BaseInfo::RefreshFunc() {
    this->gtype->SetValue(Main::proj_info->rvp->GetGuest());
    this->buildsys->SetValue(Main::proj_info->rvp->GetBuildsys());
    this->toolchain->SetValue(Main::proj_info->rvp->GetToolchain());
}
bool Virtual_BaseInfo::IsLegal() {
    bool j1 = name->IsLegal();
    bool j2 = Main::body_splitter->workarea_panel->IsUniqueVmName(id, name);
    bool j3 = extra_captbl->IsLegal();
    bool j4 = vs_size->IsLegal();
    bool j5 = us_size->IsLegal();
    bool j6 = priority->IsLegal(Main::proj_info->rvp->GetVirtPrio());
    bool j7 = timeslice->IsLegal();
    bool j8 = period->IsLegal();
    bool j9 = watchdog->IsLegal();
    bool j10 = vnum->IsLegal();
    bool j11 = coprocessor->IsLegal();

    return j1 && j2 && j3 && j4 && j5 && j6 && j7 && j8 && j9 && j10 && j11;
}

void Virtual_BaseInfo::ClearError() {
    name->CorrectSet();
    extra_captbl->CorrectSet();
    vs_size->CorrectSet();
    us_size->CorrectSet();
    priority->CorrectSet();
    timeslice->CorrectSet();
    period->CorrectSet();
    watchdog->CorrectSet();
    vnum->CorrectSet();
    coprocessor->CorrectSet();
}


void Virtual_BaseInfo::OnRefresh(const wxCommandEvent& event) {
    RefreshFunc();
}

void Virtual_BaseInfo::ConfirmFunc(std::unique_ptr<VM_BaseInfo>& ptr) {
    if (!IsLegal()) {
        wxMessageBox("错误", "虚拟机基础信息");
        return;
    }
    gtype->SetValue(Main::proj_info->rvp->GetGuest());
    buildsys->SetValue(Main::proj_info->rvp->GetBuildsys());
    toolchain->SetValue(Main::proj_info->rvp->GetToolchain());

    project_output->SetValue(wxString::Format("./%s/Project/", name->GetValue()));

    ptr->Name = name->GetValue();
    ptr->Extra_Captbl = extra_captbl->GetValue();
    ptr->Coprocessor = coprocessor->GetValue();
    ptr->Vector_Stack_Size = vs_size->GetValue();
    ptr->User_Stack_Size = us_size->GetValue();

    ptr->Priority = priority->GetValue();
    ptr->Timeslice = timeslice->GetValue();
    ptr->Period = period->GetValue();
    ptr->Watchdog = watchdog->GetValue();

    ptr->Vector_Num = vnum->GetValue();
    ptr->Guest_Type = gtype->GetValue();
    ptr->Buildsystem = buildsys->GetValue();
    ptr->Toolchain = toolchain->GetValue();
    ptr->Optimization = optimi->GetStringSelection();

    ptr->Project_Output = project_output->GetValue();
    ptr->Project_Overwrite = project_overw->GetStringValue();
    ptr->Linker_Output = linker_output->GetValue();
    ptr->Main_Header_Output = main_hdr_output->GetValue();
    ptr->Main_Source_Output = main_src_output->GetValue();

    ptr->Virtual_Header_Output = vir_hdr_output->GetValue();
    ptr->Virtual_Header_Overwrite = vir_hdr_overw->GetStringValue();

    ptr->Virtual_Source_Output = vir_src_output->GetValue();
    ptr->Virtual_Source_Overwrite = vir_src_overw->GetStringValue();
}


