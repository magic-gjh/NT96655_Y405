
#include "display.h"
#include "dispdev_ifmi.h"
#include "dispdev_panlcomn.h"

/*Used in MI format*/
const tPANEL_CMD tCmdModeMiFmt8[] =
{
    //************* Start Initial Sequence **********//
    /*
    {MICMD_CMD, 0x11},//wr_comm(0x11); //Exit Sleep
    {CMDDELAY_MS, 10},//Delay(100);

    {MICMD_CMD, 0x26},//wr_comm(0x26); //Set Default Gamma
    {MICMD_DATA, 0x01},// wr_dat(0x01);
    */

    {MICMD_CMD, 0xF0},{MICMD_CMD, 0x00},{MICMD_DATA, 0x55},//Set_LCD_16B_REG(0xF000,0x55);
    {MICMD_CMD, 0xF0},{MICMD_CMD, 0x01},{MICMD_DATA, 0xaa},//Set_LCD_16B_REG(0xF001,0xAA);
    {MICMD_CMD, 0xF0},{MICMD_CMD, 0x02},{MICMD_DATA, 0x52},//Set_LCD_16B_REG(0xF002,0x52);
    {MICMD_CMD, 0xF0},{MICMD_CMD, 0x03},{MICMD_DATA, 0x08},//Set_LCD_16B_REG(0xF003,0x08);
    {MICMD_CMD, 0xF0},{MICMD_CMD, 0x04},{MICMD_DATA, 0x01},//Set_LCD_16B_REG(0xF004,0x01);//command page 1

    //);AVDD:);manual,);
    {MICMD_CMD, 0xb6},{MICMD_CMD, 0x00},{MICMD_DATA, 0x34},//Set_LCD_16B_REG(0xB600,0x34);
    {MICMD_CMD, 0xb6},{MICMD_CMD, 0x01},{MICMD_DATA, 0x34},//Set_LCD_16B_REG(0xB601,0x34);
    {MICMD_CMD, 0xb6},{MICMD_CMD, 0x02},{MICMD_DATA, 0x34},//Set_LCD_16B_REG(0xB602,0x34);
    {MICMD_CMD, 0xb0},{MICMD_CMD, 0x00},{MICMD_DATA, 0x09},//Set_LCD_16B_REG(0xB000,0x09);//AVDD
    {MICMD_CMD, 0xb0},{MICMD_CMD, 0x01},{MICMD_DATA, 0x09},//Set_LCD_16B_REG(0xB001,0x09);
    {MICMD_CMD, 0xb0},{MICMD_CMD, 0x02},{MICMD_DATA, 0x09},//Set_LCD_16B_REG(0xB002,0x09);

    //);AVEE:);manual,);6V);
    {MICMD_CMD, 0xb7},{MICMD_CMD, 0x00},{MICMD_DATA, 0x24},//Set_LCD_16B_REG(0xB700,0x24);
    {MICMD_CMD, 0xb7},{MICMD_CMD, 0x01},{MICMD_DATA, 0x24},//Set_LCD_16B_REG(0xB701,0x24);
    {MICMD_CMD, 0xb7},{MICMD_CMD, 0x02},{MICMD_DATA, 0x24},//Set_LCD_16B_REG(0xB702,0x24);
    {MICMD_CMD, 0xb1},{MICMD_CMD, 0x00},{MICMD_DATA, 0x09},//Set_LCD_16B_REG(0xB100,0x09);
    {MICMD_CMD, 0xb1},{MICMD_CMD, 0x01},{MICMD_DATA, 0x09},//Set_LCD_16B_REG(0xB101,0x09);
    {MICMD_CMD, 0xb1},{MICMD_CMD, 0x02},{MICMD_DATA, 0x09},//Set_LCD_16B_REG(0xB102,0x09);

    //Power);Control);for);VCL);
    {MICMD_CMD, 0xb8},{MICMD_CMD, 0x00},{MICMD_DATA, 0x34},//Set_LCD_16B_REG(0xB800,0x34);
    {MICMD_CMD, 0xb2},{MICMD_CMD, 0x00},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xB200,0x00);

    //);VGH:);Clamp);Enable,);
    {MICMD_CMD, 0xb9},{MICMD_CMD, 0x00},{MICMD_DATA, 0x24},//Set_LCD_16B_REG(0xB900,0x24);
    {MICMD_CMD, 0xb9},{MICMD_CMD, 0x01},{MICMD_DATA, 0x24},//Set_LCD_16B_REG(0xB901,0x24);
    {MICMD_CMD, 0xb9},{MICMD_CMD, 0x02},{MICMD_DATA, 0x24},//Set_LCD_16B_REG(0xB902,0x24);
    {MICMD_CMD, 0xb3},{MICMD_CMD, 0x00},{MICMD_DATA, 0x05},//Set_LCD_16B_REG(0xB300,0x05);
    {MICMD_CMD, 0xb3},{MICMD_CMD, 0x01},{MICMD_DATA, 0x05},//Set_LCD_16B_REG(0xB301,0x05);
    {MICMD_CMD, 0xb3},{MICMD_CMD, 0x02},{MICMD_DATA, 0x05},//Set_LCD_16B_REG(0xB302,0x05);
    {MICMD_CMD, 0xbf},{MICMD_CMD, 0x00},{MICMD_DATA, 0x01},//Set_LCD_16B_REG(0xBF00,0x01);

    //);VGL(LVGL));
    {MICMD_CMD, 0xba},{MICMD_CMD, 0x00},{MICMD_DATA, 0x34},//Set_LCD_16B_REG(0xBA00,0x34);
    {MICMD_CMD, 0xba},{MICMD_CMD, 0x01},{MICMD_DATA, 0x34},//Set_LCD_16B_REG(0xBA01,0x34);
    {MICMD_CMD, 0xba},{MICMD_CMD, 0x02},{MICMD_DATA, 0x34},//Set_LCD_16B_REG(0xBA02,0x34);

    //);VGL_REG(VGLO));
    {MICMD_CMD, 0xb5},{MICMD_CMD, 0x00},{MICMD_DATA, 0x0b},//Set_LCD_16B_REG(0xB500,0x0B);
    {MICMD_CMD, 0xb5},{MICMD_CMD, 0x01},{MICMD_DATA, 0x0b},//Set_LCD_16B_REG(0xB501,0x0B);
    {MICMD_CMD, 0xb5},{MICMD_CMD, 0x02},{MICMD_DATA, 0x0b},//Set_LCD_16B_REG(0xB502,0x0B);

    //);VGMP/VGSP);
    {MICMD_CMD, 0xbc},{MICMD_CMD, 0x00},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xBC00,0X00);
    {MICMD_CMD, 0xbc},{MICMD_CMD, 0x01},{MICMD_DATA, 0xa3},//Set_LCD_16B_REG(0xBC01,0xA3);
    {MICMD_CMD, 0xbc},{MICMD_CMD, 0x02},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xBC02,0X00);

    //VGMN/VGSN);
    {MICMD_CMD, 0xbd},{MICMD_CMD, 0x00},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xBD00,0x00);
    {MICMD_CMD, 0xbd},{MICMD_CMD, 0x01},{MICMD_DATA, 0xa3},//Set_LCD_16B_REG(0xBD01,0xA3);
    {MICMD_CMD, 0xbd},{MICMD_CMD, 0x02},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xBD02,0x00);

    //);VCOM=0.1);
    {MICMD_CMD, 0xbe},{MICMD_CMD, 0x00},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xBE00,0x00);
    {MICMD_CMD, 0xbe},{MICMD_CMD, 0x01},{MICMD_DATA, 0x50},//Set_LCD_16B_REG(0xBE01,0x50);

    //Above was page 1 register setting
    //Enable);Page0);
    {MICMD_CMD, 0xf0},{MICMD_CMD, 0x00},{MICMD_DATA, 0x55},//Set_LCD_16B_REG(0xF000,0x55);
    {MICMD_CMD, 0xf0},{MICMD_CMD, 0x01},{MICMD_DATA, 0xaa},//Set_LCD_16B_REG(0xF001,0xAA);
    {MICMD_CMD, 0xf0},{MICMD_CMD, 0x02},{MICMD_DATA, 0x52},//Set_LCD_16B_REG(0xF002,0x52);
    {MICMD_CMD, 0xf0},{MICMD_CMD, 0x03},{MICMD_DATA, 0x08},//Set_LCD_16B_REG(0xF003,0x08);
    {MICMD_CMD, 0xf0},{MICMD_CMD, 0x04},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xF004,0x00);//Page 0

    //RGB);I/F);Setting);
    {MICMD_CMD, 0xb0},{MICMD_CMD, 0x00},{MICMD_DATA, 0x08},//Set_LCD_16B_REG(0xB000,0x08);
    {MICMD_CMD, 0xb0},{MICMD_CMD, 0x01},{MICMD_DATA, 0x05},//Set_LCD_16B_REG(0xB001,0x05);//RGB interface setting
    {MICMD_CMD, 0xb0},{MICMD_CMD, 0x02},{MICMD_DATA, 0x02},//Set_LCD_16B_REG(0xB002,0x02);
    {MICMD_CMD, 0xb0},{MICMD_CMD, 0x03},{MICMD_DATA, 0x05},//Set_LCD_16B_REG(0xB003,0x05);
    {MICMD_CMD, 0xb0},{MICMD_CMD, 0x04},{MICMD_DATA, 0x02},//Set_LCD_16B_REG(0xB004,0x02);

    //);SDT:);
    {MICMD_CMD, 0xb6},{MICMD_CMD, 0x00},{MICMD_DATA, 0x0a},//Set_LCD_16B_REG(0xB600,0x0A);//data hold time

    //);Gate);EQ:);
    {MICMD_CMD, 0xb7},{MICMD_CMD, 0x00},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xB700,0x00);//EQ func
    {MICMD_CMD, 0xb7},{MICMD_CMD, 0x01},{MICMD_DATA, 0x70},//Set_LCD_16B_REG(0xB701,0x70);

    //);Source);EQ);
    {MICMD_CMD, 0xb8},{MICMD_CMD, 0x00},{MICMD_DATA, 0x01},//Set_LCD_16B_REG(0xB800,0x01);
    {MICMD_CMD, 0xb8},{MICMD_CMD, 0x01},{MICMD_DATA, 0x05},//Set_LCD_16B_REG(0xB801,0x05);
    {MICMD_CMD, 0xb8},{MICMD_CMD, 0x02},{MICMD_DATA, 0x05},//Set_LCD_16B_REG(0xB802,0x05);
    {MICMD_CMD, 0xb8},{MICMD_CMD, 0x03},{MICMD_DATA, 0x05},//Set_LCD_16B_REG(0xB803,0x05);

    //Inversion:);Column);inversion);(NVT));
    {MICMD_CMD, 0xbc},{MICMD_CMD, 0x00},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xBC00,0x00);//column inversion in all modes
    {MICMD_CMD, 0xbc},{MICMD_CMD, 0x01},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xBC01,0x00);
    {MICMD_CMD, 0xbc},{MICMD_CMD, 0x02},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xBC02,0x00);

    //BOE's);Setting);(default));
    {MICMD_CMD, 0xcc},{MICMD_CMD, 0x00},{MICMD_DATA, 0x03},//Set_LCD_16B_REG(0xCC00,0x03);
    {MICMD_CMD, 0xcc},{MICMD_CMD, 0x01},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xCC01,0x00);
    {MICMD_CMD, 0xcc},{MICMD_CMD, 0x02},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xCC02,0x00);

    //Display);Timing);
    {MICMD_CMD, 0xbd},{MICMD_CMD, 0x00},{MICMD_DATA, 0x01},//Set_LCD_16B_REG(0xBD00,0x01);
    {MICMD_CMD, 0xbd},{MICMD_CMD, 0x01},{MICMD_DATA, 0x84},//Set_LCD_16B_REG(0xBD01,0x84);
    {MICMD_CMD, 0xbd},{MICMD_CMD, 0x02},{MICMD_DATA, 0x07},//Set_LCD_16B_REG(0xBD02,0x07);
    {MICMD_CMD, 0xbd},{MICMD_CMD, 0x03},{MICMD_DATA, 0x31},//Set_LCD_16B_REG(0xBD03,0x31);
    {MICMD_CMD, 0xbd},{MICMD_CMD, 0x04},{MICMD_DATA, 0x00},//Set_LCD_16B_REG(0xBD04,0x00);
    {MICMD_CMD, 0xba},{MICMD_CMD, 0x00},{MICMD_DATA, 0x01},//Set_LCD_16B_REG(0xBA00,0x01);
    {MICMD_CMD, 0xff},{MICMD_CMD, 0x00},{MICMD_DATA, 0xaa},//Set_LCD_16B_REG(0xFF00,0xAA);
    {MICMD_CMD, 0xff},{MICMD_CMD, 0x01},{MICMD_DATA, 0x55},//Set_LCD_16B_REG(0xFF01,0x55);
    {MICMD_CMD, 0xff},{MICMD_CMD, 0x02},{MICMD_DATA, 0x25},//Set_LCD_16B_REG(0xFF02,0x25);
    {MICMD_CMD, 0xff},{MICMD_CMD, 0x03},{MICMD_DATA, 0x01},//Set_LCD_16B_REG(0xFF03,0x01);


    {MICMD_CMD, 0x36},{MICMD_CMD, 0x00},{MICMD_DATA, 0x81},   //control the high 8 bit

