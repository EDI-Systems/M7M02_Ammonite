#include "ErrorLogPanel2.h"
#include "../main.h"

ErrorLogPanel2::ErrorLogPanel2(wxWindow* parent)
	:wxPanel(parent, wxID_ANY) {

	wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
    grid = new wxGrid(this, wxID_ANY);
    grid->EnableDragGridSize(false);    
    grid->SetRowLabelSize(1);
    grid->SetColLabelSize(1);
    grid->CreateGrid(1, 4);
    grid->SetCellValue(0, 0, "类型");
    grid->SetCellValue(0, 1, "信息");
    grid->SetCellValue(0, 2, "位置");
    grid->SetCellValue(0, 3, "ID");
    grid->SetReadOnly(0, 0, true);
    grid->SetReadOnly(0, 1, true);
    grid->SetReadOnly(0, 2, true);
    grid->SetReadOnly(0, 3, true);

    box->Add(grid, 1, wxEXPAND);

    wxSize panelSize = GetClientSize();
    grid->SetSize(panelSize); // 设置 wxGrid 尺寸
    SetSizerAndFit(box);

    Bind(wxEVT_SIZE, &ErrorLogPanel2::OnSize, this);// 绑定单元格点击事件

    grid->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &ErrorLogPanel2::OnGridCellClick, this);
}


void ErrorLogPanel2::RefreshErrorLog() {

    ClearError();
    grid->AppendRows(int(Main::proj_info->errorInfo->arr.size()));
    int cnt = 1;
    for (const auto& i : Main::proj_info->errorInfo->arr) {

        grid->SetCellValue(cnt, 0, "Error");
        grid->SetCellValue(cnt, 1, i.second->cont);
        grid->SetCellValue(cnt, 2, i.second->loc);
        grid->SetCellValue(cnt, 3, wxString::Format("%d", i.first));
        grid->SetReadOnly(cnt, 0, true);
        grid->SetReadOnly(cnt, 1, true);
        grid->SetReadOnly(cnt, 2, true);
        grid->SetReadOnly(cnt, 3, true);
        cnt++;
    }

}

void ErrorLogPanel2::ClearError() {
    int rowCount = grid->GetNumberRows();
    if (rowCount > 1) {
        grid->DeleteRows(1, rowCount - 1); // 从第 1 行开始删除 (保留第 0 行)
    }
}

void ErrorLogPanel2::OnSize(wxSizeEvent& event) {
    int Width = GetClientSize().GetWidth();
    grid->SetColSize(0, 60);  // 设置第 0 列宽度为 100
    grid->SetColSize(1, (Width - 160) / 2);  // 设置第 0 列宽度为 100
    grid->SetColSize(2, (Width - 160) / 2);  // 设置第 0 列宽度为 100
    grid->SetColSize(3, 100);  // 设置第 0 列宽度为 100
    event.Skip();
}

void ErrorLogPanel2::OnGridCellClick(wxGridEvent& event) {
    if (event.GetRow() == 0)
        return;
    int uni_id = atoi(grid->GetCellValue(event.GetRow(), 3));
    if (Main::proj_info->errorInfo->arr.find(uni_id) != Main::proj_info->errorInfo->arr.end()) {
        Main::proj_info->errorInfo->arr.find(uni_id)->second->func();
    }
    else {
        wxMessageBox("无法对应错误ID", "警告", wxICON_WARNING);
    }
    event.Skip();

}