#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

#include "Native/Native.h"

class Native_Manager{
public:
	Native_Manager() {}
	void SetNativeManager(wxXmlNode* _root);
	void AddInfo(wxXmlNode*& root);
	void ClearNative();
	const Native* GetNativeByID_ReadOnly(const int& tar_id)const;
	Native* GetNativeByID_Write(const int& tar_id);
	bool DeleteNativeById(const int& id);

	std::vector<std::unique_ptr<Native>>native;
	//wxXmlNode* root;
};