#if 1 // 480x800
    {MICMD_CMD, 0x2a},{MICMD_CMD, 0x00},{MICMD_DATA, 0x00},
    {MICMD_CMD, 0x2a},{MICMD_CMD, 0x01},{MICMD_DATA, 0x00},// horizonal st 0 end 480
    {MICMD_CMD, 0x2a},{MICMD_CMD, 0x02},{MICMD_DATA, 0x01},//{MICMD_DATA, 0x01},
    {MICMD_CMD, 0x2a},{MICMD_CMD, 0x03},{MICMD_DATA, 0xdf},//{MICMD_DATA, 0xe0},
    {MICMD_CMD, 0x2b},{MICMD_CMD, 0x00},{MICMD_DATA, 0x00},
    {MICMD_CMD, 0x2b},{MICMD_CMD, 0x01},{MICMD_DATA, 0x00},// vertical st 0 end 800
    {MICMD_CMD, 0x2b},{MICMD_CMD, 0x02},{MICMD_DATA, 0x03},//{MICMD_DATA, 0x03},
    {MICMD_CMD, 0x2b},{MICMD_CMD, 0x03},{MICMD_DATA, 0x1f},//{MICMD_DATA, 0x20},
#else // 800x480
    {MICMD_CMD, 0x2a},{MICMD_CMD, 0x00},{MICMD_DATA, 0x00},
    {MICMD_CMD, 0x2a},{MICMD_CMD, 0x01},{MICMD_DATA, 0x00},// horizonal st 0 end 480
    {MICMD_CMD, 0x2a},{MICMD_CMD, 0x02},{MICMD_DATA, 0x03},//{MICMD_DATA, 0x01},
    {MICMD_CMD, 0x2a},{MICMD_CMD, 0x03},{MICMD_DATA, 0x1f},//{MICMD_DATA, 0xe0},
    {MICMD_CMD, 0x2b},{MICMD_CMD, 0x00},{MICMD_DATA, 0x00},
    {MICMD_CMD, 0x2b},{MICMD_CMD, 0x01},{MICMD_DATA, 0x00},// vertical st 0 end 800
    {MICMD_CMD, 0x2b},{MICMD_CMD, 0x02},{MICMD_DATA, 0x01},//{MICMD_DATA, 0x03},
    {MICMD_CMD, 0x2b},{MICMD_CMD, 0x03},{MICMD_DATA, 0xdf},//{MICMD_DATA, 0x20},
