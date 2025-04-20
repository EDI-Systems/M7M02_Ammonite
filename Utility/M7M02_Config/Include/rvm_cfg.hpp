/******************************************************************************
Filename    : rvm_cfg.hpp
Author      : pry
Date        : 27/12/2021
License     : Proprietary; confidential.
Description : Smart delay module writer class file header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __RVM_CFG_TYPE__
#define __RVM_CFG_TYPE__
/*****************************************************************************/
typedef char                                s8_t;
typedef short                               s16_t;
typedef int                                 s32_t;
typedef long long                           s64_t;
typedef unsigned char                       u8_t;
typedef unsigned short                      u16_t;
typedef unsigned int                        u32_t;
typedef unsigned long long                  u64_t;
typedef float                               f32_t;
typedef double                              f64_t;
/* Make things compatible in 32-bit or 64-bit environments */
typedef s32_t                               pos_t;
typedef s32_t                               cnt_t;
typedef s64_t                               ret_t;
typedef u64_t                               ptr_t;
typedef f64_t                               flt_t;
/*****************************************************************************/
/* __RVM_CFG_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __RVM_CFG_DEF__
#define __RVM_CFG_DEF__
/*****************************************************************************/
/* Debugging mode */
#ifndef __OPTIMIZE__
#define DEBUG
#endif
/* Language override */
/* #define LANGUAGE_OVERRIDE                   wxLANGUAGE_ENGLISH */
/* Disable high DPI */
/* #define HIGH_DPI_DISABLE */

/* Debug extra processing */
#ifdef DEBUG
#define wxUSE_LOG_DEBUG                     (1)
#else
#define NDEBUG
#define wxUSE_LOG_DEBUG                     (0)
#endif

/* Throw error, and rescue some unsaved files */
#define ASSERT(X,STR) \
do \
{ \
    if((X)==0) \
    { \
        class wxMessageDialog* Dialog; \
        std::string String; \
        /* Display the info pointer to what have failed, both pop-up and logging */ \
        String=std::string(__FILE__)+":"+std::to_string(__LINE__)+" - "+__FUNC__+":"+__DATE__+"\n"+_("Fatal")+": "+(STR); \
        wxLogDebug(String.c_str()); \
        Dialog=new wxMessageDialog(nullptr,String.c_str(),_("Error"),wxOK|wxICON_ERROR); \
        Dialog->ShowModal(); \
        exit(-1); \
    } \
} \
while(0)

#define __FUNC__                            __func__

/* Software version - increase only */
#define FIRMWARE_VERSION                    (0x0101)
#define SOFTWARE_VERSION                    ("R3T1-0000-20230110")
#define SOFTWARE_NAME                       _("RVM Virtual Machine Monitor Config Generator")

/* Option yes/no */
#define OPTION_NO                           (0)
#define OPTION_YES                          (1)

/* Date update flags */
#define TIME_DATE                           (0)
#define TIME_HMS                            (1)
#define TIME_DATE_HMS                       (2)

/* Message box types */
#define MSGBOX_ERROR                        (0)
#define MSGBOX_WARN                         (1)
#define MSGBOX_INFO                         (2)
#define MSGBOX_ASK                          (3)

/* State Types */
#define STATE_UI                            (0)
#define STATE_SAVE                          (1)

/* UI states */
#define UI_NONE                             (0)
#define UI_PROJ                             (1)

/* Save states */
#define SAVE_NONE                           (0)
#define SAVE_CUR                            (1)

/* Power of 2 */
#define POW2(X)                             (1UL<<(X))

/* Panels */
#define OUTPUT_INFO                         (0)
#define OUTPUT_STATUS                       (1)

/* High-DPI support (premature) */
#ifndef HIGH_DPI_DISABLE
#define I2P(X)                              RVM_CFG_App::Main->FromDIP(X)
#define P2I(X)                              RVM_CFG_App::Main->ToDIP(X)
#define F2P(X)                              ((X)*(double)RVM_CFG_App::Main->FromDIP(100000)/100000)
#define P2F(X)                              ((X)*100000/(double)RVM_CFG_App::Main->FromDIP(100000))
#else
/* If disabled, remember to disable the macro in *.rc as well */
#define I2P(X)                              (X)
#define P2I(X)                              (X)
#define F2P(X)                              (X)
#define P2F(X)                              (X)
#endif

