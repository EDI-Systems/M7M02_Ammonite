#include "func.h"

#include <queue>
namespace myfunc {
    // ֻ����root�Լ������ӽڵ�
    wxXmlNode* FindSon(wxXmlNode* root, const wxString& target) {
        if (root->GetName() == target)
            return root;
        for (wxXmlNode* temp = root->GetChildren(); temp; temp = temp->GetNext()) {
            if (temp->GetName() == target)
                return temp;
        }
        wxLogError("�ڵ���ƥ���� ������%s", __func__);
        return nullptr;
    }

    void DFS(wxXmlNode* cur, wxString& view_str, const int& level) {
        if (cur) {
            if (cur->GetType() == wxXML_ELEMENT_NODE) {
                wxString nodeName = cur->GetName();  // �ڵ�����
                wxString nodeContent = cur->GetNodeContent();  // �ڵ�����
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
            // ��ʵҲû��ֻ��DFS��ʼ�����ˣ�����Ϊʲô����û�г��ֹ������ʾ�أ�
            wxMessageBox("DFS����");
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
        if (!dir.IsOpened()) {  // ���Ŀ¼�Ƿ����
            wxLogError("�޷���Ŀ¼%s\t������%s", &location, __func__);
            return path;
        }
        wxString filename;
        wxArrayString tarFiles;
        bool cont = dir.GetFirst(&filename); // ��ȡ��һ���ļ�/��Ŀ¼
        // ����Ŀ¼
        while (cont) {        
            if (filename.EndsWith(end_sign))
                tarFiles.Add(filename);// ����׺����β�ַ�����
            cont = dir.GetNext(&filename);
        }
        // ����ҵ����ļ�����
        if (tarFiles.IsEmpty()) 
            wxLogError("δ�ҵ����Ϻ�׺Ҫ����ļ� ������%s", __func__);
        else if (tarFiles.size() > 1)
            wxLogError("���ֶ�����Ϻ�׺Ҫ����ļ� ������%s", __func__);
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
                    wxLogError("%sû�����ڴ洢���ݵ��ӽڵ� ������%s", child->GetName(), __func__);
                    return;
                }
            }
        }
        wxLogError("%sû���ӽڵ�%s ������%s", parent->GetName(), tar, __func__);
        wxXmlNode* contentnode = new wxXmlNode(wxXML_TEXT_NODE, "text", cont);
        wxXmlNode* child = new wxXmlNode(wxXML_ELEMENT_NODE, tar);
        child->AddChild(contentnode);
        parent->AddChild(child);
    }

    void Clear(wxXmlNode*& root) {
        if (!root) 
            return; // ����ڵ�Ϊ�գ�ֱ�ӷ���
        wxXmlNode* child = root->GetChildren(); // ��ȡ�ӽڵ��������ʼ�ڵ�
        while (child) {
            wxXmlNode* next = child->GetNext(); // ������һ���ӽڵ��ָ��
            root->RemoveChild(child);          // �Ӹ��ڵ����Ƴ���ǰ�ӽڵ�
            delete child;                      // ɾ���ӽڵ����ͷ��ڴ�
            child = next;                      // �ƶ�����һ���ӽڵ�
        }
    }

    wxXmlNode* FindorMake(wxXmlNode* root, const wxString& tar) {
        for (wxXmlNode* cur = root->GetChildren(); cur; cur = cur->GetNext()) {
            if (cur->GetName() == tar) {
                // ���ϱ�׼��Ҳ����Ƿ�����д���ݵĽڵ�
                if (!cur->GetChildren()) {
                    wxXmlNode* textnode = new wxXmlNode(wxXML_TEXT_NODE, "text");
                    cur->AddChild(textnode);
                }
                return cur;
            }
        }
        wxLogError("%sû���ӽڵ�%s ������%s", root->GetName(), tar, __func__);
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
        // ������Ҫ��0x����ʽ
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
            wxLogError("%s����Ϊ��", name);
            return;
        }
      
        wxXmlNode* NewNode = new wxXmlNode(wxXML_ELEMENT_NODE, name);
        NewNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "text", content));
        root->AddChild(NewNode);
        return;
    }

}
