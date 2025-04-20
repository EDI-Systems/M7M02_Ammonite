#include <wx/notebook.h>

#include "UnitBase.h"


UnitBase::UnitBase(
    wxWindow* parent,
    const wxString& _errorloc,
    std::function<void()>_func)
    :wxScrolledWindow(parent, wxID_ANY), errorloc(_errorloc), func(_func) {
    this->SetScrollRate(10, 10);

    delete_unit = new wxButton(this, wxID_ANY, "删除");

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &UnitBase::OnDelete, this, delete_unit->GetId());
}

void UnitBase::OnDelete(const wxCommandEvent& event) {
    //wxMessageBox("基类func");
    wxNotebook* notebook = (wxNotebook*)this->GetParent();  // 父窗口必须是UnitM_Manager提供的notebook
    // 删除提醒
    if (wxMessageBox("确认删除当前页信息", "确认删除", wxYES_NO | wxICON_QUESTION) != wxYES)
        return;
    for (int i = 0; i < notebook->GetPageCount(); i++) {
        UnitBase* cur = (UnitBase*)notebook->GetPage(i);
        if (cur->FindWindow(this->delete_unit->GetId())) {
            // 删除本单元 则删除本单元所有对应的错误
            cur->ClearError();
            notebook->DeletePage(i);
            // 删除数据层的相关数据

            break;
        }
    }
    return;
}
wxString UnitBase::GetName() {
    // “不分大小写地不同”，用Upper处理
    if (base_name)
        return base_name->GetValue().Upper();
    wxLogError("未找到name控件 错误在%s", __func__);
    return "Invalid";
}

void UnitBase::NameCorrectSet() {
    base_name->CorrectSet();
}

void UnitBase::NameIncorrectSet(const wxString& str) {
    base_name->IncorrectSet(str);
}
