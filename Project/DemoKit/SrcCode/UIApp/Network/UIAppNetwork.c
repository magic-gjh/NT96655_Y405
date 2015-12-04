#include "SysCommon.h"
#include "AppCommon.h"
#include "FileSysTsk.h"
#include "UIAppNetwork.h"
#if defined(__ECOS) && (_NETWORK_ == _NETWORK_SDIO_EVB_WIFI_)
#include <cyg/io/eth/rltk/819x/wlan/ieee802_mib.h>
#include <cyg/io/eth/rltk/819x/wlan/wifi_api.h>
#endif

#ifdef __ECOS
#include "net_api.h"
#include <cyg/fileio/fileio.h>
#include "ecos_FileSys.h"
#include <cyg/fs/nvtfs.h>
#include <cyg\hfs\hfs.h>
#include "DhcpNvt.h"
#include "WifiAppCmd.h"
#endif
#include "Timer.h"
#include "DCF.h"
//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         2 //0=OFF, 1=ERROR, 2=TRACE
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          UINet
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

#ifdef __ECOS
#define MOUNT_FS_ROOT        "/sdcard/DCIM"
char gNvtDhcpHostNameCli[DHCP_HOST_NAME_MAX_LEN] = "nvtdsc01cli";
char gNvtDhcpHostNameSrv[DHCP_HOST_NAME_MAX_LEN] = "nvtdsc01srv";
char gMacAddr[6] = {0,0,0,0x81,0x89,0xe5};

#define EXAM_NET_IP_ETH0            "192.168.0.3"
#define EXAM_NET_NETMASK_ETH0       "255.255.255.0"
#define EXAM_NET_BRAODCAST_ETH0     "192.168.0.255"
#define EXAM_NET_GATEWAY_ETH0       "192.168.0.1"
#define EXAM_NET_SRVIP_ETH0         "192.168.0.1"
#define EXAM_NET_LEASE_START_ETH0   "192.168.0.12"
#define EXAM_NET_LEASE_END_ETH0     "192.168.0.32"
#define EXAM_NET_LEASE_DNS_ETH0     "0.0.0.0"
#define EXAM_NET_LEASE_WINS_ETH0    "0.0.0.0"

#define EXAM_NET_IP_ETH1            "192.168.1.3"
#define EXAM_NET_NETMASK_ETH1       "255.255.255.0"
#define EXAM_NET_BRAODCAST_ETH1     "192.168.1.255"
#define EXAM_NET_GATEWAY_ETH1       "192.168.1.1"
#define EXAM_NET_SRVIP_ETH1         "192.168.1.1"

#define EXAM_NET_IP_WLAN0            "192.168.1.254"
#define EXAM_NET_NETMASK_WLAN0       "255.255.255.0"
#define EXAM_NET_BRAODCAST_WLAN0     "192.168.1.255"
#define EXAM_NET_GATEWAY_WLAN0       "192.168.1.1"
#define EXAM_NET_SRVIP_WLAN0         "192.168.1.1"
#define EXAM_NET_LEASE_START_WLAN0   "192.168.1.43"
#define EXAM_NET_LEASE_END_WLAN0     "192.168.1.52"
#define EXAM_NET_LEASE_DNS_WLAN0     "0.0.0.0"
#define EXAM_NET_LEASE_WINS_WLAN0    "0.0.0.0"

#define WIFI_SSID_STR                "ssid=dhcp-Janice"
#define WIFI_PASSPHRASE_STR          "passphrase=12345678"

