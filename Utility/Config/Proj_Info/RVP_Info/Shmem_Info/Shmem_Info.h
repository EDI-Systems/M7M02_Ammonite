#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <vector>

#include "../Unit/Memory/Memory.h"

class Shmem_Info {
public:
	Shmem_Info(){}
	void SetShmemInfo(wxXmlNode* _root);
	void AddInfo(wxXmlNode*& root);
	void ClearInfo();
	const wxString toString()const;
	//bool ReMoveMemory(const int& key);

	std::vector<std::unique_ptr<Memory>>memory;
	//wxXmlNode* root;

	// �ѷ���Ĺ����ڴ�������� + 1
	// ��ǰ��max_count���ǿ��õ�
};

