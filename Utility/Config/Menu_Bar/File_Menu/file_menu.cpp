#include "file_menu.h"
#include "../../Body_Splitter/Panels/panels.h"
#include "../../main.h"
#include "../../Func/func.h"

#include <wx/thread.h>
#include <thread>

File_Menu::File_Menu(wxWindow* parent) {

    wxMenuItem* New_FIle = new wxMenuItem(this, wxID_ANY, _("�½�"));
    Append(New_FIle);
    parent->Bind(wxEVT_MENU, &File_Menu::OnNewFile, this, New_FIle->GetId());

    wxMenuItem* Open_File = new wxMenuItem(this, wxID_ANY, _("��"));
    Append(Open_File);    
    parent->Bind(wxEVT_MENU, &File_Menu::OnImportFile, this, Open_File->GetId());

    wxMenuItem* Export_File = new wxMenuItem(this, wxID_ANY, _("����"));
    Append(Export_File);
    parent->Bind(wxEVT_MENU, &File_Menu::OnExportFile, this, Export_File->GetId());
}


void File_Menu::NewFileFunc() {
    switch (Main::work_mode) {
    case(MODE_INIT):

        break;
    default:
        if (wxMessageBox("�Ƿ��½��Ĺ��������ļ�", "�½�", wxYES_NO | wxICON_QUESTION) == wxNO) {
            return;
        }
        if (wxMessageBox("�Ƿ񱣴浱ǰ���ڱ༭�Ĺ��������ļ�", "����", wxYES_NO | wxICON_QUESTION) == wxYES) {
            // ��Ҫ���棬����е���
            if (!ExportFileFunc())
                return;
        }
        break;
    }
    // ����
    // ��յ�GUI
    Main::body_splitter->workarea_panel->ClearGUI();

    // ���Proj_Info
    Main::proj_info->rvp->ClearInfo();

    // ��� ԭ������ �� ����� �����νṹ
    Main::body_splitter->file_tree->ClearItemNativeChild();
    Main::body_splitter->file_tree->ClearItemVmChild();

    // ��������wxTreeItemData������
    Main::body_splitter->file_tree->InitItem();

    // ��մ���
    Main::proj_info->errorInfo->RemoveAll();
    Main::log_panel2->RefreshErrorLog();
  
    // ��ʼ��Proj_Info�е�Chip
    Main::proj_info->rvp->chip->InitChip();

    wxBusyCursor busyCursor; // �ı����Ϊæµ״̬
    Main::body_splitter->Freeze();

    Main::body_splitter->workarea_panel->ShowProject();

    Main::body_splitter->file_tree->SelectRoot();
    Main::body_splitter->Enable(true);
    Main::body_splitter->Thaw();

    wxMessageBox("������½��Ĺ����ļ����б༭");
    Main::work_mode = MODE_NEWFILE;
}

void File_Menu::OnNewFile(const wxCommandEvent& event) {
    NewFileFunc();
}

