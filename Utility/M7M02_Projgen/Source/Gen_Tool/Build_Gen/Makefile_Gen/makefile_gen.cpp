/******************************************************************************
Filename    : makefile_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for Keil uVision build system.
******************************************************************************/

/* Includes ******************************************************************/
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

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Gen_Tool/gen_tool.hpp"
#include "Gen_Tool/Build_Gen/build_gen.hpp"
#include "Gen_Tool/Build_Gen/Makefile_Gen/makefile_gen.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
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
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Makefile_Gen::Makefile_Gen **********************************
Description : Generator for the makefiles.
Input       : class Proj_Info* Proj - The project information.
              class Plat_Info* Plat - The platform information.
              class Chip_Info* Chip - The chip information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Makefile_Gen::Makefile_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip):
Build_Gen("Makefile", Proj, Plat, Chip)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Makefile generator:\n")+Exc.what());
    }
}
/* End Function:Makefile_Gen::Makefile_Gen ***********************************/

/* Begin Function:Makefile_Gen::Suffix ****************************************
Description : Returns suffix for a given type of file.
Input       : ptr_t Type - The file type.
Output      : None.
Return      : std::string - The file suffix.
******************************************************************************/
std::string Makefile_Gen::Suffix(ptr_t Type)
{
    if(Type==BUILD_PROJECT)
        return "";
    else
        Main::Error("XXXXX: File type not recognized.");
}
/* End Function:Makefile_Gen::Suffix *****************************************/

