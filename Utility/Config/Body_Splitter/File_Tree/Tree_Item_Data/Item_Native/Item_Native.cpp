#include "Item_Native.h"
#include "Item_Native_Child/Item_Native_Child.h"
#include "../../../../Body_Splitter/Workarea_Panel/Frames/Panel_Native_Child/Panel_Native_Child.h"
#include "../../../../main.h"

Item_Native::Item_Native(const int& _property, wxTreeItemId& _tree_item_id)
    :BaseItem(_property, _tree_item_id), max_count_GUI(0) {

}

void Item_Native::Show() {
    Main::body_splitter->workarea_panel->ShowNativeInfo();
}

void Item_Native :: Fill() {
    Main::body_splitter->workarea_panel->native_info->SetPanelNativeInfo();
}

void Item_Native::AddFunc() {
    // 更新树结构
    wxTreeItemId child = Main::body_splitter->file_tree->AppendItem(this->related_tree_item, wxString::Format("原生进程%d", max_count_GUI));
    Item_Native_Child* child_data = new Item_Native_Child(T2 | T3, child, this->related_tree_item, max_count_GUI);    // 为该子节点新建对应的data
    Main::body_splitter->file_tree->SetItemData(child, child_data);
    Main::body_splitter->file_tree->Expand(this->related_tree_item);

    // 新添加一个GUI 空白
    //Main::body_splitter->workarea_panel->AddNativeChild(max_count_GUI);

    max_count_GUI++;
}

void Item_Native::OnAdd(const wxCommandEvent& event) {
    AddFunc();
}