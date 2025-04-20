#include "Item_Shmem.h"

Item_Shmem::Item_Shmem(const int& _property, wxTreeItemId& _tree_item_id)
    :BaseItem(_property, _tree_item_id), max_count_GUI(0) {

}

void Item_Shmem::Fill() {
    Main::body_splitter->workarea_panel->shmem->SetPanelShmemTest();
}

void Item_Shmem::Show() {
    Main::body_splitter->workarea_panel->ShowShmem();
}

void Item_Shmem::Switch() {
    Main::body_splitter->workarea_panel->shmem->IsLegal();
}