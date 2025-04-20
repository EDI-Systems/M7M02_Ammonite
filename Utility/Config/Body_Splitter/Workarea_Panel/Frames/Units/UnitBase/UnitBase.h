#pragma once
#include <wx/wx.h>

#include "../../Controller/Controller.h"

class UnitBase
	:public wxScrolledWindow {
public:
	UnitBase(
		wxWindow* parent,
		const wxString& _errorloc = wxEmptyString,
		std::function<void()>_func = nullptr);

	// ����Ψһ��
	void NameCorrectSet();
	void NameIncorrectSet(const wxString& str);

	// Send��Port��Ψһ����name��process��������Ҫ��д
	virtual wxString GetName();

	// ɾ���õ�Ԫʱ���������
	virtual void ClearError() { wxMessageBox("����func"); }

	// �Ϸ����ж�
	virtual bool IsLegal() { return false; }

protected:

	SimpleText* base_name;
	wxButton* delete_unit;
	wxString errorloc;
	std::function<void()>func;
	virtual void OnDelete(const wxCommandEvent& event);
};