#include "Kfunc.h"
#include "../../../../Func/func.h"

Kfunc::Kfunc(wxXmlNode* root) {
	Name = myfunc::FindSon(root, "Name")->GetNodeContent();
	Begin = myfunc::FindSon(root, "Begin")->GetNodeContent();
	End = myfunc::FindSon(root, "End")->GetNodeContent();
}

Kfunc::Kfunc(const wxString& _Name, const wxString& _Begin, const wxString& _End)
	:Name(_Name), Begin(_Begin), End(_End) {}

const wxString Kfunc::toString(const wxString& n)const {
	return
		wxString::Format("[Kfunc%s]\n", n) +
		"\tName:\t" + Name + "\n" +
		"\tBegin:\t" + Begin + "\n" +
		"\tEnd:\t" + End + "\n";
}

void Kfunc::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", Name);
	myfunc::AddNode(root, "Begin", Begin);
	myfunc::AddNode(root, "End", End);
}