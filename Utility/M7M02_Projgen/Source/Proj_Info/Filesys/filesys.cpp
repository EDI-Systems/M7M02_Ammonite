/******************************************************************************
Filename    : rme_fsys.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The filesystem interface class.
******************************************************************************/

/* Includes ******************************************************************/
/* Kill CRT warnings for MS. This also relies on Shlwapi.lib, remember to add it */
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

extern "C"
{
#include "stdio.h"
#include "memory.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "sys/types.h"
#include "sys/stat.h"

#include "xml.h"
#include "pbfs.h"

#if(defined _MSC_VER)
#include "Windows.h"
#include "shlwapi.h"
#elif(defined linux)
#include <dirent.h>
#include <errno.h>
#else
#error "The target platform is not supported. Please compile on Windows or Linux."
#endif
}

#include "list"
#include "string"
#include "memory"
#include "vector"
#include "algorithm"
#include "stdexcept"

#define __HDR_DEFS__
#include "Main/rme_mcu.hpp"
#include "Main/rme_fsys.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "Main/rme_fsys.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Dstfs::Dstfs ***********************************************
Description : The constructor for the Dstfs class.
Input       : std::unique_ptr<std::string>& Path - The path to the directory.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Dstfs::Dstfs(std::unique_ptr<std::string>& Path)
{
    try
    {
        /* Output folder */
        this->Output=std::move(Path);
        if((*this->Output).back()!='/')
            (*this->Output)+="/";
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("System file storage:\n")+Exc.what());
    }
}
/* End Function:Dstfs::Dstfs *************************************************/

/* Begin Function:Dstfs::Dir_Present ******************************************
Description : Figure out whether the directory is present. This always goes to 
              the output directory.
Input       : std::unique_ptr<std::string>& Path - The path to the directory.
Output      : None.
Return      : ret_t - 0 for present, -1 for non-present.
******************************************************************************/
ret_t Dstfs::Dir_Present(std::unique_ptr<std::string>& Path)
{
    std::unique_ptr<std::string> Str;

    Str=std::make_unique<std::string>(*(this->Output)+(*Path));

#ifdef _MSC_VER
    u32_t Attr;
    Attr=PathIsDirectory((*Str).c_str());
    if(Attr!=0)
        return 0;
    else
        return -1;
#else
    DIR* Dir;
    Dir=opendir((*Str).c_str());
    if(Dir!=0)
    {
        closedir(Dir);
        return 0;
    }
    else
        return -1;
#endif
}
/* End Function:Dstfs::Dir_Present *******************************************/

/* Begin Function:Dstfs::Dir_Empty ********************************************
Description : Figure out whether the directory is empty. When using this function,
              the directory must be present. This always goes to the output
              directory.
Input       : std::unique_ptr<std::string>& Path - The path to the directory.
Output      : None.
Return      : ret_t - 0 for empty, -1 for non-empty.
******************************************************************************/
ret_t Dstfs::Dir_Empty(std::unique_ptr<std::string>& Path)
{
    std::unique_ptr<std::string> Str;

    Str=std::make_unique<std::string>(*(this->Output)+(*Path));

#ifdef _MSC_VER
    u32_t Attr;
    Attr=PathIsDirectoryEmpty((*Str).c_str());
    if(Attr!=0)
        return 0;
    else
        return -1;
#else
    ptr_t Num;
    DIR* Dir;

    Dir=opendir((*Str).c_str());
    if(Dir==0)
        return -1;

    while(1)
    {
        if(readdir(Dir)==0)
            break;

        Num++;
        if(Num>2)
        {
            closedir(Dir);
            return -1;
        }
    }

    closedir(Dir);
    return 0;
#endif
}
/* End Function:Dstfs::Dir_Empty *********************************************/

/* Begin Function:Dstfs::Make_Dir *********************************************
Description : Create a directory if it does not exist. This always goes to the
              output directory.
Input       : std::unique_ptr<std::string>& Path - The path to the directory.
Output      : None.
Return      : ret_t - 0 for successful, -1 for failure.
******************************************************************************/
void Dstfs::Make_Dir(std::unique_ptr<std::string>& Path)
{
    std::unique_ptr<std::string> Str;

    Str=std::make_unique<std::string>(*(this->Output)+(*Path));

    if(Dir_Present(Path)==0)
        return;

#ifdef _WIN32
    if(CreateDirectory((*Str).c_str(), NULL)!=0)
        return;
#else
    if(mkdir((*Str).c_str(), S_IRWXU)==0)
        return;
#endif

    throw std::runtime_error("F0000: Folder creation failed.");
}
/* End Function:Dstfs::Make_Dir **********************************************/

