#include "Memory.h"
#include "../../../../Func/func.h"

Memory::Memory(wxXmlNode* root){
	Name = myfunc::FindSon(root, "Name")->GetNodeContent();
	Base = myfunc::FindSon(root, "Base")->GetNodeContent();
	Size = myfunc::FindSon(root, "Size")->GetNodeContent();
	Align = myfunc::FindSon(root, "Align")->GetNodeContent();
	Type = myfunc::FindSon(root, "Type")->GetNodeContent();
	Attribute = myfunc::FindSon(root, "Attribute")->GetNodeContent();

}

Memory::Memory(
	const wxString& _Name, const wxString& _Base,
	const wxString& _Size, const wxString& _Align,
	const wxString& _Type, const wxString& _Attribute)
	:Name(_Name), Base(_Base), Size(_Size), Align(_Align), Type(_Type), Attribute(_Attribute) {}

const wxString Memory::toString(const wxString& n)const {
	return
		wxString::Format("[Memory%s]\n", n) +
		"\tName:\t" + Name + "\n" +
		"\tBase:\t" + Base + "\n" +
		"\tSize:\t" + Size + "\n" +
		"\tAlign:\t" + Align + "\n" +
		"\tType:\t" + Type + "\n" +
		"\tAttribute:\t" + Attribute + "\n";
}

void Memory::AddInfo(wxXmlNode*& root) {
	myfunc::AddNode(root, "Name", Name);
	myfunc::AddNode(root, "Base", Base);
	myfunc::AddNode(root, "Size", Size);
	myfunc::AddNode(root, "Align", Align);
	myfunc::AddNode(root, "Type", Type);
	myfunc::AddNode(root, "Attribute", Attribute);
}