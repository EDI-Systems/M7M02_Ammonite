#include "workarea_panel.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/aui/aui.h>

#include "../Panels/panels.h"
#include "../../main.h"



Workarea_Panel::Workarea_Panel(wxWindow* parent)
    :wxPanel(parent, wxID_ANY) {
    this->cur_panel = nullptr;
    this->sizer = new wxBoxSizer(wxVERTICAL);

    project = new Panel_Project(this);
    project->Hide();

    base_info = new Panel_BaseInfo(this, wxID_ANY, "基本信息/", false, [=]() {ShowBaseInfo(); });
    base_info->Hide();

    chip = new Panel_Chip(this, wxID_ANY, "芯片配置/", false, [=]() {ShowChip(); });
    chip->Hide();

    extmem = new Panel_Extmem(this, wxID_ANY, "额外存储器/", false, [=]() {ShowExtmem(); });
    extmem->Hide();

    shmem = new Panel_Shmem_Test(this, wxID_ANY, "共享内存块/", false, [=]() {ShowShmem(); });
    shmem->Hide();

    kernel = new Panel_Kernel(this, wxID_ANY, "内核配置/", false, [=]() {ShowKernel(); });
    kernel->Hide();

    monitor = new Panel_Monitor(this, wxID_ANY, "监视器配置/", false, [=]() {ShowMonitor(); });
    monitor->Hide();

    native_info = new Panel_Native_Info(this);
    native_info->Hide();

    virtual_info = new Panel_Virtual_Info(this);
    virtual_info->Hide();

    this->SetSizer(this->sizer);
    this->sizer->Layout();


}


void Workarea_Panel::Show(wxPanel* cur) {
    if (cur&& cur != cur_panel) {
        if (cur_panel) {
            sizer->Detach(cur_panel);
            cur_panel->Hide();
        }
        cur_panel = cur;
        // 绘制完成后再显示
        cur_panel->Freeze();
        sizer->Add(cur_panel, 1, wxEXPAND);
        cur_panel->Show();
        cur_panel->Layout();
        sizer->Layout();
        cur_panel->Thaw();
    }
}

void Workarea_Panel::ShowNativeChild(const int& tar_id) {
    for (int i = 0; i < native_GUIs.size();i++) {
        if (native_GUIs[i]->id == tar_id) {
            native_GUIs[i]->Freeze();
            native_GUIs[i]->Show();
            native_GUIs[i]->Raise();
            native_GUIs[i]->Thaw();
            return;
        }
    }
    //wxLogError("未找到id:%d的本地进程页面 错误在%s", tar_id, __func__);
}

void Workarea_Panel::AddNativeChild(const int& id) {
    // 先查找是否存在该id的原生线程页面
    for (int i = 0; i < native_GUIs.size(); i++) {
        if (native_GUIs[i]->id == id) {
            wxMessageBox(wxString::Format("原生进程页面已存在【id:%d】", id), "ID已占用", wxICON_WARNING);
            return;
        }
    }
    Freeze();
    Panel_Native_Child* tar = new Panel_Native_Child(this, id, wxString::Format("原生进程%d/", id));
    tar->Hide();
    Thaw();
    native_GUIs.push_back(tar);
}

void Workarea_Panel::FillNativeChild(const Native* ptr) {
    if (ptr) {
        for (int i = 0; i < native_GUIs.size(); i++) {
            if (native_GUIs[i]->id == ptr->id) {
                native_GUIs[i]->SetNativeChild(ptr);
                return;
            }
        }
        //wxLogError("未找到id:%d的原生进程页面 错误在%s", ptr->id, __func__);
    }
}

void Workarea_Panel::AddVmChild(const int& id) {
    // 先查找是否存在该id的原生线程页面
    for (int i = 0; i < vm_GUIs.size(); i++) {
        if (vm_GUIs[i]->id == id) {
            wxMessageBox(wxString::Format("原生进程页面已存在【id:%d】", id), "ID已占用", wxICON_WARNING);
            return;
        }
    }
    Freeze();
    Panel_Virtual_Child* tar = new Panel_Virtual_Child(this, id, wxString::Format("虚拟机%d/", id));
    tar->Hide();
    Thaw();
    vm_GUIs.push_back(tar);
}


void Workarea_Panel::FillVmChild(const VirtualMachine* ptr) {
    if (ptr) {
        for (int i = 0; i < vm_GUIs.size(); i++) {
            if (vm_GUIs[i]->id == ptr->id) {
                vm_GUIs[i]->SetVmChild(ptr);
                return;
            }
        }
        //wxLogError("未找到id:%d的虚拟机页面 错误在%s", ptr->id, __func__);
    }
    //wxLogError("虚拟机信息为空 错误在%s", __func__);
}

void Workarea_Panel::ShowVmChild(const int& tar_id) {
    for (int i = 0; i < vm_GUIs.size(); i++) {
        if (vm_GUIs[i]->id == tar_id) {
            vm_GUIs[i]->Freeze();
            vm_GUIs[i]->Show();
            vm_GUIs[i]->Raise();
            vm_GUIs[i]->Thaw();
            return;
        }
    }
    //wxLogError("未找到id:%d的虚拟机页面 错误在%s", tar_id, __func__);
}

