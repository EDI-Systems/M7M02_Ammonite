#include "main.h"
#include "Menu_Bar/menu_bar.h"
#include "Bottom_message/bottom_message.h"
#include "Tool_Bar/tool_bar.h"
#include "Func/func.h"
#include "Body_Splitter/Workarea_Panel/workarea_panel.h"
#include "Body_Splitter/Panels/panels.h"

//#include "ErrorLogPanel2/ErrorUnitPanel/ErrorUnitPanel.h"

// 在类外部定义静态成员变量并初始化
wxBoxSizer* Main::app_box = nullptr;
Menu_Bar* Main::menu_bar = nullptr;
Tool_Bar* Main::tool_bar = nullptr;
Body_Splitter* Main::body_splitter = nullptr;
//ErrorLogPanel* Main::log_panel = nullptr;
ErrorLogPanel2* Main::log_panel2 = nullptr;
Bottom_Message* Main::bottom_message = nullptr;
Proj_Info* Main::proj_info = nullptr;

int Main::work_mode = MODE_INIT;

//
//class myFrame
//    :public wxFrame {
//public:
//    myFrame(wxWindow* parent)
//        :wxFrame(parent, wxID_ANY, "TEST", wxDefaultPosition, wxSize(300, 300)) {
//        wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//
//        LogTest = new ErrorLogPanel2(this);
//
//    }
//    void AddLog(const int& id, const wxString& loc, const wxString& cont, std::function<void()> callback) {
//        LogTest->AddLog(id, loc, cont, callback);
//    }
//    void Refresh() {
//        LogTest->Refresh();
//    }
//
//    //void func() {
//    //    //使用 Lambda 延迟调用
//    //    std::function<void()> func = [=]() { Main::body_splitter->workarea_panel->ShowBaseInfo(); };
//    //    LogTest->AddLog(1, "hahaha", "hehehe", func);
//    //    LogTest->Refresh();
//    //}
//
//private:
//    ErrorLogPanel2* LogTest;
//};

Main::Main()
	:wxFrame(nullptr, wxID_ANY, "工程描述文件编辑器", wxDefaultPosition, wxSize(800, 700)) {
    wxString str;
    // 创建一个垂直布局管理器
    Main::app_box = new wxBoxSizer(wxVERTICAL);

    menu_bar = new Menu_Bar();
    this->SetMenuBar(menu_bar);

    tool_bar =new Tool_Bar(this);
    this->SetToolBar(tool_bar);

    body_splitter = new Body_Splitter(this);
    Main::app_box->Add(body_splitter, 4, wxEXPAND);

    log_panel2 = new ErrorLogPanel2(this);
    Main::app_box->Add(log_panel2, 1, wxEXPAND);

    bottom_message = new Bottom_Message(this);
    Main::app_box->Add(bottom_message, 0, wxEXPAND);

    proj_info = new Proj_Info();

    // 设置主窗口的布局
    SetSizer(Main::app_box);
}

Main::~Main() {


}

void Main::IniEnable() {
    // 只有打开platform文件夹的按钮有效
    Main::menu_bar->DisableFileMenu();
    Main::body_splitter->Enable(false);
    Main::tool_bar->DisableRvaPreview();
    Main::tool_bar->DisableRvcPreview();
}


class MyApp : public wxApp {
public:
    bool OnInit() override {
        Main* main = new Main();
        main->IniEnable();
        main->CenterOnScreen();
        main->Show();

        return true;
    }
};

wxIMPLEMENT_APP(MyApp);

