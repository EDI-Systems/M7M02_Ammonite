/******************************************************************************
Filename    : rvm_cfg.hpp
Author      : pry
Date        : 27/12/2021
License     : Proprietary; confidential.
Description : Smart delay module writer class file header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __RVM_CFG_HPP_TYPES__
#define __RVM_CFG_HPP_TYPES__
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
/* __RVM_CFG_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __RVM_CFG_HPP_DEFS__
#define __RVM_CFG_HPP_DEFS__
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

/* Built-in standalone translator */
#define sT(X)                               RVM_CFG_App::Translate(X)

/* Throw error, and rescue some unsaved files */
#define ASSERT(X,STR) \
do \
{ \
    if((X)==0) \
    { \
        class wxMessageDialog* Dialog; \
        std::string String; \
        /* Display the info pointer to what have failed, both pop-up and logging */ \
        String=std::string(__FILE__)+":"+std::to_string(__LINE__)+" - "+__FUNC__+":"+__DATE__+"\n"+sT("Fatal")+": "+(STR); \
        wxLogDebug(String.c_str()); \
        Dialog=new wxMessageDialog(nullptr,String.c_str(),sT("Error"),wxOK|wxICON_ERROR); \
        Dialog->ShowModal(); \
        exit(-1); \
    } \
} \
while(0)

#define __FUNC__                            __func__

/* Software version - increase only */
#define FIRMWARE_VERSION                    (0x0101)
#define SOFTWARE_VERSION                    ("R1T1-0000-20220826")
#define SOFTWARE_NAME                       sT("SmarTock (TM) Smart Delay Module Standalone Downloader")

/* Neutral/OEM options */
/* #define OEM_NEUTRAL */

/* Option yes/no */
#define OPTION_NO                           (0)
#define OPTION_YES                          (1)

/* Date update flags */
#define TIME_DATE                           (0)
#define TIME_HMS                            (1)
#define TIME_DATE_HMS                       (2)

/* Actions */
#define ACTION_NONE                         (0)
#define ACTION_DOWNLOAD                     (1)
#define ACTION_REMOTE                       (2)
#define ACTION_DETECT                       (3)
#define ACTION_CLOCK                        (4)

/* Count */
/* Status gauge count */
#define GAUGE_MIN_COUNT                     (0)
#define GAUGE_MAX_COUNT                     (100)

/* Message box types */
#define MSGBOX_ERROR                        (0)
#define MSGBOX_WARN                         (1)
#define MSGBOX_INFO                         (2)
#define MSGBOX_ASK                          (3)

/* Power of 2 */
#define POW2(X)                             (1UL<<(X))
/* Bit/byte conversions */
#define BCD2BIN(X)                          (((X)>>4)*10+((X)&0xF))
#define BIN2BCD(X)                          ((((X)/10)<<4)|((X)%10))
#define BITLEN(X)                           (((X)+7)>>3)
#define U16R(X)                             (((X)[0]<<8)|(X)[1])
#define U32R(X)                             ((((u32_t)(X)[0])<<24)|((X)[1]<<16)|((X)[2]<<8)|(X)[3])
#define U16W(X,V) \
do \
{ \
    (X)[0]=(V)>>8; \
    (X)[1]=(V)&0xFF; \
} \
while(0)

#define U32W(X,V) \
do \
{ \
    (X)[0]=(V)>>24; \
    (X)[1]=((V)>>16)&0xFF; \
    (X)[2]=((V)>>8)&0xFF; \
    (X)[3]=(V)&0xFF; \
} \
while(0)

/* 1-bit continuous bitfield */
#define BYTE1(B,X)                          ((B)[(X)>>3])
#define SHIFT1(X)                           ((X)&7)
#define BIT1(X)                             POW2(SHIFT1(X))
#define MASK1                               (1)
#define READ1(B,X)                          ((BYTE1(B,X)&BIT1(X))!=0)
#define WRITE1(B,X,V)                       ((V)?(BYTE1(B,X)|=BIT1(X)):(BYTE1(B,X)&=~BIT1(X)))

/* 2-bit continuous bitfield */
#define BYTE2(B,X)                          ((B)[(X)>>2])
#define SHIFT2(X)                           (((X)&3)<<1)
#define BIT2(X,N)                           POW2(SHIFT2(X)+(N))
#define MASK2                               (3)
#define READ2(B,X,N)                        ((BYTE2(B,X)&BIT2(X,N))!=0)
#define WRITE2(B,X,N,V)                     ((V)?(BYTE2(B,X)|=BIT2(X,N)):(BYTE2(B,X)&=~BIT2(X,N)))

/* 4-bit continuous bitfield */
#define BYTE4(X)                            ((B)[(X)>>1])
#define SHIFT4(X)                           (((X)&1)<<2)
#define BIT4(X,N)                           POW2(SHIFT4(X)+(N))
#define MASK4                               (0xF)
#define READ4(B,X,N)                        ((BYTE4(B,X)&BIT4(X,N))!=0)
#define WRITE4(B,X,N,V)                     ((V)?(BYTE4(B,X)|=BIT4(X,N)):(BYTE4(B,X)&=~BIT4(X,N)))

