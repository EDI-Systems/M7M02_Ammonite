#include "Item_Chip.h"

Item_Chip::Item_Chip(const int& _property, wxTreeItemId& _tree_item_id)
    :BaseItem(_property, _tree_item_id) {

}

void Item_Chip::Fill() {
    Main::body_splitter->workarea_panel->chip->SetChip();
}

void Item_Chip::Show() {
    Main::body_splitter->workarea_panel->ShowChip();
}

void Item_Chip::Switch() {
    Main::body_splitter->workarea_panel->chip->IsLegal();
}