/******************************************************************************
Filename    : rme_guest_rmp.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the guest class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __RME_GUEST_RMP_HPP_DEFS__
#define __RME_GUEST_RMP_HPP_DEFS__
/*****************************************************************************/
    
/*****************************************************************************/
/* __RME_GUEST_RMP_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __RME_GUEST_RMP_HPP_CLASSES__
#define __RME_GUEST_RMP_HPP_CLASSES__
/*****************************************************************************/
/* RMP guest operating system information */
class Guest_RMP
{
public:
    static void Generate(class Main* Main, class Virt* Virt,
                         std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Inc,
                         std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Src);
    static void A7M(class Main* Main, class Virt* Virt,
                    std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Inc,
                    std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Src);
    static void A7M_Keil(class Main* Main, class Virt* Virt,
                         std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Inc,
                         std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Src);
};
/*****************************************************************************/
/* __RME_GUEST_RMP_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
