/******************************************************************************
Filename    : config_tree.hpp
Author      : pry
Date        : 16/08/2021
License     : Proprietary; confidential.
Description : Config tree file header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __CONFIG_TREE_TYPE__
#define __CONFIG_TREE_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __CONFIG_TREE_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __CONFIG_TREE_DEF__
#define __CONFIG_TREE_DEF__
/*****************************************************************************/
#define SELECT_DETAIL_NONE                  (0)
#define SELECT_DETAIL_ROOT                  (1)
#define SELECT_DETAIL_BASIC                 (2)
#define SELECT_DETAIL_MEMORY                (3)
#define SELECT_DETAIL_KERNEL                (4)
#define SELECT_DETAIL_MONITOR               (5)
#define SELECT_DETAIL_NATIVE                (6)
#define SELECT_DETAIL_VIRTUAL               (7)
#define SELECT_DETAIL_NATIVE_CHILD          (8)
#define SELECT_DETAIL_VIRTUAL_CHILD         (9)
/*****************************************************************************/
/* __CONFIG_TREE_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __CONFIG_TREE_CLASS__
#define __CONFIG_TREE_CLASS__
/*****************************************************************************/
class Config_Info:public wxTreeItemData
{
public:
    std::string Title;
};

class Config_Drop:public wxFileDropTarget
{
public:
    virtual bool OnDropFiles(wxCoord X, wxCoord Y, const wxArrayString& Filenames) final override;
};

class Config_Tree:public wxTreeCtrl
{
public:
    /* This is the default and need to keep it */
    class wxTreeItemId Basic_Config;
    /* We may add child to these nodes and need to keep them */
    class wxTreeItemId Native_Config;
    class wxTreeItemId Virtual_Config;

    ptr_t Select_Detail;
    class wxTreeItemId Drag;
    class wxTreeItemId Bold;
    class wxTreeItemId Active;

    class Config_Popup* Popup;

    /* void */ Config_Tree(class wxWindow* Parent);
    /* void */ ~Config_Tree(void);

    void Load(void);
    ret_t Detail(class wxTreeItemId& Select);

    void Native_Rename(const std::string& Original, const std::string& Current);
    void Virtual_Rename(const std::string& Original, const std::string& Current);

    void State_Set(void);

    void On_Activate(class wxTreeEvent& Event);
    void On_Menu(class wxTreeEvent& Event);
    void On_Drag_Begin(class wxTreeEvent& Event);
    void On_Drag_End(class wxTreeEvent& Event);
};
/*****************************************************************************/
/* __CONFIG_TREE_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
