#pragma once
#include <wx/wx.h>
class Frame_Select
    :public wxFrame {
public:
    Frame_Select(wxWindow* parent);
    void SetArray(const wxString& path, wxArrayString& arr);
    void SetChoicer(wxChoice* choicer, const wxArrayString& arr, int default_id = 0);

    wxButton* btn_select;
    wxChoice* choicer_platform;
    wxChoice* choicer_chip;

    wxArrayString platforms;
    wxArrayString chips;
private:
    void OnSelectPlatformFolder(const wxCommandEvent& event);
    void OnSelectPlatform(const wxCommandEvent& event);
    void OnOk(const wxCommandEvent& event);
    void OnClose(const wxCloseEvent& event);
    bool select_done;
    wxString dir_path;
};
