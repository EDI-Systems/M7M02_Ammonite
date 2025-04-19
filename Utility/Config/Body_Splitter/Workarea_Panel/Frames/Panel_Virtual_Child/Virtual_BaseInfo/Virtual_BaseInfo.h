#pragma once
#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <wx/xml/xml.h>
#include <map>

#include "../../Controller/Controller.h"
#include "../../../../../Proj_Info/RVP_Info/VM_Manager/VirtualMachine/VM_BaseInfo/VM_BaseInfo.h"

class Virtual_BaseInfo
    :public wxScrolledWindow {
public:
    Virtual_BaseInfo(
        wxWindow* parent,
        const int& _id,
        const wxString& _errorloc = wxEmptyString,
        std::function<void()>func = nullptr);

    // 读取信息
    void SetVirMachineBaseInfo(const std::unique_ptr<VM_BaseInfo>& ptr);

    // 检查和法向并保存设置
    void ConfirmFunc(std::unique_ptr<VM_BaseInfo>& ptr);

    wxStaticText* title;

    MyCoprocessor* coprocessor;

    SizeSlider* vs_size;
    SizeSlider* us_size;

    MyVirtualPrio* priority;

    MyZ_G_Zero* timeslice;
    MyZ_G_Zero* period;

    MyZ_GE_Zero* extra_captbl;
    MyZ_GE_Zero* watchdog;
    MyZ_GE_Zero* vnum;

    MyChoice* optimi;

    SimpleText* name;
    SimpleText* gtype;
    SimpleText* buildsys;
    SimpleText* toolchain;
    SimpleText* project_output;
    SimpleText* linker_output;
    SimpleText* main_hdr_output;
    SimpleText* main_src_output;
    SimpleText* vir_hdr_output;
    SimpleText* vir_src_output;

    BoolBox* project_overw;
    BoolBox* vir_hdr_overw;
    BoolBox* vir_src_overw;


    wxBoxSizer* box;
    int id;

    bool IsLegal();

    void ClearError();
private:
    //void OnConfirm(const wxCommandEvent& event);
    void OnRefresh(const wxCommandEvent& event);
    void RefreshFunc();
    wxXmlNode* node;
    wxString errorloc;

};