/* Begin Function:Dstfs::Make_Dir *********************************************
Description : Create a directory if it does not exist. This always goes to the
              output directory.
Input       : const s8_t* Path - The path to the directory.
Output      : None.
Return      : ret_t - 0 for successful, -1 for failure.
******************************************************************************/
void Dstfs::Make_Dir(const s8_t* Path, ...)
{
    s8_t Buf[1024];
    va_list Args;

    va_start(Args, Path);
    vsprintf(Buf, Path, Args);
    va_end(Args);

    return Make_Dir(std::make_unique<std::string>(Buf));
}
/* End Function:Dstfs::Make_Dir **********************************************/

/* Begin Function:Srcfs::Open_File ********************************************
Description : Open a file and return descriptor. This always goes to the output
              directory.
Input       : std::unique_ptr<std::string>& File - The path to the file.
Output      : None.
Return      : FILE* - The handle to the opened file.
******************************************************************************/
FILE* Dstfs::Open_File(std::unique_ptr<std::string>& File)
{
    FILE* Desc;
    std::unique_ptr<std::string> Str;

    Str=std::make_unique<std::string>(*(this->Output)+*File);
    Desc=fopen((*Str).c_str(), "wb");
    if(Desc==0)
        throw std::runtime_error("Open file:\nF0001: Cannot open source file.");

    return Desc;
}
/* End Function:Dstfs::Open_File *********************************************/

/* Begin Function:Dstfs::Open_File ********************************************
Description : Open a file and return descriptor. This always goes to the output
              directory.
Input       : s8_t* File - The path to the file.
Output      : None.
Return      : FILE* - The handle to the opened file.
******************************************************************************/
FILE* Dstfs::Open_File(s8_t* Path, ...)
{
    s8_t Buf[1024];
    va_list Args;

    va_start(Args, Path);
    vsprintf(Buf, Path, Args);
    va_end(Args);

    return Open_File(std::make_unique<std::string>(Buf));
}
/* End Function:Dstfs::Open_File *********************************************/

/* Begin Function:Dstfs::Read_File ********************************************
Description : Read a text file to line-based buffer.
Input       : std::unique_ptr<std::string>& Path - The path to the file.
Output      : None.
Return      : std::unique_ptr<std::vector<std::unique_ptr<std::string>>> - The file returned.
******************************************************************************/
std::unique_ptr<std::list<std::unique_ptr<std::string>>> Dstfs::Read_File(std::unique_ptr<std::string>& Path)
{
    FILE* File;
    char* Buf;
    ptr_t Len;

    std::unique_ptr<std::list<std::unique_ptr<std::string>>> List;
    std::unique_ptr<std::string> Str;
    Buf=nullptr;
    File=nullptr;

    try
    {
        List=std::make_unique<std::list<std::unique_ptr<std::string>>>();
        Str=std::make_unique<std::string>(*(this->Output)+*Path);
        Buf=new s8_t[BUF_SIZE];

        /* Read using text mode */
        File=fopen((*Str).c_str(), "r");
        if(File==0)
            throw std::runtime_error("Read text file:\nF0002: Cannot read file.");

        while(1)
        {
            if(fgets(Buf,512,File)==nullptr)
                break;

            Len=strlen(Buf);

            if(Buf[Len-1]=='\n')
                Buf[Len-1]='\0';

            List->push_back(std::make_unique<std::string>(Buf));
        }

        fclose(File);
        delete[] Buf;

        return List;
    }
    catch(std::exception& Exc)
    {
        if(File!=0)
            fclose(File);
        if(Buf!=nullptr)
            delete[] Buf;

        throw std::runtime_error(std::string("Destination file storage:\n")+Exc.what());

        return nullptr;
    }
}
/* End Function:Dstfs::Read_File *********************************************/

/* Begin Function:Dstfs::File_Exist *******************************************
Description : Open a file and return descriptor. This always goes to the output
              directory.
Input       : s8_t* File - The path to the file.
Output      : None.
Return      : bool - If exists, true; else false.
******************************************************************************/
bool Dstfs::File_Exist(s8_t* Path, ...)
{
    s8_t Buf[1024];
    va_list Args;

    va_start(Args, Path);
    vsprintf(Buf, Path, Args);
    va_end(Args);

    return Open_File(std::make_unique<std::string>(Buf));
}
/* End Function:Dstfs::Open_File *********************************************/

