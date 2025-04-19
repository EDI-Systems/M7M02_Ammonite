#include "Native_Manager.h"
#include "../../../Func/func.h"

void Native_Manager::SetNativeManager(wxXmlNode* _root) {
	for (wxXmlNode* n=_root->GetChildren(); n; n = n->GetNext()) {
		native.push_back(std::make_unique<Native>(n, int(native.size())));
	}
}

const Native* Native_Manager::GetNativeByID_ReadOnly(const int& tar_id)const {
    for (const auto& cur : native) {
        if (cur->id == tar_id) {
            return cur.get();  // ֻ�����أ����޸� unique_ptr
        }
    }
    wxMessageBox(wxString::Format("û���ҵ���Ӧ��ԭ���������ݡ�id:%d��", tar_id), "��ʾ", wxICON_WARNING);
    //wxLogError("û���ҵ� id:%d ��Ӧ��ԭ���������� ������%s", tar_id, __func__);
    return nullptr;  // û�ҵ����� nullptr
}

Native* Native_Manager::GetNativeByID_Write(const int& tar_id) {
    for (auto& cur : native) {
        if (cur->id == tar_id) {
            return cur.get();  // ֻ�����أ����޸� unique_ptr
        }
    }
    wxMessageBox(wxString::Format("û���ҵ���Ӧ��ԭ���������ݡ�id:%d��", tar_id), "��ʾ", wxICON_WARNING);
    //wxLogError("û���ҵ� id:%d ��Ӧ��ԭ���������� ������%s", tar_id, __func__);
    return nullptr;  // û�ҵ����� nullptr
}

bool Native_Manager::DeleteNativeById(const int& id) {
    for (int i = 0; i < native.size(); i++) {
        if (native[i]->id == id) {
            native.erase(native.begin() + i);
            return true;
        }
    }
    return false;
}


void Native_Manager::AddInfo(wxXmlNode*&root) {
    for (int i = 0; i < native.size(); i++) {
        wxXmlNode* native_node = new wxXmlNode(wxXML_ELEMENT_NODE, wxString::Format("N%d", i));
        native[i]->AddInfo(native_node);
        root->AddChild(native_node);
    }
}


void Native_Manager::ClearNative() {
    native.clear();
}