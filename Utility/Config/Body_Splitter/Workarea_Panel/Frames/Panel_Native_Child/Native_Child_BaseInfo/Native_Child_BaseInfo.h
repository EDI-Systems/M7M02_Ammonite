#pragma once
#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <wx/xml/xml.h>
#include <map>

#include "../../Controller/Controller.h"
#include "../../../../../Proj_Info/RVP_Info/Native_Manager/Native/Native_BaseInfo/Native_BaseInfo.h"

class Native_Child_BaseInfo
    :public wxScrolledWindow {
public:
    Native_Child_BaseInfo(
        wxWindow* parent,
        const int& _id,
        const wxString& _errorloc = wxEmptyString,
        std::function<void()>func = nullptr);

    void ConfirmFunc(std::unique_ptr<Native_BaseInfo>& ptr);
    void SetNativeBaseInfo(const std::unique_ptr<Native_BaseInfo>& ptr);
    //wxString GetBaseInfoName();

    wxStaticText* title;
    SimpleText* name;
    MyZ_GE_Zero* extra_captbl;
    MyCoprocessor* coprocessor;
    SimpleText* buildsys;
    SimpleText* toolchain;
    MyChoice* optimi;

    SimpleText* project_output;
    SimpleText* linker_output;
    SimpleText* main_hdr_output;
    SimpleText* main_src_output;
    SimpleText* entry_src_output;

    BoolBox* project_overw;
    BoolBox* entry_src_overw;


    // 保存设置到node
    bool IsLegal();

    void ClearError();
    //wxXmlNode* GetNode()const { return node; }
private:
    int id;
    //void OnConfirm(const wxCommandEvent& event);
    void OnRefresh(const wxCommandEvent& event);
    void RefreshFunc();
    //wxXmlNode* node;    // 其实native frame存过了，这个就无所谓了
    wxString errorloc;
};