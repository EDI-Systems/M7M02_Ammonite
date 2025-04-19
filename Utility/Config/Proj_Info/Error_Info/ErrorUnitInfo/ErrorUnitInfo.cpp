#include "ErrorUnitInfo.h"

ErrorUnitInfo::ErrorUnitInfo(const wxString& _loc, const wxString& _cont, std::function<void()> _func)
	:loc(_loc),cont(_cont),func(_func){}