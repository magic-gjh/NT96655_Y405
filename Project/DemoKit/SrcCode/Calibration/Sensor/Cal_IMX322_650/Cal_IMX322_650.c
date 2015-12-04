/**
    Calibration item

    @file       CalibrationItem.c
    @ingroup    calibration
    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/

#include "SxCmd.h"
#include "ae_task.h"
#include "awb_task.h"
#include "IPL_CAL_Debug.h"
#include "ipl_calrw.h"
#include "DxSensor.h"
#include "IPL_Cmd.h"
#include "Ae_api.h"
#include "Awb_api.h"
#include "SysCfg.h"
#include "Utility.h"
#include "SysKer.h"
#include "GxSystem.h"
#include "IPL_Cmd.h"
#include "SysCfg.h"
#include "ImgCaptureAPI.h"
#include "GxDisplay.h"
#include "IPL_Utility.h"
#include "stdio.h"
#include "IPL_Display.h"
#include "SysCommon.h"
#include "adc.h"
#include "UIResource.h"
#include "IPL_Calibration.h"
#include "CalLens.h"
#include "CalibrationAPI.h"
#include "IPL_calapi.h"
#if 0
- par
#endif
static BOOL m_CalOpen = FALSE;
static UINT32 m_Buf_StartAddr = 0;
static UINT32 m_Buf_Size = 0;
static UINT32 m_Header_Addr = 0;
static UINT32 m_Header_Size = 0;
UINT32 shift_prv_size = 0x2000000;
BOOL CAL_DP_B_DONE = FALSE;
#if 0
- static api
#endif

static CAL_DMA_RST _PS_R(UINT32 dmaAddr,UINT32 dmaSize);
extern void IQ_GetAdjustRst(CalIQInfo *CalData);

ER Set2Preview(UINT32 BufAddr,UINT32 BufSize)
{
    extern ER Set2Preview_IMX322_EVB_FF(UINT32 BufAddr,UINT32 BufSize) ;

    return Set2Preview_IMX322_EVB_FF(BufAddr,BufSize);
}

/*
    This function load pstore data into CAL POOL(DRAM)
*/
ER Cal_PS2MemPool(void)
{
    CAL_DMA_RST ps_rst = {0};

    ps_rst=_PS_R(OS_GetMempoolAddr(POOL_ID_CAL), POOL_SIZE_CAL);

    if(ps_rst.erCode==E_OK)
    {
        m_Header_Addr = OS_GetMempoolAddr(POOL_ID_CAL);
        m_Header_Size = POOL_SIZE_CAL - ps_rst.RemainSize;
    }
    else
    {
        return E_SYS;
    }

    return E_OK;
}

pCAL_ITEM_OBJ CAL_ITEM(void)
{

    extern pCAL_ITEM_OBJ CAL_ITEM_IMX322_EVB_FF(void) ;

    return CAL_ITEM_IMX322_EVB_FF();
}

/**
    read header and all sections from pstore to dram,
    if header not exit, init dram for header
*/
static CAL_DMA_RST _PS_R(UINT32 dmaAddr,UINT32 dmaSize)
{
    CAL_DMA_RST ps_rst = {0};

    ps_rst = CAL_RW()->fpSectionAllRead(dmaAddr,dmaSize);
    if(ps_rst.erCode != E_OK)
    {
        if(ps_rst.erCode == E_NOMEM)
            DBG_IND("buf not enougth\r\n");
        //pstore header
        ps_rst = CAL_RW()->fpSectionRead_Header(dmaAddr,dmaSize);
        if(ps_rst.erCode != E_OK)
        {
            DBG_IND("no header\r\n");
            //init should only be executed once at the very first execution of calibration
            ps_rst = CAL_RW()->fpInit(dmaAddr,dmaSize,NULL);
            if(ps_rst.erCode != E_OK)
            {
                DBG_ERR("pstore/dram init fail!\r\n");
            }
            else
            {
                //inorder to let all following calibration functions can be executed correctly.
                //because all cailbration functions' 1st operation is to read header
                if(CAL_RW()->fpSectionWrite_Header() != E_OK)
                    DBG_ERR("header write 2 PS fail\r\n");
            }
        }
    }
    return ps_rst;
}

