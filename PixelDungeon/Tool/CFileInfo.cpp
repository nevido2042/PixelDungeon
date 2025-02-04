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

void CFileInfo::DirInfoExtraction(const wstring& wstrPath, list<IMGPATH*>& rPathInfoList)
{
    // CFileFind : mfc ���� �����ϴ� ���� �� ��� ���� ���� Ŭ����
    CFileFind       Find;

    wstring     wstrFilePath = wstrPath + L"\\*.*";
    // FindFile : �Ű� ������ ���޵� ��ο� ������ ������ Ȯ���ϴ� �Լ�

    BOOL    bContinue = Find.FindFile(wstrFilePath.c_str());

    while (bContinue)
    {
        // FindNextFile : ���� ��� �ȿ��� ���� ������ ã�� �Լ�, ã�� �� ���� ��� �Ǵ� ������ ����� ã���� ��� 0�� ��ȯ

        bContinue = Find.FindNextFile();

        if (Find.IsDots())
            continue;

        else if (Find.IsDirectory())
        {
            DirInfoExtraction(wstring(Find.GetFilePath()), rPathInfoList);
        }
        else
        {
            if (Find.IsSystem())
                continue;

            IMGPATH* pImgPath = new IMGPATH;
            TCHAR   szPathBuf[MAX_PATH] = L"";

            lstrcpy(szPathBuf, Find.GetFilePath().GetString());

            PathRemoveFileSpec(szPathBuf);

            pImgPath->iCount = DirFileCount(szPathBuf);

            // 1. ������ �̸��� ������

            // GetFileTitle : ���� �̸��� ������ �Լ�
            wstring     wstrTextureName = Find.GetFileTitle().GetString();

            // 2. ������ �̸� �� ������ ���ڸ� �߶���
            // substr(����, ��) : ���ۿ��� ���� �ش��ϴ� ���ڸ� �߶�
            wstrTextureName = wstrTextureName.substr(0, wstrTextureName.size() - 1) + L"%d.png";

            // 3. ������ ���� ��ġ�� %d�� ��������      
            //     PathCombine(��� ��, src1, src2)
            // src1�� src2�� �̾� �ٿ� �ϼ��� ��θ� ��� ���� ��ġ�� ����
            // src1�� src2 ���̿� \\ �ڵ����� ����
            PathCombine(szPathBuf, szPathBuf, wstrTextureName.c_str());

            pImgPath->wstrPath = Convert_RelativePath(szPathBuf);
            //D:\����ȯ\151��\Frame151\Texture\Stage\Effect\BossMultiAttack\BossMultiAttack%d.png

            PathRemoveFileSpec(szPathBuf);
            //..\Frame151\Texture\Stage\Effect\BossMultiAttack

            // PathFindFileName : ���ϸ��� ã�ų� ���� ������ �������� ã�Ƴ�
            pImgPath->wstrStateKey = PathFindFileName(szPathBuf);

            PathRemoveFileSpec(szPathBuf);
            //..\Frame151\Texture\Stage\Effect\

            // PathFindFileName : ���ϸ��� ã�ų� ���� ������ �������� ã�Ƴ�
            pImgPath->wstrObjKey = PathFindFileName(szPathBuf);

            rPathInfoList.push_back(pImgPath);

            bContinue = 0;
        }
    }
}

int CFileInfo::DirFileCount(const wstring& wstrPath)
{
    // CFileFind : mfc ���� �����ϴ� ���� �� ��� ���� ���� Ŭ����
    CFileFind       Find;

    wstring     wstrFilePath = wstrPath + L"\\*.*";

    // FindFile : �Ű� ������ ���޵� ��ο� ������ ������ Ȯ���ϴ� �Լ�

    BOOL    bContinue = Find.FindFile(wstrFilePath.c_str());

    int     iFileCnt(0);

    while (bContinue)
    {
        // FindNextFile : ���� ��� �ȿ��� ���� ������ ã�� �Լ�, ã�� �� ���� ��� �Ǵ� ������ ����� ã���� ��� 0�� ��ȯ

        bContinue = Find.FindNextFile();

        if (Find.IsDots())
            continue;

        if (Find.IsSystem())
            continue;

        iFileCnt++;
    }

    return iFileCnt;

}
