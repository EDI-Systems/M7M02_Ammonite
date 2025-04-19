#include "Item_BaseInfo.h"

Item_BaseInfo::Item_BaseInfo(const int& _property, wxTreeItemId& _tree_item_id)
	:BaseItem(_property, _tree_item_id) {

}

void Item_BaseInfo::Fill() {
	Main::body_splitter->workarea_panel->base_info->SetBaseInfo();
}

void Item_BaseInfo::Show(){
    Main::body_splitter->workarea_panel->ShowBaseInfo();
}

void Item_BaseInfo::Switch() {
	Main::body_splitter->workarea_panel->base_info->IsLegal();
}