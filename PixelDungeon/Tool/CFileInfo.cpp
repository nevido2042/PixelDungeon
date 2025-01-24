#include "pch.h"
#include "CFileInfo.h"

CString CFileInfo::Convert_RelativePath(CString strFullPath)
{
    TCHAR   szRelativePath[MAX_PATH] = L"";
    TCHAR   szCurDirPath[MAX_PATH] = L"";

    GetCurrentDirectory(MAX_PATH, szCurDirPath);

    // PathRelativePathTo : ���� ��θ� ��� ��η� ��ȯ

    // szCurDirPath���� strFullPath�� ���� ��� ��θ� ���ؼ� szRelativePath ����
    // ���� ���� : ���� ����̺� �ȿ����� ��� ������ ����

    PathRelativePathTo(szRelativePath,
                        szCurDirPath,
                        FILE_ATTRIBUTE_DIRECTORY,
                        strFullPath.GetString(),
                        FILE_ATTRIBUTE_DIRECTORY);

    return CString(szRelativePath);
}
