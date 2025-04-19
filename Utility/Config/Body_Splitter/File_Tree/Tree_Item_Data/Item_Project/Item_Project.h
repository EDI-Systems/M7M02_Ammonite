#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>

#include "../BaseItem/BaseItem.h"
#include "../../../../main.h"
class Item_Project
	:public BaseItem {
public:
	Item_Project(const int& _property, wxTreeItemId& _tree_item_id);
	void Show()override;
};