#if 0
- extern api
#endif
/**
    open all ipl calibration resource
    requirement:
    1. sensor need to be closed.
    2. display ctrl task need to be closed.
    3. ipl_ctrl task need to be closed.
*/
ER Cal_IplOpen(UINT32 BufAddr, UINT32 BufSize)
{
    AE_OBJ_INFO AeObj = {0};
    AWB_OBJ_INFO AwbObj = {0};
//    SENSOR_INIT_OBJ SenObj_1,SenObj_2;
//    SENSOR_DRVTAB *pSenTab_1,*pSenTab_2;
    CAL_DMA_RST ps_rst;
    IPL_CHGMODE_DATA CmdData = {0};
    //TODO:
        //need to review if ipl_task had been used by UI, in running status!

    if(m_CalOpen)
    {
        DBG_ERR("\r\n");
        return E_SYS;
    }

    CmdData.Mode = IPL_MODE_OFF;
    IPL_SetCmd(IPL_SET_CHGMODE, (void *)&CmdData);
    IPL_WaitCmdFinish();

    if(IPL_IsOpen())
    {
        DBG_ERR("\r\n");
//        return E_SYS;
    }
    else
    {
        IPL_Open();
    }

//    IPL_Display_OpenTsk();
    //IPL_Display_CloseTsk();//sun 0605
    DBG_ERR("Cal_IplOpen IPL_Display_OpenTsk\r\n");
#if 0
    //open sensor
    SenObj_1 = DrvSensor_GetObj1st();
    pSenTab_1 = DrvSensor_GetTab1st();

    SenObj_2 = DrvSensor_GetObj2nd();
    pSenTab_2 = DrvSensor_GetTab2nd();

    if(pSenTab_1!= NULL)
    {
       if(Sensor_Open(SENSOR_ID_1, &SenObj_1, pSenTab_1))
       {
           DBG_ERR("\r\n");
           return E_SYS;
       }
    }
    if(pSenTab_2!= NULL)
    {
        if(Sensor_Open(SENSOR_ID_2, &SenObj_2, pSenTab_2))
        {
            DBG_ERR("\r\n");
            return E_SYS;
        }
    }
#endif
    //open ae
    AeObj.EventTab[IPL_UTI_CONV2_AE_ID(IPL_ID_1)].Start = AE_Init;
    AeObj.EventTab[IPL_UTI_CONV2_AE_ID(IPL_ID_1)].Process = AE_Process;
    AeObj.EventTab[IPL_UTI_CONV2_AE_ID(IPL_ID_1)].Pause = NULL;
    AeObj.ProcNum = 1;
    if(AE_Open(&AeObj))
    {
       DBG_ERR("\r\n");
       return E_SYS;
    }

    //open awb
    AwbObj.EventTab[IPL_UTI_CONV2_AWB_ID(IPL_ID_1)].Start = AWB_Init;
    AwbObj.EventTab[IPL_UTI_CONV2_AWB_ID(IPL_ID_1)].Process = AWB_Process;
    AwbObj.EventTab[IPL_UTI_CONV2_AWB_ID(IPL_ID_1)].Pause = NULL;
    AwbObj.ProcNum = 1;
    if(AWB_Open(&AwbObj))
    {
       DBG_ERR("\r\n");
       return E_SYS;
    }

    ps_rst = _PS_R(BufAddr,BufSize);
    if(ps_rst.erCode != E_OK)
        return E_SYS;
    m_Header_Addr = BufAddr;
    m_Header_Size = BufSize - ps_rst.RemainSize;

    m_Buf_StartAddr = ps_rst.RemainAddr;
    m_Buf_Size = ps_rst.RemainSize;

   if(Set2Preview(m_Buf_StartAddr,m_Buf_Size))
   {
       debug_err(("Set2Preview fail\r\n"));
   }
    m_CalOpen = TRUE;

    return E_OK;
}