/* Begin Function:Makefile_Gen::Makefile_Proj *****************************************
Description : Generate the keil project for ARMv7-M.
Input       : std::unique_ptr<std::vector<std::string>>& List - The file.
              const std::string& After1 - The first command to run after compilation.
              const std::string& After2 - The second command to run after compilation.
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
    const std::string& After1, const std::string& After2,
    const std::string& Target, const std::string& Optimization,
    const std::vector<std::string>& Include,
    const std::vector<std::string>& Source,
    const std::string& Linker, const std::string& Linker_Misc)
{
    ptr_t Opt_Level;
    std::string CPU_Type;
    std::string FPU_Type;
    std::string Endian;
    std::string Dlloption;
    std::string Device;
    std::string Include_Path;

    /* CPU Type */
    CPU_Type = this->Chip->Attribute["CPU"];
    if (CPU_Type == "CM0")
    {
        CPU_Type = "Cortex-M0";
        Dlloption = "-pCM0";
    }
    else if (CPU_Type == "CM0P")
    {
        CPU_Type = "Cortex-M0+";
        Dlloption = "-pCM0+";
    }
    else if (CPU_Type == "CM1")
    {
        CPU_Type = "Cortex-M1";
        Dlloption = "-pCM1";
    }
    else if (CPU_Type == "CM3")
    {
        CPU_Type = "Cortex-M3";
        Dlloption = "-pCM3";
    }
    else if (CPU_Type == "CM4")
    {
        CPU_Type = "Cortex-M4";
        Dlloption = "-pCM4";
    }
    else if (CPU_Type == "CM7")
    {
        CPU_Type = "Cortex-M7";
        Dlloption = "-pCM7";
    }
    else
        Main::Error("XXXXX: Internal processor type error.");

    /* FPU Type */
    if ((this->Chip->Attribute["CPU"] == "CM0") || (this->Chip->Attribute["CPU"] == "CM0P"))
        FPU_Type = "";
    else
    {
        FPU_Type = this->Chip->Attribute["FPU"];
        if (FPU_Type == "None")
            FPU_Type = "";
        else if (FPU_Type == "FPV4_SP")
            FPU_Type = "fpv4-sp-d16";
        else if (FPU_Type == "FPV5_SP")
            FPU_Type == "fpv5-sp-d16";
        else if (FPU_Type == "FPV5_DP")
            FPU_Type == "fpv5-dp-d16";
        else
            Main::Error("XXXXX: Internal FPU type error.");
    }

    /* Endianness */
    Endian = this->Chip->Attribute["Endian"];
    if (Endian == "Little")
        Endian = "ELITTLE";
    else if (Endian == "Big")
        Endian = "EBIG";
    else
        Main::Error("XXXXX: Internal endianness error.");

    /* Decide device */
    if (this->Chip->Name.find("STM32") != std::string::npos)
    {
        /* Except for STM32F1, all STM32s end with suffix, and the last number is substituted with 'x' */
        if (this->Chip->Name.find("STM32F1") != std::string::npos)
            Device = this->Chip->Name;
        else
        {
            Device = this->Proj->Chip->Name.substr(0, 9);
            Device += 'xx';
        }
    }
    else
        Device = this->Proj->Chip->Name;

    List->push_back(std::string("TARGET = ") + Target);
    List->push_back("");
    List->push_back("DEBUG = 1");
    List->push_back(std::string("OPT = -") + Optimization);
    List->push_back("");
    List->push_back(std::string("BUILD_DIR = ./Objects"));
    List->push_back("");

    /* Construct include paths */
    for (const std::string& Inc : Include)
        List->push_back(std::string("C_INCLUDES += -I ") + Inc);
    List->push_back("");
    /* Construct source paths */
    for (const std::string& Src : Source)
    {
        if (Src.back() == 'c')
            List->push_back(std::string("C_SOURCES += ") + Src );
    }
    List->push_back("");
    /* Construct startup file paths */
    for (const std::string& Src : Source)
    {
        if (Src.back() != 'c')
            List->push_back(std::string("ASM_SOURCES += ") + Src);
    }
    List->push_back("");
    List->push_back("PREFIX = arm-none-eabi-");
    List->push_back("CC = $(PREFIX)gcc");
    List->push_back("AS = $(PREFIX)gcc -x assembler-with-cpp");
    List->push_back("CP = $(PREFIX)objcopy");
    List->push_back("SZ = $(PREFIX)size");
    List->push_back("");
    List->push_back("");
    List->push_back("HEX = $(CP) -O ihex");
    List->push_back("BIN = $(CP) -O binary -S");
    List->push_back("");
    List->push_back(std::string("CPU = -mcpu=") + CPU_Type);
    if (FPU_Type != "")
    {
        List->push_back(std::string("FPU = -mfpu=") + FPU_Type);
        List->push_back("FLOAT-ABI = -mfloat-abi=hard");
        List->push_back("MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)");
    }
    else
        List->push_back("MCU = $(CPU) -mthumb ");
    List->push_back(std::string("C_DEFS = -D") + Device);
    List->push_back("");
    List->push_back("ASFLAGS = $(MCU) $(OPT) -Wall -fdata-sections -ffunction-sections");
    List->push_back("CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections");
    List->push_back("CFLAGS += -MMD -MP -MF\"$(@:%.o=%.d)\"");
    List->push_back("");
    List->push_back(std::string("LDSCRIPT = ") + Linker);
    List->push_back("LIBS = -lc -lm -lnosys");
    List->push_back("LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections");
    List->push_back("");
    List->push_back("SRC_RAW = $(wildcard $(C_SOURCES))");
    List->push_back("OBJECTS = $(SRC_RAW:%.c=%.o)");
    List->push_back("TEMP = $(SRC_RAW:%.c=%.d)");
    List->push_back("");
    List->push_back("STARTUP_RAW = $(wildcard $(ASM_SOURCES))");
    List->push_back("STARTUP_OBJECTS = $(STARTUP_RAW:%.s=%.o)");
    List->push_back("");
    List->push_back("OUTPUT_OBJ = Objects/*.o");
    List->push_back("");
    if (Target.find("Kernel") != std::string::npos)
    {
        List->push_back("all: subsystem $(OBJECTS) $(STARTUP_OBJECTS) $(TARGET).elf $(TARGET).hex $(TARGET).bin");
        List->push_back("\t-mv $(TEMP) $(BUILD_DIR)");
        List->push_back("");
        List->push_back("subsystem:");
        List->push_back("\tcd ../../Monitor/Project && $(MAKE)");
        for (const std::unique_ptr<class Process>& Prc : this->Proj->Process)
        {
            try
            {
                List->push_back(std::string("\tcd ../../") + Prc->Name + "/Project && $(MAKE)");
            }
            catch (std::exception& Exc)
            {
                Main::Error(std::string("Process '") + Prc->Name + "' project generation:\n" + Exc.what());
            }
        }
    }
    else
    {
        List->push_back("all: $(OBJECTS) $(STARTUP_OBJECTS) $(TARGET).elf $(TARGET).hex $(TARGET).bin");
        List->push_back("\t-mv $(TEMP) $(BUILD_DIR)");
        List->push_back("\t" + After1);
        List->push_back("\t" + After2);
    }
    List->push_back("");
    List->push_back("");

    List->push_back("%.o: %.c");
    List->push_back("\t$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@");
    List->push_back("\t-mv $@  $(BUILD_DIR)");
    List->push_back("");
    List->push_back("%.o: %.s");
    List->push_back("\t$(AS) -c $(ASFLAGS) $< -o $@");
    List->push_back("\t-mv $@  $(BUILD_DIR)");
    List->push_back("");
    List->push_back("$(TARGET).elf: $(OBJECTS) $(STARTUP_OBJECTS) Makefile");
    List->push_back("\t$(CC) $(OUTPUT_OBJ) $(LDFLAGS) -o $@");
    List->push_back("\t$(SZ) $@");
    List->push_back("");
    List->push_back("%.hex: %.elf");
    List->push_back("\t$(HEX) $< $@");
    List->push_back("");
    List->push_back("%.bin: %.elf");
    List->push_back("\t$(BIN) $< $@");
    List->push_back("");
    List->push_back("clean:");
    List->push_back("\t-rm -fR $(BUILD_DIR)");
    List->push_back("");
    List->push_back("-include $(wildcard $(BUILD_DIR)/*.d)");
    List->push_back("");
}
/* End Function:Makefile_Gen::Makefile_Proj ******************************************/

