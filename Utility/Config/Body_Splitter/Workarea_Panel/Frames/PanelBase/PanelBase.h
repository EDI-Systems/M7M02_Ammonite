#pragma once
#include <wx/wx.h>

class PanelBase
    :public wxPanel {
public:
    PanelBase(
        wxWindow* parent,
        wxWindowID id,
        const wxString& _errorloc = wxEmptyString,
        const bool& _makeGUI = false,
        std::function<void()> func = nullptr);
    bool makeGUI;   // 当前wxPanel被显示 且填充了信息

protected:
    virtual void OnConfirm(const wxCommandEvent& event) {}; // 确认
    virtual void OnPreview(const wxCommandEvent& event) {}; // 预览
    virtual void OnRefresh(const wxCommandEvent& event) {}; // 刷新
    virtual bool IsLegal() = 0; // 判断GUI中信息是否合法
    virtual void ClearError() = 0;  // 清除GUI中的报错信息
    wxBoxSizer* box;    // ScrolledWindow中的BoxSizer
    wxBoxSizer* highbox;    // 本页面中的BoxSizer 管理ScrolledWindow和按钮
    wxScrolledWindow* swindow;
    wxString errorloc;  // 出现错误的位置

    std::function<void()> func;
};