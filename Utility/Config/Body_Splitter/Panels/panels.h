//#pragma once
//#include <wx/wx.h>
//#include <wx/spinctrl.h>
//#include <vector>
//#include <map>
//#include <set>
//#include <unordered_map>
//
//#include <wx/notebook.h>
//#include <wx/xml/xml.h>
//#include <wx/grid.h>
//#include <wx/regex.h>
//#include <wx/treectrl.h>
//#include "../../Func/func.h"
//
//
///*****************
//*   SimpleBase
//*****************/
//class SimpleBase
//    :public wxPanel{
//public:
//    SimpleBase(wxWindow* parent, wxString _title = wxEmptyString, wxString _intr = wxEmptyString, wxString _errorloc = wxEmptyString);
//    void SetDefaultColor(wxColor c) { default_color = c; }
//    void CorrectSet();
//    void IncorrectSet(const wxString& info);
//    void SetCheckTip(const wxString& tip);
//    virtual bool IsLegal() { return false; }
//    virtual bool IsLegal(const wxString& str) { return false; }
//    virtual bool IsLegal(const int& para) { return false; }
//    wxBoxSizer* box;
//protected:
//
//    wxColour default_color;
//    wxPanel* light;
//    wxString errorloc;
//    //wxString title_content;
//    int uni_id;
//private: 
//    wxStaticText* title;
//};
//
//// 加载页面
//class LoadingFrame
//    : public wxFrame {
//public:
//    LoadingFrame(wxWindow* parent);
//};
//
///*****************
//*   Align
//*****************/
//class Align
//    :public SimpleBase {
//public: 
//    Align(wxWindow* parent, wxString _title = wxEmptyString, wxString _intr = wxEmptyString, wxString _errorloc = wxEmptyString);
//    // 传入 基址
//    bool IsLegal(const wxString& base)override;
//    void SetValue(const wxString& str) { text->SetValue(str); }
//    wxString GetValue() { return text->GetValue(); }
//private:
//    wxTextCtrl* text;
//    void OnFillAuto(const wxCommandEvent& event);
//};
//
///*****************
//*   简单的文本框
//*****************/
//class SimpleText
//    :public SimpleBase{
//public:
//    SimpleText(wxWindow* parent, 
//        wxString _title = wxEmptyString, 
//        wxString _describe = wxEmptyString, 
//        wxString _content = wxEmptyString, 
//        long _style = 0L, 
//        wxString _errorloc = wxEmptyString);
//    // 检查是否为合法的C标识符
//    bool IsLegal()override;
//    void SetValue(const wxString& str) { str == wxEmptyString ? text->SetValue(default_content) : text->SetValue(str); }
//    wxString GetValue() { return text->GetValue(); }
//private:
//    wxTextCtrl* text;
//    wxString default_content;
//};
//
///*****************
//*   Bool 选择栏
//*****************/
//class BoolBox
//    :public SimpleBase {
//public:
//    BoolBox(wxWindow* parent, wxString _title = wxEmptyString, wxString _intr = wxEmptyString);
//    // 不需要进行检查
//    bool IsLegal()override { return true; }
//    void SetValue(const wxString& t) { t == "Yes" ? yes->SetValue(true) : no->SetValue(true); }
//    wxString GetStringValue(){
//        if (yes->GetValue())
//            return "Yes";
//        return "No";
//    }
//private:
//    wxRadioButton* yes;
//    wxRadioButton* no;
//};
//
///*****************
//*   Base 选择栏
//*****************/
//class BaseSlider
//    :public SimpleBase {
//public:
//    BaseSlider(wxWindow* parent,
//        const wxString& name = wxEmptyString,
//        const wxString& describe = wxEmptyString,
//        const wxString& errorloc = wxEmptyString);
//    // 检查是否为Auto 或者非空的十六进制数
//    bool IsLegal()override;
//    void SetPosition(const int& pos);
//    void SetValue(const wxString& str) { content->SetValue(str); }
//    wxString GetValue() { return content->GetValue(); }
//private:
//    wxSlider* slider;
//    wxStaticText* displayText;
//    void OnSliderUpdate(const wxCommandEvent& event);
//    void OnFillIn(const wxCommandEvent& event);
//    void OnAuto(const wxCommandEvent& event);
//
//    wxString hex_content;
//    wxTextCtrl* content;
//};
//
///*****************
//*   Size 选择栏
//*****************/
//class SizeSlider
//    :public SimpleBase {
//public:
//    SizeSlider(wxWindow* parent,
//        const wxString& name = wxEmptyString,
//        const wxString& describe = wxEmptyString,
//        const wxString& errorloc = wxEmptyString);
//
//    // 检查是否为Auto 或者非空的十六进制数
//    bool IsLegal()override;
//    void SetPosition(const int& pos);
//    void SetValue(const wxString& str) { content->SetValue(str); }
//    wxString GetValue() { return content->GetValue(); }
//private:
//    wxSlider* slider;
//    wxStaticText* displayText;
//    void OnSliderUpdate(wxCommandEvent& event);
//    void OnFillIn(const wxCommandEvent& event);
//
//    wxString hex_content;
//    wxTextCtrl* content;
//};
//
///*****************
//*   MyChoice
//*****************/
//class MyChoice
//    :public SimpleBase {
//public:
//    MyChoice(wxWindow* parent,
//        const wxString& title = wxEmptyString,
//        const wxString& describe = wxEmptyString,
//        const wxString& errorloc = wxEmptyString);
//    // 无需检查
//    bool IsLegal()override { return true; }
//    void FillChoice(wxArrayString arr);
//    void SetSelection(const int& cur) { choice->SetSelection(cur); }
//    int FindValue(const wxString& tar);
//    wxString GetStringSelection() { return choice->GetStringSelection(); }
//private:
//    wxChoice* choice;
//};
//
///*****************
//*   MyAttribute
//*****************/
//class MyAttribute
//    :public SimpleBase {
//public:
//    MyAttribute(
//        wxWindow* parent,
//        wxWindowID id,
//        const wxString& title = wxEmptyString,
//        const wxString& describe = wxEmptyString,
//        const wxString& _errorloc = wxEmptyString);
//    // 检查不能为空
//    bool IsLegal()override;
//    void SetValue(const wxString& str);
//    wxString GetValue() { return text->GetValue(); }
//private:
//    void OnRadioButtonClicked(const wxCommandEvent& event);
//    wxTextCtrl* text;
//    wxCheckBox* Readable;
//    wxCheckBox* Writable;
//    wxCheckBox* Executable;
//    wxCheckBox* Bufferable;
//    wxCheckBox* Cacheable;
//    wxCheckBox* Static;
//    std::unordered_map <char, wxCheckBox*>* mp;
//};
//
///*****************
//*   MyCoprocessor
//*****************/
//class MyCoprocessor
//    :public SimpleBase {
//public:
//    MyCoprocessor(wxWindow* parent,
//        const wxString& title = wxEmptyString,
//        const wxString& describe = wxEmptyString,
//        const wxString& errorloc = wxEmptyString);
//    // 至少选择一项非None 或只选择None
//    bool IsLegal()override;
//    void FillAllItem(wxArrayString arr);
//    void SetValue(const wxArrayString& arr);
//    wxString GetValue();
//
//private:
//    std::unordered_map<wxString, wxCheckBox*>* mp;
//    wxBoxSizer* checkbox;
//    //wxStaticBoxSizer* cur_checkbox;
//};
//
///*****************
//*   MyPrio
//*****************/
////class MyPrio
////    :public SimpleBase {
////public:
////    MyPrio(wxWindow* parent,
////        const wxString& title = wxEmptyString,
////        const wxString& describe = wxEmptyString,
////        const wxString& errorloc = wxEmptyString);
////    // 检查是否是十进制非负整数
////    bool IsLegal()override;
////    void SetValue(const wxString& str) { text->SetValue(str); }
////    wxString GetValue() { return text->GetValue(); }
////private:
////    wxTextCtrl* text;
////};
//
///*****************
//*   MyVirtPrio
//*****************/
//class MyVirtPrio
//    :public SimpleBase {
//public:
//    MyVirtPrio(wxWindow* parent, 
//        const wxString& title = wxEmptyString, 
//        const wxString& describe = wxEmptyString,
//        const wxString& errorloc = wxEmptyString,
//        const bool& hide_virt_mes = false);
//    // 检查是否是十进制非负整数
//    bool IsLegal()override;
//    void SetVmCount(const int& n) { 
//        text_vm_count->SetLabel(wxString::Format("当前虚拟机数 %d", n));
//    }
//    void FillChoice(wxArrayString arr);
//    int FindValue(const wxString& tar);
//    void SetSelection(const int& cur) { choice->SetSelection(cur); }
//    wxString GetStringSelection() { return choice->GetStringSelection(); }
//
//private:
//    wxChoice* choice;
//    //wxTextCtrl* text;
//    wxStaticText* text_vm_count;
//
//};
//
///*****************
//*   MyVirtEvent
//*****************/
////class MyVirtEvent
////    :public SimpleBase {
////public:
////    MyVirtEvent(wxWindow* parent, 
////        const wxString& title = wxEmptyString,
////        const wxString& describe = wxEmptyString,
////        const wxString & errorloc = wxEmptyString);
////    // 十进制非负整数，不超过1024的限制，启用虚拟机时不得为0
////    bool IsLegal()override;
////    void SetValue(const wxString& str) { text->SetValue(str); }
////    wxString GetValue() { return text->GetValue(); }
////private:
////    wxTextCtrl* text;
////
////};
//
///*****************
//*   MyVirtMap
//*****************/
//class MyVirtMap
//    :public SimpleBase {
//public:
//    MyVirtMap(wxWindow* parent, 
//        const wxString& title = wxEmptyString, 
//        const wxString& describe = wxEmptyString,
//        const wxString& errorloc = wxEmptyString);
//    // 十进制非负整数，大于等于virt_event，启用虚拟机时非零
//    bool IsLegal(const int& virt_event)override;
//    void SetValue(const wxString& str) { text->SetValue(str); }
//    wxString GetValue() { return text->GetValue(); }
//private:
//    wxTextCtrl* text;
//};
//
///*****************
//*   MyVersion
//*****************/
//class MyVersion
//    :public SimpleBase {
//public:
//    MyVersion(wxWindow* parent,
//        const wxString& title = wxEmptyString,
//        const wxString& describe = wxEmptyString,
//        const wxString& errorloc = wxEmptyString);
//    // 非负数（整数或小数）
//    bool IsLegal()override;
//    void SetValue(const wxString& str) { text->SetValue(str); }
//    wxString GetValue() { return text->GetValue(); }
//private:
//    wxTextCtrl* text;
//
//};
//
///*****************
//*   MyParameter
//*****************/
//class MyParameter
//    :public SimpleBase {
//public:
//    MyParameter(wxWindow* parent,
//        const wxString& title = wxEmptyString,
//        const wxString& describe = wxEmptyString,
//        const wxString& errorloc = wxEmptyString);
//    // 合法的十六进制数，或空字符串
//    bool IsLegal()override;
//    void SetValue(const wxString& str) { text->SetValue(str); }
//    wxString GetValue() { return text->GetValue(); }
//private:
//    wxTextCtrl* text;
//};
//
///*****************
//*   MyThreadPrio
//*****************/
//class MyThreadPrio
//    :public SimpleBase {
//public:
//    MyThreadPrio(wxWindow* parent,
//        const wxString& title = wxEmptyString,
//        const wxString& describe = wxEmptyString,
//        const wxString& errorloc = wxEmptyString);
//    // 十进制非负整数，且有范围限制
//    bool IsLegal(const int& kern_prio)override;
//    void SetValue(const wxString& str) { text->SetValue(str); }
//    wxString GetValue() { return text->GetValue(); }
//private:
//    wxTextCtrl* text;
//};
//
///*****************
//*   MyVirtualPrio
//*****************/
//class MyVirtualPrio
//    :public SimpleBase {
//public:
//    MyVirtualPrio(wxWindow* parent, 
//        const wxString& title = wxEmptyString, 
//        const wxString& describe = wxEmptyString, 
//        const wxString& _errorloc = wxEmptyString);
//    // 十进制非负整数，且有范围限制
//    // 但是没有找到所谓“在虚拟机优先级数”作为限制
//    bool IsLegal(const int& virt_prio);
//    void SetValue(const wxString& str) { text->SetValue(str); }
//    wxString GetValue() { return text->GetValue(); }
//private:
//    wxTextCtrl* text;
//};
//
///*****************
//*   MyZ_G_Zero
//*****************/
//class MyZ_G_Zero
//    :public SimpleBase {
//public:
//    MyZ_G_Zero(wxWindow* parent,
//        const wxString& title = wxEmptyString,
//        const wxString& describe = wxEmptyString,
//        const wxString& errorloc = wxEmptyString);
//    // 正整数
//    bool IsLegal();
//    void SetValue(const wxString& str) { text->SetValue(str); }
//    wxString GetValue() { return text->GetValue(); }
//private:
//    wxTextCtrl* text;
//};
//
///*****************
//*   MyZ_GE_Zero
//*****************/
//class MyZ_GE_Zero
//    :public SimpleBase {
//public:
//    MyZ_GE_Zero(wxWindow* parent,
//        const wxString& title = wxEmptyString,
//        const wxString& describe = wxEmptyString,
//        const wxString & errorloc = wxEmptyString);
//    // 非负整数
//    bool IsLegal();
//    void SetValue(const wxString& str) { text->SetValue(str); }
//    wxString GetValue() { return text->GetValue(); }
//private:
//    wxTextCtrl* text;
//};
//
///*****************
//*   MyGrid
//*****************/
//
////class MyGrid
////    :public wxGrid {
////public:
////    MyGrid(wxWindow* parent, wxWindowID id);
////    void AppendOneRows(const wxArrayString& arr);
////    void OnCellValueChanged(const wxGridEvent& event);
////    void MatchFirst2nd(const wxArrayString& arr);
////    wxArrayString GetValue();
////    bool IsLegal();
////
////private:
////    int max_row = 1;
////};
//class MyGrid
//    :public SimpleBase {
//public:
//    MyGrid(wxWindow* parent,
//        const wxString& title = wxEmptyString,
//        const wxString& describe = wxEmptyString,
//        const wxString& errorloc = wxEmptyString);
//    void AppendOneRows(const wxArrayString& arr);
//    void OnCellValueChanged(const wxGridEvent& event);
//    void MatchFirst2nd(const wxArrayString& arr);
//    void Clear();
//    wxArrayString GetValue();
//    bool IsLegal() override;
//
//private:
//    int max_row = 1;
//    wxGrid* grid;
//};
//
//class PanelBase
//    :public wxPanel {
//public:
//    PanelBase(wxWindow* parent, wxWindowID id);
//
//protected:
//    virtual void OnConfirm(const wxCommandEvent& event) {};
//    virtual void OnPreview(const wxCommandEvent& event) {};
//    virtual void OnRefresh(const wxCommandEvent& event) {};
//    wxBoxSizer* box;
//    wxBoxSizer* highbox;
//    wxScrolledWindow* swindow;
//
//};
//class Base_Unit
//    :public wxScrolledWindow {
//public:
//    Base_Unit(wxWindow* parent, const wxString& _errorloc = wxEmptyString);
//    void NameCorrectSet();
//    void NameIncorrectSet(const wxString& str);
//    // Send、Port的唯一性由name和process决定，需要重写
//    virtual wxString GetName();
//    virtual void ClearError() { wxMessageBox("基类func"); }
//    virtual bool IsLegal() { return false; }
//protected:
//    // thread在每个原生进程中至少要存在一个，需要重写
//    virtual void OnDelete(const wxCommandEvent& event); 
//    wxButton* delete_unit;
//    SimpleText* base_name;
//    wxString errorloc;
//};
///************************************************
//*   虚拟机（原生进程）管理 Mem unit
//************************************************/
//class Mem_Unit_Test
//    :public Base_Unit {
//public:
//    Mem_Unit_Test(wxWindow* parent, const wxString& errorloc = wxEmptyString);
//    void SetUnit(wxXmlNode* root);// 因为模板类不暴露具体的实现细节，所以只能提供接口，函数名都要为SetUnit?
//    wxXmlNode* MakeXmlUnit(const wxString& name);
//    bool IsLegal()override;
//    void ClearError()override;
//
//    
//private:
//    BaseSlider* mem_base;
//    SizeSlider* mem_size;
//    Align* mem_align;
//    MyChoice* mem_type;
//    MyAttribute* mem_attribute;
//};
//
///************************************************
//*   虚拟机（原生进程）管理   Shmem unit
//************************************************/
//
//class Shmem_Unit_Test
//    :public Base_Unit {
//public:
//    Shmem_Unit_Test(wxWindow* parent, const wxString& _errorlc = wxEmptyString);
//    void SetUnit(wxXmlNode* root);
//    wxXmlNode* MakeXmlUnit(const wxString& name);
//    bool IsLegal()override;
//    void ClearError()override;
//
//private:
//    MyAttribute* sh_attribute;
//};
//
//
///************************************************
//*   虚拟机（原生进程）管理   Send unit
//************************************************/
//
//class Send_Unit_Test
//    :public Base_Unit {
//public:
//    Send_Unit_Test(wxWindow* parent, const wxString& errorloc = wxEmptyString);
//    void SetUnit(wxXmlNode* root);
//    wxXmlNode* MakeXmlUnit(const wxString& name);
//    bool IsLegal()override;
//    void ClearError()override;
//    wxString GetName()override; // 由name和process唯一确定
//
//private:
//    SimpleText* sd_process;
//};
//
///************************************************
//*    虚拟机（原生进程）管理    kFunc
//************************************************/
//class Kfunc_Unit_Test
//    :public Base_Unit {
//public:
//    Kfunc_Unit_Test(wxWindow* parent, const wxString& errorloc = wxEmptyString);
//    void SetUnit(wxXmlNode* root);
//    wxXmlNode* MakeXmlUnit(const wxString& name);
//    bool IsLegal()override;
//    void ClearError()override;
//
//private:
//    BaseSlider* k_begin;
//    BaseSlider* k_end;
//};
//
///************************************************
//*    原生进程管理    Thread
//************************************************/
//class Thread_Unit_Test
//    :public Base_Unit {
//public:
//    Thread_Unit_Test(wxWindow* parent, const wxString& errorloc = wxEmptyString);
//    void SetUnit(wxXmlNode*);
//    wxXmlNode* MakeXmlUnit(const wxString& name);
//    bool IsLegal()override;
//    void ClearError()override;
//private:
//    // 当删除最后一个thread时，拒绝该操作
//    void OnDelete(const wxCommandEvent& event)override;
//    SizeSlider* t_stack_size;
//    MyParameter* t_parameter;
//    MyThreadPrio* t_priority;
//
//
//    //wxButton* delete_unit;
//};
///************************************************
//*    原生进程管理    Invocation
//************************************************/
//class Invocation_Unit_Test
//    :public Base_Unit {
//public:
//    Invocation_Unit_Test(wxWindow* parent, const wxString& errorloc = wxEmptyString);
//    //void DeleteFunc(const wxCommandEvent& event);
//    void SetUnit(wxXmlNode*);
//    wxXmlNode* MakeXmlUnit(const wxString& name);
//    bool IsLegal()override;
//    void ClearError()override;
//private:
//    SizeSlider* in_stack_size;
//};
//
///************************************************
//*    原生进程管理    Port
//************************************************/
//class Port_Unit_Test
//    :public Base_Unit {
//public:
//    Port_Unit_Test(wxWindow* parent, const wxString& errorloc = wxEmptyString);
//    //void DeleteFunc(const wxCommandEvent& event);
//    void SetUnit(wxXmlNode*);
//    wxXmlNode* MakeXmlUnit(const wxString& name);
//    bool IsLegal()override;
//    void ClearError()override;  
//    wxString GetName()override;  // 由name和process唯一决定
//private:
//    SimpleText* p_process;
//
//
//};
//
///************************************************
//*    原生进程管理    Receive
//************************************************/
//class Receive_Unit_Test
//    :public Base_Unit {
//public:
//    Receive_Unit_Test(wxWindow* parent, const wxString& errorloc = wxEmptyString);
//    void SetUnit(wxXmlNode*);
//    wxXmlNode* MakeXmlUnit(const wxString& name);
//    bool IsLegal()override;
//    void ClearError()override;
//private:
//    //wxTextCtrl* tcr_name;
//};
//
///************************************************
//*    原生进程管理    Vector  
//************************************************/
//class Vector_Unit_Test
//    :public Base_Unit {
//public:
//    Vector_Unit_Test(wxWindow* parent, const wxString& errorloc = wxEmptyString);
//    void SetUnit(wxXmlNode*);
//    wxXmlNode* MakeXmlUnit(const wxString& name);
//    bool IsLegal()override;
//    void ClearError()override {}
//private:
//    //wxTextCtrl* tcr_name;
//    MyZ_GE_Zero* v_number;
//
//};
//
///************************************************
//*    单元管理的模板类
//************************************************/
//template<typename U>
//class Unit_Manager
//    :public wxPanel {
//public:
//    Unit_Manager(wxWindow* parent, const wxString& _errorloc = wxEmptyString, const bool& show_button = true)
//        :wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN) {
//        errorloc = _errorloc;
//        this->max_num = 0;
//        wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
//        this->notebook = new wxNotebook(this, wxID_ANY);
//
//        wxBoxSizer* box1 = new wxBoxSizer(wxHORIZONTAL);
//        wxButton* add_unit = new wxButton(this, wxID_ANY, "添加");
//        wxButton* prev = new wxButton(this, wxID_ANY, "预览");
//        wxButton* confirm = new wxButton(this, wxID_ANY, "确定");
//        box1->Add(add_unit, 0, wxLEFT | wxBOTTOM, 5);
//        box1->Add(prev, 0, wxLEFT | wxBOTTOM, 5);
//        box1->Add(confirm, 0, wxLEFT| wxBOTTOM, 5);
//        if (!show_button) {
//            //add_unit->Hide();
//            prev->Hide();
//            confirm->Hide();
//        }
//
//        box->Add(notebook, 1, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 5);
//        box->Add(box1, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxALIGN_LEFT, 5);
//        this->SetSizer(box);
//
//        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Unit_Manager<U>::OnAdd, this, add_unit->GetId());
//        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Unit_Manager<U>::OnConfirm, this, confirm->GetId());
//        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Unit_Manager<U>::OnPreview, this, prev->GetId());
//
//    }
//
//    void AddBlankPage() {
//        U* temp = new U(this->notebook, errorloc + wxString::Format("编号%d->", this->max_num));
//        this->notebook->AddPage(temp, wxString::Format("编号%d",this->max_num));
//        this->max_num++;
//    }
//
//    void SetRoot(wxXmlNode* _root) { root = _root; }
//    void SetAllUnit(wxXmlNode* _root) {
//        root = _root;
//        if (!root) {
//            wxLogError("必要属性的root为空 错误在%s", __func__);
//            return;
//        }
//        for (wxXmlNode* child = root->GetChildren(); child; child = child->GetNext()) {
//            if (child->GetType() == wxXML_ELEMENT_NODE) {
//                U* temp = new U(this->notebook, errorloc + wxString::Format("编号%d->", this->max_num));    // 新建空白单元
//                temp->SetUnit(child);   // 填充空白单元
//                this->notebook->AddPage(temp, wxString::Format("编号%d", this->max_num));
//                this->max_num++;
//            }
//        }
//    }
//
//    void OnAdd(const wxCommandEvent& event) {
//        this->Freeze();
//        AddBlankPage();
//        this->Thaw();
//    }
//
//    bool ConfirmFunc() {
//        // 检查是否合法
//        bool islegal = true;
//        std::set<wxString>ExistName;
//        for (int i = 0; i < notebook->GetPageCount(); i++) {
//            U* cur = (U*)notebook->GetPage(i);
//            // 如果当前的my_unit已经不合法了, 就不会进行命名唯一性的检查
//            // 结果就是，命名重复的问题会在其他所有问题都消除后才显示
//            if (!cur->IsLegal()) {
//                islegal = false;
//                continue;
//            }
//            // 行命名唯一性的检查
//            wxString name = cur->GetName();
//            if (ExistName.find(name) != ExistName.end()) {
//                cur->NameIncorrectSet("命名重复");
//                islegal = false;
//            }
//            else {
//                cur->NameCorrectSet();
//                ExistName.insert(name);
//            }
//        }
//        if (!islegal)
//            return false;
//        // 把write_root全部清空然后挂新的节点
//        // 但是这样无法单独保存某一个单元
//        myfunc::Clear(root);
//        for (int i = 0; i < notebook->GetPageCount(); i++) {
//            U* my_unit = (U*)notebook->GetPage(i);
//            root->AddChild(my_unit->MakeXmlUnit(unit_sign + wxString::Format("%d", i)));
//        }
//        return true;
//    }
//
//    void OnConfirm(const wxCommandEvent& event) {
//        this->ConfirmFunc();
//    }
//
//    void PreviewFunc() {
//        if (root) {
//            wxString str;
//            myfunc::DFS(root, str);
//            myfunc::PreviewHelper(str, "源码预览");
//        }
//    }
//    void OnPreview(const wxCommandEvent& event) {
//        PreviewFunc();
//    }
//
//    wxXmlNode* MakeXmlUnit(const wxString& name) {
//        // 因为是根据notebook的page来创建单元，所以可以不需要传入节点用于修改
//        // 而是将生成的root挂到native或者virtual上就行
//        wxXmlNode* ans_root = new wxXmlNode(wxXML_ELEMENT_NODE, name);
//        for (int i = 0; i < notebook->GetPageCount(); i++) {
//            U* my_unit = (U*)notebook->GetPage(i);
//            ans_root->AddChild(my_unit->MakeXmlUnit(unit_sign + wxString::Format("%d", i)));
//        }
//        return ans_root;
//    }
//
//    void ClearError() {
//        for (int i = 0; i < notebook->GetPageCount(); i++) {
//            U* my_unit = (U*)notebook->GetPage(i);
//            my_unit->ClearError();
//        }
//    }
//
//    void SetUnitSign(const wxString& _u) { unit_sign = _u; }
//    
//private:
//    wxNotebook* notebook;
//    wxString unit_sign;
//    wxString errorloc;
//    wxXmlNode* root;
//    int max_num;    // notebook中页面的最大编号
//
//};
//
///************************************************
//*   PROJECT
//************************************************/
//class Panel_Project
//    :public wxPanel {
//public:
//    Panel_Project(wxWindow* parent);
//    void PreviewFunc(wxCommandEvent& event);
//};
//
///************************************************
//*   BASE INFOMATION
//************************************************/
//class Panel_BaseInfo
//    :public PanelBase {
//public:
//    Panel_BaseInfo(wxWindow* parent);
//    void SetBaseInfo(wxXmlNode* r);
//    void SaveBaseInfo();
//private:
//    // 控件
//    SimpleText* bs_name;
//    MyVersion* bs_version;
//
//    BoolBox* assert_e;
//    BoolBox* debug_e;
//    BoolBox* pagetble_e;
//    BoolBox* wow;
//
//    MyGrid* sys_tc_tp;
//
//    wxXmlNode* name;
//    wxXmlNode* version;
//    wxXmlNode* assert_enable;
//    wxXmlNode* debug_log_enable;
//    wxXmlNode* pagetable_enable;
//    wxXmlNode* buildsys;
//    wxXmlNode* workspace_overview;
//
//    // 函数、变量
//    bool IsLegal();
//    void OnConfirm(const wxCommandEvent& event)override;
//    wxString errorloc;
//};
//
//
//
///************************************************
//*   CHIP
//************************************************/
//class Panel_Chip
//    :public PanelBase {
//public:
//    Panel_Chip(wxWindow* parent);
//    void SetChip(wxXmlNode* root);
//
//
//private:
//    SimpleText* c_platform;
//    SimpleText* c_class;
//    MyChoice* c_name;
//    MyCoprocessor* c_coprocessor;
//
//    wxStaticBoxSizer* stbox_config;
//
//
//    bool IsLegal();
//    void OnPreview(const wxCommandEvent& event)override;
//    void OnConfirm(const wxCommandEvent& event)override;
//
//    void SaveChip();
//    wxXmlNode* chip;
//    std::map<wxString, SimpleText*>configs;
//
//};
//
//
///************************************************
//*   SHMEM
//************************************************/
//class Shmem
//    :public PanelBase {
//public:
//    Shmem(wxWindow* parent);
//    void SetShmem(wxXmlNode* root);
//private:
//    void OnConfirm(const wxCommandEvent& event)override;
//    void OnPreview(const wxCommandEvent& event)override;
//    Unit_Manager<Mem_Unit_Test>* mem_units;
//    wxXmlNode* shmem;
//    wxString errorloc;
//};
//
//
//
///************************************************
//*   KERNEL
//************************************************/
//class Panel_Kernel
//    :public PanelBase {
//public:
//    Panel_Kernel(wxWindow* parent);
//    void SetKernel(wxXmlNode* root);
//    void RefreshFunc();
//    MyChoice* choice_kom_order;
//    MyChoice* kern_prio;
//    MyChoice* k_optim;
//
//    BaseSlider* code_base;
//    BaseSlider* data_base;
//
//    SizeSlider* code_size;
//    SizeSlider* data_size;
//    SizeSlider* stack_size;
//    SizeSlider* extra_kom;
//
//
//    BoolBox* full_image;
//    BoolBox* project_overw;
//    BoolBox* hook_src_overw;
//    BoolBox* handler_scr_overw;
//
//    SimpleText* tcr_buildsys;
//    SimpleText* tcr_toolchain;
//    SimpleText* project_output;
//    SimpleText* linker_output;
//    SimpleText* conf_hdr_output;
//    SimpleText* boot_hdr_output;
//    SimpleText* boot_src_output;
//    SimpleText* hook_src_output;
//    SimpleText* handler_src_output;
//
//
//private:
//    bool IsLegal();
//    void OnPreview(const wxCommandEvent& event)override;
//    void OnConfirm(const wxCommandEvent& event)override;
//    void OnRefresh(const wxCommandEvent& event)override;
//
//    void SaveKernel();    
//    
//    wxXmlNode* kernel;
//
//    std::map<wxString, wxCheckBox*>buildsystem_choice;
//    std::map<wxString, wxCheckBox*>toolchain_choice;
//    wxString errorloc;
//};
//
///************************************************
//*   MONITOR
//************************************************/
//class Monitor
//    :public PanelBase {  // 滚动窗口
//public:
//    Monitor(wxWindow* parent);
//    void SetMonitor(wxXmlNode* root);
//    void RefreshFunc();
//
//    SizeSlider* code_size;
//    SizeSlider* data_size;
//    SizeSlider* init_stack_size;
//    SizeSlider* sftd_stack_size;
//    SizeSlider* vmmd_stack_size;
//
//    MyVirtPrio* virt_prio;
//    MyVirtPrio* virt_event; // 判断条件和virt_prio一样，只是不要超过1024
//    MyVirtMap* virt_map;
//
//    MyChoice* c_optim;
//
//    BoolBox* project_overw;
//    BoolBox* hook_src_overw;
//
//    MyZ_GE_Zero* extra_captbl;
//
//    SimpleText* c_buildsys;
//    SimpleText* c_toolchain;
//    SimpleText* project_output;
//    SimpleText* linker_output;
//    SimpleText* conf_hdr_output;
//    SimpleText* boot_hdr_output;
//    SimpleText* boot_src_output;
//    SimpleText* hook_src_output;
//
//private:
//    bool IsLegal();
//    void OnRefresh(const wxCommandEvent& event)override;
//    void OnConfirm(const wxCommandEvent& event)override;
//    void OnPreview(const wxCommandEvent& event)override;
//    void SaveMonitor();
//    wxXmlNode* monitor;
//    wxString errorloc;
//};
//
///************************************************
//*   Native
//************************************************/
//class Panel_Native
//    :public wxScrolledWindow {  // 滚动窗口
//public:
//    Panel_Native(wxWindow* parent);
//    void SetNative(wxXmlNode* root);
//
//    wxButton* btn_preview;
//    wxBoxSizer* box;
//private:
//    void PreviewFunc(wxCommandEvent& event);
//    wxXmlNode* native;
//};
//
//
//
///************************************************
//*   NATIVE  base_info
//************************************************/
//class Native_Base_Info
//    :public wxScrolledWindow {
//public:
//    Native_Base_Info(wxWindow* parent,
//        const int& _counter,
//        const wxString& _errorloc = wxEmptyString);
//    void SetNativeBaseInfo(wxXmlNode* _node);
//    //wxString GetBaseInfoName();
//
//    wxStaticText* title;
//    SimpleText* n_name;
//    SimpleText* n_extra_captbl;
//    MyCoprocessor* n_coprocessor;
//    SimpleText* n_buildsys;
//    SimpleText* n_toolchain;
//    MyChoice* n_optimi;
//
//    SimpleText* project_output;
//    BoolBox* project_overw;
//    //wxRadioButton* project_overw_yes;
//    //wxRadioButton* project_overw_no;
//    SimpleText* linker_output;
//    SimpleText* main_hdr_output;
//    SimpleText* main_src_output;
//    SimpleText* entry_src_output;
//    BoolBox* entry_src_overw;
//    //wxRadioButton* entry_src_overw_yes;
//    //wxRadioButton* entry_src_overw_no;
//    wxBoxSizer* box;
//    int counter;
//
//    // 保存设置到node
//    bool IsLegal();
//    void SaveNativeBaseInfo();
//    void ConfirmFunc();
//
//    void ClearError();
//    wxXmlNode* GetNode()const { return node; }
//private:
//    void OnConfirm(const wxCommandEvent& event);
//    void OnRefresh(const wxCommandEvent& event);
//    void RefreshFunc();
//    wxXmlNode* node;    // 其实native frame存过了，这个就无所谓了
//    wxString errorloc;
//};
//
//
///************************************************
//*   NATIVE  Frame
//************************************************/
//class Native_Frame
//    :public wxFrame {
//public:
//    Native_Frame(wxWindow* parent, wxXmlNode*_root, wxTreeItemId _tree_item_id, const wxString& _errorloc = wxEmptyString);
//    wxXmlNode* GetRoot()const { return root; }
//    //wxString GetBaseInfoName() { return base_info->GetBaseInfoName(); }
//    bool ItsMe(Native_Base_Info* tar) { return tar == base_info; }
//    void ClearError();
//    void TotConfirmFunc();
//private:
//    void OnClose(const wxCloseEvent& event);
//    void OnTotConfirm(const wxCommandEvent& event);
//    wxNotebook* nv_notebook;
//    Native_Base_Info* base_info;
//    Unit_Manager<Mem_Unit_Test>* mem_unit;
//    Unit_Manager<Shmem_Unit_Test>* shmem_unit;
//    Unit_Manager<Thread_Unit_Test>* thread_unit;
//    Unit_Manager<Invocation_Unit_Test>* invocation_unit;
//    Unit_Manager<Port_Unit_Test>* port_unit;
//    Unit_Manager<Receive_Unit_Test>* receive_unit;
//    Unit_Manager<Send_Unit_Test>* send_unit;
//    Unit_Manager<Vector_Unit_Test>* vector_unit;
//    Unit_Manager<Kfunc_Unit_Test>* kfunc_unit;
//    wxString errorloc;
//    wxXmlNode* root;
//    wxTreeItemId tree_item_id;
//
//};
//
//
//
///************************************************
//*   VIRTUAL
//************************************************/
//class Virtual_Machine
//    :public wxScrolledWindow {  // 滚动窗口
//public:
//    Virtual_Machine(wxWindow* parent);
//    void SetVirtualMachine(wxXmlNode* root);
//
//    wxButton* btn_preview;
//    wxBoxSizer* box;
//    //int vm_count;
//private:
//    void PreviewFunc(wxCommandEvent& event);
//    wxXmlNode* virtual_machine;
//
//};
//
///************************************************
//*   VIRTUAL base_info
//************************************************/
//class Virtual_Machine_Base_Info
//    :public wxScrolledWindow {
//public:
//    Virtual_Machine_Base_Info(wxWindow* parent,
//        const int& _counter,
//        const wxString& _errorloc = wxEmptyString);
//    void SetVirMachineBaseInfo(wxXmlNode* _node);
//
//    //wxString GetBaseInfoName();
//
//    wxStaticText* title;
//
//    SimpleText* v_name;
//    MyZ_GE_Zero* v_extra_captbl;
//    MyCoprocessor* v_coprocessor;
//
//    SizeSlider* v_vs_size;
//    SizeSlider* v_us_size;
//
//    MyVirtualPrio* v_priority;
//    MyZ_G_Zero* v_timesslice;
//
//    MyZ_G_Zero* v_period;
//    MyZ_GE_Zero* v_wachdog;
//    MyZ_GE_Zero* v_vnum;
//    SimpleText* v_gtype;
//    SimpleText* v_buildsys;
//    SimpleText* v_toolchain;
//    MyChoice* v_optimi;
//
//    SimpleText* project_output;
//    BoolBox* project_overw;
//    //wxRadioButton* project_overw_yes;
//    //wxRadioButton* project_overw_no;
//    SimpleText* linker_output;
//    SimpleText* main_hdr_output;
//    SimpleText* main_src_output;
//    SimpleText* vir_hdr_output;
//    BoolBox* vir_hdr_overw;
//    //wxRadioButton* vir_hdr_overw_yes;
//    //wxRadioButton* vir_hdr_overw_no;
//    SimpleText* vir_src_output;
//    BoolBox* vir_src_overw;
//    //wxRadioButton* vir_src_overw_yes;
//    //wxRadioButton* vir_src_overw_no;
//
//
//    wxBoxSizer* box;
//    int counter;
//
//    bool IsLegal();
//    void SaveVirMachineBaseInfo();
//    void ConfirmFunc();
//
//    void ClearError();
//private:
//    void OnConfirm(const wxCommandEvent& event);
//    void OnRefresh(const wxCommandEvent& event);
//    void RefreshFunc();
//    wxXmlNode* node;
//    wxString errorloc;
//
//};
//
///************************************************
//*   VIRTUAL Frame
//************************************************/
//class Virtual_Machine_Frame
//    :public wxFrame {
//public:
//    Virtual_Machine_Frame(wxWindow* parent, wxXmlNode* _root, const wxString& _errorloc = wxEmptyString);
//    wxXmlNode* GetRoot()const { return root; }
//    bool ItsMe(Virtual_Machine_Base_Info* tar)const { return tar == base_info; }
//    void ClearError();
//    void TotConfirmFunc();
//private:
//    void OnClose(const wxCloseEvent& event);
//    void OnTotConfirm(const wxCommandEvent& event);
//    wxNotebook* vm_notebook;
//    Virtual_Machine_Base_Info* base_info;
//    Unit_Manager<Mem_Unit_Test>* mem_unit;
//    Unit_Manager<Shmem_Unit_Test>* shmem_unit;
//    Unit_Manager<Send_Unit_Test>* send_unit;
//    Unit_Manager<Kfunc_Unit_Test>* kfunc_unit;
//    wxString errorloc;
//    wxXmlNode* root;
//};
