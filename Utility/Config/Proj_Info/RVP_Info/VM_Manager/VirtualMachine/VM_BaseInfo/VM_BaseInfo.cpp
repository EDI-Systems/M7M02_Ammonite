#include "VM_BaseInfo.h"
#include "../../../../../Func/func.h"

VM_BaseInfo::VM_BaseInfo(wxXmlNode* root) {
	Name = myfunc::FindorMake(root, "Name")->GetNodeContent();
	Extra_Captbl = myfunc::FindorMake(root, "Extra_Captbl")->GetNodeContent();
	Coprocessor = myfunc::FindorMake(root, "Coprocessor")->GetNodeContent();
	Vector_Stack_Size = myfunc::FindorMake(root, "Vector_Stack_Size")->GetNodeContent();
	User_Stack_Size = myfunc::FindorMake(root, "User_Stack_Size")->GetNodeContent();
	Priority = myfunc::FindorMake(root, "Priority")->GetNodeContent();
	Timeslice = myfunc::FindorMake(root, "Timeslice")->GetNodeContent();
	Period = myfunc::FindorMake(root, "Period")->GetNodeContent();
	Watchdog = myfunc::FindorMake(root, "Watchdog")->GetNodeContent();
	Vector_Num = myfunc::FindorMake(root, "Vector_Num")->GetNodeContent();
	Guest_Type = myfunc::FindorMake(root, "Guest_Type")->GetNodeContent();
	Buildsystem = myfunc::FindorMake(root, "Buildsystem")->GetNodeContent();
	Toolchain = myfunc::FindorMake(root, "Toolchain")->GetNodeContent();
	Optimization = myfunc::FindorMake(root, "Optimization")->GetNodeContent();
	Project_Output = myfunc::FindorMake(root, "Project_Output")->GetNodeContent();
	Project_Overwrite = myfunc::FindorMake(root, "Project_Overwrite")->GetNodeContent();
	Linker_Output = myfunc::FindorMake(root, "Linker_Output")->GetNodeContent();
	Main_Header_Output = myfunc::FindorMake(root, "Main_Header_Output")->GetNodeContent();
	Main_Source_Output = myfunc::FindorMake(root, "Main_Source_Output")->GetNodeContent();
	Virtual_Header_Output = myfunc::FindorMake(root, "Virtual_Header_Output")->GetNodeContent();
	Virtual_Header_Overwrite = myfunc::FindorMake(root, "Virtual_Header_Overwrite")->GetNodeContent();
	Virtual_Source_Output = myfunc::FindorMake(root, "Virtual_Source_Output")->GetNodeContent();
	Virtual_Source_Overwrite = myfunc::FindorMake(root, "Virtual_Source_Overwrite")->GetNodeContent();

}
VM_BaseInfo::VM_BaseInfo(){}

wxArrayString VM_BaseInfo::GetCoprocessor()const {
	//wxArrayString ans = myfunc::Separater(Coprocessor, ",");
	//ans.push_back("None");
	//return ans;
	return myfunc::Separater(Coprocessor, ",");
}

const wxString VM_BaseInfo::toString()const {
	return
		"Name:\t" + Name + "\n"
		+ "Extra_Captbl:\t" + Extra_Captbl + "\n"
		+ "Coprocessor:\t" + Coprocessor + "\n"
		+ "Vector_Stack_Size:\t" + Vector_Stack_Size + "\n"
		+ "User_Stack_Size:\t" + User_Stack_Size + "\n"
		+ "Priority:\t" + Priority + "\n"
		+ "Timeslice:\t" + Timeslice + "\n"
		+ "Period:\t" + Period + "\n"
		+ "Watchdog:\t" + Watchdog + "\n"
		+ "Vector_Num:\t" + Vector_Num + "\n"
		+ "Guest_Type:\t" + Guest_Type + "\n"
		+ "Buildsystem:\t" + Buildsystem + "\n"
		+ "Toolchain:\t" + Toolchain + "\n"
		+ "Optimization:\t" + Optimization + "\n"
		+ "Project_Output:\t" + Project_Output + "\n"
		+ "Project_Overwrite:\t" + Project_Overwrite + "\n"
		+ "Linker_Output:\t" + Linker_Output + "\n"
		+ "Main_Header_Output:\t" + Main_Header_Output + "\n"
		+ "Main_Source_Output:\t" + Main_Source_Output + "\n"
		+ "Virtual_Header_Output:\t" + Virtual_Header_Output + "\n"
		+ "Virtual_Header_Overwrite:\t" + Virtual_Header_Overwrite + "\n"
		+ "Virtual_Source_Output:\t" + Virtual_Source_Output + "\n"
		+ "Virtual_Source_Overwrite:\t" + Virtual_Source_Overwrite;
}


void VM_BaseInfo::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", Name);
	myfunc::AddNode(root, "Extra Captbl", Extra_Captbl);
	myfunc::AddNode(root, "Coprocessor", Coprocessor);
	myfunc::AddNode(root, "Vector Stack Size", Vector_Stack_Size);
	myfunc::AddNode(root, "User Stack Size", User_Stack_Size);
	myfunc::AddNode(root, "Priority", Priority);
	myfunc::AddNode(root, "Timeslice", Timeslice);
	myfunc::AddNode(root, "Period", Period);
	myfunc::AddNode(root, "Watchdog", Watchdog);
	myfunc::AddNode(root, "Vector Num", Vector_Num);
	myfunc::AddNode(root, "Guest Type", Guest_Type);
	myfunc::AddNode(root, "Buildsystem", Buildsystem);
	myfunc::AddNode(root, "Toolchain", Toolchain);
	myfunc::AddNode(root, "Optimization", Optimization);
	myfunc::AddNode(root, "Project Output", Project_Output);
	myfunc::AddNode(root, "Project Overwrite", Project_Overwrite);
	myfunc::AddNode(root, "Linker Output", Linker_Output);
	myfunc::AddNode(root, "Main Header Output", Main_Header_Output);
	myfunc::AddNode(root, "Main Source Output", Main_Source_Output);
	myfunc::AddNode(root, "Virtual Header Output", Virtual_Header_Output);
	myfunc::AddNode(root, "Virtual Header Overwrite", Virtual_Header_Overwrite);
	myfunc::AddNode(root, "Virtual Source Output", Virtual_Source_Output);
	myfunc::AddNode(root, "Virtual Source Overwrite", Virtual_Source_Overwrite);
}