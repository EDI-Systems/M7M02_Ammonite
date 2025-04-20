#pragma once
#include <wx/splitter.h>
#include "Workarea_Panel/workarea_panel.h"
#include "File_Tree/file_tree.h"

class Body_Splitter
	:public wxSplitterWindow{
public:
    Body_Splitter(wxWindow* parent);
    Workarea_Panel* workarea_panel;
    File_Tree* file_tree;
};
