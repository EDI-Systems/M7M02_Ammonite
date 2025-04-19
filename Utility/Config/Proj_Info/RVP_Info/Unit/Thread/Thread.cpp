#include "Thread.h"
#include "../../../../Func/func.h"

Thread::Thread(wxXmlNode* root) {

	Name = myfunc::FindSon(root, "Name")->GetNodeContent();
	Stack_Size = myfunc::FindSon(root, "Stack_Size")->GetNodeContent();
	Parameter = myfunc::FindSon(root, "Parameter")->GetNodeContent();
	Priority = myfunc::FindSon(root, "Priority")->GetNodeContent();
}

Thread::Thread(const wxString& _Name, const wxString& _Stack_Size, const wxString& _Parameter, const wxString& _Priority)
	:Name(_Name), Stack_Size(_Stack_Size), Parameter(_Parameter), Priority(_Priority) {}

const wxString Thread::toString(const wxString& n)const {
	return
		wxString::Format("[Thread%s]\n", n) +
		"\tName:\t" + Name + "\n" +
		"\tStack_Size:\t" + Stack_Size + "\n" +
		"\tParameter:\t" + Parameter + "\n" +
		"\tPriority:\t" + Priority + "\n";
}


void Thread::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", Name);
	myfunc::AddNode(root, "Stack Size", Stack_Size);
	myfunc::AddNode(root, "Parameter", Parameter);
	myfunc::AddNode(root, "Priority", Priority);

}