void File_Menu::ImportFileFunc() {
    switch (Main::work_mode) {
    case(MODE_INIT):

        break;
    default:
        if (wxMessageBox("�Ƿ�򿪹��������ļ�", "��", wxYES_NO | wxICON_QUESTION) == wxNO) {
            return;
        }
        if (wxMessageBox("�Ƿ񱣴浱ǰ���ڱ༭�Ĺ��������ļ�", "����", wxYES_NO | wxICON_QUESTION) == wxYES) {
            // ��Ҫ���棬����е���
            if (!ExportFileFunc())
                return;
        }
        break;
    }
    // ����
    // ��յ�GUI
    Main::body_splitter->workarea_panel->ClearGUI();

    // ���Proj_Info
    Main::proj_info->rvp->ClearInfo();

    // ��� ԭ������ �� ����� �����νṹ
    Main::body_splitter->file_tree->ClearItemNativeChild();
    Main::body_splitter->file_tree->ClearItemVmChild();
    // ��������wxTreeItemData������
    Main::body_splitter->file_tree->InitItem();

    wxXmlNode* root = nullptr;
    wxString path = wxEmptyString;
    wxString result = wxEmptyString;
    wxXmlDocument* doc = new wxXmlDocument();

    wxFileDialog fileDialog(nullptr, _("ѡ���ļ����ļ���"), "", "",
        _("�����ļ� (*.*)|*.*|�ļ���|*/"),
        wxFD_OPEN | wxFD_FILE_MUST_EXIST); // wxFD_OPEN ���öԻ�����ʾ�ļ���wxFD_FILE_MUST_EXIST ȷ���ļ�����
    switch (fileDialog.ShowModal()) {
    case wxID_OK:
        path = fileDialog.GetPath();     // ��ȡѡ����ļ���·��
        if (!path.ends_with(".rvp")) {
            wxLogError("ֻ����򿪹��������ļ�(.rvp) ������%s", __func__);
            return;
        }

        // ��������root;
        if (doc->Load(path)) {
            root = doc->DetachRoot();
            if (!root) {
                wxLogError("���빤�������ļ����ڵ�ʧ�� ������%s", __func__);
                return;
            }
        }
        else {
            wxLogError("���빤�������ļ�·������");
            return;
        }

        break;
    case wxID_CANCEL:
        wxMessageBox("����ȡ��");
        return;
        break;
    default:
        wxMessageBox("����");
        return;
        break;
    }

    // ���빤�������ļ�����Ϣ
    Main::proj_info->rvp->SetRVP(root);

    // ��ȡƽ̨��оƬ��Ϣ
    Main::proj_info->rva->name = Main::proj_info->rvp->chip->platform;
    Main::proj_info->rvc->name = Main::proj_info->rvp->chip->chip_class;
    Main::proj_info->rva->loc = Main::proj_info->platform_path + "\\" + Main::proj_info->rva->name;
    Main::proj_info->rvc->loc = Main::proj_info->rva->loc + "\\Chip\\" + Main::proj_info->rvc->name;


    Main::proj_info->rva->SetRVA(myfunc::GetCompletePath(Main::proj_info->rva->loc, ".rva"));
    Main::proj_info->rvc->SetRVC(myfunc::GetCompletePath(Main::proj_info->rvc->loc, ".rvc"));

    Main::bottom_message->SetBottomMessage(Main::proj_info->rva->name + "\t" + Main::proj_info->rvc->name, *wxGREEN);

    // �ı����Ϊæµ״̬
    wxBusyCursor busyCursor;
    Main::body_splitter->Freeze();

    // �������νṹ
    for (const auto& ptr : Main::proj_info->rvp->native_manager->native) {
        Main::body_splitter->file_tree->AddNativeChild();
    }

    for (const auto& ptr : Main::proj_info->rvp->vm_manager->virtual_machine) {
        Main::body_splitter->file_tree->AddVmChild();
    }

    Main::body_splitter->file_tree->SelectRoot();
    Main::body_splitter->Enable(true);
    Main::body_splitter->Thaw();

    Main::work_mode = MODE_IMPORT;

}

void File_Menu::OnImportFile(const wxCommandEvent& event) {
    ImportFileFunc();
}

bool File_Menu::ExportFileFunc() {
    // �鿴��Ϣ���Ƿ��пհ���
    // ��Ϊֻ�кϷ��Ĳ��ܱ��ŵ���Ϣ��

    // ��IsLegal��ΪSave
    // �Ϸ� �򷵻�true ������
    // ���Ϸ� �򷵻�false ���ᱣ��
    bool j1 = Main::body_splitter->workarea_panel->base_info->SaveBaseInfo();
    bool j2 = Main::body_splitter->workarea_panel->chip->SaveChip();
    bool j3 = Main::body_splitter->workarea_panel->extmem->SaveExtmem();
    bool j4 = Main::body_splitter->workarea_panel->shmem->SaveShmem();
    bool j5 = Main::body_splitter->workarea_panel->kernel->SaveKernel();
    bool j6 = Main::body_splitter->workarea_panel->monitor->SaveMonitor();
    bool j7 = Main::body_splitter->workarea_panel->IsLegal_Native();
    bool j8 = Main::body_splitter->workarea_panel->IsLegal_Vm();
    if (j1 && j2 && j3 && j4 && j5 && j6 && j7 && j8) {

        // ���ļ��Ի���ѡ�񱣴�λ��
        wxFileDialog saveFileDialog(nullptr, "Save RVP file", "", "", "RVP files (*.rvp)|*.rvp", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

        // ȷ������
        if (saveFileDialog.ShowModal() == wxID_CANCEL) {
            // ���������޷��л�work mode
            return false;
        }

        // ��ȡ�û�ѡ����ļ�·��
        wxString filePath = saveFileDialog.GetPath();

        wxXmlDocument doc;
        doc.SetRoot(Main::proj_info->rvp->ReturnRoot());

        // �����ĵ����ļ�
        if (doc.Save(filePath)) {
            wxMessageBox("�����ɹ�", "Success", wxOK | wxICON_INFORMATION);
        }
        else {
            wxMessageBox("����ʧ��", "Error", wxOK | wxICON_ERROR);
        }

        Main::work_mode = MODE_INIT;
        return true;
    }
    else {
        wxMessageBox("���ڴ���");
        return false;
    }
}

void File_Menu::OnExportFile(const wxCommandEvent& event) {
    ExportFileFunc();
}