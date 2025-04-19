#include "Item_Extmem.h"

Item_Extmem::Item_Extmem(const int& _property, wxTreeItemId& _tree_item_id)
    :BaseItem(_property, _tree_item_id) {

}

void Item_Extmem::Fill() {
    // 没有实际内容
    Main::body_splitter->workarea_panel->extmem->SetExtmem();
}

void Item_Extmem::Show() {
    Main::body_splitter->workarea_panel->ShowExtmem();
}

void Item_Extmem::Switch() {
    Main::body_splitter->workarea_panel->extmem->IsLegal();
}