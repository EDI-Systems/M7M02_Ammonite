#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

#include "../Controller/Controller.h"
#include "../PanelBase/PanelBase.h"


class Panel_Monitor
    :public PanelBase {  // 滚动窗口
public:
    Panel_Monitor(
        wxWindow* parent,
        wxWindowID id = -1,
        const wxString& errorloc = wxEmptyString,
        const bool& makeGUI = false,
        std::function<void()>func = nullptr);

    void SetMonitor();
    bool SaveMonitor();
    bool IsLegal()override;
    void ClearError()override;
    

private:
    SizeSlider* code_size;
    SizeSlider* data_size;
    SizeSlider* init_stack_size;
    SizeSlider* sftd_stack_size;
    SizeSlider* vmmd_stack_size;

    MyVirtPrio* virt_prio;
    MyVirtPrio* virt_event; // 判断条件和virt_prio一样，只是不要超过1024
    MyVirtMap* virt_map;

    MyChoice* optim;

    BoolBox* project_overw;
    BoolBox* hook_src_overw;

    MyZ_GE_Zero* extra_captbl;

    SimpleText* buildsys;
    SimpleText* toolchain;
    SimpleText* project_output;
    SimpleText* linker_output;
    SimpleText* conf_hdr_output;
    SimpleText* boot_hdr_output;
    SimpleText* boot_src_output;
    SimpleText* hook_src_output;

    void OnRefresh(const wxCommandEvent& event)override;
    void OnConfirm(const wxCommandEvent& event)override;
    void OnPreview(const wxCommandEvent& event)override;
};
