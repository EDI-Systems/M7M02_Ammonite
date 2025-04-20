#pragma once

#include <wx/xml/xml.h>

#include "../UnitBase/UnitBase.h"
#include "../../Controller/Controller.h"

#include "../../../../../Proj_Info/RVP_Info/Unit/Invocation/Invocation.h"
class Invocation_Unit
    :public UnitBase {
public:
    Invocation_Unit(
        wxWindow* parent,
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr);

    // ÃÓ≥‰–≈œ¢
    void SetUnit(const std::unique_ptr<Invocation>& ptr);
    //wxXmlNode* MakeXmlUnit(const wxString& name);

    std::unique_ptr<Invocation> GetUnit();
    bool IsLegal()override;
    void ClearError()override;
private:
    SizeSlider* invo_stack_size;
};
