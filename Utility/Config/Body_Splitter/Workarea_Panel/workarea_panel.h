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

    // �ڹ�������ʾ�������
    void ShowProject() { Show(project); }
    void ShowBaseInfo() { Show(base_info); }
    void ShowChip() { Show(chip); }
    void ShowExtmem() { Show(extmem); }
    void ShowShmem() { Show(shmem); }
    void ShowKernel() { Show(kernel); }
    void ShowMonitor() { Show(monitor); }
    void ShowNativeInfo() { Show(native_info); }
    void ShowVirtualInfo() { Show(virtual_info); }

    // ԭ�����̹���
    void AddNativeChild(const int& id);
    void FillNativeChild(const Native* ptr);
    void ShowNativeChild(const int& tar_id);
    bool IsUniqueNativeName(const int& id, SimpleText*& name);
    bool DestoryNativeById(const int& id);
    bool IsLegal_Native();
    void ClearNative();
    void IsLegalNativeById(const int& id);

    // ���������
    void AddVmChild(const int& id);
    void FillVmChild(const VirtualMachine* ptr);
    void ShowVmChild(const int& tar_id);
    bool IsUniqueVmName(const int& id, SimpleText*& name);
    bool DestoryVmById(const int& id);
    bool IsLegal_Vm();
    void ClearVm();
    void IsLegalVmById(const int& id);


    // ���GUIҳ����Ϣ
    void ClearGUI();

    Panel_Project* project;

    // ������Ϣ�༭ҳ��
    Panel_BaseInfo* base_info;

    // оƬ�༭ҳ��
    Panel_Chip* chip;

    // �����ڴ�༭ҳ��
    Panel_Extmem* extmem;
    
    // �����ڴ�༭ҳ��
    Panel_Shmem_Test* shmem;

    // �ں˱༭ҳ��
    Panel_Kernel* kernel;

    // �������༭ҳ��
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