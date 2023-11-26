/******************************************************************************
Filename    : shmem.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The memory block class. This contains the memory block information.
******************************************************************************/

/* Include *******************************************************************/
extern "C"
{
#include "xml.h"
}

#include "set"
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Proj_Info/Process/Shmem/shmem.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/Process/Shmem/shmem.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Shmem::Shmem ******************************************************
Description : Constructor for memory information class.
Input       : xml_node_t* Root - The node containing the memory block information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Shmem::Shmem(xml_node_t* Root)
{
    std::string Temp;
    std::unique_ptr<std::string> Str;

    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        Main::Idtfr_Check(this->Name,"Name","DXXXX","DXXXX");
        Name_Gen(this);

        /* Attribute */
        this->Attr=0;
        Temp=Main::XML_Get_String(Root,"Attribute","DXXXX","DXXXX");
        if(Temp.rfind('R')!=std::string::npos)
            this->Attr|=MEM_READ;
        if(Temp.rfind('W')!=std::string::npos)
            this->Attr|=MEM_WRITE;
        if(Temp.rfind('X')!=std::string::npos)
            this->Attr|=MEM_EXECUTE;
        if(this->Attr==0)
            Main::Error("XXXXX: Attribute does not allow any access and is malformed.");
        if(Temp.rfind('B')!=std::string::npos)
            this->Attr|=MEM_BUFFER;
        if(Temp.rfind('C')!=std::string::npos)
            this->Attr|=MEM_CACHE;
        if(Temp.rfind('S')!=std::string::npos)
            this->Attr|=MEM_STATIC;
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Memory: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Memory: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Shmem::Shmem *************************************************/

/* Function:Shmem::Check ******************************************************
Description : Check whether the shared memory block reference makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem::Check(void)
{
    /* Nothing to check by now */
}
/* End Function:Shmem::Check *************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
