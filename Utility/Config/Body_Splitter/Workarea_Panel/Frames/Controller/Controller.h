#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include <unordered_map>

/*************************************************
*   ControllerBase
*   控件的基类
*************************************************/
class ControllerBase
    :public wxPanel {
public:
    ControllerBase(
        wxWindow* parent,
        const wxString& _title = wxEmptyString,
        const wxString& _intr = wxEmptyString,
        const wxString& _errorloc = wxEmptyString,
        std::function<void()>_func = nullptr,
        const bool& _set_tip = false,
        const wxString& _tip_cont = wxEmptyString);

    void CorrectSet();  // 删除错误
    void IncorrectSet(const wxString& info);    // 添加错误

    // 判断控件内容是否合法
    virtual bool IsLegal() { return false; }
    virtual bool IsLegal(const wxString& str) { return false; }
    virtual bool IsLegal(const int& para) { return false; }
    wxBoxSizer* box;
private:
    int uni_id; // 每个控件的唯一ID，注册一个错误
    wxString errorloc;  // 错误发生的位置
    std::function<void()>func;  // 跳转页面和函数

    wxStaticText* title;
    wxStaticBitmap* image_tip;

};


/*************************************************
*   SimpleText
*   简单的文本框
*************************************************/
class SimpleText
    :public ControllerBase {
public:
    SimpleText(
        wxWindow* parent,
        const wxString& _title = wxEmptyString,
        const wxString& _describe = wxEmptyString,
        const wxString& _content = wxEmptyString,
        const long& _style = 0L,
        const wxString& _errorloc = wxEmptyString,
        std::function<void()>_func = nullptr,
        const bool& _set_tip = false,
        const wxString& _tip_cont = wxEmptyString);

    // 非空字符串 且 是合法的C标识符
    bool IsLegal()override;
    void SetValue(const wxString& str) { 
        str == wxEmptyString ? text->SetValue(default_content) : text->SetValue(str);
    }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
    wxString default_content;
};


/*************************************************
*   BoolBox 
*   选择栏
*************************************************/
class BoolBox
    :public ControllerBase {
public:
    BoolBox(wxWindow* parent, 
        wxString title = wxEmptyString, 
        wxString describe = wxEmptyString);
    // 不需要进行检查
    bool IsLegal()override { return true; }
    void SetValue(const wxString& t) { 
        t == "Yes" ? yes->SetValue(true) : no->SetValue(true); 
    }
    wxString GetStringValue() {
        if (yes->GetValue())
            return "Yes";
        return "No";
    }
private:
    wxRadioButton* yes;
    wxRadioButton* no;
};


/*************************************************
*   BaseSlider
*   基址滑块
*************************************************/
class BaseSlider
    :public ControllerBase {
public:
    BaseSlider(
        wxWindow* parent,
        const wxString& name = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr,
        const bool& _set_tip = false,
        const wxString& _tip_cont = wxEmptyString);
    // 检查是否为Auto 或者非空的十六进制数
    bool IsLegal()override;
    void SetPosition(const int& pos);
    void SetValue(const wxString& str);
    wxString GetValue() { return content->GetValue(); }
private:
    wxSlider* slider;
    wxStaticText* displayText;
    void OnSliderUpdate(const wxCommandEvent& event);
    void OnFillIn(const wxCommandEvent& event) {
        this->content->SetValue(this->hex_content);
    }
    void OnAuto(const wxCommandEvent& event) {
        this->content->SetValue("Auto");
    }

    wxString hex_content;
    wxTextCtrl* content;
};


/*************************************************
*   SizeSlider
*   空间滑块
*************************************************/
class SizeSlider
    :public ControllerBase {
public:
    SizeSlider(
        wxWindow* parent,
        const wxString& name = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr);

    // 检查是否为Auto 或者非空的十六进制数
    bool IsLegal()override;
    void SetPosition(const int& pos);
    void SetValue(const wxString& str);
    wxString GetValue() { return content->GetValue(); }
private:
    wxSlider* slider;
    wxStaticText* displayText;
    void OnSliderUpdate(wxCommandEvent& event);
    void OnFillIn(const wxCommandEvent& event);

    wxString hex_content;
    wxTextCtrl* content;
};


/*************************************************
*   MyVersion
*   版本号
*************************************************/
class MyVersion
    :public ControllerBase {
public:
    MyVersion(wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()>_func = nullptr);
    // 非负数整数 或 非负小数
    bool IsLegal()override;
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;

};


/*************************************************
*   MyAlign
*   对齐设置
*************************************************/
class MyAlign
    :public ControllerBase {
public:
    MyAlign(
        wxWindow* parent,
        const wxString& _title = wxEmptyString,
        const wxString& _describe = wxEmptyString,
        const wxString& _errorloc = wxEmptyString,
        std::function<void()>func = nullptr,
        const bool& set_tip = false,
        const wxString& tip_cont = wxEmptyString);
    // 判断是否合法
    bool IsLegal(const wxString& base)override;
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
    void OnFillAuto(const wxCommandEvent& event) {
        text->SetValue("Auto");
    }
};


/*************************************************
*   MyParameter
*   参数
*************************************************/
class MyParameter
    :public ControllerBase {
public:
    MyParameter(wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr,
        const bool& set_tip = false,
        const wxString& tip_cont = wxEmptyString);
    // 合法的十六进制数，或空字符串
    bool IsLegal()override;
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};


