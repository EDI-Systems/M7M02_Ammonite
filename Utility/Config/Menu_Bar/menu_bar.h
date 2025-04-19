#pragma once
#include <wx/wx.h>
#include <wx/menu.h>
#include "File_Menu/file_menu.h"
class Menu_Bar
	:public wxMenuBar {
public:
	Menu_Bar();
	void DisableFileMenu() { EnableTop(0, false); }
	void EnableFileMenu() { EnableTop(0, true); }
private:
	File_Menu* file;
};

