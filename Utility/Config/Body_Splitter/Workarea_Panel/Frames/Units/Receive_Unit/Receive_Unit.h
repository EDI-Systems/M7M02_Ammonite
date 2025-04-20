#pragma once
#include <wx/xml/xml.h>

#include "../UnitBase/UnitBase.h"
#include "../../Controller/Controller.h"

#include "../../../../../Proj_Info/RVP_Info/Unit/Receive/Receive.h"

class Receive_Unit
    :public UnitBase {
public:
    Receive_Unit(
        wxWindow* parent, 
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr);

    void SetUnit(const std::unique_ptr<Receive>& ptr);
    //wxXmlNode* MakeXmlUnit(const wxString& name);

    std::unique_ptr<Receive> GetUnit();

    bool IsLegal()override;
    void ClearError()override;
private:
    //wxTextCtrl* tcr_name;
};