#endif

    //Row / Column exchange
    {MICMD_CMD, 0x3a},{MICMD_CMD, 0x00},{MICMD_DATA, 0x55},//Set_LCD_16B_REG(0x3a00,0x55);
    {MICMD_CMD, 0x3a},{MICMD_CMD, 0x00},{MICMD_DATA, 0x55},

    {MICMD_CMD, 0x2c},{MICMD_CMD, 0x00},//{MICMD_DATA, 0x00},
    {MICMD_CMD, 0x35},{MICMD_CMD, 0x00},
    {MICMD_CMD, 0x35},{MICMD_CMD, 0x00},

    //adjust 0x4400 to set tearing effect scan line
    {MICMD_CMD, 0x44},{MICMD_CMD, 0x00},{MICMD_DATA, 0x01},
    {MICMD_CMD, 0x44},{MICMD_CMD, 0x01},{MICMD_DATA, 0x00},


    {MICMD_CMD, 0x11},{MICMD_CMD, 0x00},
    //DELAY);120);
    {CMDDELAY_MS, 120},
    {MICMD_CMD, 0x29},{MICMD_CMD, 0x00},
};


const tPANEL_CMD tCmdStandbyMI[] =
{

    //{MICMD_CMD,0x28}, //Display off
    {CMDDELAY_US, 1},
    //{MICMD_CMD,0x10}, //enter sleep mode
    //{MICMD_CMD,0x2c}, //memory write
};