bool Workarea_Panel::IsUniqueNativeName(const int& id, SimpleText*& name) {
    // 不区分大小写的唯一
    for (int i = 0; i < native_GUIs.size(); i++) {
        // 不检查自己
        if (id == native_GUIs[i]->id)
            continue;
        if (name->GetValue().Upper() == native_GUIs[i]->GetBaseInfoName().Upper()) {
            name->IncorrectSet("命名重复");
            wxLogError("%s 命名在原生进程中并不唯一 错误在%s", name->GetValue(), __func__);
            return false;
        }
    }

    for (int i = 0; i < vm_GUIs.size(); i++) {
        if (name->GetValue().Upper() == vm_GUIs[i]->GetBaseInfoName().Upper()) {
            name->IncorrectSet("命名重复");
            wxLogError("%s 命名在虚拟机中并不唯一 错误在%s", name->GetValue(), __func__);
            return false;
        }
    }

    return true;
}

bool Workarea_Panel::IsUniqueVmName(const int& id, SimpleText*& name) {
    // 不区分大小写的唯一
    for (int i = 0; i < native_GUIs.size(); i++) {
        if (name->GetValue().Upper() == native_GUIs[i]->GetBaseInfoName().Upper()) {
            name->IncorrectSet("命名重复");
            wxLogError("%s 命名在原生进程中并不唯一 错误在%s", name->GetValue() , __func__);
            return false;
        }
    }

    for (int i = 0; i < vm_GUIs.size(); i++) {
        // 不检查自己
        if (id == vm_GUIs[i]->id)
            continue;
        if (name->GetValue().Upper() == vm_GUIs[i]->GetBaseInfoName().Upper()) {
            name->IncorrectSet("命名重复");
            wxLogError("%s 命名在虚拟机中并不唯一 错误在%s", name->GetValue(), __func__);
            return false;
        }
    }

    return true;
}

bool Workarea_Panel::DestoryNativeById(const int& id) {
    for (int i = 0; i < native_GUIs.size(); i++) {
        if (native_GUIs[i]->id == id) {
            // 清除当前GUI涉及的错误
            native_GUIs[i]->ClearError();
            // 销毁GUI
            native_GUIs[i]->Destroy();
            // 删除数组中的该项
            native_GUIs.erase(native_GUIs.begin() + i);
            return true;
        }
    }
    return false;
}


bool Workarea_Panel::DestoryVmById(const int& id) {
    for (int i = 0; i < vm_GUIs.size(); i++) {
        if (vm_GUIs[i]->id == id) {
            vm_GUIs[i]->ClearError();
            vm_GUIs[i]->Destroy();
            vm_GUIs.erase(vm_GUIs.begin() + i);
            return true;
        }
    }
    return false;
}


bool Workarea_Panel::IsLegal_Native() {
    bool judge = true;
    for (int i = 0; i < native_GUIs.size(); i++) {
        // 合法则保存并返回true
        // 不合法则不保存并返回false
        bool temp = native_GUIs[i]->TotConfirmFunc();
        judge = temp && judge;
    }
    return judge;
}


bool Workarea_Panel::IsLegal_Vm() {
    bool judge = true;
    for (int i = 0; i < vm_GUIs.size(); i++) {
        bool temp = vm_GUIs[i]->TotConfirmFunc();
        judge = temp && judge;
    }
    return judge;
}


void Workarea_Panel::ClearNative() {
    for (auto i : native_GUIs) {
        if (i) {
            i->Destroy();
            delete i;
        }
    }
    native_GUIs.clear();
}

void Workarea_Panel::ClearVm() {
    for (auto i : vm_GUIs) {
        if (i) {
            i->Destroy();
            delete i;
        }
    }
    vm_GUIs.clear();
}


void Workarea_Panel::ClearGUI() {
    // 清空错误
    base_info->ClearError();
    chip->ClearError();
    extmem->ClearError();
    shmem->ClearError();
    kernel->ClearError();
    monitor->ClearError();

    // 重置标志位
    base_info->makeGUI = false;
    chip->makeGUI = false;
    extmem->makeGUI = false;
    shmem->makeGUI = false;
    kernel->makeGUI = false;
    monitor->makeGUI = false;


    // 清空GUI 即清空页面错误
    ClearNative();
    ClearVm();
}


void Workarea_Panel::IsLegalNativeById(const int& id) {
    for (int i = 0; i < native_GUIs.size(); i++) {
        if (native_GUIs[i]->id == id) {
            native_GUIs[i]->IsLegal();
        }
    }
}

void Workarea_Panel::IsLegalVmById(const int& id) {
    for (int i = 0; i < vm_GUIs.size(); i++) {
        if (vm_GUIs[i]->id == id) {
            vm_GUIs[i]->IsLegal();
        }
    }
}
