#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>

#include "../BaseItem/BaseItem.h"
#include "../../../../Proj_Info/RVP_Info/Native_Manager/Native/Native.h"

class Item_Native
	:public BaseItem {
public:
	Item_Native(const int& _property, wxTreeItemId& _tree_item_id);
	void Show()override;
	void Fill()override;
	void OnAdd(const wxCommandEvent& event)override;
	void AddFunc();
	void ResetConut() { max_count_GUI = 0; }
	//void AddFunc(const std::unique_ptr<Native>& ptr);
private:
	int max_count_GUI;
};