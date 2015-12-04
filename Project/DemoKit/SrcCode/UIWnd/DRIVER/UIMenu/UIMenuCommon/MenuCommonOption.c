//This source code is generated by UI Designer Studio.

#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIGraphics.h"
#include "NVTToolCommand.h"
#include "MenuCommonOptionRes.c"
#include "UIFlow.h"

//---------------------MenuCommonOptionCtrl Control List---------------------------
CTRL_LIST_BEGIN(MenuCommonOption)
CTRL_LIST_ITEM(MenuCommonOption_Panel)
CTRL_LIST_END

//----------------------MenuCommonOptionCtrl Event---------------------------
INT32 MenuCommonOption_OnOpen(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_OnClose(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_OnKeyLeft(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_OnKeyMenu(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_OnKeyMode(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_OnKeyShutter2(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(MenuCommonOption)
EVENT_ITEM(NVTEVT_OPEN_WINDOW,MenuCommonOption_OnOpen)
EVENT_ITEM(NVTEVT_CLOSE_WINDOW,MenuCommonOption_OnClose)
EVENT_ITEM(NVTEVT_KEY_LEFT,MenuCommonOption_OnKeyLeft)
EVENT_ITEM(NVTEVT_KEY_MENU,MenuCommonOption_OnKeyMenu)
EVENT_ITEM(NVTEVT_KEY_MODE,MenuCommonOption_OnKeyMode)
//EVENT_ITEM(NVTEVT_KEY_SHUTTER2,MenuCommonOption_OnKeyShutter2)
EVENT_END

//extern void MenuCommon_CalcPageInfo(VControl *pCtrl);

void MenuCommonOption_UpdateContent(TM_MENU* pMenu)
{
    UINT32      i;
    TM_PAGE*    pPage;
    TM_ITEM*    pItem;
    TM_OPTION*  pOption;

    pPage = &pMenu->pPages[pMenu->SelPage];
    pItem = &pPage->pItems[pPage->SelItem];

    UxMenu_SetData(&MenuCommonOption_MenuCtrl, MNU_TOTITM, pItem->Count);
    for (i = 0; i < pItem->Count; i++)
    {
        pOption = &pItem->pOptions[i];
        UxMenu_SetItemData(&MenuCommonOption_MenuCtrl, i, MNUITM_STATUS, STATUS_ENABLE);
#if (UPDATE_CFG == UPDATE_CFG_YES)
        if (pOption->TextId==0 && pOption->IconId==0)
        {
            UxMenu_SetItemData(&MenuCommonOption_MenuCtrl, i, MNUITM_STATUS, STATUS_DISABLE);
        } else {
            UxMenu_SetItemData(&MenuCommonOption_MenuCtrl, i, MNUITM_STRID,  pOption->TextId);
            UxMenu_SetItemData(&MenuCommonOption_MenuCtrl, i, MNUITM_ICONID, pOption->IconId);
        }
#else
        UxMenu_SetItemData(&MenuCommonOption_MenuCtrl, i, MNUITM_STRID,  pOption->TextId);
        UxMenu_SetItemData(&MenuCommonOption_MenuCtrl, i, MNUITM_ICONID, pOption->IconId);
#endif
    }

    UxCtrl_SetDirty(&MenuCommonOptionCtrl, TRUE);
    //Ux_RedrawAllWind();
}

INT32 MenuCommonOption_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    TM_MENU*    pMenu = TM_GetMenu();
    TM_PAGE*    pPage;
    TM_ITEM*    pItem;

    pPage = &pMenu->pPages[pMenu->SelPage];
    pItem = &pPage->pItems[pPage->SelItem];

    MenuCommonOption_UpdateContent(pMenu);
    UxMenu_SetData(&MenuCommonOption_MenuCtrl, MNU_CURITM, SysGetFlag(pItem->SysFlag));

    Ux_DefaultEvent(pCtrl,NVTEVT_OPEN_WINDOW,paramNum,paramArray);
    return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_DefaultEvent(pCtrl,NVTEVT_CLOSE_WINDOW,paramNum,paramArray);
    return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_OnKeyLeft(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_CloseWindow(pCtrl,0); // close option menu
    return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_OnKeyMenu(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_CloseWindow(&MenuCommonItemCtrl,0); // close whole tab menu
    return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_OnKeyMode(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_SendEvent(&UISetupObjCtrl,NVTEVT_EXE_CHANGEDSCMODE,1,DSCMODE_CHGTO_NEXT);
    return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_OnKeyShutter2(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_SendEvent(&UISetupObjCtrl,NVTEVT_FORCETO_LIVEVIEW_MODE,0);
    return NVTEVT_CONSUME;
}
//---------------------MenuCommonOption_PanelCtrl Control List---------------------------
CTRL_LIST_BEGIN(MenuCommonOption_Panel)
CTRL_LIST_ITEM(MenuCommonOption_Menu)
CTRL_LIST_END

//----------------------MenuCommonOption_PanelCtrl Event---------------------------
EVENT_BEGIN(MenuCommonOption_Panel)
EVENT_END

//----------------------MenuCommonOption_MenuCtrl Event---------------------------
INT32 MenuCommonOption_Menu_OnKeyUp(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_Menu_OnKeyDown(VControl *, UINT32, UINT32 *);
INT32 MenuCommonOption_Menu_OnKeyEnter(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(MenuCommonOption_Menu)
EVENT_ITEM(NVTEVT_KEY_UP,MenuCommonOption_Menu_OnKeyUp)
EVENT_ITEM(NVTEVT_KEY_DOWN,MenuCommonOption_Menu_OnKeyDown)
//EVENT_ITEM(NVTEVT_KEY_ENTER,MenuCommonOption_Menu_OnKeyEnter)
EVENT_ITEM(NVTEVT_KEY_SHUTTER2,MenuCommonOption_Menu_OnKeyEnter)
EVENT_END

INT32 MenuCommonOption_Menu_OnKeyUp(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_SendEvent(pCtrl,NVTEVT_PREVIOUS_ITEM,0);
    return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_Menu_OnKeyDown(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_SendEvent(pCtrl,NVTEVT_NEXT_ITEM,0);
    return NVTEVT_CONSUME;
}
INT32 MenuCommonOption_Menu_OnKeyEnter(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    TM_MENU*    pMenu = TM_GetMenu();
    TM_PAGE*    pPage;
    TM_ITEM*    pItem;
    UINT32      uiSelOption;

    pPage = &pMenu->pPages[pMenu->SelPage];
    pItem = &pPage->pItems[pPage->SelItem];

    uiSelOption = UxMenu_GetData(&MenuCommonOption_MenuCtrl, MNU_CURITM); // selected option

    // set system flag by selected Option
    SysSetFlag(pItem->SysFlag, uiSelOption);

    // Close current UI Window now
    Ux_CloseWindow(&MenuCommonOptionCtrl, 2,pItem->ItemId,uiSelOption);

    // notify upper layer the Option had been confirmed
    TM_MENU_CALLBACK(pMenu, TMM_CONFIRM_OPTION, MAKE_LONG(pItem->ItemId, uiSelOption));


    return NVTEVT_CONSUME;
}
