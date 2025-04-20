#include "Shmem.h"
#include "../../../../Func/func.h"

Shmem::Shmem(wxXmlNode* root) {
	Name = myfunc::FindSon(root, "Name")->GetNodeContent();
	Attribute = myfunc::FindSon(root, "Attribute")->GetNodeContent();
}

Shmem::Shmem(const wxString& _Name, const wxString& _Attribute)
	:Name(_Name), Attribute(_Attribute) {}

const wxString Shmem::toString(const wxString& n)const {
	return
		wxString::Format("[Shmem%s]\n", n) +
		"\tName:\t" + Name + "\n" +
		"\tAttribute:\t" + Attribute + "\n";
}


void Shmem::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", Name);
	myfunc::AddNode(root, "Attribute", Attribute);
}
