#include "BaseInfo.h"
#include "../../Func/func.h"

void BaseInfo::SetBaseInfo(wxXmlNode* root) {
	name = myfunc::FindorMake(root, "Name")->GetNodeContent();
	version = myfunc::FindorMake(root, "Version")->GetNodeContent();
	assert_enable = myfunc::FindorMake(root, "Assert_Enable")->GetNodeContent();
	debug_enbale = myfunc::FindorMake(root, "Debug_Log_Enable")->GetNodeContent();
	pagetable_enable = myfunc::FindorMake(root, "Pgtbl_Raw_Enable")->GetNodeContent();
	buildsys = myfunc::FindorMake(root, "Buildsystem")->GetNodeContent();
	workspace_ow = myfunc::FindorMake(root, "Workspace_Overwrite")->GetNodeContent();
}

wxString BaseInfo::toString() {
	return 
		"Name:\t" + name + "\n" +
		"Version:\t" + version + "\n" +
		"Assert Enable:\t" + assert_enable + "\n" +
		"Debug Enable:\t" + debug_enbale + "\n" +
		"Pagetable Enable:\t" + pagetable_enable + "\n" +
		"Build System:\t" + buildsys + "\n" +
		"Workspace Overwrite:\t" + workspace_ow + "\n";
}

void BaseInfo::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", name);
	myfunc::AddNode(root, "Version", version);
	myfunc::AddNode(root, "Assert_Enable", assert_enable);
	myfunc::AddNode(root, "Debug_Log_Enable", debug_enbale);
	myfunc::AddNode(root, "Pgtbl_Raw_Enable", pagetable_enable);
	myfunc::AddNode(root, "Buildsystem", buildsys);
	myfunc::AddNode(root, "Workspace_Overwrite", workspace_ow);
}


void BaseInfo::ClearInfo() {
	name = wxEmptyString;
	version = wxEmptyString;
	assert_enable = wxEmptyString;
	debug_enbale = wxEmptyString;
	pagetable_enable = wxEmptyString;
	buildsys = wxEmptyString;
	workspace_ow = wxEmptyString;
}