const tLCD_PARAM tModeMI[] =
{
    /***********       MI Serial Format 1      *************/
    {
        // tPANEL_PARAM
        {
            /* Old prototype */
            PINMUX_LCDMODE_MI_FMT1,         //!< LCDMode
            14.00f,                         //!< fDCLK
            (0x20+480),                     //!< uiHSyncTotalPeriod
            480,                            //!< uiHSyncActivePeriod
            0x10,                           //!< uiHSyncBackPorch
            (0x40+800),                     //!< uiVSyncTotalPeriod
            800,                            //!< uiVSyncActivePeriod
            0x20,                           //!< uiVSyncBackPorchOdd
            0x20,                           //!< uiVSyncBackPorchEven
            800,                            //!< uiBufferWidth
            480,                            //!< uiBufferHeight
            480,                            //!< uiWindowWidth
            800,                            //!< uiWindowHeight
            FALSE,                          //!< bYCbCrFormat

            /* New added parameters */
            0x01,                           //!< uiHSyncSYNCwidth
            0x01                            //!< uiVSyncSYNCwidth
        },

        // tIDE_PARAM
        {
            /* Old prototype */
             PINMUX_LCD_SEL_PARALLE_MI_8BITS|PINMUX_LCD_SEL_TE_ENABLE,  //!< pinmux_select_lcd;
            ICST_CCIR601,                   //!< icst;
            {FALSE, FALSE},                   //!< dithering[2];
            DISPLAY_DEVICE_MI,              //!< **DONT-CARE**
            IDE_PDIR_RGB,                   //!< pdir;
            IDE_LCD_R,                      //!< odd;
            IDE_LCD_G,                      //!< even;
            TRUE,                           //!< hsinv;
            TRUE,                           //!< vsinv;
            FALSE,                          //!< hvldinv;
            FALSE,                          //!< vvldinv;
            TRUE,                           //!< clkinv;
            FALSE,                          //!< fieldinv;
            FALSE,                          //!< **DONT-CARE**
            FALSE,                          //!< interlace;
            FALSE,                          //!< **DONT-CARE**
            0x40,                           //!< ctrst;
            0x00,                           //!< brt;
            0x40,                           //!< cmults;
            FALSE,                          //!< cex;
            FALSE,                          //!< **DONT-CARE**
            TRUE,                           //!< **DONT-CARE**
            TRUE,                           //!< tv_powerdown;
            {0x00,0x00},                    //!< **DONT-CARE**

            /* New added parameters */
            FALSE,                          //!< YCex
            FALSE,                          //!< HLPF
            {FALSE,FALSE,FALSE},            //!< subpixOdd[3]
            {FALSE,FALSE,FALSE},            //!< subpixEven[3]
            {IDE_DITHER_6BITS,IDE_DITHER_6BITS,IDE_DITHER_6BITS}//!< DitherBits[3]
        },

        (tPANEL_CMD*)tCmdModeMiFmt8,                  //!< pCmdQueue
        sizeof(tCmdModeMiFmt8)/sizeof(tPANEL_CMD),    //!< nCmd
    }
};

