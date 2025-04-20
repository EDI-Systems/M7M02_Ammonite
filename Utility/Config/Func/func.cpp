#include "func.h"

#include <queue>
namespace myfunc {
    // 只查找root以及它的子节点
    wxXmlNode* FindSon(wxXmlNode* root, const wxString& target) {
        if (root->GetName() == target)
            return root;
        for (wxXmlNode* temp = root->GetChildren(); temp; temp = temp->GetNext()) {
            if (temp->GetName() == target)
                return temp;
        }
        wxLogError("节点无匹配项 错误在%s", __func__);
        return nullptr;
    }

    void DFS(wxXmlNode* cur, wxString& view_str, const int& level) {
        if (cur) {
            if (cur->GetType() == wxXML_ELEMENT_NODE) {
                wxString nodeName = cur->GetName();  // 节点名称
                wxString nodeContent = cur->GetNodeContent();  // 节点内容
                for (int i = level; i > 0; i--)view_str += "\t";
                if (nodeContent == "")
                    nodeContent = "->";
                view_str += nodeName + ":" + nodeContent + "\n";
            }
            for (wxXmlNode* temp = cur->GetChildren(); temp; temp = temp->GetNext()) {
                DFS(temp, view_str, level + 1);
            }
        }
        else {
            // 其实也没错，只是DFS开始回溯了，但是为什么从来没有出现过这个提示呢？
            wxMessageBox("DFS出错");
        }
    }

    void PreviewHelper(const wxString& str, const wxString& frame_name) {
        wxFrame* temp = new wxFrame(nullptr, wxID_ANY, frame_name, wxDefaultPosition, wxSize(800, 400));
        wxTextCtrl* txc = new wxTextCtrl(temp, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
        wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
        box->Add(txc, 1, wxEXPAND);
        temp->SetSizer(box);
        txc->SetValue(str);
        temp->Layout();
        temp->Center();
        temp->Show();
    }


    wxString GetCompletePath(const wxString& location, const wxString& end_sign) {
        wxString path = "";
        wxDir dir(location);
        if (!dir.IsOpened()) {  // 检查目录是否存在
            wxLogError("无法打开目录%s\t错误在%s", &location, __func__);
            return path;
        }
        wxString filename;
        wxArrayString tarFiles;
        bool cont = dir.GetFirst(&filename); // 获取第一个文件/子目录
        // 遍历目录
        while (cont) {        
            if (filename.EndsWith(end_sign))
                tarFiles.Add(filename);// 检查后缀（结尾字符串）
            cont = dir.GetNext(&filename);
        }
        // 检查找到的文件数量
        if (tarFiles.IsEmpty()) 
            wxLogError("未找到符合后缀要求的文件 错误在%s", __func__);
        else if (tarFiles.size() > 1)
            wxLogError("出现多个符合后缀要求的文件 错误在%s", __func__);
        else
            path = location + "\\" + tarFiles[0];
        return path;
    }

    wxArrayString Separater(const wxString& str, const wxString& sign) {
        wxArrayString result;
        wxStringTokenizer tokenizer(str, sign);
        while (tokenizer.HasMoreTokens()) 
            result.push_back(tokenizer.GetNextToken());
        return result;
    }

    void SetNodeCont(wxXmlNode*& parent, const wxString& tar, const wxString& cont){
        for (wxXmlNode* child = parent->GetChildren(); child; child = child->GetNext()) {
            if (child->GetName() == tar) {
                wxXmlNode* contentnode = child->GetChildren();
                if (contentnode) {
                    contentnode->SetContent(cont);
                    return;
                }
                else {
                    wxXmlNode* newcontentnode = new wxXmlNode(wxXML_TEXT_NODE, "text", cont);
                    child->AddChild(newcontentnode);
                    wxLogError("%s没有用于存储内容的子节点 错误在%s", child->GetName(), __func__);
                    return;
                }
            }
        }
        wxLogError("%s没有子节点%s 错误在%s", parent->GetName(), tar, __func__);
        wxXmlNode* contentnode = new wxXmlNode(wxXML_TEXT_NODE, "text", cont);
        wxXmlNode* child = new wxXmlNode(wxXML_ELEMENT_NODE, tar);
        child->AddChild(contentnode);
        parent->AddChild(child);
    }

    void Clear(wxXmlNode*& root) {
        if (!root) 
            return; // 如果节点为空，直接返回
        wxXmlNode* child = root->GetChildren(); // 获取子节点链表的起始节点
        while (child) {
            wxXmlNode* next = child->GetNext(); // 保存下一个子节点的指针
            root->RemoveChild(child);          // 从父节点中移除当前子节点
            delete child;                      // 删除子节点以释放内存
            child = next;                      // 移动到下一个子节点
        }
    }

    wxXmlNode* FindorMake(wxXmlNode* root, const wxString& tar) {
        for (wxXmlNode* cur = root->GetChildren(); cur; cur = cur->GetNext()) {
            if (cur->GetName() == tar) {
                // 符合标准后，也检查是否有填写内容的节点
                if (!cur->GetChildren()) {
                    wxXmlNode* textnode = new wxXmlNode(wxXML_TEXT_NODE, "text");
                    cur->AddChild(textnode);
                }
                return cur;
            }
        }
        wxLogError("%s没有子节点%s 错误在%s", root->GetName(), tar, __func__);
        wxXmlNode* child = new wxXmlNode(wxXML_ELEMENT_NODE, tar);
        wxXmlNode* textnode = new wxXmlNode(wxXML_TEXT_NODE,"text");
        root->AddChild(child);
        child->AddChild(textnode);
        return child;
    }

    wxString toHex(int value, wxString size_unit) {
        size_unit = size_unit.Upper();
        if (size_unit == "B")
            return wxString::Format("0x%X", value);   // 0x1
        else if (size_unit == "KB")
            return wxString::Format("0x%X", value * 4) + "00"; // 0x400
        else if (size_unit == "MB")
            return wxString::Format("0x%X", value) + "00000";  // 0x10 0000
        else if (size_unit == "GB")
            return wxString::Format("0x%X", value * 4) + "0000000";    // 0x4000 0000
        else
            return "-1";
    }

    bool GEhex(wxString _former, wxString _later) {
        // 参数需要是0x的形式
        _later = _later.SubString(0, _later.Length() - 1);
        _former = _former.SubString(0, _former.Length() - 1);
        if (_later.size() < _former.size())
            return true;
        else if (_later.size() > _former.size())
            return false;
        else {
            for (int i = 0; i < _later.size(); i++) {
                if (_later[i] < _former[i])
                    return true;
                else if (_later[i] > _former[i])
                    return false;
            }
            return true;
        }
    }

    wxArrayString MakeUnique(wxArrayString tar) {
        wxArrayString ans;
        std::set<wxString> uniqueSet(tar.begin(), tar.end());
        for (const wxString& str : uniqueSet)
            ans.push_back(str);
        return ans;
    }

    void AddNode(wxXmlNode*& root, const wxString& name, const wxString& content) {
        if (content == wxEmptyString) {
            wxLogError("%s内容为空", name);
            return;
        }
      
        wxXmlNode* NewNode = new wxXmlNode(wxXML_ELEMENT_NODE, name);
        NewNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "text", content));
        root->AddChild(NewNode);
        return;
    }

}
