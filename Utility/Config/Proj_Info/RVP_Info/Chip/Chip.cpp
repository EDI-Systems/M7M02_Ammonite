#include "Chip.h"
#include "../../Func/func.h"
#include "../../../main.h"

Chip::Chip() {
	// 根据rva设置
}
void Chip::InitChip() {
	config.clear();
	for (int i = 0; i < Main::proj_info->rva->configs.size(); i++) {
		config.insert(
			std::make_pair(Main::proj_info->rva->configs[i]->name, wxEmptyString)
		);
	}
}


void Chip::SetChip(wxXmlNode* _root){
	name = myfunc::FindorMake(_root, "Name")->GetNodeContent();
	platform = myfunc::FindorMake(_root, "Platform")->GetNodeContent();
	chip_class = myfunc::FindorMake(_root, "Class")->GetNodeContent();
	coprocessor = myfunc::FindorMake(_root, "Coprocessor")->GetNodeContent();

	wxXmlNode* config_node = myfunc::FindorMake(_root, "Config");


	// 此处根据读入设置
	for (wxXmlNode* child = config_node->GetChildren(); child; child = child->GetNext()) {
		config.insert(std::make_pair(child->GetName(), child->GetNodeContent()));
		//if (config.find(child->GetName()) == config.end()) {
		//	wxLogError("匹配错误 %s和rva文件中的config描述不匹配 错误在%s", child->GetName(), __func__);
		//	continue;
		//}
		//config.find(child->GetName())->second = child->GetNodeContent();
	}
}

wxString Chip::toString() {
	wxString config_str = wxEmptyString;
	for (auto i : config) {
		wxString part = "\t" + i.first + ":\t" + i.second + "\n";
		config_str = config_str + part;
	}
	return
		"Platform:\t" + platform + "\n" +
		"Chip Class:\t" + chip_class + "\n" +
		"Name:\t" + name + "\n" +
		"Coprocessor:\t" + coprocessor + "\n" +
		"Config:\n" + config_str + "\n";
}

void Chip::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", name);
	myfunc::AddNode(root, "Platform", platform);
	myfunc::AddNode(root, "Class", chip_class);
	myfunc::AddNode(root, "Coprocessor", coprocessor);
	
	wxXmlNode* config_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Config");
	for (auto i : config) {
		myfunc::AddNode(config_node, i.first, i.second);
	}
	root->AddChild(config_node);
}


void Chip::ClearInfo() {
	name = wxEmptyString;
	platform = wxEmptyString;
	chip_class = wxEmptyString;
	coprocessor = wxEmptyString;
	config.clear();
}