/* Panels */
#define OUTPUT_INFO                         (0)
#define OUTPUT_STATUS                       (1)

/* Product ID */
#define PRODID_C2S0404                      (0x2404)
#define PRODID_C2S0405                      (0x2405)
#define PRODID_C4G0202                      (0x4202)
#define PRODID_C4G0203                      (0x4203)
#define PRODID_C4G0204                      (0x4204)
#define PRODID_C4G0205                      (0x4205)
#define PRODID_C4G0206                      (0x4206)
#define PRODID_C4G0212                      (0x4212)
#define PRODID_C4G0218                      (0x4218)

/* Other product IDs that may be returned but we don't care */
#define PRODID_C4G0101                      (0x4101)
#define PRODID_C4G0102                      (0x4102)
#define PRODID_C4G0103                      (0x4103)

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
/*****************************************************************************/
/* __RVM_CFG_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __RVM_CFG_HPP_CLASSES__
#define __RVM_CFG_HPP_CLASSES__
/*****************************************************************************/
class Main:public wxFrame
{
public:
    /* Executable path */
    static std::string Exe_Folder;

    /* Action state */
    static ptr_t Action_State;

    /* Exit flag */
    static ptr_t Exit_Flag;

    /* Auto-download timer - will be started for 5 seconds each time we finish downloading */
    static class wxTimer* Auto_Timer;
    /* The timer for remote detection scanning */
    static class wxTimer* Remote_Timer;

	/* Lower-level classes */
    static class Menu_Bar* Menu_Bar;
    static class wxPanel* Panel;
    static class wxBoxSizer* Panel_Sizer;
    static class wxBoxSizer* Border_Sizer;
    static class wxBoxSizer* Main_Sizer;

    static class wxStaticBoxSizer* File_Sizer;
    static class wxStaticText* File_Label;
    static class wxFilePickerCtrl* File_Picker;

    static class wxFlexGridSizer* Grid_Sizer;

    static class wxStaticBoxSizer* Info_Sizer;
    static class wxTextCtrl* Info_Text;

    static class wxStaticBoxSizer* Option_Sizer;
    static class wxFlexGridSizer* Option_Grid_Sizer;
    static class wxStaticText* Verify_After_Download_Label;
    static class wxChoice* Verify_After_Download_Choice;
    static class wxStaticText* Calib_When_Download_Label;
    static class wxChoice* Calib_When_Download_Choice;
    static class wxStaticText* Late_Remote_Pair_Label;
    static class wxChoice* Late_Remote_Pair_Choice;
    static class wxStaticText* Late_Serial_Config_Label;
    static class wxChoice* Late_Serial_Config_Choice;
    static class wxStaticText* Late_Clock_Calib_Label;
    static class wxChoice* Late_Clock_Calib_Choice;
    static class wxStaticText* Allow_Debug_Label;
    static class wxChoice* Allow_Debug_Choice;

    static class wxStaticBoxSizer* Status_Sizer;
    static class wxTextCtrl* Status_Text;

    static class wxStaticBoxSizer* Action_Sizer;
    static class wxFlexGridSizer* Action_Grid_Sizer;
    static class wxCheckBox* Auto_Download;
    static class wxCheckBox* Beep_When_Complete;
    static class wxButton* Detect_Hardware;
    static class wxButton* Calib_Clock;
    static class wxButton* Toggle_Remote_Detect;
    static class wxButton* Toggle_Download;

    static class Status_Bar* Status_Bar;

    static class About_Dialog* About_Dialog;

    /* Pre-loaded daemons */
    static class Download_Daemon* Download_Daemon;

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

    static void Output_Update(std::vector<std::string>& Reply,ptr_t Panel,ptr_t Append);
    static void Output_Clear(ptr_t Panel);
    static void Gauge_Update(ptr_t Pos);

    /* Manuals */
    static void Manual_Open(const std::string& Manual);

    static void State_Set(ptr_t State);

    void On_Auto_Timer(class wxTimerEvent& Event);
    void On_Remote_Timer(class wxTimerEvent& Event);
    void On_File_Picker(class wxFileDirPickerEvent& Event);
    void On_Calib_Clock(class wxCommandEvent& Event);
    void On_Detect_Hardware(class wxCommandEvent& Event);
    void On_Toggle_Remote_Detect(class wxCommandEvent& Event);
    void On_Toggle_Download(class wxCommandEvent& Event);
    void On_Close_Window(class wxCloseEvent& Event);
};

/* Program entry point */
class RVM_CFG_App:public wxApp
{
public:
    /* Self-contained locale - not using external translator */
    static ptr_t Locale;

    /* Main class */
    static class Main* Main;

    /* Self-contained translation */
    int Locale_Set(void);
    static class wxString Translate(const char* Input);
    static class wxString Translate(const std::string& Input);
    static class wxString Translate(const class wxString& Input);

    virtual bool OnInit(void) final override;
    virtual bool OnExceptionInMainLoop(void) final override;
#ifdef WIN32
    static LONG WINAPI OnSystemFatalException(EXCEPTION_POINTERS* Exc);
#else
    static void OnSystemFatalException(int Exc);
#endif
};
/*****************************************************************************/
/* __RVM_CFG_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
