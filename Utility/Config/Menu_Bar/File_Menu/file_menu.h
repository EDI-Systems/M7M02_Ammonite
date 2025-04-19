#pragma once
#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/xml/xml.h>
#include <wx/textctrl.h>
class File_Menu
	:public wxMenu{
public:
	File_Menu(wxWindow* parent);

private:

	wxMenuItem* New_FIle;
	wxMenuItem* Open_File;
	wxMenuItem* Export_File;
	void NewFileFunc();
	void ImportFileFunc();
	bool ExportFileFunc();

	void OnNewFile(const wxCommandEvent& event);
	void OnImportFile(const wxCommandEvent& event);
	void OnExportFile(const wxCommandEvent& event);

	//wxWindow* _parent;
};

