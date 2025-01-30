#pragma once
#include <afxwin.h>
class CPictureControl :
    public CStatic
{
public:
    CPictureControl() {}
    virtual ~CPictureControl() {}

protected:
    afx_msg void OnDropFiles(HDROP hDropInfo);

    DECLARE_MESSAGE_MAP()
};

