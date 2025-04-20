#include "Item_Native_Child.h"

Item_Native_Child::Item_Native_Child(const int& _property, wxTreeItemId& _tree_item_id, wxTreeItemId& _parent_item, const int& _id)
    :BaseItem(_property, _tree_item_id), parent_item(_parent_item), id(_id) {

}
void Item_Native_Child::Build() {
    // Ϊ��ǰԭ���̴߳���ҳ��
    Main::body_splitter->workarea_panel->AddNativeChild(this->id);
}

void Item_Native_Child::Fill() {
    // �ı����Ϊæµ״̬
    wxBusyCursor busyCursor;
    Main::body_splitter->workarea_panel->FillNativeChild(Main::proj_info->rvp->native_manager->GetNativeByID_ReadOnly(this->id));
}

void Item_Native_Child::Show() {
    Main::body_splitter->workarea_panel->ShowNativeChild(this->id);
}

void Item_Native_Child::OnDelete(const wxCommandEvent& event) {
    if (wxMessageBox(wxString::Format("ȷ��ɾ��ԭ������%d", this->id), "ɾ��", wxYES_NO | wxICON_QUESTION) == wxNO) {
        return;
    }
    Main::body_splitter->file_tree->Freeze(); // ��ͣ�����ػ�


    // �ͷ�GUI�����GUI���д�����ɾ������
    if (Main::body_splitter->workarea_panel->DestoryNativeById(this->id))
        wxMessageBox(wxString::Format("GUI\tԭ������ҳ����ɾ�� id=%d", this->id));

    // ����кϷ�����Ϣ����ɾ������Ϣ
    if (Main::proj_info->rvp->native_manager->DeleteNativeById(this->id))
        wxMessageBox(wxString::Format("Project Informationԭ��������Ϣ��ɾ�� id=%d", this->id));

    // �����ṹ���Ƴ��ڵ�treeItemId
    Main::body_splitter->file_tree->Delete(this->related_tree_item); 
    Main::body_splitter->file_tree->Thaw(); // �ָ������ػ�
}

void Item_Native_Child::Switch() {
    Main::body_splitter->workarea_panel->IsLegalNativeById(this->id);
}