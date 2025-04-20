#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>
#include "../../BaseItem/BaseItem.h"


class Item_Virtual_Child
	:public BaseItem {
public:
	Item_Virtual_Child(const int& _property, wxTreeItemId& _tree_item_id, wxTreeItemId& _parent_item, const int& _id);

	void Build()override;
	void Fill()override;
	void Show()override;
	void OnDelete(const wxCommandEvent& event)override;
	wxTreeItemId parent_item;

	int id;
};