const tPANEL_CMD tCmdRotate_None[] =
{
    {CMDDELAY_US, 200},
};
const tPANEL_CMD tCmdRotate_180[] =
{
    {CMDDELAY_US, 200},
};
const tLCD_ROT tRotMI[] =
{
    {DISPDEV_LCD_ROTATE_NONE, (tPANEL_CMD*)tCmdRotate_None, 3},
    {DISPDEV_LCD_ROTATE_180, (tPANEL_CMD*)tCmdRotate_180, 3}
};

tLCD_ROT* dispdev_getLcdRotateMiCmd(UINT32 *ModeNumber)
{
    if(tRotMI != NULL)
    {
        *ModeNumber = sizeof(tRotMI)/sizeof(tLCD_ROT);
    }
    else
    {
        *ModeNumber = 0;
    }
    return (tLCD_ROT*)tRotMI;
}

tLCD_PARAM* dispdev_getConfigModeMI(UINT32 *ModeNumber)
{
    *ModeNumber = sizeof(tModeMI)/sizeof(tLCD_PARAM);
    return (tLCD_PARAM*)tModeMI;
}

tPANEL_CMD* dispdev_getStandbyCmdMI(UINT32 *CmdNumber)
{
    *CmdNumber = sizeof(tCmdStandbyMI)/sizeof(tPANEL_CMD);
    return (tPANEL_CMD*)tCmdStandbyMI;
}

