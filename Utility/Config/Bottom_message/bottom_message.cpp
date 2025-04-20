#include "bottom_message.h"
#include "../main.h"

Bottom_Message::Bottom_Message(wxWindow* parent)
	:wxBoxSizer(wxHORIZONTAL) {
	this->alarm = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(20, 20));
	this->alarm->SetBackgroundColour(*wxRED);
	this->message = new wxTextCtrl(parent, wxID_ANY, "", wxDefaultPosition, wxSize(20, 20), wxTE_READONLY);
	this->Add(this->alarm, 0);
	this->Add(this->message, 1);
}

void Bottom_Message::SetBottomMessage(const wxString& message, const wxColour& colour) {
	this->message->SetValue(message);
	this->alarm->SetBackgroundColour(colour);
	this->alarm->Refresh();
}

