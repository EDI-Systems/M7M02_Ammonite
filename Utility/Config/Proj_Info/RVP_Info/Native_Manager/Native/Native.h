#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <vector>

#include "Native_BaseInfo/Native_BaseInfo.h"
#include "../../Unit/Memory/Memory.h"
#include "../../Unit/Shmem/Shmem.h"
#include "../../Unit/Thread/Thread.h"
#include "../../Unit/Port/Port.h"
#include "../../Unit/Receive/Receive.h"
#include "../../Unit/Send/Send.h"
#include "../../Unit/Vector/Vector.h"
#include "../../Unit/Kfunc/Kfunc.h"
#include "../../Unit/Invocation/Invocation.h"

class Native{
public:
	// 用于读入信息的构造函数
	Native(wxXmlNode* _root, const int& _id);

	// 新建信息的构造函数
	Native(const int& _id);

	const wxString toString()const;

	void AddInfo(wxXmlNode*& root);

	// base_info 使用FindOrMake 只要有原生进程，就会有基础信息
	std::unique_ptr<Native_BaseInfo> base_info;
	// 其余用FindSon 有这些标签，标签下不一定有实例
	std::vector<std::unique_ptr<Memory>> memory;
	std::vector<std::unique_ptr<Shmem>> shmem;
	std::vector<std::unique_ptr<Thread>> thread;
	std::vector<std::unique_ptr<Port>> port;
	std::vector<std::unique_ptr<Receive>> receive;
	std::vector<std::unique_ptr<Send>> send;
	std::vector<std::unique_ptr<Vector>> vector;
	std::vector<std::unique_ptr<Kfunc>> kfunc;
	std::vector<std::unique_ptr<Invocation>> invocation;

	// 原生进程数量
	//int count;
	//wxXmlNode* root;
	int id;

private:
	template<typename V>
	const wxString per_toString(const std::vector<std::unique_ptr<V>>& vec)const {
		int count = 0;
		wxString ans;
		for (const auto& i : vec) {
			wxString part = i->toString(wxString::Format("%d",count++));
			ans = ans + part;
		}
		return ans;
	}

	template<typename V>
	void AddChildInfo(wxXmlNode*& root, const wxString& sign, const std::vector<std::unique_ptr<V>>& vec) {
		for (int i = 0; i < vec.size(); i++) {
			wxXmlNode* NewUnit = new wxXmlNode(wxXML_ELEMENT_NODE, wxString::Format("%s%d", sign, i));
			vec[i]->AddInfo(NewUnit);
			root->AddChild(NewUnit);
		}
	}
};


