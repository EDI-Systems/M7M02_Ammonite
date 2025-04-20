#include "PanelBase.h"
PanelBase::PanelBase(wxWindow* parent, wxWindowID id, const wxString& _errorloc, const bool&_makeGUI , std::function<void()> _func)
    :wxPanel(parent, id), makeGUI(_makeGUI), errorloc(_errorloc),func(_func) {
    SetBackgroundColour(*wxWHITE);
    highbox = new wxBoxSizer(wxVERTICAL);
    box = new wxBoxSizer(wxVERTICAL);

    swindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,wxSIMPLE_BORDER);// wxBORDER_SUNKEN
    swindow->SetScrollRate(10, 10);

    wxPanel* btn_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btn_preview = new wxButton(btn_panel, wxID_ANY, "Ô¤ÀÀ");
    wxButton* btn_confirm = new wxButton(btn_panel, wxID_ANY, "È·ÈÏ");
    // Ë¢ÐÂ
    wxButton* btn_refresh = new wxButton(btn_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(25, 25));
    wxImage image;
    if (image.LoadFile("Image\\preview2.bmp")) {
        btn_refresh->SetBitmap(wxBitmap(image));
        btn_refresh->SetBitmapPosition(wxLEFT);
    }
    hbox->Add(btn_preview, 0, wxALL, 3);
    hbox->Add(btn_confirm, 0, wxALL, 3);
    hbox->Add(btn_refresh, 0, wxALL, 3);
    vbox->Add(hbox, 0, wxALIGN_LEFT);
    btn_panel->SetSizer(vbox);

    highbox->Add(swindow, 1, wxEXPAND);
    highbox->Add(btn_panel, 0, wxEXPAND);
    SetSizer(highbox);

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PanelBase::OnPreview, this, btn_preview->GetId());
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PanelBase::OnConfirm, this, btn_confirm->GetId());
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PanelBase::OnRefresh, this, btn_refresh->GetId());
}


