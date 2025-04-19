#pragma once
#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/xml/xml.h>


#include "Frame_Select/Frame_Select.h"

class Tool_Bar
	:public wxToolBar{
public :
	Tool_Bar(class wxWindow* Parent);
	void DisableRvaPreview() { EnableTool(rva_preveiw->GetId(), false); }
	void DisableRvcPreview() { EnableTool(rvc_preveiw->GetId(), false); }
	void EnableRvaPreview() { EnableTool(rva_preveiw->GetId(), true); }
	void EnableRvcPreview() { EnableTool(rvc_preveiw->GetId(), true); }
private:
	class wxToolBarToolBase* import_source;
	class wxToolBarToolBase* rva_preveiw;
	class wxToolBarToolBase* rvc_preveiw;

	void OnImportSrc(const wxCommandEvent& event);
	void OnRvaPreview(const wxCommandEvent& event);
	void OnRvcPreview(const wxCommandEvent& event);

	Frame_Select* select_frame;
};

