/******************************************************************************
Filename    : xml_conf.h
Author      : pry
Date        : 03/11/2018
Licence     : LGPL v3+; see COPYING for details.
Description : The configuration header for xml parsing library.
******************************************************************************/

/* Compiler Keyword **********************************************************/
#define XML_EXTERN							extern
/* End Compiler Keyword ******************************************************/

/* Configuration Typedefs ****************************************************/
typedef char                                xml_s8_t;
typedef unsigned long long                  xml_ptr_t;
typedef long long                           xml_ret_t;
/* End Configuration Typedefs ************************************************/

/* Debugging Define **********************************************************/
#define XML_ASSERT_ENABLE                   (0U)
/*
#define XML_DBGLOG(ENV,F,L,D,T,E)           XML_Log(ENV,F,L,D,T,E)
extern void XML_Log(mbrs_t* Env, const char* File, long Line, const char* Date, const char* Time, int Error);

#define XML_DBGLOG(F,L,D,T,E)               XML_Log(F,L,D,T,E)
extern void XML_Log(const char* File, long Line, const char* Date, const char* Time, int Error);
*/
/* End Debugging Define ******************************************************/

/* Configuration Define ******************************************************/
#define XML_STATIC_ENABLE                   (1U)
#define XML_MALLOC_NAME                     Malloc
#define XML_FREE_NAME                       Free
#define XML_MEMCPY_NAME                     Memcpy
#define XML_STRLEN_NAME                     Strlen
#define XML_STRNCMP_NAME                    Strncmp
/* End Configuration Define **************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
