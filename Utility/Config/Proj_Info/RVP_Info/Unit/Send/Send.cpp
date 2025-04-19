#include "Send.h"
#include "../../../../Func/func.h"

Send::Send(wxXmlNode* root) {
	Name = myfunc::FindSon(root, "Name")->GetNodeContent();
	Process = myfunc::FindSon(root, "Process")->GetNodeContent();
}

Send::Send(const wxString& _Name, const wxString& _Process)
	:Name(_Name), Process(_Process) {}

const wxString Send::toString(const wxString& n)const {
	return
		wxString::Format("[Send%s]\n", n) +
		"\tName:\t" + Name + "\n" +
		"\tProcess:\t" + Process + "\n";
}


void Send::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", Name);
	myfunc::AddNode(root, "Process", Process);
}