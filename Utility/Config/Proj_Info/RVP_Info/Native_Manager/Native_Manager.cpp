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
            return cur.get();  // 只读返回，不修改 unique_ptr
        }
    }
    wxMessageBox(wxString::Format("没有找到对应的原生进程数据【id:%d】", tar_id), "提示", wxICON_WARNING);
    //wxLogError("没有找到 id:%d 对应的原生进程数据 错误在%s", tar_id, __func__);
    return nullptr;  // 没找到返回 nullptr
}

Native* Native_Manager::GetNativeByID_Write(const int& tar_id) {
    for (auto& cur : native) {
        if (cur->id == tar_id) {
            return cur.get();  // 只读返回，不修改 unique_ptr
        }
    }
    wxMessageBox(wxString::Format("没有找到对应的原生进程数据【id:%d】", tar_id), "提示", wxICON_WARNING);
    //wxLogError("没有找到 id:%d 对应的原生进程数据 错误在%s", tar_id, __func__);
    return nullptr;  // 没找到返回 nullptr
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