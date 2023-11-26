/******************************************************************************
Filename    : makefile_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for makefile build system.
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
#include "Gen_Tool/gen_tool.hpp"
#include "Gen_Tool/Build_Gen/build_gen.hpp"
#include "Gen_Tool/Build_Gen/Makefile_Gen/makefile_gen.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Captbl/captbl.hpp"
#include "Proj_Info/Process/Pgtbl/pgtbl.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#include "Proj_Info/Process/Invocation/invocation.hpp"
#include "Proj_Info/Process/Port/port.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#include "Proj_Info/Process/Send/send.hpp"
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Vect_Info/vect_info.hpp"
#include "Conf_Info/conf_info.hpp"

#include "Gen_Tool/gen_tool.hpp"
#include "Gen_Tool/Build_Gen/build_gen.hpp"
#include "Gen_Tool/Build_Gen/Makefile_Gen/makefile_gen.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Makefile_Gen::Makefile_Gen ****************************************
Description : Generator for the makefiles.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Makefile_Gen::Makefile_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip) :
Build_Gen("Makefile", Proj, Plat, Chip)
{
    try
    {

    }
    catch (std::exception& Exc)
    {
        Main::Error(std::string("Makefile generator:\n") + Exc.what());
    }
}
/* End Function:Makefile_Gen::Makefile_Gen ***********************************/

/* Function:Makefile_Gen::Suffix **********************************************
Description : Returns suffix for a given type of file.
Input       : ptr_t Type - The file type.
Output      : None.
Return      : std::string - The file suffix.
******************************************************************************/
std::string Makefile_Gen::Suffix(ptr_t Type)
{
    if((Type==BUILD_PROJECT)||(Type==BUILD_WORKSPACE))
        return "";
    else
        Main::Error("XXXXX: File type not recognized.");
}
/* End Function:Makefile_Gen::Suffix *****************************************/

