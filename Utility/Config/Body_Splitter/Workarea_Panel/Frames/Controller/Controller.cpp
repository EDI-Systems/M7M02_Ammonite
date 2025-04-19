#include <wx/regex.h>

#include "Controller.h"
#include "../../../../main.h"

/*************************************************
*   ControllerBase
*   �ؼ��Ļ���
*************************************************/
ControllerBase::ControllerBase(
    wxWindow* parent,
    const wxString& _title,
    const wxString& _intr,
    const wxString& _errorloc,
    std::function<void()>_func,
    const bool& _set_tip,
    const wxString& _tip_cont)
    :wxPanel(parent, wxID_ANY), func(_func), errorloc(_errorloc + _title) {
    
    
    uni_id = std::hash<ControllerBase*>()(this);

    box = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

    // ���ñ���
    title = new wxStaticText(this, wxID_ANY, _title);
    wxFont title_font = title->GetFont();
    title_font.SetPointSize(10);
    title->SetFont(title_font);
    if (_title == wxEmptyString)
        title->Hide();
    hbox->Add(title, 0, wxLEFT | wxALIGN_BOTTOM, 15);

    // ������ʾ
    if (_set_tip) {
        wxBitmap bmp(wxT("Image\\tip.bmp"), wxBITMAP_TYPE_BMP); // ����ͼƬ
        image_tip = new wxStaticBitmap(this, wxID_ANY, bmp);    // ���� wxStaticBitmap �����ʾͼƬ
        image_tip->SetToolTip(_tip_cont);
        hbox->Add(image_tip, 0, wxBOTTOM | wxLEFT | wxTOP | wxALIGN_BOTTOM, 5);
    }

    // ����ע��
    wxStaticText* introduction = new wxStaticText(this, wxID_ANY, _intr);
    introduction->SetForegroundColour(RGB(128, 128, 128));
    wxFont intr_font = introduction->GetFont();
    intr_font.SetPointSize(8);
    introduction->SetFont(intr_font);
    if (_intr == wxEmptyString)
        introduction->Hide();
    hbox->Add(introduction, 0, wxLEFT | wxTOP | wxRIGHT | wxALIGN_BOTTOM, 5);

    box->Add(hbox, 0, wxTOP, 5);

    SetSizer(box);
    box->Layout();
}

void ControllerBase::CorrectSet() {
    // ��������ɫ��Ϊ��ɫ
    title->SetForegroundColour(*wxBLACK);
    // ����ID���Ӵ�����Ϣ���Ƴ�
    Main::proj_info->errorInfo->RemoveInfo(uni_id);
    // ˢ�´�����־
    Main::log_panel2->RefreshErrorLog();
    // ˢ�¿ؼ� ��ʾ��ɫ����
    Refresh();
}

void ControllerBase::IncorrectSet(const wxString& info) {
    // ��������ɫ��Ϊ��ɫ
    title->SetForegroundColour(*wxRED);

    // ��ӵ�������Ϣ��ʾ��
    if (func) 
        Main::proj_info->errorInfo->AddInfo(uni_id, errorloc, info, func);
    else 
        wxMessageBox(wxString::Format("%s", info), "û��������ת����", wxICON_WARNING);
    // ˢ�´�����־
    Main::log_panel2->RefreshErrorLog();
    // ˢ�¿ؼ� ��ʾ��ɫ����
    Refresh();
}

/*************************************************
*   SimpleText
*   �򵥵��ı���
*************************************************/
SimpleText::SimpleText(
    wxWindow* parent, 
    const wxString& _title, 
    const wxString& _describe,
    const wxString& _content,
    const long& _style,
    const wxString& _errorloc,
    std::function<void()>_func,
    const bool& _set_tip,
    const wxString& _tip_cont)
    :ControllerBase(parent, _title, _describe, _errorloc, _func, _set_tip, _tip_cont) {
    
    //func = _func;
    default_content = _content;
    text = new wxTextCtrl(this, wxID_ANY, _content, wxDefaultPosition, wxDefaultSize, _style);
    box->Add(text, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 15);
    //SetSizer(box);
}

