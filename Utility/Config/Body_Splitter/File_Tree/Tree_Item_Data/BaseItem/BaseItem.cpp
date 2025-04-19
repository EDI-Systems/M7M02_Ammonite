#include "BaseItem.h"

BaseItem::BaseItem(const int& _p, wxTreeItemId& _item, const int& _id)
    :related_tree_item(_item), makeGUI(false), id(_id) {
    menu = new wxMenu();
    wxMenuItem* new_item = this->menu->Append(wxID_ANY, "新建");
    wxMenuItem* delete_item = this->menu->Append(wxID_ANY, "删除");
    wxMenuItem* func3 = this->menu->Append(wxID_ANY, "功能3");

    this->menu->Bind(wxEVT_MENU, &BaseItem::OnAdd, this, new_item->GetId());
    this->menu->Bind(wxEVT_MENU, &BaseItem::OnDelete, this, delete_item->GetId());

    this->menu_items.push_back(new_item);
    this->menu_items.push_back(delete_item);
    this->menu_items.push_back(func3);
    // 可用菜单项
    for (int i = 0; i < menu_items.size(); i++) {
        if ((_p >> i) & 1)
            this->menu_items[i]->Enable(true);
        else
            this->menu_items[i]->Enable(false);
    }
}



void BaseItem::Show() {
    wxMessageBox("父类Function");
    return;
}

void BaseItem::OnAdd(const wxCommandEvent& event) {
    wxMessageBox("父类Function");
    return;
}

void BaseItem::OnDelete(const wxCommandEvent& event) {
    wxMessageBox("父类Function");
    return;
}

void BaseItem::SetGUItag(const bool& _makeGUI) {
    makeGUI = _makeGUI;
}