/* Optimization level */
#define OPTIMIZATION_O0                     (0)
#define OPTIMIZATION_O1                     (1)
#define OPTIMIZATION_O2                     (2)
#define OPTIMIZATION_O3                     (3)
#define OPTIMIZATION_OF                     (4)
#define OPTIMIZATION_OS                     (5)
/*****************************************************************************/
/* __RVM_CFG_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __RVM_CFG_CLASS__
#define __RVM_CFG_CLASS__
/*****************************************************************************/
class Main:public wxFrame
{
public:
    /* Executable path */
    static std::string Exe_Folder;

    /* Application states */
    static ptr_t UI_State;
    static ptr_t Save_State;

    /* Project information - need to be manually deleted */
    static class Proj_Info* Proj;

    /* Lower-level classes */
    static class Menu_Bar* Menu_Bar;
    static class Tool_Bar* Tool_Bar;
    static class wxPanel* App_Panel;
    static class wxBoxSizer* App_Sizer;
    static class wxBoxSizer* Config_Sizer;
    static class Config_Tree* Config;
    static class wxBoxSizer* Option_Sizer;
    static class Option_Notebook* Option;
    static class Output_Notebook* Output;
    static class Status_Bar* Status_Bar;

    static class About_Dialog* About_Dialog;

    /* void */ Main(void);
    /* void */ ~Main(void);

    /* Model name from Prod_ID */
    static std::string Model_Get(ret_t Prod_ID);
    /* Set main window title */
    static void Title_Set(const class wxString& Path);
    /* Get current time string (YYYY-MM-DD international standard) */
    static std::string Time_Get(ptr_t Mode);
    static void Beep(void);
    /* Message box popper */
    static ret_t Msgbox_Show(class wxWindow* Parent, ptr_t Type,
                             const class wxString& Caption, const class wxString& Text);
    /* Identifier checking */
    static ret_t Idtfr_Check(const std::string& Name);

    /* Field saving/extraction generics */
    static void CSV_Read(const std::string& Input, std::vector<std::string>& Vector);

    static class wxXmlNode* Simple_Load(class wxXmlNode* Parent, const std::string& Expect);
    static std::string Text_Load(class wxXmlNode* Node, const std::string& Expect);
    static ptr_t Yesno_Load(class wxXmlNode* Parent, const std::string& Expect);
    static ptr_t Num_Load(class wxXmlNode* Node, const std::string& Expect);
    static void CSV_Load(class wxXmlNode* Parent, const std::string& Expect,
                         std::vector<std::string>& Vector);
    static void Pair_Load(class wxXmlNode* Parent, const std::string& Expect,
                          std::map<std::string, std::string>& Map);
    static ptr_t Opt_Load(class wxXmlNode* Node, const std::string& Expect);

    static class wxXmlNode* Simple_Save(class wxXmlNode* Parent, const std::string& Name);
    static class wxXmlNode* Text_Save(class wxXmlNode* Parent,
                                      const std::string& Name, const std::string& Content);
    static class wxXmlNode* Hex_Save(class wxXmlNode* Parent,
                                     const std::string& Name, ptr_t Padding, ptr_t Content);
    static class wxXmlNode* Yesno_Save(class wxXmlNode* Parent, const std::string& Name, ptr_t Yesno);
    static class wxXmlNode* Num_Save(class wxXmlNode* Parent,
                                     const std::string& Name, ptr_t Content);
    static class wxXmlNode* Pair_Save(class wxXmlNode* Parent,
                                      const std::string& Name,
                                      const std::map<std::string, std::string>& Map);
    static class wxXmlNode* Opt_Save(class wxXmlNode* Parent,
                                     const std::string& Name, ptr_t Content);

    /* Output notebook updates */
    static void Output_Update(std::vector<std::string>& Reply,ptr_t Panel,ptr_t Append);
    static void Output_Clear(ptr_t Panel);

    /* Project operations */
    void Proj_New(const std::string& Path);
    void Proj_Open(const std::string& Path);
    void Proj_Close(void);
    void Proj_Save(void);
    void Proj_Save_As(const std::string& Path);

    /* Manuals */
    static void Manual_Open(const std::string& Manual);

    static void State_Update(ptr_t Type);
    static void State_Set(ptr_t Type, ptr_t State);

    void On_Close_Window(class wxCloseEvent& Event);
};

/* Program entry point */
class RVM_CFG_App:public wxApp
{
public:
    /* Locale */
    class wxLocale* Locale;
    /* Main class */
    static class Main* Main;

    /* Self-contained translation */
    int Locale_Set(void);

