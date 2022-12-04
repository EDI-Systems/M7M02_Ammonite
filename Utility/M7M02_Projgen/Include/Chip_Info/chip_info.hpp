/******************************************************************************
Filename    : chip_info.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the chip class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __CHIP_INFO_HPP_DEFS__
#define __CHIP_INFO_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __CHIP_INFO_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __CHIP_INFO_HPP_CLASSES__
#define __CHIP_INFO_HPP_CLASSES__
/*****************************************************************************/
/* Chip information */
class Chip_Info
{
public:
    /* The name of the chip class */
    std::string Name;
    std::string Name_Upper;
    std::string Name_Lower;
    /* Version info */
    std::string Version;
    /* The platform */
    std::string Platform;
    /* Compatible chip list */
    std::vector<std::string> Compatible;
    /* The vendor */
    std::string Vendor;
    /* The number of dual-purpose MPU regions */
    ptr_t Region;
    /* The number of instruction-dedicated MPU regions */
    ptr_t Iregion;
    /* The number of data-dedicated MPU regions */
    ptr_t Dregion;

    /* The platform-specific attributes to be passed to the platform-specific generator */
    std::map<std::string,std::string> Attribute;
    /* Memory information */
    std::vector<std::unique_ptr<class Mem_Info>> Memory;
    /* Chip-specific configuration information */
    std::vector<std::unique_ptr<class Conf_Info>> Config;
    std::map<std::string,class Conf_Info*> Config_Map;
    std::map<std::string,class Conf_Info*> Config_Macro_Map;
    /* Interrupt vector information */
    std::vector<std::unique_ptr<class Vect_Info>> Vector;
    std::map<std::string,class Vect_Info*> Vector_Map;
    std::map<ptr_t,class Vect_Info*> Vector_Number_Map;

    /* void */ Chip_Info(xml_node_t* Root);

    void Check(void);

    void Project_Config_Mark_Check(void);
};
/*****************************************************************************/
/* __CHIP_INFO_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