/* Begin Function:Dstfs::Read_Proj ********************************************
Description : Read the project XML content into a buffer. This only works for text
              files; binaries are not allowed.
Input       : std::unique_ptr<std::string>& Path - The path to the file.
Output      : None.
Return      : u8_t* - The buffer returned.
******************************************************************************/
std::unique_ptr<std::string> Dstfs::Read_Proj(std::unique_ptr<std::string>& Path)
{
    ptr_t Size;
    FILE* File;
    s8_t* Buf;
    struct stat Stat;
    std::unique_ptr<std::string> Str;

    File=nullptr;

    try
    {
        Str=std::make_unique<std::string>(*Path);

        if(stat((*Str).c_str(),&Stat)!=0)
            throw std::runtime_error("System file storage:\nF0003: Windows/Linux stat failed.");

        Size=Stat.st_size;
        Buf=new s8_t[(unsigned int)(Size+1)];

        File=fopen((*Str).c_str(), "rb");
        if(File==0)
            throw std::runtime_error("Read text file:\nF0002: Cannot read file.");

        fread(Buf, 1, (size_t)Size, File);
        Buf[Size]='\0';

        Str=std::make_unique<std::string>(Buf);
        delete[] Buf;
        fclose(File);
        return Str;
    }
    catch(std::exception& Exc)
    {
        if(File!=0)
            fclose(File);

        throw std::runtime_error(std::string("System file storage:\n")+Exc.what());

        return nullptr;
    }
}
/* End Function:Sysfs::Read_Proj *********************************************/

/* Begin Function:Srcfs::Copy_File ********************************************
Description : Copy a file from some position to another position. This function
              only need a path input, and will automatically copy stuff to the
              correct location.
Input       : s8_t* Path - The path to the file.
Output      : None.
Return      : ret_t - 0 for successful, -1 for failure.
******************************************************************************/
void Srcfs::Copy_File(s8_t* Path, ...)
{
    s8_t Buf[BUF_SIZE];
    va_list Args;

    va_start(Args, Path);
    vsprintf(Buf, Path, Args);
    va_end(Args);

    return Copy_File(std::make_unique<std::string>(Buf), std::make_unique<std::string>(Buf));
}
/* End Function:Srcfs::Copy_File *********************************************/

/* Begin Function:Srcfs::Copy_File ********************************************
Description : Copy a file from some position to another position. This requires
              both an input and an output.
Input       : s8_t* From - The path to the source file.
              s8_t* To - The path to the destination file.
Output      : None.
Return      : ret_t - 0 for successful, -1 for failure.
******************************************************************************/
void Srcfs::Copy_File(s8_t* From, s8_t* To)
{
    return Copy_File(std::make_unique<std::string>(From), std::make_unique<std::string>(To));
}
/* End Function:Srcfs::Copy_File *********************************************/

/* Begin Function:Srcfs::Read_File ********************************************
Description : Read a text file to line-based buffer.
Input       : s8_t* Path - The path to the file.
Output      : None.
Return      : std::unique_ptr<std::vector<std::unique_ptr<std::string>>> - The file returned.
******************************************************************************/
std::unique_ptr<std::list<std::unique_ptr<std::string>>> Srcfs::Read_File(s8_t* Path, ...)
{    
    s8_t Buf[BUF_SIZE];
    va_list Args;

    va_start(Args, Path);
    vsprintf(Buf, Path, Args);
    va_end(Args);

    return Read_File(std::make_unique<std::string>(Buf));
}
/* End Function:Srcfs::Read_File *********************************************/

/* Begin Function:Sysfs::Sysfs ************************************************
Description : Constructor for Sysfs class.
Input       : std::unique_ptr<std::string>& Root - The root folder containing everything.
              std::unique_ptr<std::string>& Output - The output folder.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Sysfs::Sysfs(std::unique_ptr<std::string>& Root, std::unique_ptr<std::string>& Output)
{
    try
    {
        /* Root */
        this->Root=std::move(Root);
        if((*this->Root).back()!='/')
            (*this->Root)+="/";

        /* Output folder */
        this->Output=std::move(Output);
        if((*this->Output).back()!='/')
            (*this->Output)+="/";
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("System file storage:\n")+Exc.what());
    }
}
/* End Function:Sysfs::Sysfs *************************************************/

