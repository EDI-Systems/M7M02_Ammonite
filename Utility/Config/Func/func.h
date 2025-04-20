#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/arrstr.h>

#include <map>
#include <set>
#include <sstream>
namespace myfunc {
	// ��signΪ�ָ���
	wxArrayString Separater(const wxString& str, const wxString& sign);

	// �ҵ�rootΪ����nameΪtarget�ġ��ӽڵ㡿
	wxXmlNode* FindSon(wxXmlNode* root, const wxString& target);	

	// �õ�location�ļ����£��ض���׺���ļ���
	wxString GetCompletePath(const wxString& location, const wxString& end_sign);

	// ������ʾ�ַ���str��frame
	void PreviewHelper(const wxString& str, const wxString& frame_name);


	// �õ�xml�ļ���DFS�ַ�����levelΪ��ǰDFS�ڵ�ĸ߶�
	void DFS(wxXmlNode* cur, wxString& view_str, const int& level = 0);

	// ���븸�ڵ�Ͷ�Ӧֱ���ӽڵ�����ƣ�������ֱ���ӽڵ�����ݣ����û�и���ֱ���ӽڵ㣬�򴴽�������֮
	void SetNodeCont(wxXmlNode*& parent, const wxString& tar, const wxString& cont);

	// ���root���ӽڵ�
	void Clear(wxXmlNode*& root);

	wxXmlNode* FindorMake(wxXmlNode* root, const wxString& tar);

	// ����256KBתΪʮ��������
	wxString toHex(int value, wxString size_unit);

	// former(hex) > latter(hex) || former(hex) == later(hex)
	bool GEhex(wxString former, wxString latter);

	// ȥ��
	wxArrayString MakeUnique(wxArrayString tar);

	// ���װunit��vector    
	template <typename T>
	void SetUnit(wxXmlNode* root, const wxString& tag, std::vector<std::unique_ptr<T>>& container) {

		wxXmlNode* parentNode = myfunc::FindSon(root, tag);
		if (!parentNode) return; // ��ֹ��ָ�����

		for (wxXmlNode* node = parentNode->GetChildren(); node; node = node->GetNext()) {
			container.push_back(std::make_unique<T>(node));
		}
	}

	// ����ӽڵ㲢��������
	void AddNode(wxXmlNode*& root, const wxString& name, const wxString& content);
}