/* Function:Makefile_Gen::Makefile_Proj ***************************************
Description : Generate the makefile project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::string& After - The command to run after compilation.
              const std::string& Target - The target name.
              ptr_t Optimization - The optimization level chosen.
              const std::vector<std::string>& Include - The include paths.
              const std::vector<std::string>& Source - The source file paths.
              const std::string& Linker - The linker script (scatter file) path.
              const std::string& Linker_Misc - The linker miscellaneous control string.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Return      : None.
******************************************************************************/
void Makefile_Gen::Makefile_Proj(std::unique_ptr<std::vector<std::string>>& List,
                                 const std::string& After,
                                 const std::string& Target, const std::string& Optimization,
                                 const std::vector<std::string>& Coprocessor,
                                 const std::vector<std::string>& Include,
                                 const std::vector<std::string>& Source,
                                 const std::string& Linker)
{
    std::string Temp;
    std::string Target_Lower;
    std::string Prefix;
    std::string CPU_Option;
    std::string Opt_Level;

    Target_Lower=Target;
    Main::Lower(Target_Lower);

    /* ARMv6-M, ARMv7-M, ARMv8-M */
    if((this->Chip->Platform=="A6M")||(this->Chip->Platform=="A7M")||(this->Chip->Platform=="A8M"))
    {
        Prefix="arm-none-eabi-";

        /* CPU Type */
    	Temp=this->Chip->Attribute["CPU"];
    	CPU_Option="-mcpu=";
        if (Temp=="CM0")
        	CPU_Option+="cortex-m0";
        else if(Temp=="CM0P")
        	CPU_Option+="cortex-m0+";
        else if(Temp=="CM1")
        	CPU_Option+="cortex-m1";
        else if(Temp=="CM3")
        	CPU_Option+="cortex-m3";
        else if(Temp=="CM4")
        	CPU_Option+="cortex-m4";
        else if(Temp=="CM7")
        	CPU_Option+="cortex-m7";
        else
            Main::Error("XXXXX: Internal processor type error.");

        /* Always thumb mode */
    	CPU_Option+=" -mthumb";

        /* FPU Type */
        if(!Coprocessor.empty())
        {
			if(Coprocessor.size()==1)
			{
				Temp=Coprocessor[0];
				if(Temp!="NONE")
				{
					CPU_Option+=" -mfpu=";
					if(Temp=="FPV4_SP")
						CPU_Option+="fpv4-sp-d16";
					else if(Temp=="FPV5_SP")
						CPU_Option+="fpv5-sp-d16";
					else if(Temp=="FPV5_DP")
						CPU_Option+="fpv5-d16";
					else
						Main::Error("XXXXX: Internal FPU type error.");
				}
			}
			else
				Main::Error("XXXXX: Internal FPU type error.");
        }

        /* Endianness */
        Temp=this->Chip->Attribute["Endian"];
        if(Temp=="Big")
        	CPU_Option+="-mbig-endian";
        else if(Temp!="Little")
            Main::Error("XXXXX: Internal endianness error.");
    }

    /* Generic optimization levels */
    if(Optimization=="O0")
        Opt_Level="-O0";
    else if(Optimization=="O1")
        Opt_Level="-O1";
    else if(Optimization=="O2")
        Opt_Level="-O2";
    else if(Optimization=="O3")
        Opt_Level="-O3";
    else if(Optimization=="Of")
        Opt_Level="-Ofast";
    else if(Optimization=="Os")
        Opt_Level="-Os";
    else
        Main::Error("XXXXX: Internal optimization level error.");

    List->push_back("###############################################################################");
    List->push_back(std::string("#Filename    : ")+Target_Lower);
    List->push_back(std::string("#Author      : ")+CODE_AUTHOR);
    List->push_back(std::string("#Date        : ")+Main::Time);
    List->push_back(std::string("#Licence     : ")+CODE_LICENSE);
    List->push_back("#Description : Generic Makefile (based on gcc). This file is intended");
    List->push_back(std::string("#              to be used with ") + this->Chip->Name + ", and the GNU toolchain.");
    List->push_back("###############################################################################");
    List->push_back("");
    List->push_back("# Configuration ###############################################################");
    List->push_back(std::string("TARGET=")+Target_Lower);
    List->push_back(std::string("CPU=")+CPU_Option);
    List->push_back("");
    List->push_back(std::string("CFLAGS=")+Opt_Level+" -fno-strict-aliasing -fdata-sections -ffunction-sections -ffreestanding -nostdlib -nostartfiles");
    List->push_back("AFLAGS=-fdata-sections -ffunction-sections");
    List->push_back("WFLAGS=-Wall -Wno-strict-aliasing");
    List->push_back("DFLAGS=-g -gdwarf-2");
    List->push_back("LFLAGS=-nostdlib -nostartfiles -ffreestanding -specs=nano.specs -Wl,--gc-sections,--cref");
    List->push_back("");
    List->push_back("OBJDIR=Objects");
    List->push_back(std::string("PREFIX=")+Prefix);
    List->push_back("# End Configuration ###########################################################");
    List->push_back("");
    List->push_back("# Source ######################################################################");
    for(const std::string& Inc:Include)
        List->push_back(std::string("INCS+=-I")+Inc);
    List->push_back("");
    for(const std::string& Src:Source)
    {
        if(Src.back()=='c')
            List->push_back(std::string("CSRCS+=")+Src);
    }
    List->push_back("");
    for (const std::string& Src:Source)
    {
        if (Src.back()!='c')
            List->push_back(std::string("ASRCS+=")+Src);
    }
    List->push_back("");
    List->push_back(std::string("LDSCRIPT=")+Linker);
    List->push_back("LIBS=-lc -lnosys");
    List->push_back("# End Source ##################################################################");
    List->push_back("");
    List->push_back("# Toolchain ###################################################################");
    List->push_back("ifdef GCC");
    List->push_back("CC=$(GCC)/$(PREFIX)gcc");
    List->push_back("AS=$(GCC)/$(PREFIX)gcc -x assembler-with-cpp");
    List->push_back("CP=$(GCC)/$(PREFIX)objcopy");
    List->push_back("LD=$(GCC)/$(PREFIX)gcc");
    List->push_back("SZ=$(GCC)/$(PREFIX)size");
    List->push_back("else");
    List->push_back("CC=$(PREFIX)gcc");
    List->push_back("AS=$(PREFIX)gcc -x assembler-with-cpp");
    List->push_back("CP=$(PREFIX)objcopy");
    List->push_back("LD=$(PREFIX)gcc");
    List->push_back("SZ=$(PREFIX)size");
    List->push_back("endif");
    List->push_back("");
    List->push_back("HEX=$(CP) -O ihex");
    List->push_back("BIN=$(CP) -O binary -S");
    List->push_back("# End Toolchain ###############################################################");
    List->push_back("");
    List->push_back("# User ########################################################################");
    List->push_back("-include user");
    List->push_back("# End User ####################################################################");
    List->push_back("");
    List->push_back("# Build #######################################################################");
    List->push_back("COBJS=$(CSRCS:%.c=%.o)");
    List->push_back("CDEPS=$(CSRCS:%.c=%.d)");
    List->push_back("AOBJS=$(ASRCS:%.s=%.o)");
    List->push_back("ADEPS=$(ASRCS:%.s=%.d)");
    List->push_back("");
    List->push_back("DEP=$(OBJDIR)/$(notdir $(@:%.o=%.d))");
    List->push_back("LST=$(OBJDIR)/$(notdir $(@:%.o=%.lst))");
    List->push_back("OBJ=$(OBJDIR)/$(notdir $@)");
    List->push_back("MAP=$(OBJDIR)/$(TARGET).map");
    List->push_back("");
    List->push_back("# Build all");
    if(After!="")
    {
        List->push_back("all: mkdir $(COBJS) $(AOBJS) $(TARGET).elf $(TARGET).hex $(TARGET).bin");
        List->push_back(std::string("\t")+After);
    }
    else
        List->push_back("all: mkdir $(COBJS) $(AOBJS) $(TARGET).elf $(TARGET).hex $(TARGET).bin");
    List->push_back("");
    List->push_back("# Create output folder");
    List->push_back("mkdir:");
    List->push_back("\t$(shell if [ ! -e $(OBJDIR) ];then mkdir -p $(OBJDIR); fi)");
    List->push_back("");
    List->push_back("# Compile C sources");
    List->push_back("%.o:%.c");
    List->push_back("\t$(CC) -c $(CPU) $(INCS) $(CFLAGS) -MMD -MP -MF \"$(DEP)\" -Wa,-a,-ad,-alms=\"$(LST)\" \"$<\" -o \"$(OBJ)\"");
    List->push_back("");
    List->push_back("# Assemble ASM sources");
    List->push_back("%.o:%.s");
    List->push_back("\t$(AS) -c $(CPU) $(INCS) $(AFLAGS) \"$<\" -o \"$(OBJ)\"");
    List->push_back("");
    List->push_back("# Link ELF target file and print size");
    List->push_back("$(TARGET).elf:$(COBJS) $(AOBJS)");
    List->push_back("\t$(LD) $(OBJDIR)/*.o $(CPU) $(LFLAGS) -T $(LDSCRIPT) -Wl,-Map=$(MAP) $(LIBS) -o $(OBJ)");
    List->push_back("\t$(SZ) $(OBJ)");
    List->push_back("");
    List->push_back("# Create hex/bin programming files");
    List->push_back("$(TARGET).hex:$(TARGET).elf");
    List->push_back("\t$(HEX) \"$(OBJDIR)/$<\" \"$(OBJDIR)/$@\"");
    List->push_back("");
    List->push_back("$(TARGET).bin:$(TARGET).elf");
    List->push_back("\t$(BIN) \"$(OBJDIR)/$<\" \"$(OBJDIR)/$@\"");
    List->push_back("");
    List->push_back("# Clean up");
    List->push_back("clean:");
    List->push_back("\t-rm -rf $(OBJDIR)");
    List->push_back("");
    List->push_back("# Dependencies");
    List->push_back("-include $(wildcard $(OBJDIR)/*.d)");
    List->push_back("# End Build ###################################################################");
    List->push_back("");
    List->push_back("# End Of File #################################################################");
    List->push_back("");
    List->push_back("# Copyright (C) Evo-Devo Instrum. All rights reserved #########################");
    List->push_back("");
}
/* End Function:Makefile_Gen::Makefile_Proj **********************************/

