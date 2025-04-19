#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>

#include "../BaseItem/BaseItem.h"

class Item_Virtual
	:public BaseItem {
public:
	Item_Virtual(const int& _property, wxTreeItemId& _tree_item_id);
	void Show()override;
	void Fill()override;
	void Switch()override;
	void OnAdd(const wxCommandEvent& event)override;
	void AddFunc();
	void ResetCount() { max_count_GUI = 0; }
private:
	int max_count_GUI;
};