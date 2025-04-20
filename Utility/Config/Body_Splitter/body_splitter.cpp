#include "body_splitter.h"

Body_Splitter::Body_Splitter(wxWindow* parent)
    :wxSplitterWindow(parent, wxID_ANY) {

    this->SetWindowStyleFlag(wxSP_LIVE_UPDATE); // 实时更新拖动情况
    this->file_tree = new File_Tree(this);
    this->workarea_panel = new Workarea_Panel(this);
    this->SplitVertically(this->file_tree, this->workarea_panel);// 将面板添加到分割窗口 
    this->SetSashPosition(200);// 设置分隔条的初始位置
    this->SetMinimumPaneSize(150);// 设置最小面板大小
}