#include "RVC_Info.h"
#include "../../Func/func.h"

RVC_Info::RVC_Info(){

}

void RVC_Info::SetRVC(const wxString& path) {
	wxXmlDocument* doc = new wxXmlDocument();
	if (doc->Load(path)) { // 读取 XML 文件

		xml_root = doc->DetachRoot();

		name = myfunc::FindSon(xml_root, "Name")->GetNodeContent();

		compatible.clear();
		compatible = myfunc::Separater(myfunc::FindorMake(xml_root, "Compatible")->GetNodeContent(), ",");
		compatible.push_back("None");

		coprocessor.clear();
		coprocessor = myfunc::Separater(myfunc::FindorMake(xml_root, "Coprocessor")->GetNodeContent(), ",");
		coprocessor.push_back("None");


	}
	else {
		wxLogError("%s路径下找不到rvc文件 错误在", path, __func__);
	}

}


wxXmlNode* RVC_Info::PathToNode(const wxString& path) {
	wxXmlDocument* doc = new wxXmlDocument();
	if (doc->Load(path)) {
		return doc->DetachRoot();
	}
	wxLogError("%s下不是xml文件 错误在", path, __func__);
	return nullptr;
}