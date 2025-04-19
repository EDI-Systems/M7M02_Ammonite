#include "Panel_Kernel.h"
#include "../../../../Func/func.h"
#include "../../../../main.h"

Panel_Kernel::Panel_Kernel(
    wxWindow* parent,
    wxWindowID id,
    const wxString& _errorloc,
    const bool& _makeGUI,
    std::function<void()>_func)
    :PanelBase(parent, wxID_ANY, _errorloc, _makeGUI, _func) {

    wxStaticBoxSizer* vbox1 = new wxStaticBoxSizer(wxVERTICAL, swindow, "Code Segment");
    this->code_base = new BaseSlider(swindow, "Code Base", "内核所使用的代码段的基址", errorloc, func);
    this->code_size = new SizeSlider(swindow, "Code Size", "内核所使用的代码段的长度", errorloc, func);
    vbox1->Add(code_base, 0, wxEXPAND | wxRIGHT, 15);
    vbox1->Add(code_size, 0, wxEXPAND | wxRIGHT, 15);

    wxStaticBoxSizer* vbox2 = new wxStaticBoxSizer(wxVERTICAL, swindow, "Data Segment");
    this->data_base = new BaseSlider(swindow, "Data Base", "内核所使用的数据段的基址", errorloc, func);
    this->data_size = new SizeSlider(swindow, "Data Size", "内核所使用的数据段的长度", errorloc, func);
    vbox2->Add(data_base, 0, wxEXPAND | wxRIGHT, 15);
    vbox2->Add(data_size, 0, wxEXPAND | wxRIGHT, 15);
    //data_box->Add(data_base, 0, wxEXPAND);
    //data_box->Add(data_size, 0, wxEXPAND);

    wxStaticBoxSizer* vbox3 = new wxStaticBoxSizer(wxVERTICAL, swindow);
    stack_size = new SizeSlider(swindow, "Stack Size", "内核栈段的长度（1kB即可满足大多数需求）", errorloc, func);
    extra_kom = new SizeSlider(swindow, "Extro Kernel Object Memory", "额外内核内存的长度", errorloc, func);
    vbox3->Add(stack_size, 0, wxEXPAND | wxRIGHT, 15);
    vbox3->Add(extra_kom, 0, wxEXPAND | wxRIGHT, 15);

    wxStaticBoxSizer* vbox4 = new wxStaticBoxSizer(wxVERTICAL, swindow);
    choice_kom_order = new MyChoice(swindow, "Kernel Object Memory Order", "内核内存分配粒度");
    choice_kom_order->FillChoice(wxArrayString(8, new wxString[8]{"2","4","8","16","32","64","128","256" }));
    kern_prio = new MyChoice(swindow, "Kernrl Priority", "内核态优先级的数量", errorloc);
    kern_prio->FillChoice(wxArrayString(8, new wxString[8]{"2","4","8","16","32","64","128","256" }));

    vbox4->Add(choice_kom_order, 1, wxEXPAND | wxRIGHT, 15);
    vbox4->Add(kern_prio, 1, wxEXPAND | wxRIGHT, 15);

    wxStaticBoxSizer* hbox1 = new wxStaticBoxSizer(wxHORIZONTAL, swindow);
    buildsys = new SimpleText(swindow, "Buildsystem", "构建系统", wxEmptyString, wxTE_READONLY);
    toolchain = new SimpleText(swindow, "Toolchain", "工具链", wxEmptyString, wxTE_READONLY);
    k_optim = new MyChoice(swindow, "Optimization", "优化等级");
    k_optim->FillChoice(wxArrayString(6, new wxString[6]{"O0","O1","O2","O3","Of","Os" }));
    hbox1->Add(buildsys, 1);
    hbox1->Add(toolchain, 1);
    hbox1->Add(k_optim, 1);

    wxStaticBoxSizer* vbox5 = new wxStaticBoxSizer(wxVERTICAL, swindow, "Output");
    full_image = new BoolBox(swindow, "Full Image");
    project_output = new SimpleText(swindow, "Project Output", "内核工程的输出目录", "./Kernel/Project/", wxTE_READONLY);
    project_overw = new BoolBox(swindow, "Project Overwrite", "重新生成时是否覆盖之前生成的内核工程文件");
    linker_output = new SimpleText(swindow, "Linker Output", "内核工程链接器脚本的输出目录", "./", wxTE_READONLY);
    conf_hdr_output = new SimpleText(swindow, "Config Header Output", "两个内核配置头文件的输出目录", "../Include/", wxTE_READONLY);
    boot_hdr_output = new SimpleText(swindow, "Boot Header Output", "头文件的输出目录", "../Include/", wxTE_READONLY);
    boot_src_output = new SimpleText(swindow, "Boot Source Output", "源文件的输出目录", "../Source/", wxTE_READONLY);
    hook_src_output = new SimpleText(swindow, "Hook Source Output", "钩子源文件的输出目录", "../Source/", wxTE_READONLY);
    hook_src_overw = new BoolBox(swindow, "Hook Source Overwrite", "在重新生成时是否覆盖之前生成的内核钩子源文件");
    handler_src_output = new SimpleText(swindow, "Handler Source Output", "中断向量源文件的输出目录", "../Source/", wxTE_READONLY);
    handler_scr_overw = new BoolBox(swindow, "Handler Source Overwrite", "重新生成时是否覆盖之前生成的中断向量源文件");

    vbox5->Add(full_image, 1, wxEXPAND | wxRIGHT, 15);
    vbox5->Add(project_output, 1, wxEXPAND | wxRIGHT, 15);
    vbox5->Add(project_overw, 1, wxEXPAND | wxRIGHT, 15);
    vbox5->Add(linker_output, 1, wxEXPAND | wxRIGHT, 15);

    vbox5->Add(conf_hdr_output, 1, wxEXPAND | wxRIGHT, 15);
    vbox5->Add(boot_hdr_output, 1, wxEXPAND | wxRIGHT, 15);
    vbox5->Add(boot_src_output, 1, wxEXPAND | wxRIGHT, 15);
    vbox5->Add(hook_src_output, 1, wxEXPAND | wxRIGHT, 15);

    vbox5->Add(hook_src_overw, 1, wxEXPAND | wxRIGHT, 15);
    vbox5->Add(handler_src_output, 1, wxEXPAND | wxRIGHT, 15);
    vbox5->Add(handler_scr_overw, 1, wxEXPAND | wxRIGHT, 15);
    
    box->Add(vbox1, 0, wxALL | wxEXPAND, 10);
    box->Add(vbox2, 0, wxALL | wxEXPAND, 10);
    box->Add(vbox3, 0, wxALL | wxEXPAND, 10);
    box->Add(vbox4, 0, wxALL | wxEXPAND, 10);
    box->Add(hbox1, 0, wxALL | wxEXPAND, 10);
    box->Add(vbox5, 0, wxALL | wxEXPAND, 10);

    swindow->SetSizer(box);
    swindow->FitInside();
}

