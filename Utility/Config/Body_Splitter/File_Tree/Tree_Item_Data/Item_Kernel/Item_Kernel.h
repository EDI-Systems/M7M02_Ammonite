#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>

#include "../BaseItem/BaseItem.h"
#include "../../../../main.h"

class Item_Kernel
	:public BaseItem {
public:
	Item_Kernel(const int& _property, wxTreeItemId& _tree_item_id);
	void Fill()override;
	void Show()override;
	void Switch()override;
};

