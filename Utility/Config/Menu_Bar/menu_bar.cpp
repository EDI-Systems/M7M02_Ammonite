#include "menu_bar.h"
Menu_Bar::Menu_Bar()
	:wxMenuBar(){
	this->file = new File_Menu(this);
	this->Append(this->file, "�ļ�");

	//this->others = new Others(this);
	//this->Append(this->others, "����");
}
