/******************************************************************************
Filename    : rme_pgtbl.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the page table class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __PGTBL_DEF__
#define __PGTBL_DEF__
/*****************************************************************************/
#define PGTBL_TOP           1
#define PGTBL_NOM           0
/*****************************************************************************/
/* __PGTBL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __PGTBL_CLASS__
#define __PGTBL_CLASS__
/*****************************************************************************/
/* Page table information */
class Pgtbl:public Kobj
{
public:
    /* Is this a top-level? */
    ptr_t Is_Top;
    /* The base address of the page table */
    ptr_t Base;
    /* The size order */
    ptr_t Size_Order;
    /* The number order */
    ptr_t Num_Order;
    /* The attribute (on this table, not its pages) */
    ptr_t Attr;

    /* Page directories (recursively) mapped in */
    std::vector<std::unique_ptr<class Pgtbl>> Pgdir;
    /* Pages mapped in - if not 0, then attr is directly here */
    std::vector<ptr_t> Page;

    /* void */ Pgtbl(ptr_t Base, ptr_t Size_Order, ptr_t Num_Order, ptr_t Attr, class Process* Owner);
};
/*****************************************************************************/
/* __PGTBL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