bool SimpleText::IsLegal() {
    // ���Ȳ���Ϊ��
    wxString tar = text->GetValue();
    tar = tar.Upper();
    wxRegEx regex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    if (!regex.Matches(tar)) {
        IncorrectSet("���ַ��� �� ����һ���Ϸ�C��ʶ��");
        return false;
    }
    CorrectSet();
    return true;
}

/*************************************************
*   BoolBox
*   ѡ����
*************************************************/
BoolBox::BoolBox(wxWindow* parent, wxString _title, wxString _intr)
    :ControllerBase(parent, _title, _intr) {

    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
    yes = new wxRadioButton(this, wxID_ANY, "��", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);  // ��ť�����ʼ
    no = new wxRadioButton(this, wxID_ANY, "��");
    hbox->Add(10, 0);
    hbox->Add(yes, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 5);
    hbox->Add(no, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 5);

    box->Add(hbox, 0, wxEXPAND);
    SetSizer(box);
    box->Layout();
}

/*************************************************
*   MyVersion
*   ѡ����
*************************************************/
MyVersion::MyVersion(
    wxWindow* parent,
    const wxString& title, 
    const wxString& describe, 
    const wxString& errorloc,
    std::function<void()>_func)
    :ControllerBase(parent, title, describe, errorloc, _func) {

    text = new wxTextCtrl(this, wxID_ANY);
    box->Add(text, 0, wxLEFT | wxRIGHT | wxEXPAND, 15);

    SetSizer(box);
    Layout();
}

bool MyVersion::IsLegal() {
    // �汾����Ҫ��һ���Ǹ���
    wxString str = text->GetValue();
    int value = wxAtoi(str);
    wxRegEx regex("^\\d+(\\.\\d+)?$");
    if (!regex.Matches(str)) {
        IncorrectSet("�汾�Ų��Ϸ�");
        return false;
    }
    CorrectSet();
    return true;
}


/*************************************************
*   MyAlign
*   ��������
*************************************************/
MyAlign::MyAlign(
    wxWindow* parent,
    const wxString& _title,
    const wxString& _intr,
    const wxString& _errorloc,
    std::function<void()>func,
    const bool& set_tip,
    const wxString& tip_cont)
    :ControllerBase(parent, _title, _intr, _errorloc, func, set_tip, tip_cont) {

    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
    text = new wxTextCtrl(this, wxID_ANY);
    wxButton* fill_auto = new wxButton(this, wxID_ANY, "�Զ�");
    hbox->Add(text, 1, wxLEFT, 15);
    hbox->Add(2, 0);
    hbox->Add(fill_auto, 0, wxRIGHT, 15);

    box->Add(hbox, 0, wxEXPAND);

    SetSizer(box);
    Layout();

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyAlign::OnFillAuto, this, fill_auto->GetId());
}


bool MyAlign::IsLegal(const wxString& base) {
    wxString str = text->GetValue();
    // align�����ǺϷ���ʮ���������� Auto������Ҫ���㡿
    wxRegEx regex("^-?\\d+(\\.\\d+)?$");
    if (!regex.Matches(str) && str != "Auto") {
        IncorrectSet("��дAuto����һ���Ϸ���ʮ������");
        return false;
    }

    // �λ�ַΪAutoʱ����alignû�����ơ����Ҫ���㡿
    if (base == "Auto") {
        CorrectSet();
        return true;
    }
    else {
        if (str != "Auto") {
            IncorrectSet("�λ�ַ�ǹ̶�ֵ��Align�������д Auto");
            return false;
        }
    }
    CorrectSet();
    return true;
}


