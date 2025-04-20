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

    base_info = new Panel_BaseInfo(this, wxID_ANY, "������Ϣ/", false, [=]() {ShowBaseInfo(); });
    base_info->Hide();

    chip = new Panel_Chip(this, wxID_ANY, "оƬ����/", false, [=]() {ShowChip(); });
    chip->Hide();

    extmem = new Panel_Extmem(this, wxID_ANY, "����洢��/", false, [=]() {ShowExtmem(); });
    extmem->Hide();

    shmem = new Panel_Shmem_Test(this, wxID_ANY, "�����ڴ��/", false, [=]() {ShowShmem(); });
    shmem->Hide();

    kernel = new Panel_Kernel(this, wxID_ANY, "�ں�����/", false, [=]() {ShowKernel(); });
    kernel->Hide();

    monitor = new Panel_Monitor(this, wxID_ANY, "����������/", false, [=]() {ShowMonitor(); });
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
        // ������ɺ�����ʾ
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
    //wxLogError("δ�ҵ�id:%d�ı��ؽ���ҳ�� ������%s", tar_id, __func__);
}

void Workarea_Panel::AddNativeChild(const int& id) {
    // �Ȳ����Ƿ���ڸ�id��ԭ���߳�ҳ��
    for (int i = 0; i < native_GUIs.size(); i++) {
        if (native_GUIs[i]->id == id) {
            wxMessageBox(wxString::Format("ԭ������ҳ���Ѵ��ڡ�id:%d��", id), "ID��ռ��", wxICON_WARNING);
            return;
        }
    }
    Freeze();
    Panel_Native_Child* tar = new Panel_Native_Child(this, id, wxString::Format("ԭ������%d/", id));
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
        //wxLogError("δ�ҵ�id:%d��ԭ������ҳ�� ������%s", ptr->id, __func__);
    }
}

void Workarea_Panel::AddVmChild(const int& id) {
    // �Ȳ����Ƿ���ڸ�id��ԭ���߳�ҳ��
    for (int i = 0; i < vm_GUIs.size(); i++) {
        if (vm_GUIs[i]->id == id) {
            wxMessageBox(wxString::Format("ԭ������ҳ���Ѵ��ڡ�id:%d��", id), "ID��ռ��", wxICON_WARNING);
            return;
        }
    }
    Freeze();
    Panel_Virtual_Child* tar = new Panel_Virtual_Child(this, id, wxString::Format("�����%d/", id));
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
        //wxLogError("δ�ҵ�id:%d�������ҳ�� ������%s", ptr->id, __func__);
    }
    //wxLogError("�������ϢΪ�� ������%s", __func__);
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
    //wxLogError("δ�ҵ�id:%d�������ҳ�� ������%s", tar_id, __func__);
}

bool Workarea_Panel::IsUniqueNativeName(const int& id, SimpleText*& name) {
    // �����ִ�Сд��Ψһ
    for (int i = 0; i < native_GUIs.size(); i++) {
        // ������Լ�
        if (id == native_GUIs[i]->id)
            continue;
        if (name->GetValue().Upper() == native_GUIs[i]->GetBaseInfoName().Upper()) {
            name->IncorrectSet("�����ظ�");
            wxLogError("%s ������ԭ�������в���Ψһ ������%s", name->GetValue(), __func__);
            return false;
        }
    }

    for (int i = 0; i < vm_GUIs.size(); i++) {
        if (name->GetValue().Upper() == vm_GUIs[i]->GetBaseInfoName().Upper()) {
            name->IncorrectSet("�����ظ�");
            wxLogError("%s ������������в���Ψһ ������%s", name->GetValue(), __func__);
            return false;
        }
    }

    return true;
}

bool Workarea_Panel::IsUniqueVmName(const int& id, SimpleText*& name) {
    // �����ִ�Сд��Ψһ
    for (int i = 0; i < native_GUIs.size(); i++) {
        if (name->GetValue().Upper() == native_GUIs[i]->GetBaseInfoName().Upper()) {
            name->IncorrectSet("�����ظ�");
            wxLogError("%s ������ԭ�������в���Ψһ ������%s", name->GetValue() , __func__);
            return false;
        }
    }

    for (int i = 0; i < vm_GUIs.size(); i++) {
        // ������Լ�
        if (id == vm_GUIs[i]->id)
            continue;
        if (name->GetValue().Upper() == vm_GUIs[i]->GetBaseInfoName().Upper()) {
            name->IncorrectSet("�����ظ�");
            wxLogError("%s ������������в���Ψһ ������%s", name->GetValue(), __func__);
            return false;
        }
    }

    return true;
}

bool Workarea_Panel::DestoryNativeById(const int& id) {
    for (int i = 0; i < native_GUIs.size(); i++) {
        if (native_GUIs[i]->id == id) {
            // �����ǰGUI�漰�Ĵ���
            native_GUIs[i]->ClearError();
            // ����GUI
            native_GUIs[i]->Destroy();
            // ɾ�������еĸ���
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
        // �Ϸ��򱣴沢����true
        // ���Ϸ��򲻱��沢����false
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
    // ��մ���
    base_info->ClearError();
    chip->ClearError();
    extmem->ClearError();
    shmem->ClearError();
    kernel->ClearError();
    monitor->ClearError();

    // ���ñ�־λ
    base_info->makeGUI = false;
    chip->makeGUI = false;
    extmem->makeGUI = false;
    shmem->makeGUI = false;
    kernel->makeGUI = false;
    monitor->makeGUI = false;


    // ���GUI �����ҳ�����
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