/* Function:Makefile_Gen::Kernel_Proj *****************************************
Description : Generate kernel project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Include - The include file list.
              const std::vector<std::string>& Source - The source file list.
              const std::vector<std::string>& Source - The linker script file list.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Makefile_Gen::Kernel_Proj(std::unique_ptr<std::vector<std::string>>& List,
                               const std::vector<std::string>& Include,
                               const std::vector<std::string>& Source,
                               const std::vector<std::string>& Linker)
{
    std::vector<std::string> None;

    /* Generate a Makefile for compiling system code */
    this->Makefile_Proj(List,
                        "",                                      /* After */
                        "Kernel",                                /* Target */
                        this->Proj->Kernel->Optimization,        /* Optimization */
                        None,                                    /* Coprocessor */
                        Include,                                 /* Include */
                        Source,                                  /* Source */
                        Linker[0]);                              /* Linker */
}
/* End Function:Makefile_Gen::Kernel_Proj ************************************/

/* Function:Makefile_Gen::Monitor_Proj ****************************************
Description : Generate monitor project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Include - The include file list.
              const std::vector<std::string>& Source - The source file list.
              const std::vector<std::string>& Source - The linker script file list.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Makefile_Gen::Monitor_Proj(std::unique_ptr<std::vector<std::string>>& List,
                                const std::vector<std::string>& Include,
                                const std::vector<std::string>& Source,
                                const std::vector<std::string>& Linker)
{
    std::string After;
    std::vector<std::string> Bincopy;
    std::vector<std::string> None;

    if(this->Proj->Kernel->Full_Image!=0)
    {
        Bincopy.push_back(this->Proj->Monitor->Monitor_Root + "Utility/M7M02_Bincopy/bincopy.exe");
        Gen_Tool::Path_Conv(this->Proj->Monitor->Project_Output, Bincopy);
        After=Bincopy[0]+" -i $(OBJDIR)/$(TARGET).bin -o ./monitor_image.c";
    }

    /* Generate a Makefile for compiling system code */
    this->Makefile_Proj(List,
                        After,                                   /* After */
                        "Monitor",                               /* Target */
                        this->Proj->Monitor->Optimization,       /* Optimization */
                        None,                                    /* Coprocessor */
                        Include,                                 /* Include */
                        Source,                                  /* Source */
                        Linker[0]);                              /* Linker */
}
/* End Function:Makefile_Gen::Monitor_Proj ***********************************/