/*************************************************
*   BaseSlider
*   ��ַ����
*************************************************/
BaseSlider::BaseSlider(
    wxWindow* parent,
    const wxString& name,
    const wxString& describe,
    const wxString& errorloc,
    std::function<void()>func,
    const bool& _set_tip,
    const wxString& _tip_cont)
    :ControllerBase(parent, name, describe, errorloc, func) {

    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    content = new wxTextCtrl(this, wxID_ANY, "0x1");
    hex_content = "0x1";
    wxButton* fill_auto = new wxButton(this, wxID_ANY, "�Զ�");
    wxButton* fill_info = new wxButton(this, wxID_ANY, "����");
    hbox1->Add(content, 1, wxLEFT, 15);
    hbox1->Add(fill_auto, 0, wxLEFT, 3);
    hbox1->Add(fill_info, 0, wxLEFT, 3);
    hbox1->Add(15, 0);
    box->Add(hbox1, 0, wxEXPAND);

    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    // ����һ������ؼ�����Χ�� 0 �� 3 (��Ӧ�ĸ�ѡ��)
    slider = new wxSlider(this, wxID_ANY, 0, 0, 39, wxPoint(0, 0), wxDefaultSize, wxSL_AUTOTICKS);
    slider->SetTickFreq(2);// ����ÿ10����λ��ʾһ���̶�

    displayText = new wxStaticText(this, wxID_ANY, "0x1");
    hbox2->Add(slider, 2, wxLEFT, 10);
    hbox2->Add(displayText, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, 15);

    box->Add(hbox2, 0, wxEXPAND);

    slider->Bind(wxEVT_SLIDER, &BaseSlider::OnSliderUpdate, this);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &BaseSlider::OnFillIn, this, fill_info->GetId());
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &BaseSlider::OnAuto, this, fill_auto->GetId());

}

void BaseSlider::SetPosition(const int& pos) {
    slider->SetValue(pos);
}

void BaseSlider::SetValue(const wxString& str) {
    if (str == wxEmptyString)
        return;
    content->SetValue(str);;
}
bool BaseSlider::IsLegal() {
    wxString str = content->GetValue();
    if (str == "Auto") {
        CorrectSet();
        return true;
    }
    str = str.Lower();
    wxRegEx regex("^0x[0-9a-fA-F]+$|^[0-9a-fA-F]+$");
    if (!regex.Matches(str)) {
        IncorrectSet("���ַ��� �� ���ǺϷ���ʮ��������");
        return false;
    }
    if (str.starts_with("0x"))
        str = "0x" + str.SubString(2, str.Length() - 1).Upper();
    else
        str = "0x" + str.Upper();
    SetValue(str);
    CorrectSet();
    return true;
}

void BaseSlider::OnSliderUpdate(const wxCommandEvent& event) {
    unsigned int value = slider->GetValue();
    unsigned int the_decade = value / 10;
    unsigned int the_unit = value % 10;
    wxString size_unit;
    switch (the_decade) {
    case 0:
        size_unit = "B";
        break;
    case 1:
        size_unit = "KB";
        break;
    case 2:
        size_unit = "MB";
        break;
    case 3:
        size_unit = "GB";
        break;
    default:
        break;
    }
    value = 1 << the_unit;
    this->hex_content = myfunc::toHex(value, size_unit);
    displayText->SetLabel(wxString::Format("%s", hex_content)); // �����ı���ʾ
}

/*************************************************
*   SizeSlider
*   �ռ们��
*************************************************/
SizeSlider::SizeSlider(
    wxWindow* parent,
    const wxString& name,
    const wxString& describe,
    const wxString& errorloc,
    std::function<void()>func)
    :ControllerBase(parent, name, describe, errorloc, func) {
    //this->SetBackgroundColour(*wxGREEN);

    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    content = new wxTextCtrl(this, wxID_ANY, "0x1");
    wxButton* fill_info = new wxButton(this, wxID_ANY, "����");
    hex_content = "0x1";
    hbox1->Add(content, 1, wxLEFT, 15);
    hbox1->Add(fill_info, 0, wxLEFT, 2);
    hbox1->Add(15, 0);
    box->Add(hbox1, 0, wxEXPAND);

    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    // ����һ������ؼ�����Χ�� 0 �� 3 (��Ӧ�ĸ�ѡ��)
    slider = new wxSlider(this, wxID_ANY, 0, 0, 39, wxPoint(0, 0), wxDefaultSize, wxSL_AUTOTICKS);
    slider->SetTickFreq(2);// ����ÿ10����λ��ʾһ���̶�
    //slider->SetBackgroundColour(*wxRED);
    //box->Add(slider, 0, wxEXPAND);
    displayText = new wxStaticText(this, wxID_ANY, "1 B(0x1)");

    hbox2->Add(slider, 2, wxLEFT, 10);
    hbox2->Add(displayText, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, 15);

    box->Add(hbox2, 0, wxEXPAND);

    slider->Bind(wxEVT_SLIDER, &SizeSlider::OnSliderUpdate, this);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &SizeSlider::OnFillIn, this, fill_info->GetId());
}

