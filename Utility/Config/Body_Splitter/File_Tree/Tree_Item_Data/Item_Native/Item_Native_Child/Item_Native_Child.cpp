#include "Item_Native_Child.h"

Item_Native_Child::Item_Native_Child(const int& _property, wxTreeItemId& _tree_item_id, wxTreeItemId& _parent_item, const int& _id)
    :BaseItem(_property, _tree_item_id), parent_item(_parent_item), id(_id) {

}
void Item_Native_Child::Build() {
    // 为当前原生线程创建页面
    Main::body_splitter->workarea_panel->AddNativeChild(this->id);
}

void Item_Native_Child::Fill() {
    // 改变鼠标为忙碌状态
    wxBusyCursor busyCursor;
    Main::body_splitter->workarea_panel->FillNativeChild(Main::proj_info->rvp->native_manager->GetNativeByID_ReadOnly(this->id));
}

void Item_Native_Child::Show() {
    Main::body_splitter->workarea_panel->ShowNativeChild(this->id);
}

void Item_Native_Child::OnDelete(const wxCommandEvent& event) {
    if (wxMessageBox(wxString::Format("确认删除原生进程%d", this->id), "删除", wxYES_NO | wxICON_QUESTION) == wxNO) {
        return;
    }
    Main::body_splitter->file_tree->Freeze(); // 暂停界面重绘


    // 释放GUI，如果GUI中有错误，则删除错误
    if (Main::body_splitter->workarea_panel->DestoryNativeById(this->id))
        wxMessageBox(wxString::Format("GUI\t原生进程页面已删除 id=%d", this->id));

    // 如果有合法的信息，则删除该信息
    if (Main::proj_info->rvp->native_manager->DeleteNativeById(this->id))
        wxMessageBox(wxString::Format("Project Information原生进程信息已删除 id=%d", this->id));

    // 在树结构中移除节点treeItemId
    Main::body_splitter->file_tree->Delete(this->related_tree_item); 
    Main::body_splitter->file_tree->Thaw(); // 恢复界面重绘
}

void Item_Native_Child::Switch() {
    Main::body_splitter->workarea_panel->IsLegalNativeById(this->id);
}