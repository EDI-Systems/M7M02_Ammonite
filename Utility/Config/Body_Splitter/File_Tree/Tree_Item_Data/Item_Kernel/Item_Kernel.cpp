#include "Item_Kernel.h"

Item_Kernel::Item_Kernel(const int& _property, wxTreeItemId& _tree_item_id)
    :BaseItem(_property, _tree_item_id) {

}

void Item_Kernel::Fill() {
    Main::body_splitter->workarea_panel->kernel->SetKernel();
}

void Item_Kernel::Show() {
    Main::body_splitter->workarea_panel->ShowKernel();
}

void Item_Kernel::Switch() {
    Main::body_splitter->workarea_panel->kernel->IsLegal();
}