#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>

#include "../../../../main.h"

class BaseItem
    :public wxTreeItemData {
public:
    BaseItem(const int& _p, wxTreeItemId& _item, const int& _id = 0);

    // �½�GUI����Ҫ���� ԭ������ �� �����
    virtual void Build(){}

    // ��proj_info�ж�ȡ��Ϣ��䵹GUI��
    virtual void Fill(){}

    // ��ʾGUI
    virtual void Show();

    // �л�itemʱִ�еĹ���
    virtual void Switch(){}

    // ��������ԭ�����̺������
    virtual void OnAdd(const wxCommandEvent& event);    

    // ɾ�������ԭ�����̺������
    virtual void OnDelete(const wxCommandEvent& event); 
    void SetGUItag(const bool& _makeGUI);
    bool GetGUItag()const { return makeGUI; }
    wxMenu* GetMenu()const { return menu; }


protected:
    // �Ҽ������˵�
    wxMenu* menu;

    // GUI�Ƿ��½��������Ϣ
    bool makeGUI;
    
    // ԭ�����̺�������ı�ʶ
    int id;

    // ���ڸ����ļ���
    wxTreeItemId related_tree_item;
    std::vector<wxMenuItem*>menu_items;
};
