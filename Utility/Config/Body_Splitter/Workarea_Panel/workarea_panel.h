#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <map>
#include <set>

#include "Frames/Panel_Project/Panel_Project.h"
#include "Frames/Panel_BaseInfo/Panel_BaseInfo.h"
#include "Frames/Panel_Chip/Panel_Chip.h"
#include "Frames/Panel_Shmem_Test/Panel_Shmem_Test.h"
#include "Frames/Panel_Kernel/Panel_Kernel.h"
#include "Frames/Panel_Monitor/Panel_Monitor.h"
#include "Frames/Panel_Native_Info/Panel_Native_Info.h"
#include "Frames/Panel_Virtual_Info/Panel_Virtual_Info.h"
#include "Frames/Panel_Extmem/Panel_Extmem.h"


#include "Frames/Panel_Native_Child/Panel_Native_Child.h"
#include "../../Proj_Info/RVP_Info/Native_Manager/Native/Native.h"

#include "Frames/Panel_Virtual_Child/Panel_Virtual_Child.h"
#include "../../Proj_Info/RVP_Info/VM_Manager/VirtualMachine/VirtualMachine.h"

#include "Frames/Controller/Controller.h"

class Workarea_Panel 
    : public wxPanel {
public:
    Workarea_Panel(wxWindow* parent);

    // 在工作区显示各个板块
    void ShowProject() { Show(project); }
    void ShowBaseInfo() { Show(base_info); }
    void ShowChip() { Show(chip); }
    void ShowExtmem() { Show(extmem); }
    void ShowShmem() { Show(shmem); }
    void ShowKernel() { Show(kernel); }
    void ShowMonitor() { Show(monitor); }
    void ShowNativeInfo() { Show(native_info); }
    void ShowVirtualInfo() { Show(virtual_info); }

    // 原生进程管理
    void AddNativeChild(const int& id);
    void FillNativeChild(const Native* ptr);
    void ShowNativeChild(const int& tar_id);
    bool IsUniqueNativeName(const int& id, SimpleText*& name);
    bool DestoryNativeById(const int& id);
    bool IsLegal_Native();
    void ClearNative();
    void IsLegalNativeById(const int& id);

    // 虚拟机管理
    void AddVmChild(const int& id);
    void FillVmChild(const VirtualMachine* ptr);
    void ShowVmChild(const int& tar_id);
    bool IsUniqueVmName(const int& id, SimpleText*& name);
    bool DestoryVmById(const int& id);
    bool IsLegal_Vm();
    void ClearVm();
    void IsLegalVmById(const int& id);


    // 清空GUI页面信息
    void ClearGUI();

    Panel_Project* project;

    // 基础信息编辑页面
    Panel_BaseInfo* base_info;

    // 芯片编辑页面
    Panel_Chip* chip;

    // 额外内存编辑页面
    Panel_Extmem* extmem;
    
    // 共享内存编辑页面
    Panel_Shmem_Test* shmem;

    // 内核编辑页面
    Panel_Kernel* kernel;

    // 监视器编辑页面
    Panel_Monitor* monitor;

    std::vector<Panel_Native_Child*> native_GUIs;

    std::vector<Panel_Virtual_Child*> vm_GUIs;

    Panel_Native_Info* native_info;

    Panel_Virtual_Info* virtual_info;


private:
    wxPanel* cur_panel;
    wxBoxSizer* sizer;

    void Show(wxPanel* cur);
};