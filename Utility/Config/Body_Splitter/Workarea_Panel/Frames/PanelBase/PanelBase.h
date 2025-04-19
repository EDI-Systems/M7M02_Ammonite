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
    bool makeGUI;   // ��ǰwxPanel����ʾ ���������Ϣ

protected:
    virtual void OnConfirm(const wxCommandEvent& event) {}; // ȷ��
    virtual void OnPreview(const wxCommandEvent& event) {}; // Ԥ��
    virtual void OnRefresh(const wxCommandEvent& event) {}; // ˢ��
    virtual bool IsLegal() = 0; // �ж�GUI����Ϣ�Ƿ�Ϸ�
    virtual void ClearError() = 0;  // ���GUI�еı�����Ϣ
    wxBoxSizer* box;    // ScrolledWindow�е�BoxSizer
    wxBoxSizer* highbox;    // ��ҳ���е�BoxSizer ����ScrolledWindow�Ͱ�ť
    wxScrolledWindow* swindow;
    wxString errorloc;  // ���ִ����λ��

    std::function<void()> func;
};