void Panel_Kernel::SetKernel() {
    code_base->SetValue(Main::proj_info->rvp->kernel->Code_Base);
    code_base->SetPosition(0);
    code_size->SetValue(Main::proj_info->rvp->kernel->Code_Size);
    code_size->SetPosition(0);
    data_base->SetValue(Main::proj_info->rvp->kernel->Data_Base);
    data_size->SetValue(Main::proj_info->rvp->kernel->Data_Size);
    stack_size->SetValue(Main::proj_info->rvp->kernel->Stack_Size);
    extra_kom->SetValue(Main::proj_info->rvp->kernel->Extra_Kom);
    choice_kom_order->SetSelection(choice_kom_order->FindValue(Main::proj_info->rvp->kernel->Kom_Order));
    kern_prio->SetSelection(kern_prio->FindValue(Main::proj_info->rvp->kernel->Kern_Prio));
    //buildsys->SetValue(Main::proj_info->rvp->kernel->Buildsystem);
    //toolchain->SetValue(Main::proj_info->rvp->kernel->Toolchain);
    buildsys->SetValue(Main::proj_info->rvp->GetBuildsys());
    toolchain->SetValue(Main::proj_info->rvp->GetToolchain());
    k_optim->SetSelection(k_optim->FindValue(Main::proj_info->rvp->kernel->Optimization));
    full_image->SetValue(Main::proj_info->rvp->kernel->Full_Image);
    project_output->SetValue(Main::proj_info->rvp->kernel->Project_Output);
    project_overw->SetValue(Main::proj_info->rvp->kernel->Project_Overwrite);

    linker_output->SetValue(Main::proj_info->rvp->kernel->Linker_Output);
    conf_hdr_output->SetValue(Main::proj_info->rvp->kernel->Config_Header_Output);
    boot_hdr_output->SetValue(Main::proj_info->rvp->kernel->Boot_Header_Output);
    boot_src_output->SetValue(Main::proj_info->rvp->kernel->Boot_Source_Output);
    hook_src_output->SetValue(Main::proj_info->rvp->kernel->Hook_Source_Output);
    hook_src_overw->SetValue(Main::proj_info->rvp->kernel->Hook_Source_Overwrite);
    handler_src_output->SetValue(Main::proj_info->rvp->kernel->Handler_Source_Output);
    handler_scr_overw->SetValue(Main::proj_info->rvp->kernel->Handler_Source_Overwrite);

    //Main::rvp->SetKernPrio(wxAtoi(kern_prio->GetStringSelection()));
    makeGUI = true;
}


