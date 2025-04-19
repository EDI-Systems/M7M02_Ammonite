#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <vector>

#include "VM_BaseInfo/VM_BaseInfo.h"
#include "../../Unit/Memory/Memory.h"
#include "../../Unit/Shmem/Shmem.h"
#include "../../Unit/Send/Send.h"
#include "../../Unit/Kfunc/Kfunc.h"

class VirtualMachine{
public:
	VirtualMachine(wxXmlNode* root, const int& _id);
	VirtualMachine(const int& _id);
	const wxString toString()const;
	void AddInfo(wxXmlNode*& root);
	std::unique_ptr<VM_BaseInfo> base_info;
	std::vector<std::unique_ptr<Memory>> memory;
	std::vector<std::unique_ptr<Shmem>> shmem;
	std::vector<std::unique_ptr<Send>> send;
	std::vector<std::unique_ptr<Kfunc>> kfunc;

	// ÐéÄâ»úÊýÁ¿
	// int count;
	wxXmlNode* root;
	int id;



private:
	template<typename V>
	const wxString per_toString(const std::vector<std::unique_ptr<V>>& vec)const {
		int count = 0;
		wxString ans;
		for (const auto& i : vec) {
			wxString part = i->toString(wxString::Format("%d", count++));
			ans = ans + part;
		}
		return ans;
	}

	template<typename V>
	void AddChildInfo(wxXmlNode*& root, const wxString& sign, const std::vector<std::unique_ptr<V>>& vec) {
		for (int i = 0; i < vec.size(); i++) {
			wxXmlNode* NewUnit = new wxXmlNode(wxXML_ELEMENT_NODE, wxString::Format("%s%d", sign, i));
			vec[i]->AddInfo(NewUnit);
			root->AddChild(NewUnit);
		}
	}
};

