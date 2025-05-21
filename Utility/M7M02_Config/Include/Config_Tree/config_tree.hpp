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
//#define SELECT_DETAIL_NONE          (0)
//#define SELECT_DETAIL_KERNEL        (1)
//#define SELECT_DETAIL_MONITOR       (2)
//#define SELECT_DETAIL_FOLDER        (3)
//#define SELECT_DETAIL_VIRTUAL       (4)
//#define SELECT_DETAIL_PROCESS       (5)
//
///* New define */
//#define SELECT_DETAIL_UN_PROCESS    (6)
//#define SELECT_DETAIL_NATIVE_ROOT   (7)
//#define SELECT_DETAIL_VIRTUAL_ROOT  (8)
//#define SELECT_DETAIL_NATIVE_CHILD  (9)
//#define SELECT_DETAIL_VIRTUAL_CHILD (10)


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

    /* Name of tree item */
    std::string Root_Text;
    std::string Basic_Config_Text;
    std::string Memory_Config_Text;
    std::string Kernel_Config_Text;
    std::string Monitor_Config_Text;
    std::string Native_Config_Text;
    std::string Virtual_Config_Text;

    /* Native and Virtual may need to have child nodes added. */
    class wxTreeItemId Native_Config;
    class wxTreeItemId Virtual_Config;

    ptr_t Select_Detail;
    class wxTreeItemId Drag;
    class wxTreeItemId Active;

    class Config_Popup* Popup;

    /* void */ Config_Tree(class wxWindow* Parent);
    /* void */ ~Config_Tree(void);

    void Load(void);
    ret_t Detail(class wxTreeItemId& Select);

    void State_Set(void);
    ret_t Option_Open(const std::string& Select_Text, const ptr_t& Type);

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
