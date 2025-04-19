#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/notebook.h>
#include <set>
#include <vector>

#include "../../../../Func/func.h"


// U ΪGUI����
// P Ϊ���GUI��Ӧ������
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

        wxButton* add_unit = new wxButton(this, wxID_ANY, "���");

        box->Add(notebook, 1, wxEXPAND);
        box->Add(add_unit, 0, wxALIGN_CENTER_HORIZONTAL);
        this->SetSizer(box);

        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Unit_Manager_Test<U, P>::OnAdd, this, add_unit->GetId());
    }

    void SetGUI(const std::vector<std::unique_ptr<P>>& info_ptrs) {
        max_num_GUI = 0;

        // ����ҳ�����ǲ��ͷ�ҳ��Ķ���
        // ҳ���еĶ���U��parentΪnotebook����notebook���ͷ�ʱ����ҳ����󽫱��ͷ�
        // notebook->DeleteAllPages();

        while (notebook->GetPageCount() > 0) {
            U* myUnit = (U*)notebook->GetPage(0);  // ��ȡ��ǰ��һҳ
            notebook->RemovePage(0);  // �� notebook �Ƴ���ҳ
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
        // GUI�����ϼ��ÿ����Ԫ�Ƿ�Ϸ�
        for (int i = 0; i < notebook->GetPageCount(); i++) {
            U* myUnit = (U*)notebook->GetPage(i);

            if (!myUnit->IsLegal()) {
                // ҳ���ڿؼ����Ϸ�
                judge = false;
            }
            else {
                // ҳ���ڿؼ��Ϸ�
                if (UniqueName.find(myUnit->GetName()) == UniqueName.end()) {
                    // û�м�¼�����ַ���
                    UniqueName.insert(myUnit->GetName());
                    myUnit->NameCorrectSet();
                }
                else {
                    // ��¼�����ַ���
                    myUnit->NameIncorrectSet(wxString::Format("�����ظ� ��ҳ��֮ǰ����ʹ�ù�%s", myUnit->GetName()));
                    judge = false;
                }
                //myUnit->ClearError();
            }
        }
        return judge;
    }


    void ConfirmFunc(std::vector<std::unique_ptr<P>>& info_ptrs) {
        // ����Ƿ�Ϸ�

        if (!IsLegal()) {
            wxLogError("Unit Manager��������");
            return;
        }

        // ��� vector���ͷ����� std::unique_ptr<P>
        info_ptrs.clear();

        // ʹ�úϷ���GUI�������
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
        // �½���ԭ����������Ҫ��һ���̣߳�AddFunc�����ⲿ����
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
            U* myUnit = (U*)notebook->GetPage(0);  // ��ȡ��ǰ��һҳ
            notebook->RemovePage(0);  // �� notebook �Ƴ���ҳ
            myUnit->ClearError();   // �������
            myUnit->Destroy();  // �ͷſռ�
        }
        max_num_GUI = 0;    // ����������ֵ
    }


private:
    wxNotebook* notebook;   // ����ͬ��ʵ��
    wxString unit_sign;     // ʵ���ı�ʶ
    wxString errorloc;      // ���������λ��
    std::function<void()>func;  // ������ת����ǰ�ؼ����ڵ�ģ��
    int max_num_GUI;        // notebook��ҳ��������

};