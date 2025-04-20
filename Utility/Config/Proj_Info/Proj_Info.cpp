#include "Proj_Info.h"
#include "../Func/func.h"

Proj_Info::Proj_Info(){
	rva = std::make_unique<RVA_Info>();
	rvc = std::make_unique<RVC_Info>();
	rvp = std::make_unique<RVP_Info>();
	errorInfo = std::make_unique<Error_Info>();
}
