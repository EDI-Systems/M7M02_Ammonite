#include "ErrorLogPanel.h"

ErrorLogPanel::ErrorLogPanel(wxWindow* parent)
    :wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN) {
    //SetScrollRate(10, 10);
    wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

    box->Add(text, 1, wxEXPAND);
    SetSizer(box);
    FitInside();
}

void ErrorLogPanel::FillRecords() {
    text->Clear();
    wxString ans;
    for (auto i : records)
        ans += (i.second + "\n");
    text->SetValue(ans);
}

void ErrorLogPanel::AddLog(const int& sign, const wxString& str, wxString loc) {
    // ͬһ��sign��Log�����ֻ�����һ��Error
    DeleteLog(sign);// ��ɾ���ÿؼ�����һ����������ʾ��ǰ���ϵĴ���
    records.insert(std::make_pair(sign, wxString::Format("��Error��" + loc + "\t" + str + " (%d)", sign)));
    FillRecords();
}

void ErrorLogPanel::DeleteLog(const int& sign) {
    if (records.find(sign) != records.end())
        records.erase(records.find(sign));
    FillRecords();
}


void ErrorLogPanel::Clear() {
    records.clear();
    text->SetValue(wxEmptyString);
}