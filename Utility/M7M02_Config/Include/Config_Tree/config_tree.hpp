/******************************************************************************
Filename    : config_tree.hpp
Author      : pry
Date        : 16/08/2021
License     : Proprietary; confidential.
Description : Config tree file header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __CONFIG_TREE_HPP_TYPES__
#define __CONFIG_TREE_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __CONFIG_TREE_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __CONFIG_TREE_HPP_DEFS__
#define __CONFIG_TREE_HPP_DEFS__
/*****************************************************************************/
#define SELECT_DETAIL_NONE          (0)
#define SELECT_DETAIL_KERNEL        (1)
#define SELECT_DETAIL_MONITOR       (2)
#define SELECT_DETAIL_FOLDER        (3)
#define SELECT_DETAIL_VIRTUAL       (4)
#define SELECT_DETAIL_PROCESS       (5)
/*****************************************************************************/
/* __CONFIG_TREE_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __CONFIG_TREE_HPP_CLASSES__
#define __CONFIG_TREE_HPP_CLASSES__
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
    ptr_t Select_Detail;
    class wxTreeItemId Active;
    class wxTreeItemId Drag;
    class Config_Popup* Popup;

    /* void */ Config_Tree(class wxWindow* Parent);
    /* void */ ~Config_Tree(void);

    void Load(void);
    ret_t Detail(class wxTreeItemId& Select);

    void State_Set(ptr_t Type);

    void On_Activate(class wxTreeEvent& Event);
    void On_Menu(class wxTreeEvent& Event);
    void On_Drag_Begin(class wxTreeEvent& Event);
    void On_Drag_End(class wxTreeEvent& Event);
};
/*****************************************************************************/
/* __CONFIG_TREE_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
