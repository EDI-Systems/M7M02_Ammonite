#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

#include "BaseInfo/BaseInfo.h"
#include "Chip/Chip.h"
#include "Shmem_Info/Shmem_Info.h"
#include "Kernel/Kernel.h"
#include "Monitor/Monitor.h"
#include "Native_Manager/Native_Manager.h"
#include "VM_Manager/VM_Manager.h"
#include "Extmem/Extmem.h"

class RVP_Info{
public:
	RVP_Info();
	void SetRVP(wxXmlNode* root);
	void EmptyNodeFormat(wxXmlNode*& root);
	void ClearInfo();
	wxArrayString GetCompArray();
	// 返回用于导出的xml文件的根节点
	wxXmlNode* ReturnRoot();
	
	std::unique_ptr<BaseInfo> base_info;
	std::unique_ptr<Chip> chip;
	std::unique_ptr<Shmem_Info> shmem;
	std::unique_ptr<Extmem> extmem;
	std::unique_ptr<Kernel> kernel;
	std::unique_ptr<Monitor> monitor;
	std::unique_ptr<Native_Manager> native_manager;
	std::unique_ptr<VM_Manager> vm_manager;

	// 几个检查会用到的参数
	// Virt_Prio在monitor中
	int GetVirtPrio();
	
	// Kern_Prio在kernel中
	int GetKernelPrio();

	void SetBuildsys(const wxString& _buildsys) { buildsys = _buildsys; }
	void SetToolchain(const wxString& _toolchain) { toolchain = _toolchain; }
	void SetGuest(const wxString& _guest) { guest = _guest; }
	const wxString GetBuildsys()const { return buildsys; }
	const wxString GetToolchain()const { return toolchain; }
	const wxString GetGuest()const { return guest; }

private:
	wxString buildsys;
	wxString toolchain;
	wxString guest;

};

