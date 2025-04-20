#include "Monitor.h"
#include "../../../Func/func.h"

void Monitor::SetMonitor(wxXmlNode* _root) {
	Code_Size = myfunc::FindorMake(_root, "Code_Size")->GetNodeContent();
	Data_Size = myfunc::FindorMake(_root, "Data_Size")->GetNodeContent();
	Init_Stack_Size = myfunc::FindorMake(_root, "Init_Stack_Size")->GetNodeContent();
	Sftd_Stack_Size = myfunc::FindorMake(_root, "Sftd_Stack_Size")->GetNodeContent();
	Vmmd_Stack_Size = myfunc::FindorMake(_root, "Vmmd_Stack_Size")->GetNodeContent();
	Extra_Captbl = myfunc::FindorMake(_root, "Extra_Captbl")->GetNodeContent();
	Virt_Prio = myfunc::FindorMake(_root, "Virt_Prio")->GetNodeContent();
	Virt_Event = myfunc::FindorMake(_root, "Virt_Event")->GetNodeContent();
	Virt_Map = myfunc::FindorMake(_root, "Virt_Map")->GetNodeContent();
	Buildsystem = myfunc::FindorMake(_root, "Buildsystem")->GetNodeContent();
	Toolchain = myfunc::FindorMake(_root, "Toolchain")->GetNodeContent();
	Optimization = myfunc::FindorMake(_root, "Optimization")->GetNodeContent();
	Project_Output = myfunc::FindorMake(_root, "Project_Output")->GetNodeContent();
	Project_Overwrite = myfunc::FindorMake(_root, "Project_Overwrite")->GetNodeContent();
	Linker_Output = myfunc::FindorMake(_root, "Linker_Output")->GetNodeContent();
	Config_Header_Output = myfunc::FindorMake(_root, "Config_Header_Output")->GetNodeContent();
	Boot_Header_Output = myfunc::FindorMake(_root, "Boot_Header_Output")->GetNodeContent();
	Boot_Source_Output = myfunc::FindorMake(_root, "Boot_Source_Output")->GetNodeContent();
	Hook_Source_Output = myfunc::FindorMake(_root, "Hook_Source_Output")->GetNodeContent();
	Hook_Source_Overwrite = myfunc::FindorMake(_root, "Hook_Source_Overwrite")->GetNodeContent();
}

wxString Monitor::toString() {
	return 
		"Code Size:\t" + Code_Size + "\n" +
		"Data Size:\t" + Data_Size + "\n" +
		"Init Stack Size:\t" + Init_Stack_Size + "\n" +
		"Sftd Stack Size:\t" + Sftd_Stack_Size + "\n" +
		"Vmmd Stack Size:\t" + Vmmd_Stack_Size + "\n" +
		"Extra Captbl:\t" + Extra_Captbl + "\n" +
		"Virt Prio:\t" + Virt_Prio + "\n" +
		"Virt Event:\t" + Virt_Event + "\n" +
		"Virt Map:\t" + Virt_Map + "\n" +
		"Buildsystem:\t" + Buildsystem + "\n" +
		"Toolchain:\t" + Toolchain + "\n" +
		"Optimization:\t" + Optimization + "\n" +
		"Project Output:\t" + Project_Output + "\n" +
		"Project Overwrite:\t" + Project_Overwrite + "\n" +
		"Linker Output:\t" + Linker_Output + "\n" +
		"Config Header Output:\t" + Config_Header_Output + "\n" +
		"Boot Header Output:\t" + Boot_Header_Output + "\n" +
		"Boot Source Output:\t" + Boot_Source_Output + "\n" +
		"Hook Source Output:\t" + Hook_Source_Output + "\n" +
		"Hook Source Overwrite:\t" + Hook_Source_Overwrite + "\n";
}


void Monitor::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Code Size", Code_Size);
	myfunc::AddNode(root, "Data Size", Data_Size);
	myfunc::AddNode(root, "Init Stack Size", Init_Stack_Size);
	myfunc::AddNode(root, "Sftd Stack Size", Sftd_Stack_Size);
	myfunc::AddNode(root, "Vmmd Stack Size", Vmmd_Stack_Size);
	myfunc::AddNode(root, "Extra Captbl", Extra_Captbl);
	myfunc::AddNode(root, "Virt Prio", Virt_Prio);
	myfunc::AddNode(root, "Virt Event", Virt_Event);
	myfunc::AddNode(root, "Virt Map", Virt_Map);
	myfunc::AddNode(root, "Buildsystem", Buildsystem);
	myfunc::AddNode(root, "Toolchain", Toolchain);
	myfunc::AddNode(root, "Optimization", Optimization);
	myfunc::AddNode(root, "Project Output", Project_Output);
	myfunc::AddNode(root, "Project Overwrite", Project_Overwrite);
	myfunc::AddNode(root, "Linker Output", Linker_Output);
	myfunc::AddNode(root, "Config Header Output", Config_Header_Output);
	myfunc::AddNode(root, "Boot Header Output", Boot_Header_Output);
	myfunc::AddNode(root, "Boot Source Output", Boot_Source_Output);
	myfunc::AddNode(root, "Hook Source Output", Hook_Source_Output);
	myfunc::AddNode(root, "Hook Source Overwrite", Hook_Source_Overwrite);

}


void Monitor::ClaerInfo() {
	Code_Size = wxEmptyString;
	Data_Size = wxEmptyString;
	Init_Stack_Size = wxEmptyString;
	Sftd_Stack_Size = wxEmptyString;
	Vmmd_Stack_Size = wxEmptyString;
	Extra_Captbl = wxEmptyString;
	Virt_Prio = wxEmptyString;
	Virt_Event = wxEmptyString;
	Virt_Map = wxEmptyString;
	Buildsystem = wxEmptyString;
	Toolchain = wxEmptyString;
	Optimization = wxEmptyString;
	Project_Output = wxEmptyString;
	Project_Overwrite = wxEmptyString;
	Linker_Output = wxEmptyString;
	Config_Header_Output = wxEmptyString;
	Boot_Header_Output = wxEmptyString;
	Boot_Source_Output = wxEmptyString;
	Hook_Source_Output = wxEmptyString;
	Hook_Source_Overwrite = wxEmptyString;
}