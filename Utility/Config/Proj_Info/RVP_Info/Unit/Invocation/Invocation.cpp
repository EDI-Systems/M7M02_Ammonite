#include "Invocation.h"
#include "../../../../Func/func.h"

Invocation::Invocation(wxXmlNode* root) {
	Name = myfunc::FindSon(root, "Name")->GetNodeContent();
	Stack_Size = myfunc::FindSon(root, "Stack_Size")->GetNodeContent();
}

Invocation::Invocation(const wxString& _Name, const wxString& _Stack_Size)
	:Name(_Name), Stack_Size(_Stack_Size) {}

const wxString Invocation::toString(const wxString& n)const {
	return
		wxString::Format("[Invocation%s]\n",n)+
		"\tName:\t" + Name + "\n" +
		"\tStack Size:\t" + Stack_Size + "\n";
}

void Invocation::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", Name);
	myfunc::AddNode(root, "Stack Size", Stack_Size);
}