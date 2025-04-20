#include "RVP_Info.h"
#include "../../Func/func.h"

RVP_Info::RVP_Info() {
	// 新建空的RVP信息待填写
	// 但是这个信息可能是不合法的（构造后都是EmptyString）
	base_info = std::make_unique<BaseInfo>();
	chip = std::make_unique<Chip>();
	shmem = std::make_unique<Shmem_Info>();
	extmem = std::make_unique<Extmem>();
	kernel = std::make_unique<Kernel>();
	monitor = std::make_unique<Monitor>();
	native_manager = std::make_unique<Native_Manager>();
	vm_manager = std::make_unique<VM_Manager>();
}

void RVP_Info::SetRVP(wxXmlNode* root) {

	base_info->SetBaseInfo(root);
	chip->SetChip(myfunc::FindorMake(root, "Chip"));
	shmem->SetShmemInfo(myfunc::FindorMake(root, "Shmem"));
	kernel->SetKernel(myfunc::FindorMake(root, "Kernel"));
	monitor->SetMonitor(myfunc::FindorMake(root, "Monitor"));
	native_manager->SetNativeManager(myfunc::FindorMake(root, "Native"));
	vm_manager->SetVmManager(myfunc::FindorMake(root, "Virtual"));
	// 没有实际内容
	extmem->SetExtmem(nullptr);


	buildsys = kernel->Buildsystem;
	toolchain = kernel->Toolchain;
	if (vm_manager->virtual_machine.size() > 0) {
		guest = vm_manager->virtual_machine[0]->base_info->Guest_Type;
	}
	else {
		wxLogError("没有虚拟机 无法填写Guest_Type项 错误在%s", __func__);
	}
}

wxArrayString RVP_Info::GetCompArray() {
	wxArrayString ans;
	ans.push_back(buildsys);
	ans.push_back(toolchain);
	ans.push_back(guest);
	return ans;
}

int RVP_Info::GetKernelPrio() {
	return wxAtoi(kernel->Kern_Prio);
}

int RVP_Info::GetVirtPrio() {
	return wxAtoi(monitor->Virt_Prio);
}


wxXmlNode* RVP_Info::ReturnRoot() {

	wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "Project");

	wxXmlNode* chip_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Chip");
	wxXmlNode* shmem_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Shmem");
	wxXmlNode* kernel_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Kernel");
	wxXmlNode* monitor_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Monitor");
	wxXmlNode* native_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Native");
	wxXmlNode* virtual_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Virtual");
	wxXmlNode* extmem_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Extmem");
	
	base_info->AddInfo(root);

	chip->AddInfo(chip_node);
	extmem->AddInfo(extmem_node);
	shmem->AddInfo(shmem_node);
	kernel->AddInfo(kernel_node);
	monitor->AddInfo(monitor_node);
	native_manager->AddInfo(native_node);
	vm_manager->AddInfo(virtual_node);

	root->AddChild(chip_node);
	root->AddChild(extmem_node);
	root->AddChild(shmem_node);
	root->AddChild(kernel_node);
	root->AddChild(monitor_node);
	root->AddChild(native_node);
	root->AddChild(virtual_node);

	// 必须存在但是可以不填写内容的节点需要添加wxEmptyString
	EmptyNodeFormat(root);

	return root;
}


void RVP_Info::EmptyNodeFormat(wxXmlNode*& root) {
	if (!root->GetChildren() && root->GetType() != wxXML_TEXT_NODE) {
		root->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString));
	}
	for (wxXmlNode* child = root->GetChildren(); child; child = child->GetNext()) {
		EmptyNodeFormat(child);
	}
}


void RVP_Info::ClearInfo() {
	base_info->ClearInfo();
	chip->ClearInfo();
	extmem->ClearInfo();
	kernel->ClearInfo();
	monitor->ClaerInfo();
	shmem->ClearInfo();

	native_manager->ClearNative();
	vm_manager->ClearVirtualMachine();
}