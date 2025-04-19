#pragma once
#include <wx/xml/xml.h>

#include "../UnitBase/UnitBase.h"
#include "../../Controller/Controller.h"
#include "../../../../../Proj_Info/RVP_Info/Unit/Vector/Vector.h"

class Vector_Unit
    :public UnitBase {
public:
    Vector_Unit(
        wxWindow* parent, 
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr);


    void SetUnit(const std::unique_ptr<Vector>& ptr);

    // 返回一个智能指针对象
    std::unique_ptr<Vector> GetUnit();

    //wxXmlNode* MakeXmlUnit(const wxString& name);
    bool IsLegal()override;
    void ClearError()override {}
private:
    //wxTextCtrl* tcr_name;
    MyZ_GE_Zero* vec_number;

};