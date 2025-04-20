#include "Shmem_Info.h"

void Shmem_Info::SetShmemInfo(wxXmlNode* _root) {
	for (wxXmlNode* s = _root->GetChildren(); s; s = s->GetNext()) {
		memory.push_back(std::make_unique<Memory>(s));
	}
}

const wxString Shmem_Info::toString()const {
	int count = 0;
	wxString ans = wxEmptyString;
	for (const auto& i : memory) {
		ans = ans + i->toString(wxString::Format("%d", count++));
	}
	return ans;
}

void Shmem_Info::AddInfo(wxXmlNode*& root) {
	for (int i = 0; i < memory.size(); i++) {
		wxXmlNode* NewShmem = new wxXmlNode(wxXML_ELEMENT_NODE, wxString::Format("S%d", i));
		memory[i]->AddInfo(NewShmem);
		root->AddChild(NewShmem);
	}
}

void Shmem_Info::ClearInfo() {
	memory.clear();
}
//bool Shmem_Info::ReMoveMemory(const int& _key) {
//	if (memory.size() == 0) {
//		wxLogError("û�������� ������%s", __func__);
//		return false;
//	}
//	size_t beforeSize = memory.size();  // ��¼ɾ��ǰ�Ĵ�С
//
//	memory.erase(std::remove_if(memory.begin(), memory.end(),
//		[&](const std::unique_ptr<Memory>& mem) {
//			return mem && mem->key == _key;
//		}),
//		memory.end());
//
//	return memory.size() < beforeSize;  // ���ɾ�����С��С��˵��ɾ���ɹ�
//}