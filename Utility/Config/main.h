#pragma once
#include <wx/wx.h>
#include "Menu_Bar/menu_bar.h"
#include "Tool_Bar/tool_bar.h"
#include "Body_Splitter/body_splitter.h"
#include "Bottom_message/bottom_message.h"
#include "Proj_Info/Proj_Info.h"

#include "ErrorLogPanel2/ErrorLogPanel2.h"


#define MODE_NEWFILE	0
#define MODE_IMPORT		1
#define MODE_INIT		2

#define T1	1
#define T2	2
#define T3	4
#define T4	8



class Main
	:public wxFrame {
public:
	Main();
	~Main();
	// 菜单栏
	static Menu_Bar* menu_bar;

	// 工具栏
	static Tool_Bar* tool_bar;

	// 窗口分割器
	static Body_Splitter* body_splitter;

	// 报错信息
	//static ErrorLogPanel*log_panel;
	static ErrorLogPanel2* log_panel2;

	// 底部信息栏
	static Bottom_Message* bottom_message;

	// 主窗口布局
	static wxBoxSizer* app_box;		

	// 工作模式
	static int work_mode;

	// 信息层
	static Proj_Info* proj_info;

	static void IniEnable();

};