void SizeSlider::SetPosition(const int& pos) {
    slider->SetValue(pos);
}

void SizeSlider::SetValue(const wxString& str) {
    if (str == wxEmptyString)
        return;
    content->SetValue(str);
}


bool SizeSlider::IsLegal() {
    wxString str = content->GetValue();
    str = str.Lower();
    wxRegEx regex("^0x[0-9a-fA-F]+$|^[0-9a-fA-F]+$");
    if (!regex.Matches(str)) {
        IncorrectSet("���ַ��� �� ���ǺϷ���ʮ��������");
        return false;
    }
    if (str.starts_with("0x"))
        str = "0x" + str.SubString(2, str.Length() - 1).Upper();
    else
        str = "0x" + str.Upper();
    SetValue(str);
    CorrectSet();
    return true;
}

void SizeSlider::OnFillIn(const wxCommandEvent& event) {
    this->content->SetValue(this->hex_content);
}

void SizeSlider::OnSliderUpdate(wxCommandEvent& event) {
    unsigned int value = slider->GetValue();
    unsigned int the_decade = value / 10;
    unsigned int the_unit = value % 10;
    wxString size_unit;
    switch (the_decade) {
    case 0:
        size_unit = "B";
        break;
    case 1:
        size_unit = "KB";
        break;
    case 2:
        size_unit = "MB";
        break;
    case 3:
        size_unit = "GB";
        break;
    default:
        break;
    }
    value = 1 << the_unit;
    this->hex_content = myfunc::toHex(value, size_unit);
    displayText->SetLabel(wxString::Format("%d %s(%s)", value, size_unit, hex_content)); // �����ı���ʾ
}


/*************************************************
*   MyParameter
*   ����
*************************************************/
MyParameter::MyParameter(
    wxWindow* parent,
    const wxString& title,
    const wxString& describe,
    const wxString& _errorloc,
    std::function<void()>func,
    const bool& set_tip,
    const wxString& tip_cont)
    :ControllerBase(parent, title, describe, _errorloc, func, set_tip, tip_cont) {

    text = new wxTextCtrl(this, wxID_ANY);
    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);

    SetSizer(box);
    Layout();
}

bool MyParameter::IsLegal() {
    // �汾����Ҫ��һ���Ǹ���
    wxString str = text->GetValue();
    if (str == wxEmptyString) {
        text->SetValue("0x0");
        CorrectSet();
        return true;
    }

    str = str.Lower();
    wxRegEx regex("^0x[0-9a-fA-F]+$|^[0-9a-fA-F]+$");
    if (!regex.Matches(str)) {
        IncorrectSet("���ǺϷ���ʮ��������");
        return false;
    }
    if (str.starts_with("0x"))
        str = "0x" + str.SubString(2, str.Length() - 1).Upper();
    else
        str = "0x" + str.Upper();;
    SetValue(str);
    CorrectSet();
    return true;
}


/*************************************************
*   MyThreadPrio
*   �߳����ȼ�
*************************************************/
MyThreadPrio::MyThreadPrio(
    wxWindow* parent,
    const wxString& title,
    const wxString& describe,
    const wxString& _errorloc,
    std::function<void()>func,
    const bool& set_tip,
    const wxString& tip_cont)
    :ControllerBase(parent, title, describe, _errorloc, func, set_tip, tip_cont) {

    text = new wxTextCtrl(this, wxID_ANY);
    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);

    SetSizer(box);
    Layout();
}

