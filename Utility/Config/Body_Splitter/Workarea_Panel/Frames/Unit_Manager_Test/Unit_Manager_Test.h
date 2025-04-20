#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/notebook.h>
#include <set>
#include <vector>

#include "../../../../Func/func.h"


// U 为GUI类型
// P 为填充GUI对应的数组
template<typename U, typename P>
class Unit_Manager_Test
    :public wxPanel {
public:
    Unit_Manager_Test(
        wxWindow* parent,
        const wxString& _sign,
        const wxString& _errorloc = wxEmptyString,
        std::function<void()>_func = nullptr)
        :wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN),
        max_num_GUI(0), unit_sign(_sign), errorloc(_errorloc), func(_func) {

        wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
        this->notebook = new wxNotebook(this, wxID_ANY);

        wxButton* add_unit = new wxButton(this, wxID_ANY, "添加");

        box->Add(notebook, 1, wxEXPAND);
        box->Add(add_unit, 0, wxALIGN_CENTER_HORIZONTAL);
        this->SetSizer(box);

        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Unit_Manager_Test<U, P>::OnAdd, this, add_unit->GetId());
    }

    void SetGUI(const std::vector<std::unique_ptr<P>>& info_ptrs) {
        max_num_GUI = 0;

        // 销毁页，但是不释放页面的对象
        // 页面中的对象U的parent为notebook，在notebook被释放时所有页面对象将被释放
        // notebook->DeleteAllPages();

        while (notebook->GetPageCount() > 0) {
            U* myUnit = (U*)notebook->GetPage(0);  // 获取当前第一页
            notebook->RemovePage(0);  // 从 notebook 移除该页
            myUnit->Destroy();
        }

        for (const auto& i : info_ptrs) {
            U* myUnit = new U(notebook, wxString::Format("%s/%s%d/", errorloc, unit_sign, max_num_GUI), func);
            myUnit->SetUnit(i);
            notebook->AddPage(myUnit, wxString::Format("%s%d", unit_sign, max_num_GUI));
            ++max_num_GUI;
        }
    }

    bool IsLegal() {
        
        bool judge = true;
        std::set<wxString>UniqueName;
        // GUI层面上检查每个单元是否合法
        for (int i = 0; i < notebook->GetPageCount(); i++) {
            U* myUnit = (U*)notebook->GetPage(i);

            if (!myUnit->IsLegal()) {
                // 页面内控件不合法
                judge = false;
            }
            else {
                // 页面内控件合法
                if (UniqueName.find(myUnit->GetName()) == UniqueName.end()) {
                    // 没有记录过该字符串
                    UniqueName.insert(myUnit->GetName());
                    myUnit->NameCorrectSet();
                }
                else {
                    // 记录过该字符串
                    myUnit->NameIncorrectSet(wxString::Format("命名重复 本页面之前可能使用过%s", myUnit->GetName()));
                    judge = false;
                }
                //myUnit->ClearError();
            }
        }
        return judge;
    }


    void ConfirmFunc(std::vector<std::unique_ptr<P>>& info_ptrs) {
        // 检查是否合法

        if (!IsLegal()) {
            wxLogError("Unit Manager检查出错误");
            return;
        }

        // 清空 vector，释放所有 std::unique_ptr<P>
        info_ptrs.clear();

        // 使用合法的GUI内容填充
        for (int i = 0; i < notebook->GetPageCount(); i++) {
            U* myUnit = (U*)notebook->GetPage(i);
            //notebook->SetPageText(i, wxString::Format("%d", i));
            info_ptrs.push_back(std::move(myUnit->GetUnit()));
        }
        //max_num_GUI = int(notebook->GetPageCount());
    }

    void AddFunc() {
        Freeze();
        notebook->AddPage(
            new U(notebook, wxString::Format("%s/%s%d/", errorloc, unit_sign, max_num_GUI), func),
            wxString::Format("%s%d", unit_sign, max_num_GUI)
        );
        ++max_num_GUI;
        Thaw();
    }

    void OnAdd(const wxCommandEvent& event) {
        // 新建的原生进程至少要有一个线程，AddFunc用于外部调用
        AddFunc();
    }

    void ClearError() {
        for (int i = 0; i < notebook->GetPageCount(); i++) {
            U* myUnit = (U*)notebook->GetPage(i);
            myUnit->ClearError();
        }
    }

    void ClearAll() {
        while (notebook->GetPageCount() > 0) {
            U* myUnit = (U*)notebook->GetPage(0);  // 获取当前第一页
            notebook->RemovePage(0);  // 从 notebook 移除该页
            myUnit->ClearError();   // 清除错误
            myUnit->Destroy();  // 释放空间
        }
        max_num_GUI = 0;    // 重置最大计数值
    }


private:
    wxNotebook* notebook;   // 管理不同的实例
    wxString unit_sign;     // 实例的标识
    wxString errorloc;      // 发生错误的位置
    std::function<void()>func;  // 用于跳转到当前控件存在的模块
    int max_num_GUI;        // notebook中页面的最大编号

};