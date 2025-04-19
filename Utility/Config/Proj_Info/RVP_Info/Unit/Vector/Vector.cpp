#include "Vector.h"
#include "../../../../Func/func.h"

Vector::Vector(wxXmlNode* root) {
	Name = myfunc::FindSon(root, "Name")->GetNodeContent();
	Number = myfunc::FindSon(root, "Number")->GetNodeContent();
}

Vector::Vector(const wxString& _Name, const wxString& _Number)
	:Name(_Name), Number(_Number) {}


const wxString Vector::toString(const wxString& n)const {
	return
		wxString::Format("[Vector%s]\n", n) +
		"\tName:\t" + Name + "\n" +
		"\tNumber:\t" + Number + "\n";
}


void Vector::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", Name);
	myfunc::AddNode(root, "Number", Number);

}