char gExamCurrIP[NET_IP_MAX_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char gNetIntf[3][6] =
{
    'e','t','h','0',0,0,
    'e','t','h','1',0,0,
    'w','l','a','n','0',0,
};
static cyg_uint32 xExamHfs_getCurrTime(void)
{
    return timer_getCurrentCount(timer_getSysTimerID());
}

static void UINet_HFSNotifyStatus(int status)
{
    switch (status)
    {
        // HTTP client has request coming in
        case CYG_HFS_STATUS_CLIENT_REQUEST:
             DBG_DUMP("client request %05d ms\r\n",xExamHfs_getCurrTime()/1000);
             break;
        // HTTP server send data start
        case CYG_HFS_STATUS_SERVER_RESPONSE_START:
             DBG_DUMP("send data start, time= %05d ms\r\n",xExamHfs_getCurrTime()/1000);
             break;
        // HTTP server send data start
        case CYG_HFS_STATUS_SERVER_RESPONSE_END:
             DBG_DUMP("send data end, time= %05d ms\r\n",xExamHfs_getCurrTime()/1000);
             break;
        // HTTP client disconnect
        case CYG_HFS_STATUS_CLIENT_DISCONNECT:
             DBG_DUMP("client disconnect, time= %05d ms\r\n",xExamHfs_getCurrTime()/1000);
             break;

    }
}
void Net_DhcpCb(NET_DHCP_CB_TYPE cbType, unsigned int p1, unsigned int p2, unsigned int p3)
{
    DBG_IND("::type=%d,P=%d, %d, %d\r\n",cbType, p1, p2, p3);
    if (NET_DHCP_CB_TYPE_CLI_REQ_CNT == cbType)
    {
        //post an event to UI-task show (cycle,count) that client request an IP
    }
    else if (NET_DHCP_CB_TYPE_CLI_REQ_RESULT == cbType)
    {
        if (p1)
        {
            DBG_DUMP("DHCP Client IP = 0x%x\r\n",p1);
            //post an event to UI-task to call,Dhcp_Client_Start_Ok(p1) for the first time.
            //post an event to UI-task to show Got-IP
        }
        else
        {
            DBG_DUMP("DHCP Client IP Fail\r\n");
            //post an event to UI-task to Close DHCP Client,Dhcp_Client_Stop(), and show Get-IP fail
        }
    }
    else if (NET_DHCP_CB_TYPE_MAX == cbType)
    {
        OS_DumpKernelResStatus();
    }
}

//////////////////////////////////////////////////////////////////////////
char* UINet_GetSSID(void)
{
    return ((char *)WIFI_SSID_STR+5);
}
char* UINet_GetPASSPHRASE(void)
{
    return ((char *)WIFI_PASSPHRASE_STR+11);
}


#if(_NETWORK_ == _NETWORK_SDIO_EVB_WIFI_)

static int IS_MCAST(unsigned char *da)
{
    if ((*da) & 0x01)
        return 1;
    else
        return 0;
}


INT32 UINet_WifiInit(UINT32 mode)
{
    UINT32 result=0;

    debug_msg("[SetupObj] set wifi %d \r\n",mode);

    UI_SetData(FL_NetWorkMode,mode);

    if(mode == NETWORK_AP_MODE) //////////////////////ap mode //////////////////////////////
    {
        //xExamDhcpNvt_StartWifi_init();
        if (0 > wifi_init_wlan0_netdev())
        {
            DBG_ERR("wifi_init_wlan0_netdev\r\n");
            return E_SYS;
        }

        //xExamDhcpNvt_StartWifi_ap(1);
        DBG_IND("Not input IP,Use Default=%s, wpa2=%d\r\n",EXAM_NET_IP_WLAN0, 1);
        memcpy((void *)gExamCurrIP, EXAM_NET_IP_WLAN0, NET_IP_MAX_LEN);

        //xExamDhcpNvt_WifiUtil_ap(1);
        if (is_interface_up(&gNetIntf[2][0]))
        {
            DBG_IND("Down interface=%s\r\n",&gNetIntf[2][0]);
            interface_down(&gNetIntf[2][0]);
        }
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "opmode=16", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", WIFI_SSID_STR, NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "channel=0", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "band=11", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "ampdu=1", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "use40M=1", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "2ndchoffset=1", NULL_STR); // 1:below, 2:above
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "shortGI20M=1", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "shortGI40M=1", NULL_STR);

        //xExamDhcpNvt_WifiUtil_wpa2();
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "authtype=2", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "encmode=2", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "psk_enable=2", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "wpa2_cipher=8", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", WIFI_PASSPHRASE_STR, NULL_STR);


        //xExamDhcpNvt_WifiUtil_setMacAddr();
        // mac address setting
        if (wifi_get_wlan0_efuse_mac(gMacAddr) < 0)
        {
            printf("wifi_get_wlan0_efuse_mac() fail. Use hardcoded mac.\n");
            set_mac_address("wlan0", "\x00\x00\x00\x81\x89\xe5");
        }
        else
        {
            unsigned char zero[] = {0, 0, 0, 0, 0, 0};

            printf("wlan0 efuse mac [%02x:%02x:%02x:%02x:%02x:%02x]\n",
            gMacAddr[0], gMacAddr[1], gMacAddr[2],
            gMacAddr[3], gMacAddr[4], gMacAddr[5]);
            //sanity check
            if (!memcmp(gMacAddr, zero, MACADDRLEN) || IS_MCAST((unsigned char *)gMacAddr))
            {
                printf("efuse mac format is invalid. Use hardcoded mac.\n");
                set_mac_address("wlan0", "\x00\x00\x00\x81\x89\xe5");
            }
            else
            {
                set_mac_address("wlan0", gMacAddr);
            }
        }
        interface_config("wlan0", gExamCurrIP, "255.255.255.0");
        DBG_IND(" setip=%s -\r\n", gExamCurrIP);

        //xExamDhcpNvt_SetInterfaceWifi();
        {
        char *pIntf = &gNetIntf[2][0];
        //DBG_IND(":: %s => intrf=%s\r\n",strCmd, pIntf);
        Dhcp_Set_Interface(pIntf);

        }
        //xExamDhcpNvt_OpenSrv();
        if (NET_DHCP_RET_OK != Dhcp_Server_Start(gNvtDhcpHostNameSrv))
        {
            DBG_ERR("xExamDhcpNvt_OpenSrv fail\r\n");
            return E_SYS;
        }

    }
    else if(mode == NETWORK_STATION_MODE) //////////////////////station mode //////////////////////////////
    {
        result = wifi_init_wlan0_netdev();
        if (result!=0 )
        {
            DBG_ERR("wifi init fail\r\n");
            //open error window 5sec
            return E_SYS;
        }

        if (is_interface_up(&gNetIntf[2][0]))
        {
            DBG_IND("Down interface=%s\r\n",&gNetIntf[2][0]);
            interface_down(&gNetIntf[2][0]);
        }
        //xExamDhcpNvt_WifiUtil_sta();
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "opmode=8", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "ssid=dlink", NULL_STR);
        //xExamDhcpNvt_WifiUtil_wpa2();
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "authtype=2", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "encmode=2", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "psk_enable=2", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "wpa2_cipher=8", NULL_STR);
        RunSystemCmd(NULL_FILE, "iwpriv", "wlan0", "set_mib", "passphrase=12345678", NULL_STR);
        //xExamDhcpNvt_WifiUtil_setMacAddr();
        {

            // mac address setting
            if (wifi_get_wlan0_efuse_mac(gMacAddr) < 0)
            {
                printf("wifi_get_wlan0_efuse_mac() fail. Use hardcoded mac.\n");
                set_mac_address("wlan0", "\x00\x00\x00\x81\x89\xe5");
            }
            else
            {
                unsigned char zero[] = {0, 0, 0, 0, 0, 0};

                printf("wlan0 efuse mac [%02x:%02x:%02x:%02x:%02x:%02x]\n",
                gMacAddr[0], gMacAddr[1], gMacAddr[2],
                gMacAddr[3], gMacAddr[4], gMacAddr[5]);
                //sanity check
                if (!memcmp(gMacAddr, zero, MACADDRLEN) || IS_MCAST((unsigned char *)gMacAddr))
                {
                    printf("efuse mac format is invalid. Use hardcoded mac.\n");
                    set_mac_address("wlan0", "\x00\x00\x00\x81\x89\xe5");
                    printf(gMacAddr,"%c%c%c%c%c%c",0,0, 0,0x81,0x89,0xe5);
                printf("default wlan0 efuse mac [%02x:%02x:%02x:%02x:%02x:%02x]\n",
                gMacAddr[0], gMacAddr[1], gMacAddr[2],
                gMacAddr[3], gMacAddr[4], gMacAddr[5]);

                }
                else
                {
                    set_mac_address("wlan0", gMacAddr);
                }

            }

            //start DHCP client
            {
                unsigned int gotIP = 0;
                if (NET_DHCP_RET_OK != Dhcp_Client_Start(gNvtDhcpHostNameCli, Net_DhcpCb, TRUE))
                {
                    DBG_ERR("dhcp cli fail\r\n");
                    return E_SYS;
                }
                else
                {
                    Dhcp_Client_Get_IP(&gotIP);
                    DBG_IND("^R:::: ip=0x%x\r\n",gotIP);
                    UI_SetData(FL_WIFI_IP,gotIP);
                }
            }
        }
    }
    else
    {
        result = E_SYS;
        DBG_ERR("not support mode %d\r\n",mode);
    }

    if(result==NET_DHCP_RET_OK)
    {
        result=UINet_HFSInit();
    }
    return E_OK;
}