/*************************************************
*   MyThreadPrio
*   线程优先级
*************************************************/
class MyThreadPrio
    :public ControllerBase {
public:
    MyThreadPrio(wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr,
        const bool& set_tip = false,
        const wxString& tip_cont = wxEmptyString);
    // 十进制非负整数，且有范围限制
    bool IsLegal(const int& kern_prio)override;
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};


/*************************************************
*   MyVirtualPrio
*   虚拟机优先级
*************************************************/
class MyVirtualPrio
    :public ControllerBase {
public:
    MyVirtualPrio(wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& _errorloc = wxEmptyString,
        std::function<void()>func = nullptr,
        const bool& set_tip = false,
        const wxString& tip_cont = wxEmptyString);
    // 十进制非负整数，且有范围限制
    // 但是没有找到所谓“在虚拟机优先级数”作为限制
    bool IsLegal(const int& virt_prio);
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};


/*************************************************
*   MyZ_G_Zero
*   大于零的整数
*************************************************/
class MyZ_G_Zero
    :public ControllerBase {
public:
    MyZ_G_Zero(wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()> func = nullptr,
        const bool& set_tip = false,
        const wxString& tip_cont = wxEmptyString);
    // 正整数
    bool IsLegal();
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};


/*************************************************
*   MyZ_GE_Zero
*   大于等于零的整数
*************************************************/
class MyZ_GE_Zero
    :public ControllerBase {
public:
    MyZ_GE_Zero(
        wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr,
        const bool& set_tip = false,
        const wxString& tip_cont = wxEmptyString);
    // 非负整数
    bool IsLegal();
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};


/*************************************************
*   MyGrid
*   buildsys toolchain guest选择表
*************************************************/
class MyGrid
    :public ControllerBase {
public:
    MyGrid(wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()>_func = nullptr);
    void AppendOneRows(const wxArrayString& arr);
    void OnCellValueChanged(const wxGridEvent& event);
    void MatchFirst2nd(const wxArrayString& arr);
    void Clear();
    wxArrayString GetValue();
    bool IsLegal() override;

private:
    int max_row = 1;
    wxGrid* grid;
};


/*************************************************
*   MyVirtMap

*************************************************/
class MyVirtMap
    :public ControllerBase {
public:
    MyVirtMap(
        wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr,
        const bool& set_tip = false,
        const wxString& tip = wxEmptyString);
    // 十进制非负整数，大于等于virt_event，启用虚拟机时非零
    bool IsLegal(const int& virt_event)override;
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};


/*************************************************
*   MyVirtPrio

*************************************************/
class MyVirtPrio
    :public ControllerBase {
public:
    MyVirtPrio(
        wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        const bool& hide_virt_mes = false,
        std::function<void()>func = nullptr,
        const bool& set_tip = false,
        const wxString& tip_cont = wxEmptyString);
    // 检查是否是十进制非负整数
    bool IsLegal()override;
    void SetVmCount(const int& n) {
        text_vm_count->SetLabel(wxString::Format("当前虚拟机数 %d", n));
    }
    void FillChoice(wxArrayString arr);
    int FindValue(const wxString& tar);
    void SetSelection(const int& cur) { choice->SetSelection(cur); }
    wxString GetStringSelection() { return choice->GetStringSelection(); }

private:
    wxChoice* choice;
    //wxTextCtrl* text;
    wxStaticText* text_vm_count;

};


/*************************************************
*   MyCoprocessor
*   协处理器选择栏
*************************************************/
class MyCoprocessor
    :public ControllerBase {
public:
    MyCoprocessor(
        wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()> func = nullptr);
    // 至少选择一项非None 或只选择None
    bool IsLegal()override;
    void FillAllItem(wxArrayString arr);
    void SetValue(const wxArrayString& arr);
    wxString GetValue();

private:
    std::unordered_map<wxString, wxCheckBox*>* mp;
    wxBoxSizer* checkbox;
    //wxStaticBoxSizer* cur_checkbox;
};


/*************************************************
*   MyAttribute
*   
*************************************************/
class MyAttribute
    :public ControllerBase {
public:
    MyAttribute(
        wxWindow* parent,
        wxWindowID id,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& _errorloc = wxEmptyString,
        std::function<void()>func = nullptr);
    // 检查不能为空
    bool IsLegal()override;
    void SetValue(const wxString& str);
    wxString GetValue() { return text->GetValue(); }
private:
    void OnRadioButtonClicked(const wxCommandEvent& event);
    wxTextCtrl* text;
    wxCheckBox* Readable;
    wxCheckBox* Writable;
    wxCheckBox* Executable;
    wxCheckBox* Bufferable;
    wxCheckBox* Cacheable;
    wxCheckBox* Static;
    std::unordered_map <char, wxCheckBox*>* mp;
};

/*************************************************
*   MyChoice
*   选择栏
*************************************************/
class MyChoice
    :public ControllerBase {
public:
    MyChoice(
        wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString);
    // 无需检查
    bool IsLegal()override { return true; }
    void FillChoice(wxArrayString arr);
    void SetSelection(const int& cur) { choice->SetSelection(cur); }
    int FindValue(const wxString& tar);
    wxString GetStringSelection() { return choice->GetStringSelection(); }
private:
    wxChoice* choice;
};


/*************************************************
*   NotEmpty
*   Chip中的config，保证非空
*************************************************/
class NotEmpty
    :public ControllerBase {
public:
    NotEmpty(wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr,
        const bool& set_tip = false,
        const wxString& tip_cont = wxEmptyString);
    // 非空
    bool IsLegal()override;
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};