bool MyThreadPrio::IsLegal(const int& kern_prio) {
    wxString str = text->GetValue();
    wxRegEx regex("^\\d+$");
    if (!regex.Matches(str)) {
        IncorrectSet("���ǺϷ���ʮ���ƷǸ�����");
        return false;
    }
    int value = wxAtoi(str);
    if (!(5 <= value && value <= kern_prio - 2)) {
        IncorrectSet("���ȼ����ڹ涨��Χ��");
        return false;
    }
    CorrectSet();
    return true;
}


/*************************************************
*   MyVirtualPrio
*   ��������ȼ�
*************************************************/
MyVirtualPrio::MyVirtualPrio(
    wxWindow* parent,
    const wxString& title,
    const wxString& describe,
    const wxString& _errorloc,
    std::function<void()>func,
    const bool& set_tip,
    const wxString& tip_cont)
    :ControllerBase(parent, title, describe, _errorloc, func, set_tip, tip_cont) {

    text = new wxTextCtrl(this, wxID_ANY);
    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);

    SetSizer(box);
    Layout();
}

bool MyVirtualPrio::IsLegal(const int& virt_prio) {
    wxString str = text->GetValue();
    wxRegEx regex("^\\d+$");
    if (!regex.Matches(str)) {
        IncorrectSet("���ǺϷ���ʮ������");
        return false;
    }
    int value = wxAtoi(str);
    if (!(0 <= value && value <= virt_prio - 1)) {
        IncorrectSet("���ȼ����ڹ涨��Χ��");
        return false;
    }
    CorrectSet();
    return true;
}


/*************************************************
*   MyZ_G_Zero
*   �����������
*************************************************/
MyZ_G_Zero::MyZ_G_Zero(
    wxWindow* parent,
    const wxString& title,
    const wxString& describe,
    const wxString& errorloc,
    std::function<void()> func,
    const bool& set_tip,
    const wxString& tip_cont)
    :ControllerBase(parent, title, describe, errorloc, func, set_tip, tip_cont) {

    text = new wxTextCtrl(this, wxID_ANY);
    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);

    SetSizer(box);
    Layout();
}

bool MyZ_G_Zero::IsLegal() {
    wxString str = text->GetValue();
    wxRegEx regex("^[1-9]\\d*$");  // ����������ʽ����
    if (!regex.Matches(str)) {
        IncorrectSet("���ǺϷ���������");
        return false;
    }

    CorrectSet();
    return true;
}


/*************************************************
*   MyZ_GE_Zero
*   ���ڵ����������
*************************************************/
MyZ_GE_Zero::MyZ_GE_Zero(
    wxWindow* parent,
    const wxString& title,
    const wxString& describe,
    const wxString& errorloc,
    std::function<void()>func,
    const bool& set_tip,
    const wxString& tip_cont)
    :ControllerBase(parent, title, describe, errorloc, func) {

    text = new wxTextCtrl(this, wxID_ANY);
    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);

    SetSizer(box);
    Layout();
}

bool MyZ_GE_Zero::IsLegal() {
    wxString str = text->GetValue();
    wxRegEx regex("^\\d+$");
    if (!regex.Matches(str)) {
        IncorrectSet("���ǺϷ��ķǸ�����");
        return false;
    }
    CorrectSet();
    return true;
}


/*************************************************
*   MyGrid
*   buildsys toolchain guestѡ���
*************************************************/
MyGrid::MyGrid(wxWindow* parent,
    const wxString& title,
    const wxString& describe,
    const wxString& errorloc,
    std::function<void()>_func)
    :ControllerBase(parent, title, describe, errorloc, _func) {

    grid = new wxGrid(this, wxID_ANY);
    grid->CreateGrid(1, 4);
    grid->SetColSize(0, 30);
    grid->SetCellValue(0, 1, "Buildsystem");
    grid->SetCellValue(0, 2, "ToolChain");
    grid->SetCellValue(0, 3, "Type");
    grid->SetReadOnly(0, 1, true);
    grid->SetReadOnly(0, 2, true);
    grid->SetReadOnly(0, 3, true);
    grid->SetMargins(0, 0);
    grid->SetGridLineColour(wxColour(175, 175, 175));
    grid->EnableDragGridSize(false);

    grid->SetRowLabelSize(2);
    grid->SetColLabelSize(2);

    box->Add(grid, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);

    SetSizer(box);
    Layout();

    Bind(wxEVT_GRID_CELL_CHANGED, &MyGrid::OnCellValueChanged, this);


}
void MyGrid::AppendOneRows(const wxArrayString& arr) {
    grid->AppendRows(1);
    grid->SetCellEditor(max_row, 0, new wxGridCellBoolEditor());
    grid->SetCellRenderer(max_row, 0, new wxGridCellBoolRenderer());
    for (int i = 1; i < 4; i++) {
        grid->SetCellValue(max_row, i, arr[i - 1]);
        grid->SetReadOnly(max_row, i, true);
    }
    ++max_row;
}