bool Panel_Kernel::IsLegal() {
    if (!makeGUI) {
        SetKernel();
    }
    bool j1 = code_base->IsLegal();
    bool j2 = code_size->IsLegal();
    bool j3 = data_base->IsLegal();
    bool j4 = data_size->IsLegal();
    bool j5 = stack_size->IsLegal();
    bool j6 = extra_kom->IsLegal();
    bool j7 = kern_prio->IsLegal();
    return j1 && j2 && j3 && j4 && j5 && j6 && j7;
}

void Panel_Kernel::ClearError() {
    code_base->CorrectSet();
    code_size->CorrectSet();
    data_base->CorrectSet();
    data_size->CorrectSet();
    stack_size->CorrectSet();
    extra_kom->CorrectSet();
    kern_prio->CorrectSet();
}

void Panel_Kernel::OnRefresh(const wxCommandEvent& event) {
    SetKernel();
}

void Panel_Kernel::OnPreview(const wxCommandEvent& event) {
    myfunc::PreviewHelper(Main::proj_info->rvp->kernel->toString(), "源码预览 内核配置");
}
void Panel_Kernel::OnConfirm(const wxCommandEvent& event) {

    // 检查todo
    SaveKernel();
}

bool Panel_Kernel::SaveKernel() {
    if (!IsLegal()) {
        wxMessageBox("错误", "芯片配置");
        return false;
    }
    Main::proj_info->rvp->kernel->Code_Base = code_base->GetValue();
    Main::proj_info->rvp->kernel->Code_Size = code_size->GetValue();
    Main::proj_info->rvp->kernel->Data_Base = data_base->GetValue();
    Main::proj_info->rvp->kernel->Data_Size = data_size->GetValue();
    Main::proj_info->rvp->kernel->Stack_Size = stack_size->GetValue();
    Main::proj_info->rvp->kernel->Extra_Kom = extra_kom->GetValue();
    Main::proj_info->rvp->kernel->Kom_Order = choice_kom_order->GetStringSelection();
    Main::proj_info->rvp->kernel->Kern_Prio = kern_prio->GetStringSelection();
    Main::proj_info->rvp->kernel->Buildsystem = Main::proj_info->rvp->GetBuildsys();
    Main::proj_info->rvp->kernel->Toolchain = Main::proj_info->rvp->GetBuildsys();
    Main::proj_info->rvp->kernel->Optimization = k_optim->GetStringSelection();
    Main::proj_info->rvp->kernel->Full_Image = full_image->GetStringValue();
    Main::proj_info->rvp->kernel->Project_Output = project_output->GetValue();
    Main::proj_info->rvp->kernel->Project_Overwrite = project_overw->GetStringValue();
    Main::proj_info->rvp->kernel->Linker_Output = linker_output->GetValue();
    Main::proj_info->rvp->kernel->Config_Header_Output = conf_hdr_output->GetValue();
    Main::proj_info->rvp->kernel->Boot_Header_Output = boot_hdr_output->GetValue();
    Main::proj_info->rvp->kernel->Boot_Source_Output = boot_src_output->GetValue();
    Main::proj_info->rvp->kernel->Hook_Source_Output = hook_src_output->GetValue();
    Main::proj_info->rvp->kernel->Hook_Source_Overwrite = hook_src_overw->GetStringValue();
    Main::proj_info->rvp->kernel->Handler_Source_Output = handler_src_output->GetValue();
    Main::proj_info->rvp->kernel->Handler_Source_Overwrite = handler_scr_overw->GetStringValue();

    //myfunc::SetNodeCont(kernel, "Code_Base", code_base->GetValue());
    //myfunc::SetNodeCont(kernel, "Code_Size", code_size->GetValue());
    //myfunc::SetNodeCont(kernel, "Data_Base", data_base->GetValue());
    //myfunc::SetNodeCont(kernel, "Data_Size", data_size->GetValue());
    //myfunc::SetNodeCont(kernel, "Stack_Size", stack_size->GetValue());
    //myfunc::SetNodeCont(kernel, "Extra_Kom", extra_kom->GetValue());
    //myfunc::SetNodeCont(kernel, "Kom_Order", choice_kom_order->GetStringSelection());
    //myfunc::SetNodeCont(kernel, "Kern_Prio", kern_prio->GetStringSelection());
    //myfunc::SetNodeCont(kernel, "Buildsystem", Main::rvp->GetSysArr()[0]);
    //myfunc::SetNodeCont(kernel, "Toolchain", Main::rvp->GetSysArr()[1]);
    //myfunc::SetNodeCont(kernel, "Optimization", k_optim->GetStringSelection());
    //myfunc::SetNodeCont(kernel, "Full_Image", full_image->GetStringValue());
    //myfunc::SetNodeCont(kernel, "Project_Output", project_output->GetValue());
    //myfunc::SetNodeCont(kernel, "Project_Overwrite", project_overw->GetStringValue());
    //myfunc::SetNodeCont(kernel, "Linker_Output", linker_output->GetValue());
    //myfunc::SetNodeCont(kernel, "Config_Header_Output", conf_hdr_output->GetValue());
    //myfunc::SetNodeCont(kernel, "Boot_Header_Output", boot_hdr_output->GetValue());
    //myfunc::SetNodeCont(kernel, "Boot_Source_Output", boot_src_output->GetValue());
    //myfunc::SetNodeCont(kernel, "Hook_Source_Output", hook_src_output->GetValue());
    //myfunc::SetNodeCont(kernel, "Hook_Source_Overwrite", hook_src_overw->GetStringValue());
    //myfunc::SetNodeCont(kernel, "Handler_Source_Output", handler_src_output->GetValue());
    //myfunc::SetNodeCont(kernel, "Handler_Source_Overwrite", handler_scr_overw->GetStringValue());
    //Main::rvp->SetKernPrio(wxAtoi(kern_prio->GetStringSelection()));
    //Main::body_splitter->file_tree->SavedKernel();
    return true;
}

