/******************************************************************************
Filename    : rme_proc.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The process class.
******************************************************************************/

/* Includes ******************************************************************/
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

extern "C"
{
#include "xml.h"
}

#define __HDR_DEFS__
#include "Main/rme_mcu.hpp"
#include "Main/rme_raw.hpp"
#include "Main/rme_comp.hpp"
#include "Main/rme_mem.hpp"

#include "Kobj/rme_kobj.hpp"
#include "Kobj/rme_captbl.hpp"
#include "Kobj/rme_pgtbl.hpp"
#include "Kobj/rme_thd.hpp"
#include "Kobj/rme_inv.hpp"
#include "Kobj/rme_port.hpp"
#include "Kobj/rme_recv.hpp"
#include "Kobj/rme_send.hpp"
#include "Kobj/rme_vect.hpp"
#include "Kobj/rme_kern.hpp"
#include "Kobj/rme_proc.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "Main/rme_raw.hpp"
#include "Main/rme_comp.hpp"
#include "Main/rme_mem.hpp"

#include "Kobj/rme_kobj.hpp"
#include "Kobj/rme_captbl.hpp"
#include "Kobj/rme_pgtbl.hpp"
#include "Kobj/rme_thd.hpp"
#include "Kobj/rme_inv.hpp"
#include "Kobj/rme_port.hpp"
#include "Kobj/rme_recv.hpp"
#include "Kobj/rme_send.hpp"
#include "Kobj/rme_vect.hpp"
#include "Kobj/rme_kern.hpp"
#include "Kobj/rme_proc.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:RME::RME ****************************************************
Description : Constructor for RME class.
Input       : xml_node_t* Node - The node containing the whole project.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ RME::RME(xml_node_t* Node)
{
    xml_node_t* Temp;
    xml_node_t* Trunk;
    
    try
    {
        /* Code base */
        if((XML_Child(Node,"Code_Base",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0100: Code base section is missing.");
        if(XML_Get_Hex(Temp,&(this->Code_Base))<0)
            throw std::invalid_argument("P0101: Code base is not a valid hex integer.");

        /* Code size */
        if((XML_Child(Node,"Code_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0102: Code size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Code_Size))<0)
            throw std::invalid_argument("P0103: Code size is not a valid hex integer.");

        /* Data base */
        if((XML_Child(Node,"Data_Base",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0104: Data base section is missing.");
        if(XML_Get_Hex(Temp,&(this->Data_Base))<0)
            throw std::invalid_argument("P0105: Data base is not a valid hex integer.");

        /* Data size */
        if((XML_Child(Node,"Data_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0106: Data size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Data_Size))<0)
            throw std::invalid_argument("P0107: Data size is not a valid hex integer.");

        /* Stack size */
        if((XML_Child(Node,"Stack_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0108: Stack size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Stack_Size))<0)
            throw std::invalid_argument("P0109: Stack size is not a valid hex integer.");

        /* Extra kernel memory */
        if((XML_Child(Node,"Extra_Kmem",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0110: Extra kernel memory section is missing.");
        if(XML_Get_Hex(Temp,&(this->Extra_Kmem))<0)
            throw std::invalid_argument("P0111: Extra kernel memory is not a valid hex integer.");

        /* Kmem_Order */
        if((XML_Child(Node,"Kmem_Order",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0112: Kernel memory order section is missing.");
        if(XML_Get_Uint(Temp,&(this->Kmem_Order))<0)
            throw std::invalid_argument("P0113: Kernel memory order is not a valid unsigned integer.");

        /* Priorities */
        if((XML_Child(Node,"Kern_Prios",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0114: Priority number section is missing.");
        if(XML_Get_Uint(Temp,&(this->Kern_Prios))<0)
            throw std::invalid_argument("P0115: Priority number is not a valid unsigned integer.");

        /* Compiler */
        if((XML_Child(Node,"Compiler",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0116: Compiler option section is missing.");
        this->Comp=std::make_unique<class Comp>(Temp);

        /* Platform */
        if((XML_Child(Node,"Platform",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0117: Platform option section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0118: Platform option section parsing internal error.");
        while(Trunk!=0)
        {
            this->Plat.push_back(std::make_unique<class Raw>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0118: Platform option section parsing internal error.");
        }

        /* Chip */
        if((XML_Child(Node,"Chip",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0119: Chip option section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0120: Chip option section parsing internal error.");
        while(Trunk!=0)
        {
            this->Chip.push_back(std::make_unique<class Raw>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0120: Chip option section parsing internal error.");
        }

        this->Map=std::make_unique<class RME_Memmap>();
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("RME:\n")+Exc.what());
    }
}
/* End Function:RME::RME *****************************************************/

/* Begin Function:RME::Alloc_Kmem *********************************************
Description : Allocate the kernel objects and memory for RME itself.
Input       : ptr_t Kmem_Front - The current kernel memory frontier.
              ptr_t Kmem_Order - The kernel memory order.
Output      : None.
Return      : None.
******************************************************************************/
void RME::Alloc_Kmem(ptr_t Kmem_Front, ptr_t Kmem_Order)
{
    /* Code section */
    this->Map->Code_Base=this->Code_Base;
    this->Map->Code_Size=this->Code_Size;

    /* Data section */
    this->Map->Data_Base=this->Data_Base;
    this->Map->Data_Size=this->Data_Size;

    /* Vector flag section - cut out from the data section */
    this->Map->Vctf_Size=ROUND_UP(KERNEL_VCTF_SIZE,Kmem_Order);
    this->Map->Vctf_Base=this->Map->Data_Base+this->Map->Data_Size-this->Map->Vctf_Size;
    if(this->Map->Vctf_Base<=this->Map->Data_Base)
        throw std::runtime_error("M2100: RME data section is not big enough, unable to allocate vector flags.");
    this->Map->Data_Size=this->Map->Vctf_Base-this->Map->Data_Base;

    /* Event flag section - cut out from the data section */
    this->Map->Evtf_Size=ROUND_UP(KERNEL_EVTF_SIZE,Kmem_Order);
    this->Map->Evtf_Base=this->Map->Data_Base+this->Map->Data_Size-this->Map->Evtf_Size;
    if(this->Map->Evtf_Base<=this->Map->Data_Base)
        throw std::runtime_error("M2101: RME data section is not big enough, unable to allocate event flags.");
    this->Map->Data_Size=this->Map->Evtf_Base-this->Map->Data_Base;

    /* Stack section - cut out from the data section */
    this->Map->Stack_Size=ROUND_UP(this->Stack_Size,Kmem_Order);
    this->Map->Stack_Base=this->Map->Data_Base+this->Map->Data_Size-this->Map->Stack_Size;
    if(this->Map->Stack_Base<=this->Map->Data_Base)
        throw std::runtime_error("M2102: RME data section is not big enough, unable to allocate kernel stacks.");
    this->Map->Data_Size=this->Map->Stack_Base-this->Map->Data_Base;

    /* Kernel memory section - cut out from the data section, at least aligned to order 6 */
    this->Map->Kmem_Size=Kmem_Front+this->Extra_Kmem;
    if(Kmem_Order<6)
        this->Map->Kmem_Size=ROUND_UP(this->Map->Kmem_Size,6);
    else
        this->Map->Kmem_Size=ROUND_UP(this->Map->Kmem_Size,Kmem_Order);

    this->Map->Kmem_Base=this->Map->Data_Base+this->Map->Data_Size-this->Map->Kmem_Size;

    if(Kmem_Order>6)
        this->Map->Kmem_Base=ROUND_DOWN(this->Map->Kmem_Base,Kmem_Order);
    else
        this->Map->Kmem_Base=ROUND_DOWN(this->Map->Kmem_Base,6);

    if(this->Map->Kmem_Base<=this->Map->Data_Base)
        throw std::runtime_error("M2103: RME data section is not big enough, unable to allocate kernel object memory.");

    this->Map->Data_Size=this->Map->Kmem_Base-this->Map->Data_Base;
}
/* End Function:RME::Alloc_Kmem **********************************************/

/* Begin Function:Cap::Cap ****************************************************
Description : Constructor for Cap class.
Input       : class Proc* Proc - The pointer to the process.
              class Kobj* Kobj - The pointer to the kernel object.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Cap::Cap(class Proc* Proc, class Kobj* Kobj)
{
    this->Proc=Proc;
    this->Kobj=Kobj;
}
/* End Function:Cap::Cap *****************************************************/

/* Begin Function:RVM::RVM ****************************************************
Description : Constructor for RVM class.
Input       : xml_node_t* Node - The node containing the whole project.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ RVM::RVM(xml_node_t* Node)
{
    xml_node_t* Temp;
    std::unique_ptr<std::string> Str;
    
    try
    {
        /* Code size */
        if((XML_Child(Node,"Code_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0200: Code size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Code_Size))<0)
            throw std::invalid_argument("P0201: Code size is not a valid hex integer.");

        /* Data size */
        if((XML_Child(Node,"Data_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0202: Data size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Data_Size))<0)
            throw std::invalid_argument("P0203: Data size is not a valid hex integer.");

        /* Stack size */
        if((XML_Child(Node,"Stack_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0204: Stack size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Stack_Size))<0)
            throw std::invalid_argument("P0205: Stack size is not a valid hex integer.");

        /* Extra Captbl */
        if((XML_Child(Node,"Extra_Captbl",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0206: Extra capability table size section is missing.");
        if(XML_Get_Uint(Temp,&(this->Extra_Captbl))<0)
            throw std::invalid_argument("P0207: Extra capability table size is not a valid unsigned integer.");

        /* Compiler */
        if((XML_Child(Node,"Compiler",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0208: Compiler section is missing.");
        this->Comp=std::make_unique<class Comp>(Temp);

        /* Number of virtual machine priorities */
        if((XML_Child(Node,"Virt_Prios",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0209: Virtual machine priorities section is missing.");
        if(XML_Get_Uint(Temp,&(this->Virt_Prios))<0)
            throw std::invalid_argument("P0210: Virtual machine priorities is not a valid unsigned integer.");

        /* Number of virtual machine events from native processes */
        if((XML_Child(Node,"Virt_Evts",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0211: Virtual machine event number section is missing.");
        if(XML_Get_Uint(Temp,&(this->Virt_Evts))<0)
            throw std::invalid_argument("P0212: Virtual machine event number is not a valid unsigned integer.");

        /* Number of virtual machine events from native processes */
        if((XML_Child(Node,"Virt_Maps",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0213: Virtual machine mapping total number section is missing.");
        if(XML_Get_Uint(Temp,&(this->Virt_Maps))<0)
            throw std::invalid_argument("P0214: Virtual machine mapping total number is not a valid unsigned integer.");
        
        this->Map=std::make_unique<class RVM_Memmap>();
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("RVM:\n")+Exc.what());
    }
}
/* End Function:RVM::RVM *****************************************************/

/* Begin Function:RVM::Alloc_Mem **********************************************
Description : Allocate the kernel objects and memory for RVM user-level library.
Input       : ptr_t Code_Start - The code start position for RVM.
              ptr_t Data_Start - The data start position for RVM.
              ptr_t Kmem_Order - The kernel memory order.
Output      : None.
Return      : None.
******************************************************************************/
void RVM::Alloc_Mem(ptr_t Code_Start, ptr_t Data_Start, ptr_t Kmem_Order)
{
    /* Code section */
    this->Map->Code_Base=Code_Start;
    this->Map->Code_Size=this->Code_Size;

    /* Data section */
    this->Map->Data_Base=Data_Start;
    this->Map->Data_Size=this->Data_Size;

    /* Init stack section - cut out from the data section */
    this->Map->Init_Stack_Size=ROUND_UP(this->Stack_Size,Kmem_Order);
    this->Map->Init_Stack_Base=this->Map->Data_Base+this->Map->Data_Size-this->Map->Init_Stack_Size;
    if(this->Map->Init_Stack_Base<=this->Map->Data_Base)
        throw std::runtime_error("M2200: RVM data section is not big enough, unable to allocate init thread stack.");
    this->Map->Data_Size=this->Map->Init_Stack_Base-this->Map->Data_Base;

    /* Safety daemon stack section - cut out from the data section */
    this->Map->Sftd_Stack_Size=ROUND_UP(this->Stack_Size,Kmem_Order);
    this->Map->Sftd_Stack_Base=this->Map->Data_Base+this->Map->Data_Size-this->Map->Sftd_Stack_Size;
    if(this->Map->Sftd_Stack_Base<=this->Map->Data_Base)
        throw std::runtime_error("M2201: RVM data section is not big enough, unable to allocate safety daemon thread stack.");
    this->Map->Data_Size=this->Map->Sftd_Stack_Base-this->Map->Data_Base;

    /* Only do these when we are using the virtual machine portion */
    if(this->Virt_Prios!=0)
    {
        /* VMM daemon stack section - cut out from the data section */
        this->Map->Vmmd_Stack_Size=ROUND_UP(this->Stack_Size,Kmem_Order);
        this->Map->Vmmd_Stack_Base=this->Map->Data_Base+this->Map->Data_Size-this->Map->Vmmd_Stack_Size;
        if(this->Map->Vmmd_Stack_Base<=this->Map->Data_Base)
            throw std::runtime_error("M2202: RVM data section is not big enough, unable to allocate virtual machine monitor daemon thread stack.");
        this->Map->Data_Size=this->Map->Vmmd_Stack_Base-this->Map->Data_Base;
    
        /* Vector daemon stack section - cut out from the data section */
        this->Map->Vctd_Stack_Size=ROUND_UP(this->Stack_Size,Kmem_Order);
        this->Map->Vctd_Stack_Base=this->Map->Data_Base+this->Map->Data_Size-this->Map->Vctd_Stack_Size;
        if(this->Map->Vctd_Stack_Base<=this->Map->Data_Base)
            throw std::runtime_error("M2203: RVM data section is not big enough, unable to allocate vector handling daemon stack.");
        this->Map->Data_Size=this->Map->Vctd_Stack_Base-this->Map->Data_Base;

        /* Timer stack section - cut out from the data section */
        this->Map->Timd_Stack_Size=ROUND_UP(this->Stack_Size,Kmem_Order);
        this->Map->Timd_Stack_Base=this->Map->Data_Base+this->Map->Data_Size-this->Map->Timd_Stack_Size;
        if(this->Map->Timd_Stack_Base<=this->Map->Data_Base)
            throw std::runtime_error("M2204: RVM data section is not big enough, unable to allocate timer handling thread stack.");
        this->Map->Data_Size=this->Map->Timd_Stack_Base-this->Map->Data_Base;
    }
    else
    {
        this->Map->Vmmd_Stack_Base=0;
        this->Map->Vmmd_Stack_Size=0;
        this->Map->Vctd_Stack_Base=0;
        this->Map->Vctd_Stack_Size=0;
        this->Map->Timd_Stack_Base=0;
        this->Map->Timd_Stack_Size=0;
    }
}
/* End Function:RVM::Alloc_Mem ***********************************************/

/* Begin Function:Proc::Proc **************************************************
Description : Constructor for Proc class.
Input       : xml_node_t* Node - The node containing the process information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Proc::Proc(xml_node_t* Node)
{
    xml_node_t* Temp;
    xml_node_t* Trunk;
    xml_node_t* Mem_Type;
    std::unique_ptr<std::string> Str;

    try
    {
        /* The type of this process is a native process */
        this->Type=PROC_TYPE_NATIVE;

        /* Name */
        if((XML_Child(Node,"Name",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0300: Name section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("P0301: Name section is empty.");
        this->Name=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);

        /* Extra_Captbl */
        if((XML_Child(Node,"Extra_Captbl",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0302: Extra capability table size section is missing.");
        this->Captbl=std::make_unique<class Captbl>();
        if(XML_Get_Uint(Temp,&(this->Captbl->Extra))<0)
            throw std::invalid_argument("P0303: Extra capability table size is not a valid unsigned integer.");
        /* Need extra captbl space to place event sender kernel capability */
        this->Captbl->Extra+=PROC_EXTRA_CAPTBL;

        /* Compiler */
        if((XML_Child(Node,"Compiler",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0304: Compiler section is missing.");
        this->Comp=std::make_unique<class Comp>(Temp);

        /* Memory */
        if((XML_Child(Node,"Memory",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0305: Memory section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0306: Memory section parsing internal error.");
        if(Trunk==0)
            throw std::invalid_argument("P0307: Memory section is empty.");
        while(Trunk!=0)
        {
            if((XML_Child(Trunk,"Type",&Mem_Type)<0)||(Mem_Type==0))
                throw std::invalid_argument("P0308: Memory type section is missing.");
            if(Mem_Type->XML_Val_Len==0)
                throw std::invalid_argument("P0309: Memory type section is empty.");
            
            Str=std::make_unique<std::string>(Mem_Type->XML_Val,(int)Mem_Type->XML_Val_Len);

            if(*Str=="Code")
                this->Code.push_back(std::make_unique<class Mem>(Trunk));
            else if(*Str=="Data")
                this->Data.push_back(std::make_unique<class Mem>(Trunk));
            else if(*Str=="Device")
                this->Device.push_back(std::make_unique<class Mem>(Trunk));
            else
                throw std::invalid_argument("P0310: Memory type is malformed.");

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0306: Memory section parsing internal error.");
        }

        /* Shared memory - not necessarily exist */
        if((XML_Child(Node,"Shmem",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0311: Shared memory section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0312: Shared memory section parsing internal error.");
        while(Trunk!=0)
        {
            if((XML_Child(Trunk,"Type",&Mem_Type)<0)||(Mem_Type==0))
                throw std::invalid_argument("P0313: Shared memory type section is missing.");
            if(Mem_Type->XML_Val_Len==0)
                throw std::invalid_argument("P0314: Shared memory type section is empty.");
            
            Str=std::make_unique<std::string>(Mem_Type->XML_Val,(int)Mem_Type->XML_Val_Len);

            if(*Str=="Code")
                this->Shmem_Code.push_back(std::make_unique<class Shmem>(Trunk));
            else if(*Str=="Data")
                this->Shmem_Data.push_back(std::make_unique<class Shmem>(Trunk));
            else if(*Str=="Device")
                this->Shmem_Device.push_back(std::make_unique<class Shmem>(Trunk));
            else
                throw std::invalid_argument("P0315: Shared memory type is malformed.");

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0312: Shared memory parsing internal error.");
        }

        /* Thread */
        if((XML_Child(Node,"Thread",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0316: Thread section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0317: Thread section parsing internal error.");
        if(Trunk==0)
            throw std::invalid_argument("P0318: Thread section is empty.");
        while(Trunk!=0)
        {
            this->Thd.push_back(std::make_unique<class Thd>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0317: Thread section parsing internal error.");
        }

        /* Invocation */
        if((XML_Child(Node,"Invocation",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0319: Invocation section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0320: Invocation section parsing internal error.");
        while(Trunk!=0)
        {
            this->Inv.push_back(std::make_unique<class Inv>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0320: Invocation section parsing internal error.");
        }

        /* Port */
        if((XML_Child(Node,"Port",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0321: Port section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0322: Port section parsing internal error.");
        while(Trunk!=0)
        {
            this->Port.push_back(std::make_unique<class Port>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0322: Port section parsing internal error.");
        }

        /* Receive */
        if((XML_Child(Node,"Receive",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0323: Receive section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0324: Receive section parsing internal error.");
        while(Trunk!=0)
        {
            this->Recv.push_back(std::make_unique<class Recv>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0324: Receive section parsing internal error.");
        }

        /* Send */
        if((XML_Child(Node,"Send",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0325: Send section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0326: Send section parsing internal error.");
        while(Trunk!=0)
        {
            this->Send.push_back(std::make_unique<class Send>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0326: Send section parsing internal error.");
        }

        /* Vector */
        if((XML_Child(Node,"Vector",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0327: Vector section missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0328: Vector section parsing internal error.");
        while(Trunk!=0)
        {
            this->Vect.push_back(std::make_unique<class Vect>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0328: Vector section parsing internal error.");
        }

        /* Kernel function */
        if((XML_Child(Node,"Kernel",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0329: Kernel function section missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0330: Kernel function section parsing internal error.");
        while(Trunk!=0)
        {
            this->Kern.push_back(std::make_unique<class Kern>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0330: Kernel function section parsing internal error.");
        }

        /* Every process must have at least one code and data segment, and they must be static. 
         * The primary code segment allow RXS, the primary data segment must allow RWS */
        if(this->Code.size()==0)
            throw std::invalid_argument("P0331: No primary code section exists.");
        if(this->Data.size()==0)
            throw std::invalid_argument("P0332: No primary data section exists.");

        if(((this->Code[0]->Attr)&(MEM_READ|MEM_EXECUTE|MEM_STATIC))!=(MEM_READ|MEM_EXECUTE|MEM_STATIC))
            throw std::invalid_argument("P0333: Primary code section does not have RXS attribute.");
        
        if(((this->Data[0]->Attr)&(MEM_READ|MEM_WRITE|MEM_STATIC))!=(MEM_READ|MEM_WRITE|MEM_STATIC))
            throw std::invalid_argument("P0334: Primary data section does not have RWS attribute.");

        /* All processes shall have at least one thread */
        if(this->Thd.size()==0)
            throw std::invalid_argument("P0335: No thread exists.");

        this->Map=std::make_unique<class Proc_Memmap>();
    }
    catch(std::exception& Exc)
    {
        if(this->Name!=nullptr)
            throw std::runtime_error(std::string("Process: ")+*(this->Name)+"\n"+Exc.what());
        else
            throw std::runtime_error(std::string("Process: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Proc::Proc ***************************************************/

/* Begin Function:Proc::Check_Kobj ********************************************
Description : Check kernel objects within a process.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proc::Check_Kobj(void)
{
    std::string* Errmsg;
    try
    {
        /* Check for duplicate threads */
        Errmsg=Kobj::Check_Kobj<class Thd>(this->Thd);
        if(Errmsg!=0)
            throw std::invalid_argument(std::string("Thread: ")+*Errmsg+"\nM1015: Name is duplicate or invalid.");

        /* Check for duplicate invocations */
        Errmsg=Kobj::Check_Kobj<class Inv>(this->Inv);
        if(Errmsg!=0)
            throw std::invalid_argument(std::string("Invocation: ")+*Errmsg+"\nM1015: Name is duplicate or invalid.");

        /* Check for duplicate ports */
        Errmsg=Kobj::Check_Kobj_Proc_Name<class Port>(this->Port);
        if(Errmsg!=0)
            throw std::invalid_argument(std::string("Port: ")+*Errmsg+"\nM1016: Name/process name is duplicate or invalid.");

        /* Check for duplicate receive endpoints */
        Errmsg=Kobj::Check_Kobj<class Recv>(this->Recv);
        if(Errmsg!=0)
            throw std::invalid_argument(std::string("Receive endpoint: ")+*Errmsg+"\nM1015: Name is duplicate or invalid.");

        /* Check for duplicate send endpoints */
        Errmsg=Kobj::Check_Kobj_Proc_Name<class Send>(this->Send);
        if(Errmsg!=0)
            throw std::invalid_argument(std::string("Send endpoint: ")+*Errmsg+"\nM1016: Name/process name is duplicate or invalid.");

        /* Check for duplicate kernel functions */
        Errmsg=Kobj::Check_Kobj<class Kern>(this->Kern);
        if(Errmsg!=0)
            throw std::invalid_argument(std::string("Kernel function: ")+*Errmsg+"\nM1015: Name is duplicate or invalid.");
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Process: ")+*(this->Name)+"\n"+Exc.what());
    }
}
/* End Function:Proc::Check_Kobj *********************************************/

/* Begin Function:Proc::Alloc_Loc *********************************************
Description : Allocate local capability table. If this is a virtual machine, 
              allocation always starts with 2 because the first two slots are
              reserved for other purposes.
Input       : ptr_t Capacity - The capacity of the capability table.
Output      : None.
Return      : None.
******************************************************************************/
void Proc::Alloc_Loc(ptr_t Capacity)
{
    ptr_t Capid;

    if(this->Type==PROC_TYPE_NATIVE)
        Capid=PROC_EXTRA_CAPTBL;
    else
        Capid=VIRT_EXTRA_CAPTBL;

    for(std::unique_ptr<class Port>& Port:this->Port)
        Port->Loc_Capid=Capid++;

    for(std::unique_ptr<class Recv>& Recv:this->Recv)
        Recv->Loc_Capid=Capid++;

    for(std::unique_ptr<class Send>& Send:this->Send)
        Send->Loc_Capid=Capid++;

    for(std::unique_ptr<class Vect>& Vect:this->Vect)
        Vect->Loc_Capid=Capid++;

    for(std::unique_ptr<class Kern>& Kern:this->Kern)
        Kern->Loc_Capid=Capid++;

    this->Captbl->Front=Capid;
    this->Captbl->Size=this->Captbl->Front+this->Captbl->Extra;

    if(this->Captbl->Size>Capacity)
        throw std::runtime_error("Process: "+*(this->Name)+"\n"+"M1100: Capability table size exceeded architectural limits.");
}
/* End Function:Proc::Alloc_Loc **********************************************/

/* Begin Function:Proc::Alloc_RVM_Pgtbl ***************************************
Description : Recursively allocate page tables.
Input       : std::unique_ptr<class Proc>& Proc, std::unique_ptr<class Pgtbl>& Pgtbl.
Output      : struct Proj_Info* Proj - The updated project structure.
Return      : None.
******************************************************************************/
void Proc::Alloc_RVM_Pgtbl(std::unique_ptr<class RVM>& RVM,
                           std::unique_ptr<class Pgtbl>& Pgtbl)
{
    ptr_t Count;

    Pgtbl->RVM_Capid=RVM->Pgtbl.size();
    RVM->Pgtbl.push_back(std::make_unique<class Cap>(this,Pgtbl.get()));

    /* Recursively do allocation */
    for(Count=0;Count<Pgtbl->Pgdir.size();Count++)
    {
        if(Pgtbl->Pgdir[(unsigned int)Count]!=nullptr)
            Alloc_RVM_Pgtbl(RVM, Pgtbl->Pgdir[(unsigned int)Count]);
    }
}
/* End Function:Main::Alloc_RVM_Pgtbl ****************************************/

/* Begin Function:Proc::Alloc_RVM *********************************************
Description : Allocate (relative) global capability IDs for all kernel objects. 
              Each global object will reside in its own capability table. 
              This facilitates management, and circumvents the capability size
              limit that may present on 32-bit systems.
              How many distinct kernel objects are there? We just need to add up
              the following: All captbls (each process have one), all processes,
              all threads, all invocations, all receive endpoints. The ports and
              send endpoints do not have a distinct kernel object; the vector 
              endpoints are created by the kernel at boot-time, while the pgtbls
              are decided by architecture-specific code.
Input       : std::unique_ptr<class RVM>& RVM - The RVM struct.
Output      : None.
Return      : None.
******************************************************************************/
void Proc::Alloc_RVM(std::unique_ptr<class RVM>& RVM)
{
    this->Captbl->RVM_Capid=RVM->Captbl.size();
    RVM->Captbl.push_back(std::make_unique<class Cap>(this,this->Captbl.get()));
    Alloc_RVM_Pgtbl(RVM, this->Pgtbl);
    this->RVM_Capid=RVM->Proc.size();
    RVM->Proc.push_back(std::make_unique<class Cap>(this,this));

    for(std::unique_ptr<class Thd>& Thd:this->Thd)
    {
        Thd->RVM_Capid=RVM->Thd.size();
        RVM->Thd.push_back(std::make_unique<class Cap>(this,Thd.get()));
    }

    for(std::unique_ptr<class Inv>& Inv:this->Inv)
    {
        Inv->RVM_Capid=RVM->Inv.size();
        RVM->Inv.push_back(std::make_unique<class Cap>(this,Inv.get()));
    }

    for(std::unique_ptr<class Recv>& Recv:this->Recv)
    {
        Recv->RVM_Capid=RVM->Recv.size();
        RVM->Recv.push_back(std::make_unique<class Cap>(this,Recv.get()));
    }

    for(std::unique_ptr<class Vect>& Vect:this->Vect)
    {
        Vect->RVM_Capid=RVM->Vect.size();
        RVM->Vect.push_back(std::make_unique<class Cap>(this,Vect.get()));
    }
}
/* End Function:Proc::Alloc_RVM **********************************************/

/* Begin Function:Proc::Alloc_Macro_Pgtbl *************************************
Description : Recursively allocate page tables.
Input       : std::unique_ptr<class Proc>& Proc, std::unique_ptr<class Pgtbl>& Pgtbl.
Output      : struct Proj_Info* Proj - The updated project structure.
Return      : None.
******************************************************************************/
void Proc::Alloc_Macro_Pgtbl(std::unique_ptr<class Pgtbl>& Pgtbl)
{
    static ptr_t Serial=0;
    ptr_t Count;

    Serial++;

    if(Pgtbl->Is_Top!=0)
        Serial=0;
    
    Pgtbl->RVM_Macro=std::make_unique<std::string>(std::string("RVM_PGTBL_")+*(this->Name)+
                                                   "_N"+std::to_string(Serial));
    Kobj::To_Upper(Pgtbl->RVM_Macro);

    /* Recursively do allocation */
    for(Count=0;Count<Pgtbl->Pgdir.size();Count++)
    {
        if(Pgtbl->Pgdir[(unsigned int)Count]!=nullptr)
            Alloc_Macro_Pgtbl(Pgtbl->Pgdir[(unsigned int)Count]);
    }
}
/* End Function:Proc::Alloc_Macro_Pgtbl **************************************/

/* Begin Function:Proc::Alloc_Macro *******************************************
Description : Allocate the capability ID macros. Both the local one and the global
              one will be allocated. For virtual machines, only those non-system
              kernel objects plus two threads will have their resources allocated.
              The allocation table is shown below:
-------------------------------------------------------------------------------
Type            Local                           Global
-------------------------------------------------------------------------------
Process         -                               RVM_PROC_<PROCNAME>
-------------------------------------------------------------------------------
Pgtbl           -                               RVM_PGTBL_<PROCNAME>_N#num
-------------------------------------------------------------------------------
Captbl          -                               RVM_CAPTBL_<PROCNAME>
-------------------------------------------------------------------------------
Thread          -                               RVM_PROC_<PROCNAME>_THD_<THDNAME>
-------------------------------------------------------------------------------
Invocation      -                               RVM_PROC_<PROCNAME>_INV_<INVNAME>
-------------------------------------------------------------------------------
Port            PROC_<PROCNAME>_PORT_<PORTNAME> (Inherit invocation name)
-------------------------------------------------------------------------------
Receive         RECV_<ENDPNAME>                 RVM_PROC_<PROCNAME>_RECV_<RECVNAME>
-------------------------------------------------------------------------------
Send            PROC_<PROCNAME>_SEND_<ENDPNAME> (Inherit receive endpoint name)
-------------------------------------------------------------------------------
Vector          VECT_<VECTNAME>                 RVM_BOOT_VECT_<VECTNAME> (RVM)
                                                RME_BOOT_VECT_<VECTNAME> (RME)
-------------------------------------------------------------------------------
Kernel          KERN_<KERNNAME>                 -
-------------------------------------------------------------------------------
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proc::Alloc_Macro(void)
{
    this->RVM_Macro=std::make_unique<std::string>(std::string("RVM_PROC_")+*(this->Name));
    Kobj::To_Upper(this->RVM_Macro);
    Alloc_Macro_Pgtbl(this->Pgtbl);
    this->Captbl->RVM_Macro=std::make_unique<std::string>(std::string("RVM_CAPTBL_")+*(this->Name));
    Kobj::To_Upper(this->Captbl->RVM_Macro);

    for(std::unique_ptr<class Thd>& Thd:this->Thd)
    {
        Thd->RVM_Macro=std::make_unique<std::string>(std::string("RVM_PROC_")+*(this->Name)+"_THD_"+*(Thd->Name));
        Kobj::To_Upper(Thd->RVM_Macro);
    }

    for(std::unique_ptr<class Inv>& Inv:this->Inv)
    {
        Inv->RVM_Macro=std::make_unique<std::string>(std::string("RVM_PROC_")+*(this->Name)+"_INV_"+*(Inv->Name));
        Kobj::To_Upper(Inv->RVM_Macro);
    }

    for(std::unique_ptr<class Port>& Port:this->Port)
    {
        Port->Loc_Macro=std::make_unique<std::string>(std::string("PORT_")+*(Port->Name));
        Kobj::To_Upper(Port->Loc_Macro);
    }

    for(std::unique_ptr<class Recv>& Recv:this->Recv)
    {
        Recv->Loc_Macro=std::make_unique<std::string>(std::string("RECV_")+*(Recv->Name));
        Kobj::To_Upper(Recv->Loc_Macro);
        Recv->RVM_Macro=std::make_unique<std::string>(std::string("RVM_PROC_")+*(this->Name)+"_RECV_"+*(Recv->Name));
        Kobj::To_Upper(Recv->RVM_Macro);
    }

    for(std::unique_ptr<class Send>& Send:this->Send)
    {
        Send->Loc_Macro=std::make_unique<std::string>(std::string("SEND_")+*(Send->Name));
        Kobj::To_Upper(Send->Loc_Macro);
    }

    for(std::unique_ptr<class Vect>& Vect:this->Vect)
    {
        Vect->Loc_Macro=std::make_unique<std::string>(std::string("VECT_")+*(Vect->Name));
        Kobj::To_Upper(Vect->Loc_Macro);
        Vect->RVM_Macro=std::make_unique<std::string>(std::string("RVM_BOOT_VECT_")+*(Vect->Name));
        Kobj::To_Upper(Vect->RVM_Macro);
        Vect->RME_Macro=std::make_unique<std::string>(std::string("RME_BOOT_VECT_")+*(Vect->Name));
        Kobj::To_Upper(Vect->RME_Macro);
    }

    for(std::unique_ptr<class Kern>& Kern:this->Kern)
    {
        Kern->Loc_Macro=std::make_unique<std::string>(std::string("Kern_")+*(Kern->Name));
        Kobj::To_Upper(Kern->Loc_Macro);
    }
}
/* End Function:Proc::Alloc_Macro ********************************************/

/* Begin Function:Proc::Alloc_Mem *********************************************
Description : Allocate process memory.
Input       : ptr_t Word_Bits - The number of bits in a word.
              ptr_t Kmem_Order - The kernel memory order.
Output      : None.
Return      : None.
******************************************************************************/
void Proc::Alloc_Mem(ptr_t Word_Bits, ptr_t Kmem_Order)
{
    class Mem* Mem;
    class Virt* Virt;

    /* Deal with primary code and data sections */
    Mem=this->Code[0].get();
    this->Map->Code_Base=Mem->Base;
    this->Map->Code_Size=Mem->Size;
    this->Map->Entry_Code_Front=this->Map->Code_Base;
    
    Mem=this->Data[0].get();
    this->Map->Data_Base=Mem->Base;
    this->Map->Data_Size=Mem->Size;

    /* Sort the threads according to their priority - The highest priority comes first */
    std::sort(this->Thd.begin(),this->Thd.end(),
    [](std::unique_ptr<class Thd> const& Left, std::unique_ptr<class Thd> const& Right)
    {
            return Left->Prio>Right->Prio;
    });

    /* Threads come first */
    for(std::unique_ptr<class Thd>& Thd:this->Thd)
    {
        try
        {
            /* Allocate stack from the main data memory */
            Thd->Map->Stack_Size=ROUND_UP(Thd->Stack_Size,Kmem_Order);
            Thd->Map->Stack_Base=this->Map->Data_Base+this->Map->Data_Size-Thd->Map->Stack_Size;
            if(Thd->Map->Stack_Base<=this->Map->Data_Base)
                throw std::runtime_error("M2300: Data section size is not big enough, unable to allocate stack.");
            this->Map->Data_Size=Thd->Map->Stack_Base-this->Map->Data_Base;

            /* Allocate entry from code memory */
            Thd->Map->Entry_Addr=this->Map->Entry_Code_Front;
            this->Map->Entry_Code_Front+=Word_Bits/8*ENTRY_SLOT_SIZE;

            /* The parameter is always the param, turned into an unsigned integer */
            Thd->Map->Param_Value=Thd->Param;
        }
        catch(std::exception& Exc)
        {
            throw std::runtime_error(std::string("Thread: ")+*(Thd->Name)+"\n"+Exc.what());
        }
    }

    /* Then invocations */
    for(std::unique_ptr<class Inv>& Inv:this->Inv)
    {
        try
        {
            /* Allocate stack from the main data memory */
            Inv->Map->Stack_Size=ROUND_UP(Inv->Stack_Size,Kmem_Order);
            Inv->Map->Stack_Base=this->Map->Data_Base+this->Map->Data_Size-Inv->Map->Stack_Size;
            if(Inv->Map->Stack_Base<=this->Map->Data_Base)
                throw std::runtime_error("M2300: Data section size is not big enough, unable to allocate stack.");
            this->Map->Data_Size=Inv->Map->Stack_Base-this->Map->Data_Base;

            /* Allocate entry from code memory */
            Inv->Map->Entry_Addr=this->Map->Entry_Code_Front;
            this->Map->Entry_Code_Front+=Word_Bits/8*ENTRY_SLOT_SIZE;
        }
        catch(std::exception& Exc)
        {
            throw std::runtime_error(std::string("Invocation: ")+*(Inv->Name)+"\n"+Exc.what());
        }
    }

    /* See if this is a virtual machine. If yes, we go on to allocate its register set space, 
     * parameter space and interrupt space. These are used for communicating through the VM. */
    if(this->Type==PROC_TYPE_VIRT)
    {
        Virt=static_cast<class Virt*>(this);

        /* Vector flag space */
        Virt->Virt_Map->Vctf_Size=ROUND_UP(VIRT_VCTF_SIZE,Kmem_Order);
        Virt->Virt_Map->Vctf_Base=this->Map->Data_Base+this->Map->Data_Size-Virt->Virt_Map->Vctf_Size;
        if(Virt->Virt_Map->Vctf_Base<=this->Map->Data_Base)
            throw std::runtime_error("M2301: Data section size is not big enough, unable to allocate virtual machine interrupt flags.");
        this->Map->Data_Size=Virt->Virt_Map->Vctf_Base-this->Map->Data_Base;

        /* Hypercall parameter space */
        Virt->Virt_Map->Param_Size=ROUND_UP(VIRT_PARAM_SIZE,Kmem_Order);
        Virt->Virt_Map->Param_Base=this->Map->Data_Base+this->Map->Data_Size-Virt->Virt_Map->Param_Size;
        if(Virt->Virt_Map->Param_Base<=this->Map->Data_Base)
            throw std::runtime_error("M2302: Data section size is not big enough, unable to allocate virtual machine parameters.");
        this->Map->Data_Size=Virt->Virt_Map->Param_Base-this->Map->Data_Base;

        /* Register space */
        Virt->Virt_Map->Reg_Size=ROUND_UP(VIRT_REG_SIZE,Kmem_Order);
        Virt->Virt_Map->Reg_Base=this->Map->Data_Base+this->Map->Data_Size-Virt->Virt_Map->Reg_Size;
        if(Virt->Virt_Map->Reg_Base<=this->Map->Data_Base)
            throw std::runtime_error("M2303: Data section size is not big enough, unable to allocate virtual machine registers.");
        this->Map->Data_Size=Virt->Virt_Map->Reg_Base-this->Map->Data_Base;
    }
}
/* End Function:Proc::Alloc_Mem **********************************************/

/* Begin Function:Virt::Virt **************************************************
Description : Constructor for virtual machine class. The virtual machine is a special
              type of process, in which we populate the kernel objects autonomously.
Input       : xml_node_t* Node - The node containing the virtual machine description.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Virt::Virt(xml_node_t* Node)
{
    xml_node_t* Temp;
    xml_node_t* Trunk;
    xml_node_t* Mem_Type;
    std::unique_ptr<std::string> Str;
    std::unique_ptr<class Thd> Thd;
    std::unique_ptr<class Send> Send;
    std::unique_ptr<class Recv> Recv;

    try
    {
        /* The type of this process is a virtual machine */
        this->Type=PROC_TYPE_VIRT;

        /* Name */
        if((XML_Child(Node,"Name",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0400: Name section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("P0401: Name section is empty.");
        this->Name=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);

        /* Guest */
        if((XML_Child(Node,"Guest",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0402: Guest operating system selection section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("P0403: Guest operating system selection section is empty.");
        this->Guest=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);

        /* Extra_Captbl */
        if((XML_Child(Node,"Extra_Captbl",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0404: Extra capability table size section is missing.");
        this->Captbl=std::make_unique<class Captbl>();
        if(XML_Get_Uint(Temp,&(this->Captbl->Extra))<0)
            throw std::invalid_argument("P0405: Extra capability table size is not a valid unsigned integer.");
        /* Need extra captbl space to place initial endpoints */
        this->Captbl->Extra+=VIRT_EXTRA_CAPTBL;

        /* Stack Size */
        if((XML_Child(Node,"Stack_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0406: Stack size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Stack_Size))<0)
            throw std::invalid_argument("P0407: Stack size is not a valid hex integer.");

        /* Priority */
        if((XML_Child(Node,"Priority",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0408: Priority section is missing.");
        if(XML_Get_Uint(Temp,&(this->Prio))<0)
            throw std::invalid_argument("P0409: Priority is not a valid unsigned integer.");

        /* Slices */
        if((XML_Child(Node,"Slices",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0410: Timeslices section is missing.");
        if(XML_Get_Uint(Temp,&(this->Slices))<0)
            throw std::invalid_argument("P0411: Timeslices is not a valid unsigned integer.");
        if(this->Slices==0)
            throw std::invalid_argument("P0412: Timeslices cannot be zero.");

        /* Period */
        if((XML_Child(Node,"Period",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0413: Timer interrupt period section is missing.");
        if(XML_Get_Uint(Temp,&(this->Period))<0)
            throw std::invalid_argument("P0414: Timer interrupt period is not a valid unsigned integer.");
        if(this->Period==0)
            throw std::invalid_argument("P0415: Timer interrupt period cannot be zero.");

        /* Watchdog */
        if((XML_Child(Node,"Watchdog",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0416: Watchdog timeout section is missing.");
        if(XML_Get_Uint(Temp,&(this->Watchdog))<0)
            throw std::invalid_argument("P0417: Watchdog timeout is not a valid unsigned integer.");

        /* Number of interrupts - currently not to exceed 1024 */
        if((XML_Child(Node,"Vect_Num",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0418: Virtual vector number section is missing.");
        if(XML_Get_Uint(Temp,&(this->Vect_Num))<0)
            throw std::invalid_argument("P0419: Virtual vector number is not a valid unsigned integer.");
        if(this->Vect_Num>VIRT_VECT_MAX_NUM)
            throw std::invalid_argument("P0420: Virtual vector number too large.");
        if(this->Vect_Num==0)
            throw std::invalid_argument("P0421: Virtual vector number cannot be zero.");

        /* Compiler */
        if((XML_Child(Node,"Compiler",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0422: Compiler section is missing.");
        this->Comp=std::make_unique<class Comp>(Temp);

        /* Memory */
        if((XML_Child(Node,"Memory",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0423: Memory section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0424: Memory section parsing internal error.");
        if(Trunk==0)
            throw std::invalid_argument("P0425: Memory section is empty.");
        while(Trunk!=0)
        {
            if((XML_Child(Trunk,"Type",&Mem_Type)<0)||(Mem_Type==0))
                throw std::invalid_argument("P0426: Memory type section is missing.");
            if(Mem_Type->XML_Val_Len==0)
                throw std::invalid_argument("P0427: Memory type section is empty.");
            
            Str=std::make_unique<std::string>(Mem_Type->XML_Val,(int)Mem_Type->XML_Val_Len);

            if(*Str=="Code")
                this->Code.push_back(std::make_unique<class Mem>(Trunk));
            else if(*Str=="Data")
                this->Data.push_back(std::make_unique<class Mem>(Trunk));
            else if(*Str=="Device")
                this->Device.push_back(std::make_unique<class Mem>(Trunk));
            else
                throw std::invalid_argument("P0428: Memory type is malformed.");

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0424: Memory section parsing internal error.");
        }

        /* Shared memory - not necessarily exist */
        if((XML_Child(Node,"Shmem",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0429: Shared memory section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0430: Shared memory section parsing internal error.");
        while(Trunk!=0)
        {
            if((XML_Child(Trunk,"Type",&Mem_Type)<0)||(Mem_Type==0))
                throw std::invalid_argument("P0431: Shared memory type section is missing.");
            if(Mem_Type->XML_Val_Len==0)
                throw std::invalid_argument("P0432: Shared memory type section is empty.");
            
            Str=std::make_unique<std::string>(Mem_Type->XML_Val,(int)Mem_Type->XML_Val_Len);

            if(*Str=="Code")
                this->Shmem_Code.push_back(std::make_unique<class Shmem>(Trunk));
            else if(*Str=="Data")
                this->Shmem_Data.push_back(std::make_unique<class Shmem>(Trunk));
            else if(*Str=="Device")
                this->Shmem_Device.push_back(std::make_unique<class Shmem>(Trunk));
            else
                throw std::invalid_argument("P0433: Shared memory type is malformed.");

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0430: Shared memory parsing internal error.");
        }

        /* Vector thread */
        Thd=std::make_unique<class Thd>();
        Thd->Name=std::make_unique<std::string>("Vect");
        Thd->Stack_Size=this->Stack_Size;
        Thd->Param=0;
        Thd->Prio=VIRT_VECT_PRIO;
        Thd->Map=std::make_unique<class Thd_Memmap>();
        this->Thd.push_back(std::move(Thd));

        /* User thread */
        Thd=std::make_unique<class Thd>();
        Thd->Name=std::make_unique<std::string>("User");
        Thd->Stack_Size=this->Stack_Size;
        Thd->Param=0;
        Thd->Prio=VIRT_USER_PRIO;
        Thd->Map=std::make_unique<class Thd_Memmap>();
        this->Thd.push_back(std::move(Thd));

        /* Virtual machine does not allow receiving from other processes, but does allow send operations */
        if((XML_Child(Node,"Send",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0434: Send section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0435: Send section parsing internal error.");
        while(Trunk!=0)
        {
            this->Send.push_back(std::make_unique<class Send>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0435: Send section parsing internal error.");
        }

        /* Virtual machine allows kernel functions as well */
        if((XML_Child(Node,"Kernel",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0436: Kernel function section missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0437: Kernel function section parsing internal error.");
        while(Trunk!=0)
        {
            this->Kern.push_back(std::make_unique<class Kern>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0437: Kernel function section parsing internal error.");
        }

        /* Every process must have at least one code and data segment, and they must be static. 
         * The primary code segment allow RXS, the primary data segment must allow RWS */
        if(this->Code.size()==0)
            throw std::invalid_argument("P0438: No primary code section exists.");
        if(this->Data.size()==0)
            throw std::invalid_argument("P0439: No primary data section exists.");

        if(((this->Code[0]->Attr)&(MEM_READ|MEM_EXECUTE|MEM_STATIC))!=(MEM_READ|MEM_EXECUTE|MEM_STATIC))
            throw std::invalid_argument("P0440: Primary code section does not have RXS attribute.");

        if(((this->Data[0]->Attr)&(MEM_READ|MEM_WRITE|MEM_STATIC))!=(MEM_READ|MEM_WRITE|MEM_STATIC))
            throw std::invalid_argument("P0441: Primary data section does not have RWS attribute.");

        this->Map=std::make_unique<class Proc_Memmap>();
        this->Virt_Map=std::make_unique<class Virt_Memmap>();
    }
    catch(std::exception& Exc)
    {
        if(this->Name!=nullptr)
            throw std::runtime_error(std::string("Virtual machine: ")+*(this->Name)+"\n"+Exc.what());
        else
            throw std::runtime_error(std::string("Virtual machine: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Virt::Virt ***************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
