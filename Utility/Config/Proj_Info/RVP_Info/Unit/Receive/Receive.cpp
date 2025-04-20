#include "Receive.h"
#include "../../../../Func/func.h"
Receive::Receive(wxXmlNode* root) {
	Name = myfunc::FindSon(root, "Name")->GetNodeContent();
}

Receive::Receive(const wxString& _Name)
	:Name(_Name) {}


void Receive::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", Name);
}