    virtual bool OnInit(void) final override;
    virtual bool OnExceptionInMainLoop(void) final override;
#ifdef WIN32
    static LONG WINAPI OnSystemFatalException(EXCEPTION_POINTERS* Exc);
#else
    static void OnSystemFatalException(int Exc);
#endif
};

/* Template:Trunk_Load ********************************************************
Description : Load a trunk of data from XML. This is the base version.
Input       : class wxXmlNode* Root - The root node for the elements.
              const std::string& Prefix - The expected prefix of the elements.
Output      : std::vector<std::unique_ptr<ELEM>>& Vect - The vector to fill in.
Return      : None.
******************************************************************************/
template <typename ELEM>
void Trunk_Load(class wxXmlNode* Root, const std::string& Prefix, std::vector<std::unique_ptr<ELEM>>& Vect)
{
    class wxXmlNode* Temp;
    cnt_t Count;

    Count=0;
    for(Temp=Root->GetChildren();Temp!=nullptr;Temp=Temp->GetNext())
    {
        if(Temp->GetName()!=(Prefix+std::to_string(Count)))
            wxLogDebug("Prefix sequence is incorrect. This is not a hard error.");
        Vect.push_back(std::make_unique<ELEM>(Temp));
        Count++;
    }
}
/* End Template:Trunk_Load ***************************************************/

/* Template:Trunk_Load ********************************************************
Description : Load a trunk of data from XML. This accepts a type into the constructor.
Input       : class wxXmlNode* Root - The root node for the elements.
              const std::string& Prefix - The expected prefix of the elements.
              ptr_t Type - The type of the elements. Passed into the constructor.
Output      : std::vector<std::unique_ptr<ELEM>>& Vect - The vector to fill in.
Return      : None.
******************************************************************************/
template <typename ELEM>
void Trunk_Load(class wxXmlNode* Root, const std::string& Prefix, std::vector<std::unique_ptr<ELEM>>& Vect, ptr_t Type)
{
    class wxXmlNode* Temp;
    cnt_t Count;

    Count=0;
    for(Temp=Root->GetChildren();Temp!=nullptr;Temp=Temp->GetNext())
    {
        if(Temp->GetName()!=(Prefix+std::to_string(Count)))
            wxLogDebug("Prefix sequence is incorrect. This is not a hard error.");
        Vect.push_back(std::make_unique<ELEM>(Temp,Type));
        Count++;
    }
}
/* End Template:Trunk_Load ***************************************************/

/* Template:Trunk_Load ********************************************************
Description : Load a trunk of data from XML. This accepts a type into the constructor,
              and allows construction of derived classes.
Input       : class wxXmlNode* Root - The root node for the elements.
              const std::string& Prefix - The expected prefix of the elements.
              ptr_t Type - The type of the elements. Passed into the constructor.
Output      : std::vector<std::unique_ptr<ELEM>>& Vect - The vector to fill in.
Return      : None.
******************************************************************************/
template <typename BASE,typename DERIVED>
void Trunk_Load(class wxXmlNode* Root, const std::string& Prefix, std::vector<std::unique_ptr<BASE>>& Vect, ptr_t Type)
{
    class wxXmlNode* Temp;
    cnt_t Count;

    Count=0;
    for(Temp=Root->GetChildren();Temp!=nullptr;Temp=Temp->GetNext())
    {
        if(Temp->GetName()!=(Prefix+std::to_string(Count)))
            wxLogDebug("Prefix sequence is incorrect. This is not a hard error.");
        Vect.push_back(std::make_unique<DERIVED>(Temp,Type));
        Count++;
    }
}
/* End Template:Trunk_Load ***************************************************/

/* Template:Trunk_Save ********************************************************
Description : Save a trunk of data to XML.
Input       : class wxXmlNode* Root - The root node for the elements.
              const std::string& Prefix - The expected prefix of the elements.
              std::vector<std::unique_ptr<ELEM>>& Vect - The vector to save.
Output      : None.
Return      : None.
******************************************************************************/
template <typename ELEM>
void Trunk_Save(class wxXmlNode* Root, const std::string& Prefix, std::vector<std::unique_ptr<ELEM>>& Vect)
{
    class wxXmlNode* Temp;
    cnt_t Count;

    Count=0;
    for(std::unique_ptr<ELEM>& Elem:Vect)
    {
        Temp=Main::Simple_Save(Root,Prefix+std::to_string(Count));
        Elem->Save(Temp);
        Count++;
    }
}
/* End Template:Trunk_Save ***************************************************/
/*****************************************************************************/
/* __RVM_CFG_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
