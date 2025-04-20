#include "Item_Virtual.h"
#include "Item_Virtual_Child/Item_Virtual_Child.h"

Item_Virtual::Item_Virtual(const int& _property, wxTreeItemId& _tree_item_id)
    :BaseItem(_property, _tree_item_id), max_count_GUI(0) {

}

void Item_Virtual::Show() {
    Main::body_splitter->workarea_panel->ShowVirtualInfo();
}

void Item_Virtual::Fill() {
    Main::body_splitter->workarea_panel->virtual_info->SetPanelVirtualInfo();
}

void Item_Virtual::AddFunc() {
    // 更新树结构
    wxTreeItemId child = Main::body_splitter->file_tree->AppendItem(this->related_tree_item, wxString::Format("虚拟机%d", max_count_GUI));
    Item_Virtual_Child* child_data = new Item_Virtual_Child(T2 | T3, child, this->related_tree_item, max_count_GUI);    // 为该子节点新建对应的data
    Main::body_splitter->file_tree->SetItemData(child, child_data);
    Main::body_splitter->file_tree->Expand(this->related_tree_item);
    max_count_GUI++;
}


void Item_Virtual::OnAdd(const wxCommandEvent& event) {
    AddFunc();
}

void Item_Virtual::Switch() {
    Main::body_splitter->workarea_panel->IsLegalVmById(this->id);
}