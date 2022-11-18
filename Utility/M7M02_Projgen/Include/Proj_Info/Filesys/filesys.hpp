/******************************************************************************
Filename    : rme_fsys.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the file system access class.
******************************************************************************/

/* Defines *******************************************************************/
namespace rme_mcu
{
#ifdef __HDR_DEFS__
#ifndef __RME_FSYS_HPP_DEFS__
#define __RME_FSYS_HPP_DEFS__
/*****************************************************************************/
#define PARA_DOC
#define PARA_HEADER
/*****************************************************************************/
/* __RME_FSYS_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __RME_FSYS_HPP_CLASSES__
#define __RME_FSYS_HPP_CLASSES__
/*****************************************************************************/
/* Destination file system */
class Dstfs
{
    std::unique_ptr<std::string> Output;
public:
    Dstfs(std::unique_ptr<std::string>& Path);

    /* Directory operations w.r.t. output path */
    ret_t Dir_Present(std::unique_ptr<std::string>& Path);
    ret_t Dir_Empty(std::unique_ptr<std::string>& Path);
    void Make_Dir(std::unique_ptr<std::string>& Path);
    void Make_Dir(const s8_t* Path, ...);

    /* File operations w.r.t. output path */
    FILE* Open_File(std::unique_ptr<std::string>& File);
    FILE* Open_File(s8_t* Path, ...);
    bool File_Exist(s8_t* Path, ...);
    std::unique_ptr<std::list<std::unique_ptr<std::string>>> Read_File(std::unique_ptr<std::string>& Path);
   
    /* For project XML loading only, not w.r.t. output path */
    std::unique_ptr<std::string> Dstfs::Read_Proj(std::unique_ptr<std::string>& Path);
};

/* Source file system */
class Srcfs
{
public:
    /* This will be used by the copy file functionality */
    std::unique_ptr<std::string> Output;

    virtual ~Srcfs(void){};

    void Copy_File(s8_t* Path, ...);
    void Copy_File(s8_t* From, s8_t* To);
    std::unique_ptr<std::list<std::unique_ptr<std::string>>> Read_File(s8_t* Path, ...);

    virtual void Copy_File(std::unique_ptr<std::string>& From, std::unique_ptr<std::string>& To)=0;

    virtual std::unique_ptr<std::string> Read_Chip(std::unique_ptr<std::string>& Path)=0;
    virtual std::unique_ptr<std::list<std::unique_ptr<std::string>>> Read_File(std::unique_ptr<std::string>& Path)=0;
};

class Sysfs:public Srcfs
{
public:
    std::unique_ptr<std::string> Root;

    Sysfs(std::unique_ptr<std::string>& Root, std::unique_ptr<std::string>& Output);

    virtual void Copy_File(std::unique_ptr<std::string>& From, std::unique_ptr<std::string>& To) final override;

    virtual std::unique_ptr<std::string> Read_Chip(std::unique_ptr<std::string>& Path) final override;
    virtual std::unique_ptr<std::list<std::unique_ptr<std::string>>> Read_File(std::unique_ptr<std::string>& Path) final override;
};

/* PBFS-based solution - currently not implemented */
class Pbfs:public Srcfs
{
public:
    struct PBFS_Env PBFS;

    Pbfs(std::unique_ptr<std::string>& PBFS, std::unique_ptr<std::string>& Output);

    virtual void Copy_File(std::unique_ptr<std::string>& From, std::unique_ptr<std::string>& To) final override;
    virtual std::unique_ptr<std::string> Read_Chip(std::unique_ptr<std::string>& Path) final override;
    virtual std::unique_ptr<std::list<std::unique_ptr<std::string>>> Read_File(std::unique_ptr<std::string>& Path) final override;
};
/*****************************************************************************/
/* __RME_FSYS_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
