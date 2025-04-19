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
	// 以sign为分隔符
	wxArrayString Separater(const wxString& str, const wxString& sign);

	// 找到root为根，name为target的【子节点】
	wxXmlNode* FindSon(wxXmlNode* root, const wxString& target);	

	// 得到location文件夹下，特定后缀的文件名
	wxString GetCompletePath(const wxString& location, const wxString& end_sign);

	// 生成显示字符串str的frame
	void PreviewHelper(const wxString& str, const wxString& frame_name);


	// 得到xml文件的DFS字符串，level为当前DFS节点的高度
	void DFS(wxXmlNode* cur, wxString& view_str, const int& level = 0);

	// 传入父节点和对应直接子节点的名称，并设置直接子节点的内容，如果没有给该直接子节点，则创建后设置之
	void SetNodeCont(wxXmlNode*& parent, const wxString& tar, const wxString& cont);

	// 清除root的子节点
	void Clear(wxXmlNode*& root);

	wxXmlNode* FindorMake(wxXmlNode* root, const wxString& tar);

	// 形如256KB转为十六进制数
	wxString toHex(int value, wxString size_unit);

	// former(hex) > latter(hex) || former(hex) == later(hex)
	bool GEhex(wxString former, wxString latter);

	// 去重
	wxArrayString MakeUnique(wxArrayString tar);

	// 填充装unit的vector    
	template <typename T>
	void SetUnit(wxXmlNode* root, const wxString& tag, std::vector<std::unique_ptr<T>>& container) {

		wxXmlNode* parentNode = myfunc::FindSon(root, tag);
		if (!parentNode) return; // 防止空指针访问

		for (wxXmlNode* node = parentNode->GetChildren(); node; node = node->GetNext()) {
			container.push_back(std::make_unique<T>(node));
		}
	}

	// 添加子节点并设置内容
	void AddNode(wxXmlNode*& root, const wxString& name, const wxString& content);
}
