#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <map>

#include "../PanelBase/PanelBase.h"
#include "../Controller/Controller.h"

class Panel_Chip
	:public PanelBase {
public:
	Panel_Chip(
		wxWindow* parent,
		wxWindowID id = -1,
		const wxString& errorloc = wxEmptyString,
		const bool& makeGUI = false,
		std::function<void()>func = nullptr);

	void SetChip();
	bool SaveChip();
	bool IsLegal()override;
	void ClearError()override;

private:
	SimpleText* platform;
	SimpleText* chip_class;
	MyChoice* name;
	MyCoprocessor* coprocessor;
	wxStaticBoxSizer* stbox_config;
	std::map<wxString, NotEmpty*>configs; 


	void OnRefresh(const wxCommandEvent& event)override;
	void OnPreview(const wxCommandEvent& event)override;
	void OnConfirm(const wxCommandEvent& event)override;

};