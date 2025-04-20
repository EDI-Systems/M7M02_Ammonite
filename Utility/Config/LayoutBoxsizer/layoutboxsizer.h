#pragma once
#include <wx/wx.h>
class LayoutBoxsizer :public wxFrame
{
public:
    LayoutBoxsizer(const wxString& title) :wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(680, 280))
    {
        // ��Ϊ����frame����䣬Ҳ�ǽ����������ӿؼ���parent����
        wxPanel* panel = new wxPanel(this, wxID_ANY);
        panel->SetBackgroundColour(*wxRED); // ����۲�

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);  // ��ֱ

        wxStaticText* st = new wxStaticText(panel, wxID_ANY, "Class Name");
        wxTextCtrl* text = new wxTextCtrl(panel, wxID_ANY);

        wxBoxSizer* boxClassName = new wxBoxSizer(wxHORIZONTAL);    // ˮƽ
        boxClassName->Add(st, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);   // ˮƽ����|��ֱ����
        boxClassName->Add(text, 1);
        vbox->Add(boxClassName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
        vbox->Add(wxID_ANY, 10);//�հ׿�
        // �м���һ����ɫ��panel
        wxPanel* panel_blue = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(0, 10));
        panel_blue->SetBackgroundColour(*wxBLUE);
        vbox->Add(panel_blue, 0, wxEXPAND | wxTOP | wxBOTTOM, 5);

        wxStaticText* stMatch = new wxStaticText(panel, wxID_ANY, "Matching Classes");
        wxBoxSizer* boxMatch = new wxBoxSizer(wxHORIZONTAL);    // ��ֱ
        boxMatch->Add(stMatch);
        vbox->Add(boxMatch, 0, wxLEFT | wxTOP, 10);

        wxTextCtrl* textMatch = new wxTextCtrl(panel, wxID_ANY, "",
            wxPoint(-1, -1), wxSize(-1, -1), wxTE_MULTILINE);
        wxBoxSizer* boxText = new wxBoxSizer(wxHORIZONTAL);     // ��ֱ
        boxText->Add(textMatch, 1, wxEXPAND);   // �������չ��������ÿռ�
        vbox->Add(boxText, 1, wxLEFT | wxRIGHT | wxEXPAND, 10); // �������չ��������ÿռ�
        vbox->Add(wxID_ANY, 25);//�հ׿�

        // ������ѡ��
        wxCheckBox* cb1 = new wxCheckBox(panel, wxID_ANY, "Case Sensitive");
        wxCheckBox* cb2 = new wxCheckBox(panel, wxID_ANY, "Nested Classes");
        wxCheckBox* cb3 = new wxCheckBox(panel, wxID_ANY, "Non_Project Classes");
        wxBoxSizer* boxCK = new wxBoxSizer(wxHORIZONTAL);   // ˮƽ
        boxCK->Add(cb1, 1);
        boxCK->Add(cb2, 1);
        boxCK->Add(cb3, 1);
        vbox->Add(boxCK, 0, wxLEFT, 10);
        vbox->Add(wxID_ANY, 25);//�հ׿�

        // ����������ť
        wxButton* btnOK = new wxButton(panel, wxID_ANY, "OK");
        wxButton* btnClose = new wxButton(panel, wxID_ANY, "Close");
        wxBoxSizer* boxBtn = new wxBoxSizer(wxHORIZONTAL);
        boxBtn->Add(btnOK, 0);
        boxBtn->Add(btnClose, 0, wxLEFT | wxBOTTOM, 10);
        vbox->Add(boxBtn, 0, wxRIGHT | wxALIGN_RIGHT, 10);

        panel->SetSizer(vbox);


        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LayoutBoxsizer::OnQuit, this, btnClose->GetId());

        this->Center(); // ��ǰframe���ɵ�ʱ��λ����Ļ����
    }

protected:
    void OnQuit(wxCommandEvent& event)
    {
        Close(true);
    }

};