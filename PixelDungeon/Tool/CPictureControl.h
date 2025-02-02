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

private:
	void PassImageToParent(CImage* pImage, CString strRelativePath);

    DECLARE_MESSAGE_MAP()
};

