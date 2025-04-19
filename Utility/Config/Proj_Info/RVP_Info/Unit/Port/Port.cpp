#include "Port.h"
#include "../../../../Func/func.h"

Port::Port(wxXmlNode* root) {
	Name = myfunc::FindSon(root, "Name")->GetNodeContent();
	Process = myfunc::FindSon(root, "Process")->GetNodeContent();
}

Port::Port(const wxString& _Name, const wxString& _Process)
	:Name(_Name), Process(_Process) {}

const wxString Port::toString(const wxString& n)const {
	return
		wxString::Format("[Port%s]\n", n) +
		"\tName:\t" + Name + "\n" +
		"\tProcess:\t" + Process + "\n";
}

void Port::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", Name);
	myfunc::AddNode(root, "Process", Process);

}