#include "Item_Virtual_Child.h"
#include "../../../../../main.h"

Item_Virtual_Child::Item_Virtual_Child(const int& _property, wxTreeItemId& _tree_item_id, wxTreeItemId& _parent_item, const int& _id)
    :BaseItem(_property, _tree_item_id), parent_item(_parent_item), id(_id) {

}
void Item_Virtual_Child::Build() {
    // Ϊ������̴���ҳ��
    Main::body_splitter->workarea_panel->AddVmChild(this->id);
}

void Item_Virtual_Child::Fill() {
    // �ı����Ϊæµ״̬
    wxBusyCursor busyCursor;
    Main::body_splitter->workarea_panel->FillVmChild(Main::proj_info->rvp->vm_manager->GetVmByID_ReadOnly(this->id));
}

void Item_Virtual_Child::Show() {
    Main::body_splitter->workarea_panel->ShowVmChild(this->id);
}

void Item_Virtual_Child::OnDelete(const wxCommandEvent& event) {
    if (wxMessageBox(wxString::Format("ȷ��ɾ�������%d", this->id), "ɾ��", wxYES_NO | wxICON_QUESTION) == wxNO) {
        return;
    }
    Main::body_splitter->file_tree->Freeze(); // ��ͣ�����ػ�

    // �ͷ�GUI
    if (Main::body_splitter->workarea_panel->DestoryVmById(this->id))
        wxMessageBox(wxString::Format("GUI\t�����ҳ����ɾ�� id=%d", this->id));

    // ����кϷ�����Ϣ����ɾ������Ϣ
    if (Main::proj_info->rvp->vm_manager->DeleteVmById(this->id))
        wxMessageBox(wxString::Format("Project Information\t�������Ϣ��ɾ�� id=%d", this->id));

    // �����ṹ���Ƴ��ڵ�treeItemId
    Main::body_splitter->file_tree->Delete(this->related_tree_item); 
    Main::body_splitter->file_tree->Thaw(); // �ָ������ػ�
}