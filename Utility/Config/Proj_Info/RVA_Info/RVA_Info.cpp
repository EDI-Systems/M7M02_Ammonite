#include "RVA_Info.h"
#include "../../Func/func.h"
#include "../../main.h"

RVA_Info::RVA_Info(){

}

void RVA_Info::SetRVA(const wxString& path) {
	configs.clear();

	xml_root = PathToNode(path);

	this->name = myfunc::FindSon(xml_root, "Name")->GetNodeContent();

	// Buildsystem选项
	wxString buildsys_str = myfunc::FindSon(xml_root, "Buildsystem")->GetNodeContent();
	buildsys = myfunc::Separater(buildsys_str, ",");

	// Toolchain选项
	wxString toolchain_str = myfunc::FindSon(xml_root, "Toolchain")->GetNodeContent();
	toolchain = myfunc::Separater(toolchain_str, ",");

	// guest选项
	wxString guest_str = myfunc::FindSon(xml_root, "Guest")->GetNodeContent();
	guest = myfunc::Separater(guest_str, ",");

	// Compatible兼容性组合
	compatible.clear();
	wxXmlNode* cpt_node = myfunc::FindSon(xml_root, "Compatible");
	for (wxXmlNode* child = cpt_node->GetChildren(); child; child = child->GetNext()) {
		compatible.push_back(myfunc::Separater(child->GetNodeContent(), ","));
	}

	// Config参数
	wxXmlNode* config_node = myfunc::FindSon(xml_root, "Config");
	for (wxXmlNode* c = config_node->GetChildren(); c; c = c->GetNext()) {
		std::unique_ptr<RVA_Config>config = std::make_unique<RVA_Config>();
		// 其中Name是Chip的必填项
		config->name = myfunc::FindSon(c, "Name")->GetNodeContent();
		config->macro = myfunc::FindSon(c, "Macro")->GetNodeContent();
		config->type = myfunc::FindSon(c, "Type")->GetNodeContent();
		config->range = myfunc::Separater(myfunc::FindSon(c, "Range")->GetNodeContent(), ",");

		// 最后移交config的所有权
		this->configs.push_back(std::move(config));
	}
}

wxXmlNode* RVA_Info::PathToNode(const wxString& path) {
	wxXmlDocument* doc = new wxXmlDocument();
	if (doc->Load(path)) {
		return doc->DetachRoot();
	}
	wxLogError("%s下不是xml文件 错误在", path, __func__);
	return nullptr;
}