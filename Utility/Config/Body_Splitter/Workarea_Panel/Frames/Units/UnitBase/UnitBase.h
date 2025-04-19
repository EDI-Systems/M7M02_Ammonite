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

	// 命名唯一性
	void NameCorrectSet();
	void NameIncorrectSet(const wxString& str);

	// Send、Port的唯一性由name和process决定，需要重写
	virtual wxString GetName();

	// 删除该单元时，清除错误
	virtual void ClearError() { wxMessageBox("基类func"); }

	// 合法性判断
	virtual bool IsLegal() { return false; }

protected:

	SimpleText* base_name;
	wxButton* delete_unit;
	wxString errorloc;
	std::function<void()>func;
	virtual void OnDelete(const wxCommandEvent& event);
};