void MyGrid::OnCellValueChanged(const wxGridEvent& event) {
    // ��ȡ�¼��е��к���
    int row = event.GetRow();
    int col = event.GetCol();
    //wxMessageBox(wxString::Format("message %d %d", row, col));
    if (grid->GetCellValue(row, col) != wxEmptyString) {
        for (int i = 1; i < max_row; i++) {
            if (row == i)
                continue;
            grid->SetCellValue(i, 0, wxEmptyString);
        }
    }
    else {
        // ѡ��֮������ѡȡ��
        grid->SetCellValue(row, col, "1");
    }
}

bool MyGrid::IsLegal() {
    for (int i = 1; i < max_row; i++) {
        if (grid->GetCellValue(i, 0) != wxEmptyString) {
            CorrectSet();
            return true;
        }
    }
    IncorrectSet("����ѡ��һ���Ϸ������");
    return false;
}

void MyGrid::MatchFirst2nd(const wxArrayString& arr) {
    for (int row = 1; row < max_row; row++) {
        if (grid->GetCellValue(row, 1) == arr[0] &&
            grid->GetCellValue(row, 2) == arr[1] &&
            grid->GetCellValue(row, 3) == arr[2]) {
            grid->SetCellValue(row, 0, "1");
            return;
        }
    }
    wxLogError("δ�ҵ�ƥ���� ������%s", __func__);
    return;
}

wxArrayString MyGrid::GetValue() {
    wxArrayString ans;
    for (int row = 1; row < max_row; row++) {
        if (grid->GetCellValue(row, 0) != wxEmptyString) {
            ans.push_back(grid->GetCellValue(row, 1));
            ans.push_back(grid->GetCellValue(row, 2));
            ans.push_back(grid->GetCellValue(row, 3));
        }
    }
    return ans;
}

void MyGrid::Clear() {
    if (max_row >= 2) {
        grid->DeleteRows(1, max_row - 1);
        max_row = 1;
    }
}


/*************************************************
*   MyVirtMap

*************************************************/
MyVirtMap::MyVirtMap(
    wxWindow* parent,
    const wxString& title,
    const wxString& describe,
    const wxString& errorloc,
    std::function<void()>func,
    const bool& set_tip,
    const wxString& tip)
    :ControllerBase(parent, title, describe, errorloc, func, set_tip, tip) {

    text = new wxTextCtrl(this, wxID_ANY);
    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);

    SetSizer(box);
    Layout();
}

bool MyVirtMap::IsLegal(const int& virt_event) {
    // �Ϸ���ʮ������
    wxString str = text->GetValue();
    int value = wxAtoi(str);
    wxRegEx regex("^\\d+$");
    if (!regex.Matches(str)) {
        IncorrectSet("���ǺϷ���ʮ���ƷǸ�����");
        return false;
    }
    else {
        // �������
        if (value < virt_event) {
            IncorrectSet("���������ӳ������������¼�Դ������");
            return false;
        }
        else if (value == 0 && int(Main::proj_info->rvp->vm_manager->virtual_machine.size()) != 0) {
            
            IncorrectSet("��ʹ�������������� ��ֵ����Ϊ0");
            return false;
        }
    }
    CorrectSet();
    return true;
}


