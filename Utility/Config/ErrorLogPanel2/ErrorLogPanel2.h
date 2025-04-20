#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/scrolwin.h>
#include <map>

#include "../Proj_Info/Error_Info/Error_Info.h"

class ErrorLogPanel2
	:public wxPanel{
public:
	ErrorLogPanel2(wxWindow* parent);
	void RefreshErrorLog();

private:
	void ClearError();
	void OnSize(wxSizeEvent& event);
	void OnGridCellClick(wxGridEvent& event);

	wxGrid* grid;
};

