#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <map>

#include "../PanelBase/PanelBase.h"
#include "../Controller/Controller.h"

class Panel_Extmem
	:public PanelBase {
public:
	Panel_Extmem(
        wxWindow* parent,
        wxWindowID id = -1,
        const wxString& errorloc = wxEmptyString,
        const bool& makeGUI = false,
        std::function<void()>func = nullptr);

	void SetExtmem();
    bool SaveExtmem();
    bool IsLegal()override;
    void ClearError()override;

private:

    BaseSlider* extmem_base;
    SizeSlider* extmem_size;
    MyChoice* extmem_type;
    MyAttribute* extmem_attribute;

    void OnRefresh(const wxCommandEvent& event)override;
    void OnPreview(const wxCommandEvent& event)override;
    void OnConfirm(const wxCommandEvent& event)override;
};