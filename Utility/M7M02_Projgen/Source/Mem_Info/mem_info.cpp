/******************************************************************************
Filename    : rme_mem.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The memory block class.
******************************************************************************/

/* Includes ******************************************************************/
#include "string"
#include "memory"
#include "vector"
#include "bitset"
#include "stdexcept"

extern "C"
{
#include "xml.h"
}

#define __HDR_DEFS__
#include "Main/rme_mcu.hpp"
#include "Main/rme_mem.hpp"

#include "Kobj/rme_kobj.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "Main/rme_mem.hpp"

#include "Kobj/rme_kobj.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Mem::Mem ****************************************************
Description : Constructor for Mem class.
Input       : xml_node_t* Node - The node containing the memory block information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem::Mem(xml_node_t* Node)
{
    xml_node_t* Temp;
    std::unique_ptr<std::string> Str;

    try
    {
        /* By default, we are not shared memory (this attribute used only in processes) */
        this->Is_Shared=0;

        /* Name - not necessarily exist */
        if((XML_Child(Node,"Name",&Temp)==0)&&(Temp!=0))
        {
            if(Temp->XML_Val_Len==0)
                this->Name=nullptr;
            this->Name=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);
        }
        else
            this->Name=nullptr;

        /* Base */
        if((XML_Child(Node,"Base",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0500: Base section is missing.");
        if((Temp->XML_Val_Len==4)&&(std::string(Temp->XML_Val,4)=="Auto"))
            this->Base=MEM_AUTO;
        else if(XML_Get_Hex(Temp,&(this->Base))<0)
            throw std::invalid_argument("P0501: Base is neither automatically allocated nor a valid hex integer.");

        /* Size */
        if((XML_Child(Node,"Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0502: Size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Size))<0)
            throw std::invalid_argument("P0503: Size is not a valid hex integer.");
        if(this->Size==0)
            throw std::invalid_argument("P0504: Size cannot be zero.");
        if(this->Size>0x100000000ULL)
            throw std::invalid_argument("P0505: Size is out of bound.");

        /* Type */
        if((XML_Child(Node,"Type",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0506: Type section is missing.");
        if((Temp->XML_Val_Len==6)&&(std::string(Temp->XML_Val,6)=="Device"))
        {
            if(this->Base==MEM_AUTO)
                throw std::invalid_argument("P0507: Device-type memory cannot be automatically allocated.");
        }

        /* Attribute */
        if((XML_Child(Node,"Attribute",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0508: Attribute section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("P0509: Attribute section is empty.");
        Str=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);
        this->Attr=0;

        if(Str->rfind('R')!=std::string::npos)
            this->Attr|=MEM_READ;
        if(Str->rfind('W')!=std::string::npos)
            this->Attr|=MEM_WRITE;
        if(Str->rfind('X')!=std::string::npos)
            this->Attr|=MEM_EXECUTE;

        if(this->Attr==0)
            throw std::invalid_argument("P0510: Attribute does not allow any access and is malformed.");

        if(Str->rfind('B')!=std::string::npos)
            this->Attr|=MEM_BUFFERABLE;
        if(Str->rfind('C')!=std::string::npos)
            this->Attr|=MEM_CACHEABLE;
        if(Str->rfind('S')!=std::string::npos)
            this->Attr|=MEM_STATIC;
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Memory:\n")+Exc.what());
    }
}
/* End Function:Mem::Mem *****************************************************/

/* Begin Function:Mem::Mem ****************************************************
Description : Constructor for Mem class.
Input       : ptr_t Base - The start address.
              ptr_t Size - The memory trunk size.
              ptr_t Attr - The attributes of this memory block.
              ptr_t Align - The alignment size.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem::Mem(ptr_t Base, ptr_t Size, ptr_t Attr, ptr_t Align)
{
    this->Base=Base;
    this->Size=Size;
    this->Attr=Attr;
    this->Align=Align;
    /* By default, we are not shared memory (this attribute used only in processes) */
    this->Is_Shared=0;
}
/* End Function:Mem::Mem *****************************************************/

/* Begin Function:Mem::Mem ****************************************************
Description : Constructor for Mem class.
Input       : class Mem* Block - The block to copy from.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Mem::Mem(class Mem* Block)
{
    /* Copy everything of that source block */
    if(Block->Name!=nullptr)
        this->Name=std::make_unique<std::string>(Block->Name->c_str());
    else
        this->Name=nullptr;

    this->Base=Block->Base;
    this->Size=Block->Size;
    this->Attr=Block->Attr;
    this->Align=Block->Align;
    this->Is_Shared=Block->Is_Shared;
}
/* End Function:Mem::Mem *****************************************************/

/* Begin Function:Shmem::Shmem ************************************************
Description : Constructor for Shmem class.
Input       : xml_node_t* Node - The node containing the memory block information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Shmem::Shmem(xml_node_t* Node)
{
    xml_node_t* Temp;
    std::unique_ptr<std::string> Str;

    try
    {
        /* Name */
        if((XML_Child(Node,"Name",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0600: Name section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("P0601: Name section is empty.");
        this->Name=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);

        /* Attribute */
        if((XML_Child(Node,"Attribute",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0602: Attribute section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("P0603: Attribute section is empty.");
        Str=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);
        this->Attr=0;

        if(Str->rfind('R')!=std::string::npos)
            this->Attr|=MEM_READ;
        if(Str->rfind('W')!=std::string::npos)
            this->Attr|=MEM_WRITE;
        if(Str->rfind('X')!=std::string::npos)
            this->Attr|=MEM_EXECUTE;

        if(this->Attr==0)
            throw std::invalid_argument("P0604: Attribute does not allow any access and is malformed.");

        if(Str->rfind('B')!=std::string::npos)
            this->Attr|=MEM_BUFFERABLE;
        if(Str->rfind('C')!=std::string::npos)
            this->Attr|=MEM_CACHEABLE;
        if(Str->rfind('S')!=std::string::npos)
            this->Attr|=MEM_STATIC;
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Shared memory:\n")+Exc.what());
    }
}
/* End Function:Shmem::Shmem *************************************************/

/* Begin Function:Memmap::Memmap **********************************************
Description : Constructor for Memmap class.
Input       : std::unique_ptr<class Mem>& Mem - The memory trunk.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Memmap::Memmap(std::unique_ptr<class Mem>& Mem)
{
    /* This pointer does not assume ownership */
    this->Mem=Mem.get();
    this->Map.resize((unsigned int)(Mem->Size/MAP_ALIGN+1));
    std::fill(this->Map.begin(), this->Map.end(), false);
}
/* End Function:Memmap::Memmap ***********************************************/

/* Begin Function:Memmap::Try *************************************************
Description : See if this bitmap segment is already covered.
Input       : std::unique_ptr<class Memmap>& Map - The bitmap.
              ptr_t Base - The starting address.
              ptr_t Size - The size to allocate.
Output      : None.
Return      : ret_t - If can be marked, 0; else -1.
******************************************************************************/
ret_t Memmap::Try(std::unique_ptr<class Memmap>& Map, ptr_t Base, ptr_t Size)
{
    ptr_t Count;
    ptr_t Bit_Base;
    ptr_t Bit_Size;

    /* See if we can fit there */
    if((Base<Map->Mem->Base)&&(Base>=(Map->Mem->Base+Map->Mem->Size)))
        return -1;
    if((Map->Mem->Base+Map->Mem->Size)<(Base+Size))
        return -1;

    Bit_Base=(Base-Map->Mem->Base)/MAP_ALIGN;
    Bit_Size=Size/MAP_ALIGN;
    
    for(Count=Bit_Base;Count<Bit_Base+Bit_Size;Count++)
    {
        if(Map->Map[(unsigned int)Count]!=false)
            return -1;
    }
    return 0;
}
/* End Function:Memmap::Try **************************************************/

/* Begin Function:Memmap::Mark ************************************************
Description : Actually mark this bitmap segment. Each bit is always 4 bytes.
Input       : std::unique_ptr<class Memmap>& Map - The bitmap.
              ptr_t Base - The starting address.
              ptr_t Size - The size to allocate.
Output      : std::unique_ptr<class Memmap>& Map - The updated bitmap.
Return      : None.
******************************************************************************/
ret_t Memmap::Mark(std::unique_ptr<class Memmap>& Map, ptr_t Base, ptr_t Size)
{
    ptr_t Count;
    ptr_t Bit_Base;
    ptr_t Bit_Size;

    /* See if we can fit there */
    if((Base<Map->Mem->Base)&&(Base>=(Map->Mem->Base+Map->Mem->Size)))
        return -1;
    if((Map->Mem->Base+Map->Mem->Size)<(Base+Size))
        return -1;

    Bit_Base=(Base-Map->Mem->Base)/MAP_ALIGN;
    Bit_Size=Size/MAP_ALIGN;

    for(Count=Bit_Base;Count<Bit_Base+Bit_Size;Count++)
        Map->Map[(unsigned int)Count]=true;

    return 0;
}
/* End Function:Memmap::Mark *************************************************/

/* Begin Function:Memmap::Fit_Static ******************************************
Description : Populate the memory data structure with this memory segment.
              This operation will be conducted with no respect to whether this
              portion have been populated with someone else.
Input       : std::vector<std::unique_ptr<class Memmap>>& Map - The memory map.
              ptr_t Base - The start address of the memory.
              ptr_t Size - The size of the memory.
              ptr_t Attr - The attributes of the memory.
Output      : std::vector<std::unique_ptr<class Memmap>>& Map - The updated memory map.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Memmap::Fit_Static(std::vector<std::unique_ptr<class Memmap>>& Map,
                         ptr_t Base, ptr_t Size, ptr_t Attr)
{
    class Mem* Mem;

    /* See if we can even find a segment that accomodates this */
    for(std::unique_ptr<class Memmap>& Info:Map)
    {
        Mem=Info->Mem;
        if((Base>=Mem->Base)&&(Base<(Mem->Base+Mem->Size)))
        {
            /* See if we can fit there */
            if((Mem->Base+Mem->Size)<(Base+Size))
                return -1;

            /* Is the attributes correct? */
            if((Mem->Attr&Attr)!=Attr)
                return -1;

            /* It is clear that we can fit now. Mark all the bits. We do not check it it
             * is already marked, because we allow overlapping. */
            return Mark(Info, Base, Size);
        }
    }

    return -1;
}
/* End Function:Memmap::Fit_Static *******************************************/

/* Begin Function:Memmap::Fit_Auto ********************************************
Description : Fit the auto-placed memory segments to a fixed location.
Input       : std::vector<std::unique_ptr<class Memmap>>& Map - The memory map.
              ptr_t Base - The start address of the memory.
              ptr_t Size - The size of the memory.
              ptr_t Attr - The attributes of the memory.
              ptr_t Align - The alignment granularity of the memory.
Output      : std::vector<std::unique_ptr<class Memmap>>& Map - The updated memory map.
              ptr_t* Start - The start address of the memory.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Memmap::Fit_Auto(std::vector<std::unique_ptr<class Memmap>>& Map,
                       ptr_t* Base, ptr_t Size, ptr_t Align, ptr_t Attr)
{
    ptr_t Fit_Start;
    ptr_t Fit_End;
    ptr_t Fit_Try;
    class Mem* Fit;

    /* Find somewhere to fit this memory trunk, and if found, we will populate it */
    for(std::unique_ptr<class Memmap>& Info:Map)
    {
        Fit=Info->Mem;

        /* Is the size possibly sufficient? */
        if(Size>Fit->Size)
            continue;

        /* Is the attribute a superset of what we require? */
        if((Fit->Attr&Attr)!=Attr)
            continue;

        /* Round start address up, round end address down, to alignment */
        Fit_Start=((Fit->Base+Align-1)/Align)*Align;
        Fit_End=((Fit->Base+Fit->Size)/Align)*Align;

        if(Size>(Fit_End-Fit_Start))
            continue;

        for(Fit_Try=Fit_Start;Fit_Try<Fit_End;Fit_Try+=Align)
        {
            if(Try(Info,Fit_Try,Size)==0)
            {
                /* Found a fit */
                Mark(Info,Fit_Try,Size);
                *Base=Fit_Try;
                return 0;
            }
        }
    }

    /* Can't find any fit */
    return -1;
}
/* End Function:Memmap::Fit_Auto *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