ER Cal_IplClose(void)
{
    // UINT32 j = SENSOR_ID_MAX_NUM;
    if(!m_CalOpen)
    {
        DBG_ERR("\r\n");
        return E_SYS;
    }

    //stop pipeline ycc part
    CalApi()->fpSetMode(CAL_IPL_STOP,NULL,NULL);
    //pause sie
    CalApi()->fpSetMode(CAL_IPL_SIESTOP,NULL,NULL);
    //close sie
    CalApi()->fpSetMode(CAL_IPL_SIECLOSE,NULL,NULL);

    CalApi()->fpBufEvent()->fpUnInit(IPL_AlgGetUIInfo(IPL_SEL_PRVSENMODE));

    //close AWB
    AWB_Close();

    //close AE
    AE_Close();

    IPL_Close();

    /*
        we set ipl off mode before entry calibration,
        but when shutdown, ipl check status func will hang on,
        so we must set it  to the entry status.
    */
    DscPhoto_Open();

    /*
    IPL_Display_CloseTsk();

    //close sensor
    while(j--)
        Sensor_Close(j);
    */

    m_CalOpen = FALSE;
    return E_OK;
}

UINT32 Cal_WiteDebug_Info(UINT32 Addr)
{
    static CalIQInfo gCalIQData;
    static BOOL bRead=FALSE;
    char *print_status[2]={"DONE", "NG"};
    char *chk_status[4]={print_status[1], print_status[1], print_status[1], print_status[1]};    // cal item sequence: ECS, DP, ISO, AWB

    // run only once when system boot
    if(bRead==FALSE)
    {
        Cal_PS2MemPool();

        //spark IQ_GetAdjustRst(&gCalIQData);
        CAL_ITEM()->fpGetAdjustRst(NULL,&gCalIQData);

        bRead=TRUE;
    }

     if(gCalIQData.Cal_Status.ECS_STATUS == _CAL_OK)
    {
        chk_status[0]=print_status[0];
    }

    if(gCalIQData.Cal_Status.DP_STATUS== _CAL_OK)
    {
        chk_status[1]=print_status[0];
    }

    if(gCalIQData.Cal_Status.ISO_STATUS == _CAL_OK)
    {
        chk_status[2]=print_status[0];
    }

    if(gCalIQData.Cal_Status.AWB_STATUS == _CAL_OK)
    {
        chk_status[3]=print_status[0];
    }

    Addr += sprintf((char *)Addr, "\r\n\r\nCalibration Debug Info-OV5653-[NTK#RandyHo]<2013/08/22-19:48>");

    //Addr += sprintf((char *)Addr, "ECS=%s    DP=%s    ISO=%s    AWB=%s    \r\n\r\n", chk_status[0], chk_status[1], chk_status[2], chk_status[3]);

    Addr += sprintf((char *)Addr, "\r\n ECS=%s ", chk_status[0]);

    Addr += sprintf((char *)Addr, "\r\n DP=%s ", chk_status[1]);

    Addr += sprintf((char *)Addr, "\r\n ISO=%s  ", chk_status[2]);
    if(gCalIQData.Cal_Status.ISO_STATUS == _CAL_OK)
    {
        Addr += sprintf((char *)Addr, "Cal_Gain_Base=%d", gCalIQData.Cal_ISO_Base);
    }

    Addr += sprintf((char *)Addr, "\r\n AWB=%s  ", chk_status[3]);
    if(gCalIQData.Cal_Status.AWB_STATUS== _CAL_OK)
    {
        Addr += sprintf((char *)Addr, "Cal Rg=%d, Bg=%d", gCalIQData.Cal_AWB_Rg, gCalIQData.Cal_AWB_Bg);
    }

    Addr += sprintf((char *)Addr, "\r\n");

    return Addr;
}

