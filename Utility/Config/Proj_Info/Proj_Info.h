#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>

#include "RVA_Info/RVA_Info.h"
#include "RVC_Info/RVC_Info.h"
#include "RVP_Info/RVP_Info.h"
#include "Error_Info/Error_Info.h"

class Proj_Info{
public:
	Proj_Info();
	wxString platform_path;

	std::unique_ptr<RVA_Info> rva;
	std::unique_ptr<RVC_Info> rvc;
	std::unique_ptr<RVP_Info> rvp;
	std::unique_ptr<Error_Info>errorInfo;
};

