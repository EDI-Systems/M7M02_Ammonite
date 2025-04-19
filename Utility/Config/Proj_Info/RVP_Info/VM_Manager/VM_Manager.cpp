#include "VM_Manager.h"

void VM_Manager::SetVmManager(wxXmlNode* _root){
	for (wxXmlNode* v = _root->GetChildren(); v; v = v->GetNext()) {
        virtual_machine.push_back(std::make_unique<VirtualMachine>(v, int(virtual_machine.size())));
	}
}


const VirtualMachine* VM_Manager::GetVmByID_ReadOnly(const int& tar_id) const {
    for (const auto& cur : virtual_machine) {
        if (cur->id == tar_id) {
            return cur.get();  // ֻ�����أ����޸� unique_ptr
        }
    }
    wxMessageBox(wxString::Format("û���ҵ���Ӧ����������ݡ�id:%d��", tar_id), "��ʾ", wxICON_WARNING);
    //wxLogError("û���ҵ� id:%d ��Ӧ����������� ������%s", tar_id, __func__);
    return nullptr;  // û�ҵ����� nullptr
}

VirtualMachine* VM_Manager::GetVmByID_Write(const int& tar_id) {
    for (auto& cur : virtual_machine) {
        if (cur->id == tar_id) {
            return cur.get();  // ֻ�����أ����޸� unique_ptr
        }
    }
    wxMessageBox(wxString::Format("û���ҵ���Ӧ����������ݡ�id:%d��", tar_id), "��ʾ", wxICON_WARNING);
    //wxLogError("û���ҵ� id:%d ��Ӧ����������� ������%s", tar_id, __func__);
    return nullptr;  // û�ҵ����� nullptr
}

bool VM_Manager::DeleteVmById(const int& id) {
    for (int i = 0; i < virtual_machine.size(); i++) {
        if (virtual_machine[i]->id == id) {
            virtual_machine.erase(virtual_machine.begin() + i);
            return true;
        }
    }
    return false;
}

void VM_Manager::AddInfo(wxXmlNode*& root) {
    for (int i = 0; i < virtual_machine.size(); i++) {
        wxXmlNode* virtual_node = new wxXmlNode(wxXML_ELEMENT_NODE, wxString::Format("V%d", i));
        virtual_machine[i]->AddInfo(virtual_node);
        root->AddChild(virtual_node);
    }
}

void VM_Manager::ClearVirtualMachine() {
    virtual_machine.clear();
}