void dispdev_writeToLcdMi(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{
    if(addr & MICMD_DATA)
    {
        mi_setCmd1(WrDATA, (value&0xff));
    }
    else
    {
        mi_setCmd1(WrCMD, (value&0xff));
    }
    mi_setCmdCnt(1);
    mi_waitCmdDone();
}

void dispdev_readFromLcdMi(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 *pValue)
{
    *pValue = 0;
    debug_err(("No MI Read reg implement!\r\n"));
}

void dispdev_setMiConfig(tMI_CONFIG *pMiConfig)
{
    //FLOAT   MiTargetClk = 48.0; // Max is 60MHz
    FLOAT   MiTargetClk = 64.0; // Max is 60MHz?

    mi_setCtrl(PARALLEL_I80, MI_AUTOFMT3, pMiConfig->DataFmt, FALSE, FALSE, FALSE, FALSE);
    //mi_setSerialCsCtrl(TRUE);
    //mi_setPolCtrl(FALSE, FALSE, FALSE, FALSE, SCLMOD0);
    mi_setSerialAutoRsBitValue(TRUE);

    mi_setConfig(MI_CONFIG_ID_FREQ, MiTargetClk*1000000);
}

void dispdev_setMiAutoCmd(void)
{
    BOOL        bTeSyncEn = TRUE;
    MI_TYPE     Type;
    MI_AUTO_FMT AutoType;
    MI_DATAFMT  DataFMT;
    BOOL        bRGBSWAP;
    BOOL        bBITSWAP;
    BOOL        bSync;
    BOOL        bPINMUX;

    mi_setCmd1(WrCMD,0x2c);
    mi_setCmd2(WrCMD,0x00);
    mi_setCmdCnt(2);
    mi_waitCmdDone();

    mi_getCtrl(&Type, &AutoType, &DataFMT, &bRGBSWAP, &bBITSWAP, &bSync, &bPINMUX);
    mi_setCtrl(Type, AutoType, DataFMT, bRGBSWAP, bBITSWAP, bTeSyncEn, bPINMUX);

    mi_setSyncTiming(0, 0, FALSE, FALSE); // trigger TE on positive edge, CPU write speed is slower than display
    return;
}