INT32 UINet_WifiUnInit(UINT32 mode)
{
    UINT32 ret = 0;

    ret =interface_down(&gNetIntf[2][0]);
    if(ret==E_OK)
    {
        if(mode == NETWORK_AP_MODE) //ap mode
            ret =Dhcp_Server_Stop(FALSE);
        else
            ret =Dhcp_Client_Stop();
    }
    else
    {
        DBG_ERR("stop wifi fail %d\r\n",ret);
    }
    return ret;
}
#elif defined(__ECOS) && (_NETWORK_ == _NETWORK_SPI_EVB_ETHERNET_)
static NET_IP_PAIR gExamNetIP[NET_IP_PAIR_MAX_CNT] =
{
    //eth0
    {EXAM_NET_IP_ETH0,EXAM_NET_NETMASK_ETH0,EXAM_NET_BRAODCAST_ETH0,EXAM_NET_GATEWAY_ETH0,EXAM_NET_SRVIP_ETH0, 0},
    //eth1
    {EXAM_NET_IP_ETH1,EXAM_NET_NETMASK_ETH1,EXAM_NET_BRAODCAST_ETH1,EXAM_NET_GATEWAY_ETH1,EXAM_NET_SRVIP_ETH1, 1},
    //wlan0
    {EXAM_NET_IP_WLAN0,EXAM_NET_NETMASK_WLAN0,EXAM_NET_BRAODCAST_WLAN0,EXAM_NET_GATEWAY_WLAN0,EXAM_NET_SRVIP_WLAN0, 2}
};