/* Begin Function:Sysfs::Copy_File ********************************************
Description : Copy a file from some position to another position. This function
              only need a path input, and will automatically copy stuff to the
              correct location.
Input       : std::unique_ptr<std::string>& From - The path to the source file.
              std::unique_ptr<std::string>& To - The path to the destination file.
Output      : None.
Return      : ret_t - 0 for successful, -1 for failure.
******************************************************************************/
void Sysfs::Copy_File(std::unique_ptr<std::string>& From, std::unique_ptr<std::string>& To)
{
    FILE* Dst_File;
    FILE* Src_File;
    s8_t* Buf;
    ptr_t Size;

    std::unique_ptr<std::string> Src;
    std::unique_ptr<std::string> Dst;
    Buf=nullptr;

    try
    {
        Buf=new s8_t[BUF_SIZE];

        Src=std::make_unique<std::string>(*(this->Root)+*From);
        Src_File=fopen((*Src).c_str(), "rb");
        if(Src_File==0)
            throw std::runtime_error("Copy file:\nF0001: Cannot open source file.");

        /* This will wipe the contents of the file */
        Dst=std::make_unique<std::string>(*(this->Output)+*To);
        Dst_File=fopen((*Dst).c_str(), "wb");
        if(Dst_File==0)
            throw std::runtime_error("Copy file:\nF0004: Cannot open destination file.");

        Size=fread(Buf, 1, BUF_SIZE, Src_File);
        while(Size!=0)
        {
            fwrite(Buf, 1, (size_t)Size, Dst_File);
            Size=fread(Buf, 1, BUF_SIZE, Src_File);
        }

        fclose(Src_File);
        fclose(Dst_File);

        delete[] Buf;
    }
    catch(std::exception& Exc)
    {
        if(Buf!=nullptr)
            delete[] Buf;

        throw std::runtime_error(std::string("System file storage:\n")+Exc.what());
    }
}
/* End Function:Sysfs::Copy_File *********************************************/

/* Begin Function:Sysfs::Read_Chip ********************************************
Description : Read the chip configuration file into a buffer. This only works
              for text files; binaries are not allowed.
Input       : std::unique_ptr<std::string>& Path - The path to the file.
Output      : None.
Return      : u8_t* - The buffer returned.
******************************************************************************/
std::unique_ptr<std::string> Sysfs::Read_Chip(std::unique_ptr<std::string>& Path)
{
    ptr_t Size;
    FILE* File;
    s8_t* Buf;
    struct stat Stat;
    std::unique_ptr<std::string> Str;

    try
    {
        Str=std::make_unique<std::string>(*(this->Root)+*Path);

        if(stat((*Str).c_str(),&Stat)!=0)
            throw std::runtime_error("System file storage:\nF0003: Windows/Linux stat failed.");

        Size=Stat.st_size;
        Buf=new s8_t[(unsigned int)(Size+1)];

        File=fopen((*Str).c_str(), "rb");
        if(File==0)
            throw std::runtime_error("Read text file:\nF0002: Cannot read file.");

        fread(Buf, 1, (size_t)Size, File);
        Buf[Size]='\0';

        Str=std::make_unique<std::string>(Buf);
        delete[] Buf;
        fclose(File);
        return Str;
    }
    catch(std::exception& Exc)
    {
        if(File!=0)
            fclose(File);
        throw std::runtime_error(std::string("System file storage:\n")+Exc.what());
        return nullptr;
    }
}
/* End Function:Sysfs::Read_Chip *********************************************/

/* Begin Function:Sysfs::Read_File ********************************************
Description : Read a text file to line-based buffer.
Input       : std::unique_ptr<std::string>& Path - The path to the file.
Output      : None.
Return      : std::unique_ptr<std::vector<std::unique_ptr<std::string>>> - The file returned.
******************************************************************************/
std::unique_ptr<std::list<std::unique_ptr<std::string>>> Sysfs::Read_File(std::unique_ptr<std::string>& Path)
{
    FILE* File;
    char Buf[512];
    ptr_t Len;

    std::unique_ptr<std::list<std::unique_ptr<std::string>>> List;
    std::unique_ptr<std::string> Str;

    try
    {
        List=std::make_unique<std::list<std::unique_ptr<std::string>>>();
        Str=std::make_unique<std::string>(*(this->Root)+*Path);

        /* Read using text mode */
        File=fopen((*Str).c_str(), "r");
        if(File==0)
            throw std::runtime_error("Read text file:\nF0002: Cannot read file.");

        while(1)
        {
            if(fgets(Buf,512,File)==nullptr)
                break;

            Len=strlen(Buf);

            if(Buf[Len-1]=='\n')
                Buf[Len-1]='\0';

            List->push_back(std::make_unique<std::string>(Buf));
        }

        fclose(File);
        return List;
    }
    catch(std::exception& Exc)
    {
        if(File!=0)
            fclose(File);
        throw std::runtime_error(std::string("System file storage:\n")+Exc.what());
        return nullptr;
    }
}
/* End Function:Sysfs::Read_File *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/