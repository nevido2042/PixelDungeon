#pragma once
#include "CGameObject.h"
class CTile :
    public CGameObject
{
public:
    CTile();
    virtual ~CTile();
public:
    void Initialize() override;
    void Late_Update() override;
    void Render() override;
public:
    void Set_DrawID(BYTE _byDrawID) { m_byDrawID = _byDrawID; }
    void Set_Option(BYTE _byOption) { m_byOption = _byOption; }
    void Set_Tile_Index(int _index) { m_index = _index; }
    void Set_PrevTile_Index(int _iPrvindex) { m_iPrvindex = _iPrvindex; }

    int Get_Tile_Index() { return m_index; }
    int Get_PrevTile_Index() { return m_iPrvindex; }

    BYTE Get_DrawID()
    {
        return m_byDrawID;
    }
    BYTE Get_Option()
    {
        return m_byOption;
    }
private:
    BYTE    m_byDrawID;
    BYTE    m_byOption;

    int m_index;
    int m_iPrvindex;
    
};

