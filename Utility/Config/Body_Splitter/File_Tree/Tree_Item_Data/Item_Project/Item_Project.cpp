#include "Item_Project.h"
Item_Project::Item_Project(const int& _property, wxTreeItemId& _tree_item_id)
    :BaseItem(_property, _tree_item_id) {

}

void Item_Project::Show() {
    Main::body_splitter->workarea_panel->ShowProject();
}