#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>

//#include "../Units/Units.h"
#include "../Unit_Manager_Test/Unit_Manager_Test.h"

#include "../Units/Mem_Unit/Mem_Unit.h"
#include "../Units/Shmem_Unit/Shmem_Unit.h"
#include "../Units/Thread_Unit/Thread_Unit.h"
#include "../Units/Invocation_Unit/Invocation_Unit.h"
#include "../Units/Port_Unit/Port_Unit.h"
#include "../Units/Receive_Unit/Receive_Unit.h"
#include "../Units/Send_Unit/Send_Unit.h"
#include "../Units/Vector_Unit/Vector_Unit.h"
#include "../Units/Kfunc_Unit/Kfunc_Unit.h"

#include "Native_Child_BaseInfo/Native_Child_BaseInfo.h"

#include "../../../../Proj_Info/RVP_Info/Unit/Memory/Memory.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Shmem/Shmem.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Thread/Thread.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Invocation/Invocation.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Port/Port.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Receive/Receive.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Send/Send.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Vector/Vector.h"
#include "../../../../Proj_Info/RVP_Info/Unit/Kfunc/Kfunc.h"
#include "../../../../Proj_Info/RVP_Info/Native_Manager/Native/Native.h"



class Panel_Native_Child
    :public wxFrame {
public:
    Panel_Native_Child(wxWindow* parent, const int& _id, const wxString& _errorloc = wxEmptyString);
    void SetNativeChild(const Native* ptr);
    // ·µ»ØbaseInfoÖÐµÄName
    wxString GetBaseInfoName();
    bool IsLegal();
    bool TotConfirmFunc();
    void ClearError();
    int id;
private:
    void OnClose(const wxCloseEvent& event);
    void OnTotConfirm(const wxCommandEvent& event);
    void OnPreview(const wxCommandEvent& event);
    wxNotebook* nv_notebook;
    Native_Child_BaseInfo* base_info;
    Unit_Manager_Test<Mem_Unit, Memory>* memUnit;
    Unit_Manager_Test<Shmem_Unit, Shmem>* shmemUnit;
    Unit_Manager_Test<Thread_Unit, Thread>* threadUnit;
    Unit_Manager_Test<Invocation_Unit, Invocation>* invocationUnit;
    Unit_Manager_Test<Port_Unit, Port>* portUnit;
    Unit_Manager_Test<Receive_Unit, Receive>* receiveUnit;
    Unit_Manager_Test<Send_Unit, Send>* sendUnit;
    Unit_Manager_Test<Vector_Unit, Vector>* vectorUnit;
    Unit_Manager_Test<Kfunc_Unit, Kfunc>* kfuncUnit;
    wxString errorloc;

};

