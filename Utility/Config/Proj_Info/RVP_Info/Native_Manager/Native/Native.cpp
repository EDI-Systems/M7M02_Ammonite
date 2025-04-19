#include "Native.h"
#include "../../../../Func/func.h"

Native::Native(wxXmlNode* _root, const int& _id)
	:id(_id) {
	base_info = std::make_unique<Native_BaseInfo>(_root);
	myfunc::SetUnit(_root, "Memory", memory);
	myfunc::SetUnit(_root, "Shmem", shmem);
	myfunc::SetUnit(_root, "Thread", thread);
	myfunc::SetUnit(_root, "Port", port);
	myfunc::SetUnit(_root, "Receive", receive);
	myfunc::SetUnit(_root, "Send", send);
	myfunc::SetUnit(_root, "Vector", vector);
	myfunc::SetUnit(_root, "Kfunc", kfunc);
	myfunc::SetUnit(_root, "Invocation", invocation);
}

Native::Native(const int& _id)
	:id(_id) {
	base_info = std::make_unique<Native_BaseInfo>();
}

const wxString Native::toString()const {
	return
		wxString::Format("Native %d\n", this->id) +
		base_info->toString() + "\n" +
		per_toString<Memory>(memory) +
		per_toString<Shmem>(shmem) +
		per_toString<Thread>(thread) +
		per_toString<Port>(port) +
		per_toString<Receive>(receive) +
		per_toString<Send>(send) +
		per_toString<Vector>(vector) +
		per_toString<Kfunc>(kfunc) +
		per_toString<Invocation>(invocation);
}


void Native::AddInfo(wxXmlNode*& root) {
	base_info->AddInfo(root);

	wxXmlNode* memory_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Memory");
	wxXmlNode* shmem_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Shmem");
	wxXmlNode* thread_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Thread");
	wxXmlNode* port_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Port");
	wxXmlNode* receive_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Receive");
	wxXmlNode* send_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Send");
	wxXmlNode* vector_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Vector");
	wxXmlNode* kfunc_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Kfunc");
	wxXmlNode* invocation_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Invocation");

	AddChildInfo(memory_node, "M", memory);
	AddChildInfo(shmem_node, "S", shmem);
	AddChildInfo(thread_node, "T", thread);
	AddChildInfo(port_node, "P", port);
	AddChildInfo(receive_node, "R", receive);
	AddChildInfo(send_node, "S", send);
	AddChildInfo(vector_node, "V", vector);
	AddChildInfo(kfunc_node, "K", kfunc);
	AddChildInfo(invocation_node, "I", invocation);

	root->AddChild(memory_node);
	root->AddChild(shmem_node);
	root->AddChild(thread_node);
	root->AddChild(port_node);
	root->AddChild(receive_node);
	root->AddChild(send_node);
	root->AddChild(vector_node);
	root->AddChild(kfunc_node);
	root->AddChild(invocation_node);
}
