#include "Extmem.h"
#include "../../Func/func.h"

Extmem::Extmem() {

}

void Extmem::SetExtmem(wxXmlNode* _root) {

}

void Extmem::AddInfo(wxXmlNode*& root) {

	myfunc::AddNode(root, "Base", Base);
	myfunc::AddNode(root, "Size", Size);
	myfunc::AddNode(root, "Type", Type);
	myfunc::AddNode(root, "Attribute", Attribute);
}

const wxString Extmem::toString()const {
	return
		"Base:\t" + Base + "\n" +
		"Size:\t" + Size + "\n" +
		"Type:\t" + Type + "\n" +
		"Attribute:\t" + Attribute + "\n";
}

void Extmem::ClearInfo() {
	Base = wxEmptyString;
	Size = wxEmptyString;
	Type = wxEmptyString;
	Attribute = wxEmptyString;
}