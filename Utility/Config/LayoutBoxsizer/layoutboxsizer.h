#pragma once
#include <wx/wx.h>
class LayoutBoxsizer :public wxFrame
{
public:
    LayoutBoxsizer(const wxString& title) :wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(680, 280))
    {
        // 作为整个frame的填充，也是接下来所有子控件的parent窗口
        wxPanel* panel = new wxPanel(this, wxID_ANY);
        panel->SetBackgroundColour(*wxRED); // 方便观察

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);  // 垂直

        wxStaticText* st = new wxStaticText(panel, wxID_ANY, "Class Name");
        wxTextCtrl* text = new wxTextCtrl(panel, wxID_ANY);

        wxBoxSizer* boxClassName = new wxBoxSizer(wxHORIZONTAL);    // 水平
        boxClassName->Add(st, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);   // 水平靠左|垂直居中
        boxClassName->Add(text, 1);
        vbox->Add(boxClassName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
        vbox->Add(wxID_ANY, 10);//空白框
        // 中间横插一条蓝色的panel
        wxPanel* panel_blue = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(0, 10));
        panel_blue->SetBackgroundColour(*wxBLUE);
        vbox->Add(panel_blue, 0, wxEXPAND | wxTOP | wxBOTTOM, 5);

        wxStaticText* stMatch = new wxStaticText(panel, wxID_ANY, "Matching Classes");
        wxBoxSizer* boxMatch = new wxBoxSizer(wxHORIZONTAL);    // 垂直
        boxMatch->Add(stMatch);
        vbox->Add(boxMatch, 0, wxLEFT | wxTOP, 10);

        wxTextCtrl* textMatch = new wxTextCtrl(panel, wxID_ANY, "",
            wxPoint(-1, -1), wxSize(-1, -1), wxTE_MULTILINE);
        wxBoxSizer* boxText = new wxBoxSizer(wxHORIZONTAL);     // 垂直
        boxText->Add(textMatch, 1, wxEXPAND);   // 竖向可扩展，充分利用空间
        vbox->Add(boxText, 1, wxLEFT | wxRIGHT | wxEXPAND, 10); // 横向可扩展，充分利用空间
        vbox->Add(wxID_ANY, 25);//空白框

        // 三个勾选框
        wxCheckBox* cb1 = new wxCheckBox(panel, wxID_ANY, "Case Sensitive");
        wxCheckBox* cb2 = new wxCheckBox(panel, wxID_ANY, "Nested Classes");
        wxCheckBox* cb3 = new wxCheckBox(panel, wxID_ANY, "Non_Project Classes");
        wxBoxSizer* boxCK = new wxBoxSizer(wxHORIZONTAL);   // 水平
        boxCK->Add(cb1, 1);
        boxCK->Add(cb2, 1);
        boxCK->Add(cb3, 1);
        vbox->Add(boxCK, 0, wxLEFT, 10);
        vbox->Add(wxID_ANY, 25);//空白框

        // 两个操作按钮
        wxButton* btnOK = new wxButton(panel, wxID_ANY, "OK");
        wxButton* btnClose = new wxButton(panel, wxID_ANY, "Close");
        wxBoxSizer* boxBtn = new wxBoxSizer(wxHORIZONTAL);
        boxBtn->Add(btnOK, 0);
        boxBtn->Add(btnClose, 0, wxLEFT | wxBOTTOM, 10);
        vbox->Add(boxBtn, 0, wxRIGHT | wxALIGN_RIGHT, 10);

        panel->SetSizer(vbox);


        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LayoutBoxsizer::OnQuit, this, btnClose->GetId());

        this->Center(); // 当前frame生成的时候位于屏幕中央
    }

protected:
    void OnQuit(wxCommandEvent& event)
    {
        Close(true);
    }

};