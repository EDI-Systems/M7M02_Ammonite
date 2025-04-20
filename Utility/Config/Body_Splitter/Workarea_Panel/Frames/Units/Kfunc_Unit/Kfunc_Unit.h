#pragma once
#include <wx/xml/xml.h>

#include "../UnitBase/UnitBase.h"
#include "../../Controller/Controller.h"

#include "../../../../../Proj_Info/RVP_Info/Unit/Kfunc/Kfunc.h"

class Kfunc_Unit
    :public UnitBase {
public:
    Kfunc_Unit(
        wxWindow* parent, 
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr);

    void SetUnit(const std::unique_ptr<Kfunc>& ptr);
    //wxXmlNode* MakeXmlUnit(const wxString& name);

    std::unique_ptr<Kfunc> GetUnit();
    bool IsLegal()override;
    void ClearError()override;

private:
    BaseSlider* k_begin;
    BaseSlider* k_end;
};