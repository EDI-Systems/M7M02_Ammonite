#pragma once
#include <wx/xml/xml.h>

#include "../UnitBase/UnitBase.h"
#include "../../Controller/Controller.h"
#include "../../../../../Proj_Info/RVP_Info/Unit/Port/Port.h"

class Port_Unit
    :public UnitBase {
public:
    Port_Unit(
        wxWindow* parent, 
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr);
    //void DeleteFunc(const wxCommandEvent& event);
    void SetUnit(const std::unique_ptr<Port>& ptr);

    //wxXmlNode* MakeXmlUnit(const wxString& name);

    std::unique_ptr<Port> GetUnit();
    bool IsLegal()override;
    void ClearError()override;
    wxString GetName()override;  // 由name和process唯一决定
private:
    SimpleText* p_process;


};
