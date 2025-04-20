#include "file_menu.h"
#include "../../Body_Splitter/Panels/panels.h"
#include "../../main.h"
#include "../../Func/func.h"

#include <wx/thread.h>
#include <thread>

File_Menu::File_Menu(wxWindow* parent) {

    wxMenuItem* New_FIle = new wxMenuItem(this, wxID_ANY, _("新建"));
    Append(New_FIle);
    parent->Bind(wxEVT_MENU, &File_Menu::OnNewFile, this, New_FIle->GetId());

    wxMenuItem* Open_File = new wxMenuItem(this, wxID_ANY, _("打开"));
    Append(Open_File);    
    parent->Bind(wxEVT_MENU, &File_Menu::OnImportFile, this, Open_File->GetId());

    wxMenuItem* Export_File = new wxMenuItem(this, wxID_ANY, _("导出"));
    Append(Export_File);
    parent->Bind(wxEVT_MENU, &File_Menu::OnExportFile, this, Export_File->GetId());
}


void File_Menu::NewFileFunc() {
    switch (Main::work_mode) {
    case(MODE_INIT):

        break;
    default:
        if (wxMessageBox("是否新建的工程描述文件", "新建", wxYES_NO | wxICON_QUESTION) == wxNO) {
            return;
        }
        if (wxMessageBox("是否保存当前正在编辑的工程描述文件", "保存", wxYES_NO | wxICON_QUESTION) == wxYES) {
            // 需要保存，则进行导出
            if (!ExportFileFunc())
                return;
        }
        break;
    }
    // 重置
    // 清空的GUI
    Main::body_splitter->workarea_panel->ClearGUI();

    // 清空Proj_Info
    Main::proj_info->rvp->ClearInfo();

    // 清空 原生进程 和 虚拟机 的树形结构
    Main::body_splitter->file_tree->ClearItemNativeChild();
    Main::body_splitter->file_tree->ClearItemVmChild();

    // 重置所有wxTreeItemData的属性
    Main::body_splitter->file_tree->InitItem();

    // 清空错误
    Main::proj_info->errorInfo->RemoveAll();
    Main::log_panel2->RefreshErrorLog();
  
    // 初始化Proj_Info中的Chip
    Main::proj_info->rvp->chip->InitChip();

    wxBusyCursor busyCursor; // 改变鼠标为忙碌状态
    Main::body_splitter->Freeze();

    Main::body_splitter->workarea_panel->ShowProject();

    Main::body_splitter->file_tree->SelectRoot();
    Main::body_splitter->Enable(true);
    Main::body_splitter->Thaw();

    wxMessageBox("允许对新建的工程文件进行编辑");
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
        if (wxMessageBox("是否打开工程描述文件", "打开", wxYES_NO | wxICON_QUESTION) == wxNO) {
            return;
        }
        if (wxMessageBox("是否保存当前正在编辑的工程描述文件", "保存", wxYES_NO | wxICON_QUESTION) == wxYES) {
            // 需要保存，则进行导出
            if (!ExportFileFunc())
                return;
        }
        break;
    }
    // 重置
    // 清空的GUI
    Main::body_splitter->workarea_panel->ClearGUI();

    // 清空Proj_Info
    Main::proj_info->rvp->ClearInfo();

    // 清空 原生进程 和 虚拟机 的树形结构
    Main::body_splitter->file_tree->ClearItemNativeChild();
    Main::body_splitter->file_tree->ClearItemVmChild();
    // 重置所有wxTreeItemData的属性
    Main::body_splitter->file_tree->InitItem();

    wxXmlNode* root = nullptr;
    wxString path = wxEmptyString;
    wxString result = wxEmptyString;
    wxXmlDocument* doc = new wxXmlDocument();

    wxFileDialog fileDialog(nullptr, _("选择文件或文件夹"), "", "",
        _("所有文件 (*.*)|*.*|文件夹|*/"),
        wxFD_OPEN | wxFD_FILE_MUST_EXIST); // wxFD_OPEN 是让对话框显示文件，wxFD_FILE_MUST_EXIST 确保文件存在
    switch (fileDialog.ShowModal()) {
    case wxID_OK:
        path = fileDialog.GetPath();     // 获取选择的文件夹路径
        if (!path.ends_with(".rvp")) {
            wxLogError("只允许打开工程配置文件(.rvp) 错误在%s", __func__);
            return;
        }

        // 尝试设置root;
        if (doc->Load(path)) {
            root = doc->DetachRoot();
            if (!root) {
                wxLogError("读入工程描述文件根节点失败 错误在%s", __func__);
                return;
            }
        }
        else {
            wxLogError("读入工程描述文件路径错误");
            return;
        }

        break;
    case wxID_CANCEL:
        wxMessageBox("操作取消");
        return;
        break;
    default:
        wxMessageBox("其他");
        return;
        break;
    }

    // 读入工程描述文件的信息
    Main::proj_info->rvp->SetRVP(root);

    // 读取平台和芯片信息
    Main::proj_info->rva->name = Main::proj_info->rvp->chip->platform;
    Main::proj_info->rvc->name = Main::proj_info->rvp->chip->chip_class;
    Main::proj_info->rva->loc = Main::proj_info->platform_path + "\\" + Main::proj_info->rva->name;
    Main::proj_info->rvc->loc = Main::proj_info->rva->loc + "\\Chip\\" + Main::proj_info->rvc->name;


    Main::proj_info->rva->SetRVA(myfunc::GetCompletePath(Main::proj_info->rva->loc, ".rva"));
    Main::proj_info->rvc->SetRVC(myfunc::GetCompletePath(Main::proj_info->rvc->loc, ".rvc"));

    Main::bottom_message->SetBottomMessage(Main::proj_info->rva->name + "\t" + Main::proj_info->rvc->name, *wxGREEN);

    // 改变鼠标为忙碌状态
    wxBusyCursor busyCursor;
    Main::body_splitter->Freeze();

    // 更新树形结构
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
    // 查看信息中是否有空白项
    // 因为只有合法的才能被放到信息层

    // 从IsLegal改为Save
    // 合法 则返回true 并保存
    // 不合法 则返回false 不会保存
    bool j1 = Main::body_splitter->workarea_panel->base_info->SaveBaseInfo();
    bool j2 = Main::body_splitter->workarea_panel->chip->SaveChip();
    bool j3 = Main::body_splitter->workarea_panel->extmem->SaveExtmem();
    bool j4 = Main::body_splitter->workarea_panel->shmem->SaveShmem();
    bool j5 = Main::body_splitter->workarea_panel->kernel->SaveKernel();
    bool j6 = Main::body_splitter->workarea_panel->monitor->SaveMonitor();
    bool j7 = Main::body_splitter->workarea_panel->IsLegal_Native();
    bool j8 = Main::body_splitter->workarea_panel->IsLegal_Vm();
    if (j1 && j2 && j3 && j4 && j5 && j6 && j7 && j8) {

        // 打开文件对话框选择保存位置
        wxFileDialog saveFileDialog(nullptr, "Save RVP file", "", "", "RVP files (*.rvp)|*.rvp", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

        // 确定导出
        if (saveFileDialog.ShowModal() == wxID_CANCEL) {
            // 不导出就无法切换work mode
            return false;
        }

        // 获取用户选择的文件路径
        wxString filePath = saveFileDialog.GetPath();

        wxXmlDocument doc;
        doc.SetRoot(Main::proj_info->rvp->ReturnRoot());

        // 保存文档到文件
        if (doc.Save(filePath)) {
            wxMessageBox("导出成功", "Success", wxOK | wxICON_INFORMATION);
        }
        else {
            wxMessageBox("导出失败", "Error", wxOK | wxICON_ERROR);
        }

        Main::work_mode = MODE_INIT;
        return true;
    }
    else {
        wxMessageBox("存在错误");
        return false;
    }
}

void File_Menu::OnExportFile(const wxCommandEvent& event) {
    ExportFileFunc();
}