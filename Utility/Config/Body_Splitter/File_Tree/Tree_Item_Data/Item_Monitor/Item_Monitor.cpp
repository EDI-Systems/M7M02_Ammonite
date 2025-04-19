#include "Item_Monitor.h"

Item_Monitor::Item_Monitor(const int& _property, wxTreeItemId& _tree_item_id)
    :BaseItem(_property, _tree_item_id) {

}

void Item_Monitor::Fill() {
    Main::body_splitter->workarea_panel->monitor->SetMonitor();
}

void Item_Monitor::Show() {
    Main::body_splitter->workarea_panel->ShowMonitor();
}

void Item_Monitor::Switch() {
    Main::body_splitter->workarea_panel->monitor->IsLegal();
}