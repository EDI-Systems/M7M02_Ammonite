#pragma once
#include <wx/xml/xml.h>

#include "../UnitBase/UnitBase.h"
#include "../../Controller/Controller.h"

#include "../../../../../Proj_Info/RVP_Info/Unit/Thread/Thread.h"

class Thread_Unit
    :public UnitBase {
public:
    Thread_Unit(
        wxWindow* parent, 
        const wxString& errorloc = wxEmptyString,
        std::function<void()>func = nullptr);

    void SetUnit(const std::unique_ptr<Thread>& ptr);

    std::unique_ptr<Thread> GetUnit();
    bool IsLegal()override;
    void ClearError()override;
private:
    // 当删除最后一个thread时，拒绝该操作
    void OnDelete(const wxCommandEvent& event)override;
    SizeSlider* t_stack_size;
    MyParameter* t_parameter;
    MyThreadPrio* t_priority;
};