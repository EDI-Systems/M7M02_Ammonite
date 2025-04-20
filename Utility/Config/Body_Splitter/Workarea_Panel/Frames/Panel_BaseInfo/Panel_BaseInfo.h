#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

#include "../Controller/Controller.h"
#include "../PanelBase/PanelBase.h"
#include "../Frames.h"
//#include "../../../../main.h"

class Panel_BaseInfo
    :public PanelBase {
public:
    Panel_BaseInfo(
        wxWindow* parent,
        wxWindowID id = -1,
        const wxString& errorloc = wxEmptyString,
        const bool& makeGUI = false,
        std::function<void()>func = nullptr);
    void SetBaseInfo();
    bool SaveBaseInfo();
    bool IsLegal()override;
    void ClearError()override;

private:
    // 控件
    SimpleText* name;

    MyVersion* version;

    BoolBox* assert_enable;
    BoolBox* debug_enable;
    BoolBox* pagetble_enable;
    BoolBox* workplace_overwrite;

    // 平台兼容性限制 buildsys toolchain guest
    MyGrid* compatible;

    void OnRefresh(const wxCommandEvent& event)override;
    void OnConfirm(const wxCommandEvent& event)override;
    void OnPreview(const wxCommandEvent& event)override;

};