//void Panel_Kernel::ClearGUI() {
//    choice_kom_order->SetSelection(0);
//    kern_prio->SetSelection(0);
//    k_optim->SetSelection(0);
//
//    code_base->SetValue("0x1");
//    code_base->SetPosition(0);
//    data_base->SetValue("0x1");
//    data_base->SetPosition(0);
//
//
//    code_size->SetValue("0x1");
//    code_size->SetPosition(0);
//    data_size->SetValue("0x1");
//    data_size->SetPosition(0);
//    stack_size->SetValue("0x1");
//    stack_size->SetPosition(0);
//    extra_kom->SetValue("0x1");
//    extra_kom->SetPosition(0);
//
//    full_image->SetValue("Yes");
//    project_overw->SetValue("Yes");
//    hook_src_overw->SetValue("Yes");
//    handler_scr_overw->SetValue("Yes");
//
//
//    buildsys->SetValue(wxEmptyString);
//    toolchain->SetValue(wxEmptyString);
//    project_output->SetValue(wxEmptyString);
//    linker_output->SetValue(wxEmptyString);
//    conf_hdr_output->SetValue(wxEmptyString);
//    boot_hdr_output->SetValue(wxEmptyString);
//    boot_src_output->SetValue(wxEmptyString);
//    hook_src_output->SetValue(wxEmptyString);
//    handler_src_output->SetValue(wxEmptyString);
//}