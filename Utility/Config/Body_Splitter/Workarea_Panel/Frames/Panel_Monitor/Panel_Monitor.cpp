#include "Panel_Monitor.h"
#include "../../../../main.h"
#include "../../../../Func/func.h"

Panel_Monitor::Panel_Monitor(
    wxWindow* parent,
    wxWindowID id,
    const wxString& _errorloc,
    const bool& _makeGUI,
    std::function<void()>_func)
    :PanelBase(parent, wxID_ANY, _errorloc, _makeGUI, _func) {

    wxStaticBoxSizer* vbox1 = new wxStaticBoxSizer(wxVERTICAL, swindow);
    code_size = new SizeSlider(swindow, "Code Size", "代码段长度", errorloc, func);
    data_size = new SizeSlider(swindow, "Data Size", "数据段长度", errorloc, func);
    vbox1->Add(code_size, 0, wxRIGHT | wxEXPAND, 25);
    vbox1->Add(data_size, 0, wxRIGHT | wxEXPAND, 25);


    wxStaticBoxSizer* vbox2 = new wxStaticBoxSizer(wxVERTICAL, swindow);
    init_stack_size = new SizeSlider(swindow, "Initial Stack Size", "启动守护线程INIT的栈长度（推荐大小 256B）", errorloc, func);
    sftd_stack_size = new SizeSlider(swindow, "SFTD Stack Size", "安全守护线程SFTD的栈长度（推荐大小 256B）", errorloc, func);
    vmmd_stack_size = new SizeSlider(swindow, "VMMD Stack Size", "了虚拟机守护线程VMMD的栈长度（推荐大小 512B）", errorloc, func);
    vbox2->Add(init_stack_size, 0, wxRIGHT | wxEXPAND, 25);
    vbox2->Add(sftd_stack_size, 0, wxRIGHT | wxEXPAND, 25);
    vbox2->Add(vmmd_stack_size, 0, wxRIGHT | wxEXPAND, 25);


    wxStaticBoxSizer* vbox3 = new wxStaticBoxSizer(wxVERTICAL, swindow);

    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    extra_captbl = new MyZ_GE_Zero(swindow, "Extra Capability", "额外权能表槽位数", errorloc, func);
    //wxStaticBoxSizer* other_box = new wxStaticBoxSizer(wxVERTICAL, swindow);
    virt_prio = new MyVirtPrio(swindow, "Virtual Priority", "虚拟化优先级数", errorloc, false, func,
        true, "使用虚拟机时该值不为0");
    virt_prio->FillChoice(wxArrayString(12, new wxString[12]{ "1","2","4","8","16","32","64","128","256","512","1024","0" }));

    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    virt_event = new MyVirtPrio(swindow, "Virtual Event", "虚拟机事件源数", errorloc, true, func,
        true, "使用虚拟机时该值不为0");
    virt_event->FillChoice(wxArrayString(12, new wxString[12]{ "1","2","4","8","16","32","64","128","256","512","1024","0" }));

    virt_map = new MyVirtMap(swindow, "Virtual Map", "虚拟机向量映射数", errorloc, func,
        true, "使用虚拟机时该值不为0，且该值需要大于等于Virtual Event");
    hbox1->Add(extra_captbl, 1, wxALL, 5);
    hbox1->Add(virt_prio, 1, wxALL, 5);
    hbox1->Add(10, 0);
    hbox2->Add(virt_event, 1, wxALL, 5);
    hbox2->Add(virt_map, 1, wxALL, 5);
    hbox2->Add(10, 0);

    vbox3->Add(hbox1, 0, wxEXPAND);
    vbox3->Add(hbox2, 0, wxEXPAND);


    wxStaticBoxSizer* vbox4 = new wxStaticBoxSizer(wxVERTICAL, swindow);
    wxBoxSizer* hbox3 = new wxBoxSizer(wxHORIZONTAL);
    buildsys = new SimpleText(swindow, "Buildsystem", "构建系统", wxEmptyString, wxTE_READONLY);
    toolchain = new SimpleText(swindow, "Toolchain", "工具链", wxEmptyString, wxTE_READONLY);
    optim = new MyChoice(swindow, "Optimization", "优化等级");
    optim->FillChoice(wxArrayString(6, new wxString[6]{ "O0","O1","O2","O3","Of","Os" }));
    hbox3->Add(buildsys, 1, wxALL, 5);
    hbox3->Add(toolchain, 1, wxALL, 5);
    hbox3->Add(optim, 1, wxALL, 5);
    hbox3->Add(10, 0);
    vbox4->Add(hbox3, 1, wxEXPAND);


    wxStaticBoxSizer* vbox5 = new wxStaticBoxSizer(wxVERTICAL, swindow, "Output");
    project_output = new SimpleText(swindow, "Project Output", wxEmptyString, "./Monitor/Project/", wxTE_READONLY);
    project_overw = new BoolBox(swindow, "Project Overwrite");
    linker_output = new SimpleText(swindow, "Linker Output", wxEmptyString, "./", wxTE_READONLY);
    conf_hdr_output = new SimpleText(swindow, "Config Header Output", wxEmptyString, "../Include", wxTE_READONLY);
    boot_hdr_output = new SimpleText(swindow, "Boot Header Output", wxEmptyString, "../Include/", wxTE_READONLY);
    boot_src_output = new SimpleText(swindow, "Boot Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
    hook_src_output = new SimpleText(swindow, "Hook Source Output", wxEmptyString, "../Source/", wxTE_READONLY);
    hook_src_overw = new BoolBox(swindow, "Hook Source Overwrite");
    vbox5->Add(project_output, 1, wxRIGHT | wxEXPAND, 15);
    vbox5->Add(project_overw, 1, wxRIGHT | wxEXPAND, 15);
    vbox5->Add(linker_output, 1, wxRIGHT | wxEXPAND, 15);
    vbox5->Add(conf_hdr_output, 1, wxRIGHT | wxEXPAND, 15);
    vbox5->Add(boot_hdr_output, 1, wxRIGHT | wxEXPAND, 15);
    vbox5->Add(boot_src_output, 1, wxRIGHT | wxEXPAND, 15);
    vbox5->Add(hook_src_output, 1, wxRIGHT | wxEXPAND, 15);
    vbox5->Add(hook_src_overw, 1, wxRIGHT | wxEXPAND, 15);

    box->Add(vbox1, 0, wxALL | wxEXPAND, 10);
    box->Add(vbox2, 0, wxALL | wxEXPAND, 10);
    box->Add(vbox3, 0, wxALL | wxEXPAND, 10);
    box->Add(vbox4, 0, wxALL | wxEXPAND, 10);
    box->Add(vbox5, 0, wxALL | wxEXPAND, 10);
    swindow->SetSizer(box);
    swindow->FitInside();
    SetSizer(highbox);
}

bool Panel_Monitor::IsLegal() {
    if (!makeGUI) {
        SetMonitor();
    }
    bool j1 = code_size->IsLegal();
    bool j2 = data_size->IsLegal();
    bool j3 = init_stack_size->IsLegal();
    bool j4 = sftd_stack_size->IsLegal();
    bool j5 = vmmd_stack_size->IsLegal();
    bool j6 = virt_prio->IsLegal();
    bool j7 = virt_event->IsLegal();
    // j8的检查理论上应该用xml层上的值，但是如果virt_event在此之前检查合法了
    // 是否可以用GUI层上的值
    bool j8 = virt_map->IsLegal(wxAtoi(virt_event->GetStringSelection()));
    bool j9 = extra_captbl->IsLegal();
    return j1 && j2 && j3 && j4 && j5 && j6 && j7 && j8 && j9;
}

void Panel_Monitor::ClearError() {
    code_size->CorrectSet();
    data_size->CorrectSet();
    init_stack_size->CorrectSet();
    sftd_stack_size->CorrectSet();
    vmmd_stack_size->CorrectSet();
    virt_prio->CorrectSet();
    virt_event->CorrectSet();
    virt_map->CorrectSet();
    extra_captbl->CorrectSet();

}

void Panel_Monitor::OnRefresh(const wxCommandEvent& event) {
    SetMonitor();
}

void Panel_Monitor::OnPreview(const wxCommandEvent& event) {
    myfunc::PreviewHelper(Main::proj_info->rvp->monitor->toString(), "源码预览 监视器配置");
}

void Panel_Monitor::OnConfirm(const wxCommandEvent& event) {
    SaveMonitor();
}

void Panel_Monitor::SetMonitor() {

    code_size->SetValue(Main::proj_info->rvp->monitor->Code_Size);
    data_size->SetValue(Main::proj_info->rvp->monitor->Data_Size);
    init_stack_size->SetValue(Main::proj_info->rvp->monitor->Init_Stack_Size);
    sftd_stack_size->SetValue(Main::proj_info->rvp->monitor->Sftd_Stack_Size);
    vmmd_stack_size->SetValue(Main::proj_info->rvp->monitor->Vmmd_Stack_Size);
    extra_captbl->SetValue(Main::proj_info->rvp->monitor->Extra_Captbl);
    virt_prio->SetSelection(virt_prio->FindValue(Main::proj_info->rvp->monitor->Virt_Prio));    
    virt_prio->SetVmCount(Main::proj_info->rvp->vm_manager->virtual_machine.size());    // 设置虚拟机数字
    virt_event->SetSelection(virt_event->FindValue(Main::proj_info->rvp->monitor->Virt_Event));
    virt_map->SetValue(Main::proj_info->rvp->monitor->Virt_Map);
    //buildsys->SetValue(Main::proj_info->rvp->monitor->Buildsystem);
    //toolchain->SetValue(Main::proj_info->rvp->monitor->Toolchain);
    buildsys->SetValue(Main::proj_info->rvp->GetBuildsys());
    toolchain->SetValue(Main::proj_info->rvp->GetToolchain());
    optim->SetSelection(optim->FindValue(Main::proj_info->rvp->monitor->Optimization));

    project_output->SetValue(Main::proj_info->rvp->monitor->Project_Output);
    project_overw->SetValue(Main::proj_info->rvp->monitor->Project_Overwrite);
    linker_output->SetValue(Main::proj_info->rvp->monitor->Linker_Output);
    conf_hdr_output->SetValue(Main::proj_info->rvp->monitor->Config_Header_Output);
    boot_hdr_output->SetValue(Main::proj_info->rvp->monitor->Boot_Header_Output);
    boot_src_output->SetValue(Main::proj_info->rvp->monitor->Boot_Source_Output);
    hook_src_output->SetValue(Main::proj_info->rvp->monitor->Hook_Source_Output);
    hook_src_overw->SetValue(Main::proj_info->rvp->monitor->Hook_Source_Overwrite);
    //Main::rvp->SetVirtPrio(wxAtoi(virt_prio->GetStringSelection()));
    makeGUI = true;
}

bool Panel_Monitor::SaveMonitor() {
    if (!IsLegal()) {
        wxMessageBox("错误", "监视器设置");
        return false;
    }
    Main::proj_info->rvp->monitor->Code_Size = code_size->GetValue();
    Main::proj_info->rvp->monitor->Data_Size = data_size->GetValue();
    Main::proj_info->rvp->monitor->Init_Stack_Size = init_stack_size->GetValue();
    Main::proj_info->rvp->monitor->Sftd_Stack_Size = sftd_stack_size->GetValue();
    Main::proj_info->rvp->monitor->Vmmd_Stack_Size = vmmd_stack_size->GetValue();
    Main::proj_info->rvp->monitor->Extra_Captbl = extra_captbl->GetValue();
    Main::proj_info->rvp->monitor->Virt_Prio = virt_prio->GetStringSelection();
    Main::proj_info->rvp->monitor->Virt_Event = virt_event->GetStringSelection();
    Main::proj_info->rvp->monitor->Virt_Map = virt_map->GetValue();
    Main::proj_info->rvp->monitor->Buildsystem = Main::proj_info->rvp->GetBuildsys();
    Main::proj_info->rvp->monitor->Toolchain = Main::proj_info->rvp->GetToolchain();
    Main::proj_info->rvp->monitor->Optimization = optim->GetStringSelection();
    Main::proj_info->rvp->monitor->Project_Output = project_output->GetValue();
    Main::proj_info->rvp->monitor->Project_Overwrite = project_overw->GetStringValue();
    Main::proj_info->rvp->monitor->Linker_Output = linker_output->GetValue();
    Main::proj_info->rvp->monitor->Config_Header_Output = conf_hdr_output->GetValue();
    Main::proj_info->rvp->monitor->Boot_Header_Output = boot_hdr_output->GetValue();
    Main::proj_info->rvp->monitor->Boot_Source_Output = boot_src_output->GetValue();
    Main::proj_info->rvp->monitor->Hook_Source_Output = hook_src_output->GetValue();
    Main::proj_info->rvp->monitor->Hook_Source_Overwrite = hook_src_overw->GetStringValue();
    return true;
}

