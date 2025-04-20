#pragma once
#include <wx/wx.h>
#include <map>

class ErrorLogPanel
    :public wxPanel {
public:
    ErrorLogPanel(wxWindow* parent);
    void AddLog(const int& sign, const wxString& str, wxString loc = wxEmptyString);
    void DeleteLog(const int& sign);
    void Clear();
private:
    wxTextCtrl* text;
    std::map<int, wxString> records;
    void FillRecords();
};