/* Begin Function:Makefile_Gen::Kernel_Proj ***********************************
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
    this->Makefile_Proj(List,
                        "",                                      /* After 1 */
                        "",                                      /* After 2 */
                        "Kernel",                                /* Target */
                        this->Proj->Kernel->Optimization,        /* Optimization */
                        Include,                                 /* Include */
                        Source,                                  /* Source */
                        Linker[0],                               /* Linker */
                        "");
}
/* End Function:Makefile_Gen::Kernel_Proj ************************************/

/* Begin Function:Makefile_Gen::Monitor_Proj **********************************
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
    std::string After1;
    std::string After2;
    std::vector<std::string> Bincopy;

    if (this->Proj->Kernel->Full_Image != 0)
    {
        Bincopy.push_back(this->Proj->Monitor->Monitor_Root + "Utility/M7M02_Bincopy/bincopy.exe");
        Gen_Tool::Path_Conv(this->Proj->Monitor->Project_Output, Bincopy);
        //After1 = "fromelf.exe --binCombined -o \"$L@L.bin\" \"#L\"";
        After2 = Bincopy[0] + " -i $(TARGET).bin -o ./monitor_image.c";
    }

    this->Makefile_Proj(List,
        After2,                                  /* After 1 */
        "",                                      /* After 2 */
        "Monitor",                               /* Target */
        this->Proj->Monitor->Optimization,       /* Optimization */
        Include,                                 /* Include */
        Source,                                  /* Source */
        Linker[0],                               /* Linker */
        "");                                     /* Linker_Misc */
}
/* End Function:Makefile_Gen::Monitor_Proj ***********************************/

/* Begin Function:Makefile_Gen::Process_Proj **********************************
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
    std::string After1;
    std::string After2;
    std::vector<std::string> Bincopy;

    if (this->Proj->Kernel->Full_Image != 0)
    {
        Bincopy.push_back(this->Proj->Monitor->Monitor_Root + "Utility/M7M02_Bincopy/bincopy.exe");
        Gen_Tool::Path_Conv(Prc->Project_Output, Bincopy);
        //After1 = "fromelf.exe --binCombined -o \"$L@L.bin\" \"#L\"";
        After2 = Bincopy[0] + " -i $(TARGET).bin -o ./" + Prc->Name_Lower + "_image.c";
    }

    this->Makefile_Proj(List,
        After2,                                  /* After 1 */
        "",                                      /* After 2 */
        Prc->Name,                               /* Target */
        Prc->Optimization,                       /* Optimization */
        Include,                                 /* Include */
        Source,                                  /* Source */
        Linker[0],                               /* Linker */
        "--keep=*_desc");                        /* Linker_Misc */
}
/* End Function:Makefile_Gen::Process_Proj ***********************************/

/* Begin Function:Makefile_Gen::Workspace_Proj ********************************
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

}
/* End Function:Makefile_Gen::Workspace_Proj *********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