INT32 UINet_LanInit(UINT32 mode)
{
    NET_DHCP_RET ret =0;

    debug_msg("[SetupObj] set Lan %d\r\n",mode);

    if(mode == NETWORK_AP_MODE) //ap mode
    {
        ret=Dhcp_Init_Network_With_IP(gExamNetIP, NET_IP_PAIR_MAX_CNT);
        if (NET_DHCP_RET_OK == ret)
        {
            ret=Dhcp_Server_Start(gNvtDhcpHostNameSrv);
            if(NET_DHCP_RET_OK !=ret )
            {
                debug_msg("AP fail %d\r\n",ret);
            }
        }
        else
        {
            debug_msg("eth0 fail %d\r\n",ret);
        }
    }
    else if(mode == NETWORK_STATION_MODE) //station mode
    {
        //start DHCP client
        unsigned int gotIP = 0;
        if (NET_DHCP_RET_OK != Dhcp_Client_Start(gNvtDhcpHostNameCli, Net_DhcpCb, TRUE))
        {
            DBG_ERR("dhcp cli fail\r\n");
            return E_SYS;
        }
        else
        {
            Dhcp_Client_Get_IP(&gotIP);
            DBG_IND("^R:::: ip=0x%x\r\n",gotIP);
            UI_SetData(FL_WIFI_IP,gotIP);
        }
    }
    else
    {
        ret = E_SYS;
        DBG_ERR("not support mode %d\r\n",mode);
    }

    if(ret==NET_DHCP_RET_OK)
    {
        ret=UINet_HFSInit();
    }

    return ret;
}



INT32 UINet_LanUnInit(UINT32 mode)
{
    UINT32 ret = 0;
    if(mode == NETWORK_AP_MODE) //ap mode
        ret =Dhcp_Server_Stop(FALSE);
    else
        ret =Dhcp_Client_Stop();
    return ret;
}
#endif

INT32 UINet_HFSInit(void)
{
    int err;

    cyg_FileSys_RegisterCB();
    err = mount( "", MOUNT_FS_ROOT, "nvtfs" );
    if( err < 0 ) DBG_ERR("mount fs fail %d\r\n",err);
    //hfs start
    {
        cyg_hfs_open_obj  hfsObj={0};
        // register callback function of get XML data.
        hfsObj.getCustomData = UIAP_getData;
        // noify some status of HTTP server
        hfsObj.notify = UINet_HFSNotifyStatus;
        // if need to check user & password
        hfsObj.check_pwd = NULL;//xExamHfs_CheckPasswd;
        // set port number
        hfsObj.portNum = 80;
        // set thread priority
        hfsObj.threadPriority = 6;
        // set HFS root dir path
        strcpy(hfsObj.rootdir, MOUNT_FS_ROOT);
        // start HFS
        cyg_hfs_open(&hfsObj);

    }
    return err;
}

INT32 UINet_HFSUnInit(void)
{
    cyg_hfs_close();
    umount(MOUNT_FS_ROOT);
    return E_OK;
}

#else
INT32 UINet_WifiInit(UINT32 mode)
{
   return E_OK;
}
INT32 UINet_LanInit(UINT32 mode)
{
   return E_OK;
}
INT32 UINet_HFSInit(void)
{
   return E_OK;
}
INT32 UINet_WifiUnInit(UINT32 mode)
{
   return E_OK;
}
INT32 UINet_LanUnInit(UINT32 mode)
{
   return E_OK;
}
INT32 UINet_HFSUnInit(void)
{
   return E_OK;
}
char* UINet_GetSSID(void)
{
    return NULL;
}
char* UINet_GetPASSPHRASE(void)
{
    return NULL;
}
#endif
