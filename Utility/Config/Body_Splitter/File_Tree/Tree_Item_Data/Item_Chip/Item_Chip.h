#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>

#include "../BaseItem/BaseItem.h"
#include "../../../../main.h"
class Item_Chip
	:public BaseItem {
public:
	Item_Chip(const int& _property, wxTreeItemId& _tree_item_id);
	void Fill()override;
	void Show()override;
	void Switch()override;
};