ER Cal_SenDP(void)
{
    ER erCode=E_OK;

    CAL_DMA_RST ps_rst;

    Cal_IplOpen(OS_GetMempoolAddr(POOL_ID_APP)+(POOL_SIZE_APP/2),(POOL_SIZE_APP/2));

    if(!m_CalOpen)
    {
        DBG_ERR("Calibration not open yet\r\n");
        return E_SYS;
    }

    if(CAL_ITEM()->fpDP_Adj(CAL_UICB()->DP_W_UI,m_Buf_StartAddr+shift_prv_size,m_Buf_Size-shift_prv_size) == E_OK)
    {
        if(CAL_RW()->fpSectionWrite_Header() != E_OK)
            DBG_ERR("\r\n");

        //re-read all section because of maybe pstore have new section information after calibration function.
        ps_rst = _PS_R(m_Header_Addr,(m_Header_Size + m_Buf_Size));
        //must be return E_OK at each calibration function excluding Cal_IplOpen()
        if(ps_rst.erCode != E_OK)
            return E_SYS;
        m_Header_Size = (m_Header_Size + m_Buf_Size) - ps_rst.RemainSize;
        m_Buf_StartAddr = ps_rst.RemainAddr;
        m_Buf_Size = ps_rst.RemainSize;
    }

    Cal_IplClose();

    return  erCode;
}

ER Cal_SenDP_Bright(void)
{
    ER erCode=E_OK;

    CAL_DMA_RST ps_rst;
    DBG_ERR("\r\nNTK::::::::::::::::start Cal_SenDP_Bright  \r\n");  //lydia test

    Cal_IplOpen(OS_GetMempoolAddr(POOL_ID_APP)+(POOL_SIZE_APP/2),(POOL_SIZE_APP/2));
    if(!m_CalOpen)
    {
        DBG_ERR("Calibration not open yet\r\n");
        return E_SYS;
    }
debug_err((">>>>>DP Bright\r\n"));
    if(CAL_ITEM()->fpDP_Adj_Bright(CAL_UICB()->DP_W_UI,m_Buf_StartAddr+shift_prv_size,m_Buf_Size-shift_prv_size) == E_OK)
    {
        if(CAL_RW()->fpSectionWrite_Header() != E_OK)
            DBG_ERR("\r\n");
debug_err((">>>>>DP Bright end\r\n"));
        //re-read all section because of maybe pstore have new section information after calibration function.
        ps_rst = _PS_R(m_Header_Addr,(m_Header_Size + m_Buf_Size));
        //must be return E_OK at each calibration function excluding Cal_IplOpen()
        if(ps_rst.erCode != E_OK)
            return E_SYS;
        m_Header_Size = (m_Header_Size + m_Buf_Size) - ps_rst.RemainSize;
        m_Buf_StartAddr = ps_rst.RemainAddr;
        m_Buf_Size = ps_rst.RemainSize;

        CAL_DP_B_DONE = TRUE;
    }
debug_err((">>>>>DP Bright to Preview\r\n"));
    if(Set2Preview(m_Buf_StartAddr,m_Buf_Size))
        DBG_ERR("\r\n");

    return erCode;
}

ER Cal_SenDP_Dark(void)
{
    ER erCode=E_OK;

    CAL_DMA_RST ps_rst;

    Cal_IplOpen(OS_GetMempoolAddr(POOL_ID_APP)+(POOL_SIZE_APP/2),(POOL_SIZE_APP/2));

    if(!m_CalOpen)
    {
        DBG_ERR("Calibration not open yet\r\n");
        return E_SYS;
    }

    if (CAL_DP_B_DONE != TRUE)
    {
        DBG_ERR("DO DP Bright Adjust first\r\n");
        return E_SYS;
    }

debug_err((">>>>>DP Dark\r\n"));
    if(CAL_ITEM()->fpDP_Adj_Dark(CAL_UICB()->DP_W_UI,m_Buf_StartAddr+shift_prv_size,m_Buf_Size-shift_prv_size) == E_OK)
    {
        if(CAL_RW()->fpSectionWrite_Header() != E_OK)
            DBG_ERR("\r\n");
debug_err((">>>>>DP Dark end\r\n"));
        //re-read all section because of maybe pstore have new section information after calibration function.
        ps_rst = _PS_R(m_Header_Addr,(m_Header_Size + m_Buf_Size));
        //must be return E_OK at each calibration function excluding Cal_IplOpen()
        if(ps_rst.erCode != E_OK)
            return E_SYS;
        m_Header_Size = (m_Header_Size + m_Buf_Size) - ps_rst.RemainSize;
        m_Buf_StartAddr = ps_rst.RemainAddr;
        m_Buf_Size = ps_rst.RemainSize;

        CAL_DP_B_DONE = FALSE;
    }
debug_err((">>>>>DP Dark to preview\r\n"));
    if(Set2Preview(m_Buf_StartAddr,m_Buf_Size))
        DBG_ERR("\r\n");

    return erCode;
}

