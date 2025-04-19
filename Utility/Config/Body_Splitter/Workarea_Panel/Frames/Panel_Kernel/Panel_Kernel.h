#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <map>

#include "../PanelBase/PanelBase.h"
#include "../Controller/Controller.h"


class Panel_Kernel
    :public PanelBase {
public:
    Panel_Kernel(
        wxWindow* parent,
        wxWindowID id = -1,
        const wxString& errorloc = wxEmptyString,
        const bool& makeGUI = false,
        std::function<void()>func = nullptr);

    void SetKernel();
    bool SaveKernel();
    bool IsLegal()override;
    void ClearError()override;

private:
    MyChoice* choice_kom_order;
    MyChoice* kern_prio;
    MyChoice* k_optim;

    BaseSlider* code_base;
    BaseSlider* data_base;

    SizeSlider* code_size;
    SizeSlider* data_size;
    SizeSlider* stack_size;
    SizeSlider* extra_kom;


    BoolBox* full_image;
    BoolBox* project_overw;
    BoolBox* hook_src_overw;
    BoolBox* handler_scr_overw;

    SimpleText* buildsys;
    SimpleText* toolchain;
    SimpleText* project_output;
    SimpleText* linker_output;
    SimpleText* conf_hdr_output;
    SimpleText* boot_hdr_output;
    SimpleText* boot_src_output;
    SimpleText* hook_src_output;
    SimpleText* handler_src_output;

    std::map<wxString, wxCheckBox*>buildsystem_choice;
    std::map<wxString, wxCheckBox*>toolchain_choice;


    void OnPreview(const wxCommandEvent& event)override;
    void OnConfirm(const wxCommandEvent& event)override;
    void OnRefresh(const wxCommandEvent& event)override;
};
