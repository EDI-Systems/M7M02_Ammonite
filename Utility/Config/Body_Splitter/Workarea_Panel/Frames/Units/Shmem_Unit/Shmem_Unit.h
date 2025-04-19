#pragma once
#include <wx/xml/xml.h>
#include "../UnitBase/UnitBase.h"

#include "../../../../../Proj_Info/RVP_Info/Unit/Shmem/Shmem.h"

class Shmem_Unit
    :public UnitBase {
public:
    Shmem_Unit(
        wxWindow* parent,
        const wxString& _errorlc = wxEmptyString,
        std::function<void()>func = nullptr);

    void SetUnit(const std::unique_ptr<Shmem>& ptr);
    //wxXmlNode* MakeXmlUnit(const wxString& name);

    std::unique_ptr<Shmem> GetUnit();

    bool IsLegal()override;
    void ClearError()override;

private:
    MyAttribute* sh_attribute;
};
