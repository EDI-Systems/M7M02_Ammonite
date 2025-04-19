#include "Native_BaseInfo.h"
#include "../../../../../Func/func.h"

Native_BaseInfo::Native_BaseInfo(wxXmlNode* root) {

	Name = myfunc::FindorMake(root, "Name")->GetNodeContent();
	Extra_Captbl = myfunc::FindorMake(root, "Extra_Captbl")->GetNodeContent();
	Coprocessor = myfunc::FindorMake(root, "Coprocessor")->GetNodeContent();
	Buildsystem = myfunc::FindorMake(root, "Buildsystem")->GetNodeContent();
	Toolchain = myfunc::FindorMake(root, "Toolchain")->GetNodeContent();
	Optimization = myfunc::FindorMake(root, "Optimization")->GetNodeContent();
	Project_Output = myfunc::FindorMake(root, "Project_Output")->GetNodeContent();
	Project_Overwrite = myfunc::FindorMake(root, "Project_Overwrite")->GetNodeContent();
	Linker_Output = myfunc::FindorMake(root, "Linker_Output")->GetNodeContent();
	Main_Header_Output = myfunc::FindorMake(root, "Main_Header_Output")->GetNodeContent();
	Main_Source_Output = myfunc::FindorMake(root, "Main_Source_Output")->GetNodeContent();
	Entry_Source_Output = myfunc::FindorMake(root, "Entry_Source_Output")->GetNodeContent();
	Entry_Source_Overwrite = myfunc::FindorMake(root, "Entry_Source_Overwrite")->GetNodeContent();
}

Native_BaseInfo::Native_BaseInfo(){}

wxArrayString Native_BaseInfo::GetCoprocessor()const {
	return myfunc::Separater(Coprocessor, ",");
}

wxString Native_BaseInfo::toString() {
	return 
		"Name:\t" + Name + "\n" +
		"Extra Captbl:\t" + Extra_Captbl + "\n" +
		"Coprocessor:\t" + Coprocessor + "\n" +
		"Buildsystem:\t" + Buildsystem + "\n" +
		"Toolchain:\t" + Toolchain + "\n" +
		"Optimization:\t" + Optimization + "\n" +
		"Project Output:\t" + Project_Output + "\n" +
		"Project Overwrite:\t" + Project_Overwrite + "\n" +
		"Linker Output:\t" + Linker_Output + "\n" +
		"Main Header Output:\t" + Main_Header_Output + "\n" +
		"Main Source Output:\t" + Main_Source_Output + "\n" +
		"Entry Source Output:\t" + Entry_Source_Output + "\n" +
		"Entry Source Overwrite:\t" + Entry_Source_Overwrite + "\n";
}

void Native_BaseInfo::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", Name);
	myfunc::AddNode(root, "Extra Captbl", Extra_Captbl);
	myfunc::AddNode(root, "Coprocessor", Coprocessor);
	myfunc::AddNode(root, "Buildsystem", Buildsystem);
	myfunc::AddNode(root, "Toolchain", Toolchain);
	myfunc::AddNode(root, "Optimization", Optimization);
	myfunc::AddNode(root, "Project Output", Project_Output);
	myfunc::AddNode(root, "Project Overwrite", Project_Overwrite);
	myfunc::AddNode(root, "Linker Output", Linker_Output);
	myfunc::AddNode(root, "Main Header Output", Main_Header_Output);
	myfunc::AddNode(root, "Main Source Output", Main_Source_Output);
	myfunc::AddNode(root, "Entry Source Output", Entry_Source_Output);
	myfunc::AddNode(root, "Entry Source Overwrite", Entry_Source_Overwrite);

}