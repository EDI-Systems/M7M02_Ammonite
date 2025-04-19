#include "Kernel.h"
#include "../../Func/func.h"


void Kernel::SetKernel(wxXmlNode* _root) {

	Code_Base = myfunc::FindorMake(_root, "Code_Base")->GetNodeContent();
	Code_Size = myfunc::FindorMake(_root, "Code_Size")->GetNodeContent();
	Data_Base = myfunc::FindorMake(_root, "Data_Base")->GetNodeContent();
	Data_Size = myfunc::FindorMake(_root, "Data_Size")->GetNodeContent();
	Stack_Size = myfunc::FindorMake(_root, "Stack_Size")->GetNodeContent();
	Extra_Kom = myfunc::FindorMake(_root, "Extra_Kom")->GetNodeContent();
	Kom_Order = myfunc::FindorMake(_root, "Kom_Order")->GetNodeContent();
	Kern_Prio = myfunc::FindorMake(_root, "Kern_Prio")->GetNodeContent();
	Buildsystem = myfunc::FindorMake(_root, "Buildsystem")->GetNodeContent();
	Toolchain = myfunc::FindorMake(_root, "Toolchain")->GetNodeContent();
	Optimization = myfunc::FindorMake(_root, "Optimization")->GetNodeContent();
	Full_Image = myfunc::FindorMake(_root, "Full_Image")->GetNodeContent();
	Project_Output = myfunc::FindorMake(_root, "Project_Output")->GetNodeContent();
	Project_Overwrite = myfunc::FindorMake(_root, "Project_Overwrite")->GetNodeContent();
	Linker_Output = myfunc::FindorMake(_root, "Linker_Output")->GetNodeContent();
	Config_Header_Output = myfunc::FindorMake(_root, "Config_Header_Output")->GetNodeContent();
	Boot_Header_Output = myfunc::FindorMake(_root, "Boot_Header_Output")->GetNodeContent();
	Boot_Source_Output = myfunc::FindorMake(_root, "Boot_Source_Output")->GetNodeContent();
	Hook_Source_Output = myfunc::FindorMake(_root, "Hook_Source_Output")->GetNodeContent();
	Hook_Source_Overwrite = myfunc::FindorMake(_root, "Hook_Source_Overwrite")->GetNodeContent();
	Handler_Source_Output = myfunc::FindorMake(_root, "Handler_Source_Output")->GetNodeContent();
	Handler_Source_Overwrite = myfunc::FindorMake(_root, "Handler_Source_Overwrite")->GetNodeContent();

}


wxString Kernel::toString() {
	return 
		"Code Base:\t" + Code_Base + "\n" +
		"Code Size:\t" + Code_Size + "\n" +
		"Data Base:\t" + Data_Base + "\n" +
		"Data Size:\t" + Data_Size + "\n" +
		"Stack Size:\t" + Stack_Size + "\n" +
		"Extra Kom:\t" + Extra_Kom + "\n" +
		"Kom Order:\t" + Kom_Order + "\n" +
		"Kern Prio:\t" + Kern_Prio + "\n" +
		"Buildsystem:\t" + Buildsystem + "\n" +
		"Toolchain:\t" + Toolchain + "\n" +
		"Optimization:\t" + Optimization + "\n" +
		"Full Image:\t" + Full_Image + "\n" +
		"Project Output:\t" + Project_Output + "\n" +
		"Project Overwrite:\t" + Project_Overwrite + "\n" +
		"Linker Output:\t" + Linker_Output + "\n" +
		"Config Header Output:\t" + Config_Header_Output + "\n" +
		"Boot Header Output:\t" + Boot_Header_Output + "\n" +
		"Boot Source Output:\t" + Boot_Source_Output + "\n" +
		"Hook Source Output:\t" + Hook_Source_Output + "\n" +
		"Hook Source Overwrite:\t" + Hook_Source_Overwrite + "\n" +
		"Handler Source Output:\t" + Handler_Source_Output + "\n" +
		"Handler Source Overwrite:\t" + Handler_Source_Overwrite + "\n";
}

void Kernel::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Code Base", Code_Base);
	myfunc::AddNode(root, "Code Size", Code_Size);
	myfunc::AddNode(root, "Data Base", Data_Base);
	myfunc::AddNode(root, "Data Size", Data_Size);
	myfunc::AddNode(root, "Stack Size", Stack_Size);
	myfunc::AddNode(root, "Extra Kom", Extra_Kom);
	myfunc::AddNode(root, "Kom Order", Kom_Order);
	myfunc::AddNode(root, "Kern Prio", Kern_Prio);
	myfunc::AddNode(root, "Buildsystem", Buildsystem);
	myfunc::AddNode(root, "Toolchain", Toolchain);
	myfunc::AddNode(root, "Optimization", Optimization);
	myfunc::AddNode(root, "Full Image", Full_Image);
	myfunc::AddNode(root, "Project Output", Project_Output);
	myfunc::AddNode(root, "Project Overwrite", Project_Overwrite);
	myfunc::AddNode(root, "Linker Output", Linker_Output);
	myfunc::AddNode(root, "Config Header Output", Config_Header_Output);
	myfunc::AddNode(root, "Boot Header Output", Boot_Header_Output);
	myfunc::AddNode(root, "Boot Source Output", Boot_Source_Output);
	myfunc::AddNode(root, "Hook Source Output", Hook_Source_Output);
	myfunc::AddNode(root, "Hook Source Overwrite", Hook_Source_Overwrite);
	myfunc::AddNode(root, "Handler Source Output", Handler_Source_Output);
	myfunc::AddNode(root, "Handler Source Overwrite", Handler_Source_Overwrite);
}


void Kernel::ClearInfo() {
	Code_Base = wxEmptyString;
	Code_Size = wxEmptyString;
	Data_Base = wxEmptyString;
	Data_Size = wxEmptyString;
	Stack_Size = wxEmptyString;
	Extra_Kom = wxEmptyString;
	Kom_Order = wxEmptyString;
	Kern_Prio = wxEmptyString;
	Buildsystem = wxEmptyString;
	Toolchain = wxEmptyString;
	Optimization = wxEmptyString;
	Full_Image = wxEmptyString;
	Project_Output = wxEmptyString;
	Project_Overwrite = wxEmptyString;
	Linker_Output = wxEmptyString;
	Config_Header_Output = wxEmptyString;
	Boot_Header_Output = wxEmptyString;
	Boot_Source_Output = wxEmptyString;
	Hook_Source_Output = wxEmptyString;
	Hook_Source_Overwrite = wxEmptyString;
	Handler_Source_Output = wxEmptyString;
	Handler_Source_Overwrite = wxEmptyString;
}