void Cal_SenAE_Check(void)
{
    CAL_DMA_RST ps_rst;

    Cal_IplOpen(OS_GetMempoolAddr(POOL_ID_APP)+(POOL_SIZE_APP/2),(POOL_SIZE_APP/2));

    if(!m_CalOpen)
    {
        DBG_ERR("Calibration not open yet\r\n");
        return;
    }

    if(CAL_ITEM()->fpAE_Check(CAL_UICB()->DP_W_UI,m_Buf_StartAddr+shift_prv_size,m_Buf_Size-shift_prv_size) == E_OK)
    {
        if(CAL_RW()->fpSectionWrite_Header() != E_OK)
            DBG_ERR("\r\n");

        //re-read all section because of maybe pstore have new section information after calibration function.
        ps_rst = _PS_R(m_Header_Addr,(m_Header_Size + m_Buf_Size));
        //must be return E_OK at each calibration function excluding Cal_IplOpen()
        if(ps_rst.erCode != E_OK)
            return;
        m_Header_Size = (m_Header_Size + m_Buf_Size) - ps_rst.RemainSize;
        m_Buf_StartAddr = ps_rst.RemainAddr;
        m_Buf_Size = ps_rst.RemainSize;
    }

    DBG_ERR("after cal seniso\r\n");

    if(Set2Preview(m_Buf_StartAddr,m_Buf_Size))
        DBG_ERR("\r\n");

}

ER Cal_SenISO(void)
{
    ER erCode=E_OK;
    CAL_DMA_RST ps_rst;

    Cal_IplOpen(OS_GetMempoolAddr(POOL_ID_APP)+(POOL_SIZE_APP/2),(POOL_SIZE_APP/2));

    if(!m_CalOpen)
    {
        DBG_ERR("Calibration not open yet\r\n");
        return E_SYS;
    }

    if(CAL_ITEM()->fpISOAdj(CAL_UICB()->ISO_UI,m_Buf_StartAddr+shift_prv_size,m_Buf_Size-shift_prv_size) == E_OK)
    {
        if(CAL_RW()->fpSectionWrite_Header() != E_OK)
            DBG_ERR("\r\n");

        //re-read all section because of maybe pstore have new section information after calibration function.
        ps_rst = _PS_R(m_Header_Addr,(m_Header_Size + m_Buf_Size));
        //must be return E_OK at each calibration function excluding Cal_IplOpen()
        if(ps_rst.erCode != E_OK)
            return E_SYS;
        m_Header_Size = (m_Header_Size + m_Buf_Size) - ps_rst.RemainSize;
        m_Buf_StartAddr = ps_rst.RemainAddr;
        m_Buf_Size = ps_rst.RemainSize;
    }

    DBG_ERR("after cal seniso\r\n");

    Cal_IplClose();

    return erCode;
}

