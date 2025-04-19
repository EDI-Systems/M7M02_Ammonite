#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>

#include "../../../../main.h"

class BaseItem
    :public wxTreeItemData {
public:
    BaseItem(const int& _p, wxTreeItemId& _item, const int& _id = 0);

    // 新建GUI，主要用于 原生进程 和 虚拟机
    virtual void Build(){}

    // 从proj_info中读取信息填充倒GUI中
    virtual void Fill(){}

    // 显示GUI
    virtual void Show();

    // 切换item时执行的功能
    virtual void Switch(){}

    // 添加项，用于原生进程和虚拟机
    virtual void OnAdd(const wxCommandEvent& event);    

    // 删除项，用于原生进程和虚拟机
    virtual void OnDelete(const wxCommandEvent& event); 
    void SetGUItag(const bool& _makeGUI);
    bool GetGUItag()const { return makeGUI; }
    wxMenu* GetMenu()const { return menu; }


protected:
    // 右键弹出菜单
    wxMenu* menu;

    // GUI是否新建并填充信息
    bool makeGUI;
    
    // 原生进程和虚拟机的标识
    int id;

    // 用于跟新文件树
    wxTreeItemId related_tree_item;
    std::vector<wxMenuItem*>menu_items;
};
