#pragma once
//#include "./Tree_Item_Data/tree_item_data.h"
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/xml/xml.h>

#include "../../Proj_Info/RVP_Info/Native_Manager/Native/Native.h"


class File_Tree
    :public wxTreeCtrl {
public:
    File_Tree(wxWindow* parent);
    void OnItemSelected(wxTreeEvent& event);
    void OnItemSelected_Right(wxTreeEvent& event);

    void ClearItemNativeChild();
    void ClearItemVmChild();

    void InitItem();

    void SelectRoot() { SelectItem(project_root); }

    // 只对树结构进行更新
    // GUI的建立、填充和展示在点击时才进行
    void AddNativeChild();
    void AddVmChild();


private:
    wxTreeItemId project_root;
    wxTreeItemId native;
    wxTreeItemId virtual_machine;

    wxTreeItemData* project_root_data;
    wxTreeItemData* base_info_data;
    wxTreeItemData* chip_data; 
    wxTreeItemData* extmem_data;
    wxTreeItemData* shmem_data;
    wxTreeItemData* kernel_data;
    wxTreeItemData* monitor_data;
    wxTreeItemData* native_data;
    wxTreeItemData* vm_data;

    wxTreeItemId last_select_item;
};
