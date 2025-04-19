#include "body_splitter.h"

Body_Splitter::Body_Splitter(wxWindow* parent)
    :wxSplitterWindow(parent, wxID_ANY) {

    this->SetWindowStyleFlag(wxSP_LIVE_UPDATE); // ʵʱ�����϶����
    this->file_tree = new File_Tree(this);
    this->workarea_panel = new Workarea_Panel(this);
    this->SplitVertically(this->file_tree, this->workarea_panel);// �������ӵ��ָ�� 
    this->SetSashPosition(200);// ���÷ָ����ĳ�ʼλ��
    this->SetMinimumPaneSize(150);// ������С����С
}