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
	// �˵���
	static Menu_Bar* menu_bar;

	// ������
	static Tool_Bar* tool_bar;

	// ���ڷָ���
	static Body_Splitter* body_splitter;

	// ������Ϣ
	//static ErrorLogPanel*log_panel;
	static ErrorLogPanel2* log_panel2;

	// �ײ���Ϣ��
	static Bottom_Message* bottom_message;

	// �����ڲ���
	static wxBoxSizer* app_box;		

	// ����ģʽ
	static int work_mode;

	// ��Ϣ��
	static Proj_Info* proj_info;

	static void IniEnable();

};