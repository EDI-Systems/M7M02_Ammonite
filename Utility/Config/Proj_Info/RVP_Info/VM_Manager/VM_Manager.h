#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <vector>

#include "VirtualMachine/VirtualMachine.h"
class VM_Manager{
public:
	VM_Manager() {};
	void SetVmManager(wxXmlNode* _root);
	void AddInfo(wxXmlNode*& root);
	void ClearVirtualMachine();
	const VirtualMachine* GetVmByID_ReadOnly(const int& tar_id)const;
	VirtualMachine* GetVmByID_Write(const int& tar_id);
	bool DeleteVmById(const int& id);

	std::vector<std::unique_ptr<VirtualMachine>>virtual_machine;
};

