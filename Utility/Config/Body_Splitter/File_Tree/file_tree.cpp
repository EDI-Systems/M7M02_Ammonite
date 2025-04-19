
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




// 创建
// 删除
// 功能3
File_Tree::File_Tree(wxWindow* parent)
    :wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_EDIT_LABELS) {

    // 创建树的根节点
    project_root = this->AddRoot("配置文件");
    project_root_data = new Item_Project(0, project_root);
    SetItemData(project_root, project_root_data);

    // done
    wxTreeItemId base_info = this->AppendItem(project_root, "基本信息");
    base_info_data = new Item_BaseInfo(T3, base_info);
    SetItemData(base_info, base_info_data);

    // done
    wxTreeItemId chip = this->AppendItem(project_root, "芯片配置");
    chip_data = new Item_Chip(T3, chip);
    SetItemData(chip, chip_data);

    // done
    wxTreeItemId extmem= this->AppendItem(project_root, "额外存储器");
    extmem_data = new Item_Extmem(T3, extmem);
    SetItemData(extmem, extmem_data);

    // done
    wxTreeItemId shmem = this->AppendItem(project_root, "共享存储块");
    shmem_data = new Item_Shmem(T1 | T3, shmem);
    SetItemData(shmem, shmem_data);

    // done
    wxTreeItemId kernel = this->AppendItem(project_root, "内核配置");
    kernel_data = new Item_Kernel(T3, kernel);
    SetItemData(kernel, kernel_data);

    // done
    wxTreeItemId monitor = this->AppendItem(project_root, "监视器配置");
    monitor_data = new Item_Monitor(T3, monitor);
    SetItemData(monitor, monitor_data);

    // done
    native = this->AppendItem(project_root, "原生进程配置");
    native_data = new Item_Native(T1 | T3, native);
    SetItemData(native, native_data);

    // done
    virtual_machine = this->AppendItem(project_root, "虚拟机配置");  
    vm_data = new Item_Virtual(T1 | T3, virtual_machine);
    SetItemData(virtual_machine, vm_data);

    Collapse(project_root);

    // 绑定树控件的事件
    //Bind(wxEVT_TREE_SEL_CHANGED, &File_Tree::OnItemSelected, this);      // 单击
    Bind(wxEVT_TREE_ITEM_ACTIVATED, &File_Tree::OnItemSelected, this);// 双击
    Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &File_Tree::OnItemSelected_Right, this); // 右键单击事件

}

void File_Tree::OnItemSelected(wxTreeEvent& event) {
    // 获取被选中的节点 ID
    wxTreeItemId selectedItem = event.GetItem();

    // 执行上一次点击节点的Switch
    if (last_select_item) {
        BaseItem* last_item_data = dynamic_cast<BaseItem*>(this->GetItemData(last_select_item));
        if (last_item_data) {
            // 检查上次打开的页面，并尝试保存
            last_item_data->Switch();
        }
    }

    last_select_item = selectedItem;

    // 获取节点的文本
    wxString itemText = this->GetItemText(selectedItem);
    BaseItem*base= dynamic_cast<BaseItem*>(this->GetItemData(selectedItem));


    if (base) {
        if (!base->GetGUItag()) {
            // 新建并填充数据
            base->Build();
            base->Fill();
            base->SetGUItag(true);
        }
        base->Show();
    }
    else {
        wxMessageBox(wxString::Format("%s 我没有设置Data", itemText));
    }
}

void File_Tree::OnItemSelected_Right(wxTreeEvent& event) {
    // 获取右键点击的节点
    wxTreeItemId selectedItem = event.GetItem();
    if (!selectedItem.IsOk()) {
        return; // 无效节点，直接返回
    }

    // 获取节点文本
    wxString itemText = GetItemText(selectedItem);
    //SelectItem(selectedItem); // 右键也可以使之进入选中状态
    BaseItem* base = dynamic_cast<BaseItem*>(this->GetItemData(selectedItem));
    if (base) {
        PopupMenu(base->GetMenu());
    }
    else {
        wxMessageBox(wxString::Format("%s 我没有设置Data", itemText));
    }
}

void File_Tree::ClearItemNativeChild() {
    if (ItemHasChildren(native)) {
        DeleteChildren(native);  // 删除所有子节点
    }
    // 子节点计数从0开始
    Item_Native* base = dynamic_cast<Item_Native*>(this->GetItemData(native));
    base->ResetConut();
}

void File_Tree::ClearItemVmChild() {
    if (ItemHasChildren(virtual_machine)) {
        DeleteChildren(virtual_machine);
    }
    // 子节点计数从0开始
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

    last_select_item = wxTreeItemId(); // 设为无效的 wxTreeItemId
}