ER Cal_SenMShutter(void)
{
    ER erCode=E_OK;

    CAL_DMA_RST ps_rst;
    if(!m_CalOpen)
    {
        DBG_ERR("Calibration not open yet\r\n");
        return E_SYS;
    }


    if(CAL_ITEM()->fpMSHUTAdj(CAL_UICB()->MSHUT_UI,m_Buf_StartAddr,m_Buf_Size) == E_OK)
    {
        if(CAL_RW()->fpSectionWrite_Header() != E_OK)
            DBG_ERR("\r\n");

        //re-read all section because of maybe pstore have new section information after calibration function.
        ps_rst = _PS_R(m_Header_Addr,(m_Header_Size + m_Buf_Size));
        //must be return E_OK at each calibration function excluding Cal_IplOpen()
        if(ps_rst.erCode != E_OK)
            return E_SYS;
        m_Header_Size = (m_Header_Size + m_Buf_Size) - ps_rst.RemainSize;
        m_Buf_StartAddr = ps_rst.RemainAddr;
        m_Buf_Size = ps_rst.RemainSize;
    }

    if(Set2Preview(m_Buf_StartAddr,m_Buf_Size))
            DBG_ERR("\r\n");

    return erCode;
}

ER Cal_SenAWB(void)
{
    ER erCode=E_OK;

    CAL_DMA_RST ps_rst;

    Cal_IplOpen(OS_GetMempoolAddr(POOL_ID_APP)+(POOL_SIZE_APP/2),(POOL_SIZE_APP/2));

    if(!m_CalOpen)
    {
        DBG_ERR("Calibration not open yet\r\n");
        return E_SYS;
    }

    if(CAL_ITEM()->fpAWBAdj(CAL_UICB()->AWB_UI,m_Buf_StartAddr+shift_prv_size,m_Buf_Size-shift_prv_size) == E_OK)
    {
        if(CAL_RW()->fpSectionWrite_Header() != E_OK)
            DBG_ERR("\r\n");

        //re-read all section because of maybe pstore have new section information after calibration function.
        ps_rst = _PS_R(m_Header_Addr,(m_Header_Size + m_Buf_Size));
        //must be return E_OK at each calibration function excluding Cal_IplOpen()
        if(ps_rst.erCode != E_OK)
            return E_SYS;
        m_Header_Size = (m_Header_Size + m_Buf_Size) - ps_rst.RemainSize;
        m_Buf_StartAddr = ps_rst.RemainAddr;
        m_Buf_Size = ps_rst.RemainSize;
    }

    Cal_IplClose();

    return erCode;
}

ER Cal_SenAWBGS(void)
{
    ER erCode=E_OK;

    CAL_DMA_RST ps_rst;

    Cal_IplOpen(OS_GetMempoolAddr(POOL_ID_APP)+(POOL_SIZE_APP/2),(POOL_SIZE_APP/2));

    if(!m_CalOpen)
    {
        DBG_ERR("Calibration not open yet\r\n");
        return E_SYS;
    }

    if(CAL_ITEM()->fpAWBGSAdj(CAL_UICB()->AWB_UI,m_Buf_StartAddr+shift_prv_size,m_Buf_Size-shift_prv_size) == E_OK)
    {
        if(CAL_RW()->fpSectionWrite_Header() != E_OK)
            DBG_ERR("\r\n");

        //re-read all section because of maybe pstore have new section information after calibration function.
        ps_rst = _PS_R(m_Header_Addr,(m_Header_Size + m_Buf_Size));
        //must be return E_OK at each calibration function excluding Cal_IplOpen()
        if(ps_rst.erCode != E_OK)
            return E_SYS;
        m_Header_Size = (m_Header_Size + m_Buf_Size) - ps_rst.RemainSize;
        m_Buf_StartAddr = ps_rst.RemainAddr;
        m_Buf_Size = ps_rst.RemainSize;
    }

    Cal_IplClose();

    return erCode;
}