/*************************************************
*   MyVirtPrio

*************************************************/
MyVirtPrio::MyVirtPrio(
    wxWindow* parent,
    const wxString& title,
    const wxString& describe,
    const wxString& errorloc,
    const bool& hide_virt_mes,
    std::function<void()>func,
    const bool& set_tip,
    const wxString& tip_cont)
    :ControllerBase(parent, title, describe, errorloc, func, set_tip, tip_cont) {

    choice = new wxChoice(this, wxID_ANY);
    box->Add(choice, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);

    text_vm_count = new wxStaticText(this, wxID_ANY, "��ǰ������� 0");
    wxFont font = text_vm_count->GetFont();
    font.SetPointSize(7);
    text_vm_count->SetFont(font);
    box->Add(text_vm_count, 0, wxALIGN_RIGHT | wxRIGHT, 15);
    if (hide_virt_mes)
        box->Hide(text_vm_count);

    SetSizer(box);
    Layout();
}

void MyVirtPrio::FillChoice(wxArrayString arr) {
    choice->Clear();
    choice->Append(arr);
    choice->SetSelection(0);
}

int MyVirtPrio::FindValue(const wxString& tar) {
    int size = choice->GetCount();
    for (int i = 0; i < size; i++) {
        if (choice->GetString(i) == tar)
            return i;
    }
    //wxMessageBox("δ�ҵ�Ԥ��ѡ��");
    return 0;
}

bool MyVirtPrio::IsLegal() {
    // �Ϸ���ʮ������
    int value = wxAtoi(choice->GetStringSelection());
    if (value == 0 &&
        int(Main::proj_info->rvp->vm_manager->virtual_machine.size()) != 0) {
        IncorrectSet("��ʹ�������������� ��ֵ����Ϊ0");
        return false;
    }
    CorrectSet();
    return true;
}

/*************************************************
*   MyCoprocessor
*   Э������ѡ����
*************************************************/
MyCoprocessor::MyCoprocessor( 
    wxWindow* parent,
    const wxString& title,
    const wxString& describe,
    const wxString& errorloc,
    std::function<void()> func)
    :ControllerBase(parent, title, describe, errorloc, func) {
    

    checkbox = new wxBoxSizer(wxVERTICAL);
    box->Add(checkbox, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
    mp = new std::unordered_map<wxString, wxCheckBox*>;
}

void MyCoprocessor::FillAllItem(wxArrayString arr) {
    mp->clear();
    checkbox->Clear(true);
    arr = myfunc::MakeUnique(arr);  // ȥ��
    for (auto i : arr) {
        wxCheckBox* wxcheckbox = new wxCheckBox(this, wxID_ANY, i);
        checkbox->Add(wxcheckbox, 0, wxALL, 3);
        mp->insert(std::make_pair(i, wxcheckbox));
    }
    Refresh();
}

void MyCoprocessor::SetValue(const wxArrayString& arr) {
    for (auto i : arr) {
        if (mp->find(i) != mp->end()) {
            mp->find(i)->second->SetValue(true);
        }
    }
}

wxString MyCoprocessor::GetValue() {
    wxString ans;
    for (auto i : *mp) {
        if (i.second->GetValue()) {
            ans += (i.first + ",");
        }
    }
    return ans.SubString(0, ans.length() - 2);
}

bool MyCoprocessor::IsLegal() {
    // �����ѡһ��
    bool select_one = false;
    for (auto i : *mp)
        select_one = i.second->GetValue() || select_one;
    if (!select_one) {
        IncorrectSet("����ѡ��һ�� ������Э��������ѡ�� None");
        return false;
    }
    // ѡ��None����ѡ����
    if (mp->find("None") != mp->end() && mp->find("None")->second->GetValue()) {
        for (auto i : *mp) {
            if (i.first == "None")
                continue;
            if (i.second->GetValue()) {
                IncorrectSet("ѡ�� Noneѡ������ѡ������");
                return false;
            }
        }
    }
    CorrectSet();
    return true;
}


/*************************************************
*   MyAttribute
*
*************************************************/
MyAttribute::MyAttribute(
    wxWindow* parent,
    wxWindowID id,
    const wxString& title,
    const wxString& describe,
    const wxString& _errorloc,
    std::function<void()>func)
    :ControllerBase(parent, title, describe, _errorloc, func) {

    mp = new std::unordered_map<char, wxCheckBox*>;
    text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);

    Readable = new wxCheckBox(this, wxID_ANY, "Readable");
    Writable = new wxCheckBox(this, wxID_ANY, "Writable");
    Executable = new wxCheckBox(this, wxID_ANY, "Executable");
    Bufferable = new wxCheckBox(this, wxID_ANY, "Bufferable");
    Cacheable = new wxCheckBox(this, wxID_ANY, "Cacheable");
    Static = new wxCheckBox(this, wxID_ANY, "Static");
    mp->insert(std::make_pair('R', Readable));
    mp->insert(std::make_pair('W', Writable));
    mp->insert(std::make_pair('X', Executable));
    mp->insert(std::make_pair('B', Bufferable));
    mp->insert(std::make_pair('C', Cacheable));
    mp->insert(std::make_pair('S', Static));

    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
    box->Add(0, 3);
    box->Add(Readable, 0, wxLEFT, 15);
    box->Add(0, 3);
    box->Add(Writable, 0, wxLEFT, 15);
    box->Add(0, 3);
    box->Add(Executable, 0, wxLEFT, 15);
    box->Add(0, 3);
    box->Add(Bufferable, 0, wxLEFT, 15);
    box->Add(0, 3);
    box->Add(Cacheable, 0, wxLEFT, 15);
    box->Add(0, 3);
    box->Add(Static, 0, wxLEFT, 15);

    Readable->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
    Writable->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
    Executable->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
    Bufferable->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
    Cacheable->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
    Static->Bind(wxEVT_CHECKBOX, &MyAttribute::OnRadioButtonClicked, this);
}

