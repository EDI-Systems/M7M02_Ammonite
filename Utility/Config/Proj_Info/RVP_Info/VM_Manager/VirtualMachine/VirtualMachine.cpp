#include "VirtualMachine.h"
#include "../../../../Func/func.h"

VirtualMachine::VirtualMachine(wxXmlNode* _root, const int& _id)
	:id(_id), root(_root) {
	base_info = std::make_unique<VM_BaseInfo>(_root);
	myfunc::SetUnit(_root, "Memory", memory);
	myfunc::SetUnit(_root, "Shmem", shmem);
	myfunc::SetUnit(_root, "Send", send);
	myfunc::SetUnit(_root, "Kfunc", kfunc);
}
VirtualMachine::VirtualMachine(const int& _id)
	:id(_id) {
	base_info = std::make_unique<VM_BaseInfo>();
}

const wxString VirtualMachine::toString()const {
	return
		wxString::Format("Virtual Machine %d\n", this->id) +
		base_info->toString() + "\n" +
		per_toString<Memory>(memory) +
		per_toString<Shmem>(shmem) +
		per_toString<Send>(send) +
		per_toString<Kfunc>(kfunc);
}

void VirtualMachine::AddInfo(wxXmlNode*& root) {
	base_info->AddInfo(root);

	wxXmlNode* memory_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Memory");
	wxXmlNode* shmem_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Shmem");
	wxXmlNode* send_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Send");
	wxXmlNode* kfunc_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Kfunc");

	AddChildInfo(memory_node, "M", memory);
	AddChildInfo(shmem_node, "S", shmem);
	AddChildInfo(send_node, "S", send);
	AddChildInfo(kfunc_node, "K", kfunc);

	root->AddChild(memory_node);
	root->AddChild(shmem_node);
	root->AddChild(send_node);
	root->AddChild(kfunc_node);
}