/* Function:Makefile_Gen::Process_Proj ****************************************
Description : Generate process project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Include - The include file list.
              const std::vector<std::string>& Source - The source file list.
              const std::vector<std::string>& Source - The linker script file list.
              const class Process* Prc - The process to generate for.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Makefile_Gen::Process_Proj(std::unique_ptr<std::vector<std::string>>& List,
                                const std::vector<std::string>& Include,
                                const std::vector<std::string>& Source,
                                const std::vector<std::string>& Linker,
                                const class Process* Prc)
{
    std::string After;
    std::vector<std::string> Bincopy;
    std::vector<std::string> None;

    if(this->Proj->Kernel->Full_Image!=0)
    {
        Bincopy.push_back(this->Proj->Monitor->Monitor_Root + "Utility/M7M02_Bincopy/bincopy.exe");
        Gen_Tool::Path_Conv(Prc->Project_Output, Bincopy);
        After=Bincopy[0]+" -i $(OBJDIR)/$(TARGET).bin -o ./" + Prc->Name_Lower + "_image.c";
    }

    /* Generate a Makefile for compiling system code */
    this->Makefile_Proj(List,
                        After,                                   /* After */
                        Prc->Name,                               /* Target */
                        Prc->Optimization,                       /* Optimization */
                        None,                                    /* Coprocessor */
                        Include,                                 /* Include */
                        Source,                                  /* Source */
                        Linker[0]);                              /* Linker */
}
/* End Function:Makefile_Gen::Process_Proj ***********************************/

/* Function:Makefile_Gen::Workspace_Proj **************************************
Description : Generate workspace project.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::vector<std::string>& Project - The project file list.
Output      : std::unique_ptr<std::vector<std::string>>& List - The updated file.
Output      : None.
Return      : None.
******************************************************************************/
void Makefile_Gen::Workspace_Proj(std::unique_ptr<std::vector<std::string>>& List,
                                  const std::vector<std::string>& Project)
{
    List->push_back("###############################################################################");
    List->push_back(std::string("#Filename    : ")+this->Proj->Name_Lower);
    List->push_back(std::string("#Author      : ")+CODE_AUTHOR);
    List->push_back(std::string("#Date        : ")+Main::Time);
    List->push_back(std::string("#Licence     : ")+CODE_LICENSE);
    List->push_back("#Description : Project Makefile (based on gcc). This file is intended");
    List->push_back(std::string("#              to be used with ") + this->Chip->Name + ", and the GNU toolchain.");
    List->push_back("###############################################################################");
    List->push_back("");
    List->push_back("# Build #######################################################################");
    List->push_back("all:");
    List->push_back(std::string("\tcd Monitor/Project && $(MAKE) -f monitor -j8"));
    for(const std::unique_ptr<class Process>& Prc:this->Proj->Process)
    	List->push_back(std::string("\tcd ")+Prc->Name+"/Project && $(MAKE) -f prc_"+Prc->Name_Lower+" -j8");
    List->push_back(std::string("\tcd Kernel/Project && $(MAKE) -f kernel -j8"));
    List->push_back("# End Build ###################################################################");
    List->push_back("");
    List->push_back("# Clean #######################################################################");
    List->push_back("clean:");
    List->push_back(std::string("\tcd Monitor/Project && $(MAKE) clean -f monitor"));
    for(const std::unique_ptr<class Process>& Prc:this->Proj->Process)
    	List->push_back(std::string("\tcd ")+Prc->Name+"/Project && $(MAKE) clean -f prc_" + Prc->Name_Lower);
    List->push_back(std::string("\tcd Kernel/Project && $(MAKE) clean -f kernel"));
    List->push_back("# End Clean ###################################################################");
    List->push_back("");
    List->push_back("# End Of File #################################################################");
    List->push_back("");
    List->push_back("# Copyright (C) Evo-Devo Instrum. All rights reserved #########################");
    List->push_back("");
}
/* End Function:Makefile_Gen::Workspace_Proj *********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