ER Cal_SenECS(void)
{
    ER erCode=E_OK;
    CAL_DMA_RST ps_rst;

    if(m_Header_Addr==0)
    {
        Cal_PS2MemPool();
    }

    Cal_IplOpen(OS_GetMempoolAddr(POOL_ID_APP)+(POOL_SIZE_APP/2),(POOL_SIZE_APP/2));

    if(!m_CalOpen)
    {
        DBG_ERR("Calibration not open yet\r\n");
        return E_SYS;
    }

    if(CAL_ITEM()->fpECSAdj(CAL_UICB()->ECS_UI,m_Buf_StartAddr+shift_prv_size,m_Buf_Size-shift_prv_size) == E_OK)
    {
        if(CAL_RW()->fpSectionWrite_Header() != E_OK)
            DBG_ERR("\r\n");

        //re-read all section because of maybe pstore have new section information after calibration function.
        ps_rst = _PS_R(m_Header_Addr,(m_Header_Size + m_Buf_Size));
        //must be return E_OK at each calibration function excluding Cal_IplOpen()
        if(ps_rst.erCode != E_OK)
            return E_SYS;
        m_Header_Size = (m_Header_Size + m_Buf_Size) - ps_rst.RemainSize;
        m_Buf_StartAddr = ps_rst.RemainAddr;
        m_Buf_Size = ps_rst.RemainSize;
    }

    Cal_IplClose();

    return erCode;
}

ER Cal_Get_DNP_Status(void)
{
    ER erCode=E_OK;
    CalIQInfo gCalIQData;


    Cal_PS2MemPool();

    //spark
    //IQ_GetAdjustRst(&gCalIQData);
    CAL_ITEM()->fpGetAdjustRst(NULL,&gCalIQData);


    CAL_UICB()->CAL_STATUS_UI(UICB_START, "CAL STATUS", 0);

    if(gCalIQData.Cal_Status.ECS_STATUS==_CAL_OK)
    {
        CAL_UICB()->CAL_STATUS_UI(UICB_PROCESSING, "1 ECS  OK", 0);
    }
    else
    {
        CAL_UICB()->CAL_STATUS_UI(UICB_PROCESSING, "1 ECS   NG", 0);
    }

    if(gCalIQData.Cal_Status.DP_STATUS==_CAL_OK)
    {
        CAL_UICB()->CAL_STATUS_UI(UICB_PROCESSING, "2 DP   OK", 0);
    }
    else
    {
        CAL_UICB()->CAL_STATUS_UI(UICB_PROCESSING, "2 DP   NG", 0);
    }

    if(gCalIQData.Cal_Status.ISO_STATUS==_CAL_OK)
    {
        CAL_UICB()->CAL_STATUS_UI(UICB_PROCESSING, "3 ISO   OK", 0);
    }
    else
    {
        CAL_UICB()->CAL_STATUS_UI(UICB_PROCESSING, "3 ISO   NG", 0);
    }

    if(gCalIQData.Cal_Status.AWB_STATUS==_CAL_OK)
    {
        CAL_UICB()->CAL_STATUS_UI(UICB_PROCESSING, "4 AWB   OK", 0);
    }
    else
    {
        CAL_UICB()->CAL_STATUS_UI(UICB_PROCESSING, "4 AWB   NG", 0);
    }

#if 0
    if(CalLens_IsFarAdjust())
    {
        Cal_ShowStringWithColor("Far Adjust : OK",20, 50, 3);
    }
    else
    {
        Cal_ShowStringWithColor("Far Adjust : NG",20, 50, 3);
    }

    if(CalLens_IsNearAdjust())
    {
        Cal_ShowStringWithColor("Near Adjust : OK",20, 70, 3);
    }
    else
    {
        Cal_ShowStringWithColor("Near Adjust : NG",20, 70, 3);
    }
#endif

    CAL_UICB()->CAL_STATUS_UI(UICB_WAITKEY, NULL, 0);

    return erCode;
}

void Cal_SenDNP(void)
{
    Cal_IplOpen(OS_GetMempoolAddr(POOL_ID_APP)+(POOL_SIZE_APP/2),(POOL_SIZE_APP/2));
//    Cal_SenDP();
    Cal_SenDP_Dark();
    Cal_SenECS();
    Cal_SenISO();
    Cal_SenAWB();
    Cal_Get_DNP_Status();
}
