
#include <wx/thread.h>
#include <wx/event.h>
#include <thread>

#include "file_tree.h"
#include "./Tree_Item_Data/BaseItem/BaseItem.h"

#include "Tree_Item_Data/Item_Project/Item_Project.h"
#include "Tree_Item_Data/Item_BaseInfo/Item_BaseInfo.h"
#include "Tree_Item_Data/Item_Chip/Item_Chip.h"
#include "Tree_Item_Data/Item_Extmem/Item_Extmem.h"
#include "Tree_Item_Data/Item_Shmem/Item_Shmem.h"
#include "Tree_Item_Data/Item_Kernel/Item_Kernel.h"
#include "Tree_Item_Data/Item_Monitor/Item_Monitor.h"
#include "Tree_Item_Data/Item_Native/Item_Native.h"
#include "Tree_Item_Data/Item_Virtual/Item_Virtual.h"




// ����
// ɾ��
// ����3
File_Tree::File_Tree(wxWindow* parent)
    :wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_EDIT_LABELS) {

    // �������ĸ��ڵ�
    project_root = this->AddRoot("�����ļ�");
    project_root_data = new Item_Project(0, project_root);
    SetItemData(project_root, project_root_data);

    // done
    wxTreeItemId base_info = this->AppendItem(project_root, "������Ϣ");
    base_info_data = new Item_BaseInfo(T3, base_info);
    SetItemData(base_info, base_info_data);

    // done
    wxTreeItemId chip = this->AppendItem(project_root, "оƬ����");
    chip_data = new Item_Chip(T3, chip);
    SetItemData(chip, chip_data);

    // done
    wxTreeItemId extmem= this->AppendItem(project_root, "����洢��");
    extmem_data = new Item_Extmem(T3, extmem);
    SetItemData(extmem, extmem_data);

    // done
    wxTreeItemId shmem = this->AppendItem(project_root, "����洢��");
    shmem_data = new Item_Shmem(T1 | T3, shmem);
    SetItemData(shmem, shmem_data);

    // done
    wxTreeItemId kernel = this->AppendItem(project_root, "�ں�����");
    kernel_data = new Item_Kernel(T3, kernel);
    SetItemData(kernel, kernel_data);

    // done
    wxTreeItemId monitor = this->AppendItem(project_root, "����������");
    monitor_data = new Item_Monitor(T3, monitor);
    SetItemData(monitor, monitor_data);

    // done
    native = this->AppendItem(project_root, "ԭ����������");
    native_data = new Item_Native(T1 | T3, native);
    SetItemData(native, native_data);

    // done
    virtual_machine = this->AppendItem(project_root, "���������");  
    vm_data = new Item_Virtual(T1 | T3, virtual_machine);
    SetItemData(virtual_machine, vm_data);

    Collapse(project_root);

    // �����ؼ����¼�
    //Bind(wxEVT_TREE_SEL_CHANGED, &File_Tree::OnItemSelected, this);      // ����
    Bind(wxEVT_TREE_ITEM_ACTIVATED, &File_Tree::OnItemSelected, this);// ˫��
    Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &File_Tree::OnItemSelected_Right, this); // �Ҽ������¼�

}

void File_Tree::OnItemSelected(wxTreeEvent& event) {
    // ��ȡ��ѡ�еĽڵ� ID
    wxTreeItemId selectedItem = event.GetItem();

    // ִ����һ�ε���ڵ��Switch
    if (last_select_item) {
        BaseItem* last_item_data = dynamic_cast<BaseItem*>(this->GetItemData(last_select_item));
        if (last_item_data) {
            // ����ϴδ򿪵�ҳ�棬�����Ա���
            last_item_data->Switch();
        }
    }

    last_select_item = selectedItem;

    // ��ȡ�ڵ���ı�
    wxString itemText = this->GetItemText(selectedItem);
    BaseItem*base= dynamic_cast<BaseItem*>(this->GetItemData(selectedItem));


    if (base) {
        if (!base->GetGUItag()) {
            // �½����������
            base->Build();
            base->Fill();
            base->SetGUItag(true);
        }
        base->Show();
    }
    else {
        wxMessageBox(wxString::Format("%s ��û������Data", itemText));
    }
}

void File_Tree::OnItemSelected_Right(wxTreeEvent& event) {
    // ��ȡ�Ҽ�����Ľڵ�
    wxTreeItemId selectedItem = event.GetItem();
    if (!selectedItem.IsOk()) {
        return; // ��Ч�ڵ㣬ֱ�ӷ���
    }

    // ��ȡ�ڵ��ı�
    wxString itemText = GetItemText(selectedItem);
    //SelectItem(selectedItem); // �Ҽ�Ҳ����ʹ֮����ѡ��״̬
    BaseItem* base = dynamic_cast<BaseItem*>(this->GetItemData(selectedItem));
    if (base) {
        PopupMenu(base->GetMenu());
    }
    else {
        wxMessageBox(wxString::Format("%s ��û������Data", itemText));
    }
}

void File_Tree::ClearItemNativeChild() {
    if (ItemHasChildren(native)) {
        DeleteChildren(native);  // ɾ�������ӽڵ�
    }
    // �ӽڵ������0��ʼ
    Item_Native* base = dynamic_cast<Item_Native*>(this->GetItemData(native));
    base->ResetConut();
}

void File_Tree::ClearItemVmChild() {
    if (ItemHasChildren(virtual_machine)) {
        DeleteChildren(virtual_machine);
    }
    // �ӽڵ������0��ʼ
    Item_Virtual* base = dynamic_cast<Item_Virtual*>(this->GetItemData(virtual_machine));
    base->ResetCount();
}

void File_Tree::AddNativeChild() {
    Item_Native* base = dynamic_cast<Item_Native*>(this->GetItemData(native));
    base->AddFunc();
}

void File_Tree::AddVmChild() {
    Item_Virtual* base = dynamic_cast<Item_Virtual*>(this->GetItemData(virtual_machine));
    base->AddFunc();
}

void File_Tree::InitItem() {
    dynamic_cast<Item_BaseInfo*>(base_info_data)->SetGUItag(false);
    dynamic_cast<Item_Chip*>(chip_data)->SetGUItag(false);
    dynamic_cast<Item_Extmem*>(extmem_data)->SetGUItag(false);
    dynamic_cast<Item_Shmem*>(shmem_data)->SetGUItag(false);
    dynamic_cast<Item_Kernel*>(kernel_data)->SetGUItag(false);
    dynamic_cast<Item_Monitor*>(monitor_data)->SetGUItag(false);
    dynamic_cast<Item_Native*>(native_data)->SetGUItag(false);
    dynamic_cast<Item_Virtual*>(vm_data)->SetGUItag(false);

    last_select_item = wxTreeItemId(); // ��Ϊ��Ч�� wxTreeItemId
}
