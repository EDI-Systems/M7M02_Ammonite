#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include <unordered_map>

/*************************************************
*   ControllerBase
*   �ؼ��Ļ���
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

    void CorrectSet();  // ɾ������
    void IncorrectSet(const wxString& info);    // ��Ӵ���

    // �жϿؼ������Ƿ�Ϸ�
    virtual bool IsLegal() { return false; }
    virtual bool IsLegal(const wxString& str) { return false; }
    virtual bool IsLegal(const int& para) { return false; }
    wxBoxSizer* box;
private:
    int uni_id; // ÿ���ؼ���ΨһID��ע��һ������
    wxString errorloc;  // ��������λ��
    std::function<void()>func;  // ��תҳ��ͺ���

    wxStaticText* title;
    wxStaticBitmap* image_tip;

};


/*************************************************
*   SimpleText
*   �򵥵��ı���
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

    // �ǿ��ַ��� �� �ǺϷ���C��ʶ��
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
*   ѡ����
*************************************************/
class BoolBox
    :public ControllerBase {
public:
    BoolBox(wxWindow* parent, 
        wxString title = wxEmptyString, 
        wxString describe = wxEmptyString);
    // ����Ҫ���м��
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
*   ��ַ����
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
    // ����Ƿ�ΪAuto ���߷ǿյ�ʮ��������
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
*   �ռ们��
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

    // ����Ƿ�ΪAuto ���߷ǿյ�ʮ��������
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
*   �汾��
*************************************************/
class MyVersion
    :public ControllerBase {
public:
    MyVersion(wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString,
        std::function<void()>_func = nullptr);
    // �Ǹ������� �� �Ǹ�С��
    bool IsLegal()override;
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;

};


/*************************************************
*   MyAlign
*   ��������
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
    // �ж��Ƿ�Ϸ�
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
*   ����
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
    // �Ϸ���ʮ��������������ַ���
    bool IsLegal()override;
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};


/*************************************************
*   MyThreadPrio
*   �߳����ȼ�
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
    // ʮ���ƷǸ����������з�Χ����
    bool IsLegal(const int& kern_prio)override;
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};


/*************************************************
*   MyVirtualPrio
*   ��������ȼ�
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
    // ʮ���ƷǸ����������з�Χ����
    // ����û���ҵ���ν������������ȼ�������Ϊ����
    bool IsLegal(const int& virt_prio);
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};


/*************************************************
*   MyZ_G_Zero
*   �����������
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
    // ������
    bool IsLegal();
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};


/*************************************************
*   MyZ_GE_Zero
*   ���ڵ����������
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
    // �Ǹ�����
    bool IsLegal();
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};


/*************************************************
*   MyGrid
*   buildsys toolchain guestѡ���
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
    // ʮ���ƷǸ����������ڵ���virt_event�����������ʱ����
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
    // ����Ƿ���ʮ���ƷǸ�����
    bool IsLegal()override;
    void SetVmCount(const int& n) {
        text_vm_count->SetLabel(wxString::Format("��ǰ������� %d", n));
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
*   Э������ѡ����
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
    // ����ѡ��һ���None ��ֻѡ��None
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
    // ��鲻��Ϊ��
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
*   ѡ����
*************************************************/
class MyChoice
    :public ControllerBase {
public:
    MyChoice(
        wxWindow* parent,
        const wxString& title = wxEmptyString,
        const wxString& describe = wxEmptyString,
        const wxString& errorloc = wxEmptyString);
    // ������
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
*   Chip�е�config����֤�ǿ�
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
    // �ǿ�
    bool IsLegal()override;
    void SetValue(const wxString& str) { text->SetValue(str); }
    wxString GetValue() { return text->GetValue(); }
private:
    wxTextCtrl* text;
};