void MyAttribute::SetValue(const wxString& str) {
    text->SetValue(str);
    for (auto i : str) {
        if (mp->find(i) != mp->end()) {
            mp->find(i)->second->SetValue(true);
        }
    }
}

bool MyAttribute::IsLegal() {
    if (text->GetValue() == wxEmptyString) {
        IncorrectSet("�ڴ���������ò���Ϊ��");
        return false;
    }
    CorrectSet();
    return true;
}

void MyAttribute::OnRadioButtonClicked(const wxCommandEvent& event) {
    wxString ans;
    wxCheckBox* curButton = dynamic_cast<wxCheckBox*>(event.GetEventObject());
    if (curButton) {
        wxString label = curButton->GetLabel();

        for (auto i : *mp) {
            if (i.second->GetValue()) {
                ans.append(i.first);
            }
        }
        text->SetValue(ans);
    }
}


/*************************************************
*   MyChoice
*   ѡ����
*************************************************/
MyChoice::MyChoice(wxWindow* parent,
    const wxString& title,
    const wxString& describe,
    const wxString& errorloc)
    :ControllerBase(parent, title, describe, errorloc) {
    choice = new wxChoice(this, wxID_ANY);
    box->Add(choice, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
}

void MyChoice::FillChoice(wxArrayString arr) {
    choice->Clear();
    choice->Append(arr);
    choice->SetSelection(0);
}

int MyChoice::FindValue(const wxString& tar) {
    int size = choice->GetCount();
    for (int i = 0; i < size; i++) {
        if (choice->GetString(i) == tar)
            return i;
    }
    return 0;
}

NotEmpty::NotEmpty(
    wxWindow* parent,
    const wxString& title,
    const wxString& describe,
    const wxString& _errorloc,
    std::function<void()>func,
    const bool& set_tip,
    const wxString& tip_cont)
    :ControllerBase(parent, title, describe, _errorloc, func, set_tip, tip_cont) {

    text = new wxTextCtrl(this, wxID_ANY);
    box->Add(text, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);

    SetSizer(box);
    Layout();
}

bool NotEmpty::IsLegal() {
    if (text->GetValue() == wxEmptyString) {
        IncorrectSet("�����Ϊ��");
        return false;
    }
    CorrectSet();
    return true;
}