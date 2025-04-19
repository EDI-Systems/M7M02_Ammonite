#pragma once
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/notebook.h>

#include "../UnitBase/UnitBase.h"
#include "../../Controller/Controller.h"

#include "../../../../../Proj_Info/RVP_Info/Unit/Memory/Memory.h"

class Mem_Unit
    :public UnitBase {
public:
    Mem_Unit(
        wxWindow* parent,
        const wxString& errorloc = wxEmptyString,
        std::function<void()> func = nullptr);

    // 填充信息
    void SetUnit(const std::unique_ptr<Memory>& ptr);

    // 返回一个智能指针对象
    std::unique_ptr<Memory> GetUnit();

    //wxXmlNode* MakeXmlUnit(const wxString& name);
    bool IsLegal()override;
    void ClearError()override;
private:

    BaseSlider* mem_base;
    SizeSlider* mem_size;
    MyAlign* mem_align;
    MyChoice* mem_type;
    MyAttribute* mem_attribute;
};
