#pragma once
#include <wx/xml/xml.h>

#include "../UnitBase/UnitBase.h"
#include "../../Controller/Controller.h"

#include "../../../../../Proj_Info/RVP_Info/Unit/Send/Send.h"
class Send_Unit
    :public UnitBase {
public:
    Send_Unit(
        wxWindow* parent, 
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr);

    void SetUnit(const std::unique_ptr<Send>& ptr);
    //wxXmlNode* MakeXmlUnit(const wxString& name);

    std::unique_ptr<Send> GetUnit();

    bool IsLegal()override;
    void ClearError()override;

    wxString GetName()override; // 由name和process唯一确定

private:
    SimpleText* sd_process;
};
