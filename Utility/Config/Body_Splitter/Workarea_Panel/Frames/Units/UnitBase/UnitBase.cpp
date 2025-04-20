#include <wx/notebook.h>

#include "UnitBase.h"


UnitBase::UnitBase(
    wxWindow* parent,
    const wxString& _errorloc,
    std::function<void()>_func)
    :wxScrolledWindow(parent, wxID_ANY), errorloc(_errorloc), func(_func) {
    this->SetScrollRate(10, 10);

    delete_unit = new wxButton(this, wxID_ANY, "ɾ��");

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &UnitBase::OnDelete, this, delete_unit->GetId());
}

void UnitBase::OnDelete(const wxCommandEvent& event) {
    //wxMessageBox("����func");
    wxNotebook* notebook = (wxNotebook*)this->GetParent();  // �����ڱ�����UnitM_Manager�ṩ��notebook
    // ɾ������
    if (wxMessageBox("ȷ��ɾ����ǰҳ��Ϣ", "ȷ��ɾ��", wxYES_NO | wxICON_QUESTION) != wxYES)
        return;
    for (int i = 0; i < notebook->GetPageCount(); i++) {
        UnitBase* cur = (UnitBase*)notebook->GetPage(i);
        if (cur->FindWindow(this->delete_unit->GetId())) {
            // ɾ������Ԫ ��ɾ������Ԫ���ж�Ӧ�Ĵ���
            cur->ClearError();
            notebook->DeletePage(i);
            // ɾ�����ݲ���������

            break;
        }
    }
    return;
}
wxString UnitBase::GetName() {
    // �����ִ�Сд�ز�ͬ������Upper����
    if (base_name)
        return base_name->GetValue().Upper();
    wxLogError("δ�ҵ�name�ؼ� ������%s", __func__);
    return "Invalid";
}

void UnitBase::NameCorrectSet() {
    base_name->CorrectSet();
}

void UnitBase::NameIncorrectSet(const wxString& str) {
    base_name->IncorrectSet(str);
}
