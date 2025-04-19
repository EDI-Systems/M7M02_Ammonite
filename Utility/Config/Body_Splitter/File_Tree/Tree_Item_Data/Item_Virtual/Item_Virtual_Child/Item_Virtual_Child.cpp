#include "Item_Virtual_Child.h"
#include "../../../../../main.h"

Item_Virtual_Child::Item_Virtual_Child(const int& _property, wxTreeItemId& _tree_item_id, wxTreeItemId& _parent_item, const int& _id)
    :BaseItem(_property, _tree_item_id), parent_item(_parent_item), id(_id) {

}
void Item_Virtual_Child::Build() {
    // 为虚拟机程创建页面
    Main::body_splitter->workarea_panel->AddVmChild(this->id);
}

void Item_Virtual_Child::Fill() {
    // 改变鼠标为忙碌状态
    wxBusyCursor busyCursor;
    Main::body_splitter->workarea_panel->FillVmChild(Main::proj_info->rvp->vm_manager->GetVmByID_ReadOnly(this->id));
}

void Item_Virtual_Child::Show() {
    Main::body_splitter->workarea_panel->ShowVmChild(this->id);
}

void Item_Virtual_Child::OnDelete(const wxCommandEvent& event) {
    if (wxMessageBox(wxString::Format("确认删除虚拟机%d", this->id), "删除", wxYES_NO | wxICON_QUESTION) == wxNO) {
        return;
    }
    Main::body_splitter->file_tree->Freeze(); // 暂停界面重绘

    // 释放GUI
    if (Main::body_splitter->workarea_panel->DestoryVmById(this->id))
        wxMessageBox(wxString::Format("GUI\t虚拟机页面已删除 id=%d", this->id));

    // 如果有合法的信息，则删除该信息
    if (Main::proj_info->rvp->vm_manager->DeleteVmById(this->id))
        wxMessageBox(wxString::Format("Project Information\t虚拟机信息已删除 id=%d", this->id));

    // 在树结构中移除节点treeItemId
    Main::body_splitter->file_tree->Delete(this->related_tree_item); 
    Main::body_splitter->file_tree->Thaw(); // 恢复界面重绘
}