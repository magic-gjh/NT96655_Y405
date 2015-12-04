#ifdef __ECOS
#include "SysCommon.h"
#include "AppCommon.h"
#include "FileSysTsk.h"
#include "net_api.h"
#include <cyg/fileio/fileio.h>
#include "ecos_FileSys.h"
#include <cyg/fs/nvtfs.h>
#include <cyg\hfs\hfs.h>
#include "Timer.h"
#include "DCF.h"
#include "WifiAppCmd.h"
//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         2 //0=OFF, 1=ERROR, 2=TRACE
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          WifiAppCmd
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////



#define CMD_STR "custom=1&cmd="
#define PAR_STR "&par="

#define HTML_PATH "A:\\test.htm"



#define MOUNT_FS_ROOT        "/sdcard"
#define FAT_GET_DAY_FROM_DATE(x)     (x & 0x1F)              ///<  get day from date
#define FAT_GET_MONTH_FROM_DATE(x)   ((x >> 5) & 0x0F)       ///<  get month from date
#define FAT_GET_YEAR_FROM_DATE(x)    ((x >> 9) & 0x7F)+1980  ///<  get year from date
#define FAT_GET_SEC_FROM_TIME(x)     (x & 0x001F)<<1         ///<  seconds(2 seconds / unit)
#define FAT_GET_MIN_FROM_TIME(x)     (x & 0x07E0)>>5         ///<  Minutes
#define FAT_GET_HOUR_FROM_TIME(x)    (x & 0xF800)>>11        ///<  hours

static void xExamHfs_ecos2NvtPath(const char* inPath, char* outPath)
{
    outPath+=sprintf(outPath,"A:");
    //inPath+=strlen(MOUNT_FS_ROOT);
    while (*inPath !=0)
    {
        if (*inPath == '/')
            *outPath = '\\';
        else
            *outPath = *inPath;
        inPath++;
        outPath++;
    }
    *outPath++ = '\\';
    *outPath = 0;
}

static void xExamHfs_Nvt2ecosPath(const char* inPath, char* outPath)
{
    //outPath+=sprintf(outPath,MOUNT_FS_ROOT);
    inPath+=strlen("A:");
    while (*inPath !=0)
    {
        if (*inPath == '\\')
            *outPath = '/';
        else
            *outPath = *inPath;
        inPath++;
        outPath++;
    }
    *outPath = 0;
}

static int Hfs_PBGetData(char* path, char* argument, cyg_uint32 bufAddr, cyg_uint32* bufSize, char* mimeType, cyg_uint32 segmentCount)
{
    static UINT32     gIndex;

    UINT32            len, fileCount, i, bufflen = *bufSize, FileType;
    char*             buf = (char*)bufAddr;
    char              tempPath[128];
    FST_FILE_STATUS   FileStat;
    FST_FILE          filehdl;
    char              dcfFilePath[128];


    // set the data mimetype
    strcpy(mimeType,"text/xml");
    DBG_IND("path = %s, argument -> %s, mimeType= %s, bufsize= %d, segmentCount= %d\r\n",path,argument, mimeType, *bufSize, segmentCount);
    xExamHfs_ecos2NvtPath(path, tempPath);
    DBG_IND("tempPath = %s\r\n",tempPath);

    if (segmentCount == 0)
    {
        len = sprintf(buf,"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<LIST>\n<PATHROOT>%s</PATHROOT>\n<PATH>%s</PATH>\n",MOUNT_FS_ROOT,path);
        buf+=len;
    }
    fileCount = DCF_GetDBInfo(DCF_INFO_TOL_FILE_COUNT);
    if (segmentCount == 0)
    {
        // reset some global variables
        gIndex = 1;
    }
    DBG_IND("gIndex = %d\r\n",gIndex);
    for (i=gIndex;i<=fileCount;i++)
    {
        // check buffer length , reserved 512 bytes
        // should not write data over buffer length
        if ((cyg_uint32)buf-bufAddr > bufflen - 512)
        {
            DBG_IND("totallen=%d >  bufflen(%d)-512\r\n",(cyg_uint32)buf-bufAddr,bufflen);
            *bufSize = (cyg_uint32)(buf)-bufAddr;
            gIndex = i;
            return CYG_HFS_CB_GETDATA_RETURN_CONTINUE;
        }
        // get dcf file
        DCF_SetCurIndex(i);
        FileType = DCF_GetDBInfo(DCF_INFO_CUR_FILE_TYPE);

        if (FileType & DCF_FILE_TYPE_JPG)
        {
            DCF_GetObjPath(i, DCF_FILE_TYPE_JPG, dcfFilePath);
        }
        else if (FileType & DCF_FILE_TYPE_MOV)
        {
            DCF_GetObjPath(i, DCF_FILE_TYPE_MOV, dcfFilePath);
        }
        else
        {
            continue;
        }
        // get file state
        filehdl = FileSys_OpenFile(dcfFilePath,FST_OPEN_READ);
        FileSys_StatFile(filehdl, &FileStat);
        FileSys_CloseFile(filehdl);
        xExamHfs_Nvt2ecosPath(dcfFilePath,tempPath);
        // this is a dir
        if (M_IsDirectory(FileStat.uiAttrib))
        {
            len = sprintf(buf,"<Dir>\n<NAME>\n<![CDATA[%s]]></NAME><FPATH>\n<![CDATA[%s]]></FPATH>",&tempPath[22],tempPath);
            buf+=len;
            len = sprintf(buf,"<TIMECODE>%ld</TIMECODE><TIME>%04d/%02d/%02d %02d:%02d:%02d</TIME>\n</Dir>\n",(FileStat.uiModifiedDate <<16) + FileStat.uiModifiedTime,
                  FAT_GET_YEAR_FROM_DATE(FileStat.uiModifiedDate),FAT_GET_MONTH_FROM_DATE(FileStat.uiModifiedDate),FAT_GET_DAY_FROM_DATE(FileStat.uiModifiedDate),
                  FAT_GET_HOUR_FROM_TIME(FileStat.uiModifiedTime),FAT_GET_MIN_FROM_TIME(FileStat.uiModifiedTime),FAT_GET_SEC_FROM_TIME(FileStat.uiModifiedTime));
            buf+=len;
        }
        // this is a file
        else
        {
            len = sprintf(buf,"<ALLFile><File>\n<NAME>\n<![CDATA[%s]]></NAME><FPATH>\n<![CDATA[%s]]></FPATH>",&tempPath[22],tempPath);
            buf+=len;
            len = sprintf(buf,"<SIZE>%lld</SIZE>\n<TIMECODE>%ld</TIMECODE>\n<TIME>%04d/%02d/%02d %02d:%02d:%02d</TIME>\n</File>\n</ALLFile>\n",FileStat.uiFileSize,(FileStat.uiModifiedDate <<16) + FileStat.uiModifiedTime,
                  FAT_GET_YEAR_FROM_DATE(FileStat.uiModifiedDate),FAT_GET_MONTH_FROM_DATE(FileStat.uiModifiedDate),FAT_GET_DAY_FROM_DATE(FileStat.uiModifiedDate),
                  FAT_GET_HOUR_FROM_TIME(FileStat.uiModifiedTime),FAT_GET_MIN_FROM_TIME(FileStat.uiModifiedTime),FAT_GET_SEC_FROM_TIME(FileStat.uiModifiedTime));
            buf+=len;
        }
    }
    len = sprintf(buf,"</LIST>\n");
    buf+=len;
    *bufSize = (cyg_uint32)(buf)-bufAddr;
    return CYG_HFS_CB_GETDATA_RETURN_OK;
}
UINT32 UIAP_CmdExe(UINT32 cmd,UINT32 par,UINT32 *WriteXml)
{
    UINT32 ret = 0;
    *WriteXml = 0;

    switch(cmd)
    {
        case WIFIAPP_CMD_CAPTURE:
            Ux_PostEvent(NVTEVT_WIFI_EXE_PHOTO_CAPTURE, 0);
        break;
        case WIFIAPP_CMD_CAPTURESIZE:
            Ux_PostEvent(NVTEVT_WIFI_EXE_PHOTO_SIZE, 1,par);
        break;
        case WIFIAPP_CMD_CAPTUREWB:
            Ux_PostEvent(NVTEVT_WIFI_EXE_PHOTO_WB, 1,par);
        break;
        case WIFIAPP_CMD_CAPTUREFOV:
            Ux_PostEvent(NVTEVT_WIFI_EXE_PHOTO_FOV, 1,par);
        break;
        case WIFIAPP_CMD_BURSTMODE:
            Ux_PostEvent(NVTEVT_WIFI_EXE_CONTINUE_SHOT, 1,par);
        break;
        case WIFIAPP_CMD_RECORD:
        {
            if(par==1)
                Ux_PostEvent(NVTEVT_WIFI_EXE_MOVIE_REC_START, 0);
            else
                Ux_PostEvent(NVTEVT_WIFI_EXE_MOVIE_REC_STOP, 0);
        }
        break;
        case WIFIAPP_CMD_MOVIESIZE:
            Ux_PostEvent(NVTEVT_WIFI_EXE_MOVIE_SIZE, 1,par);
        break;
        case WIFIAPP_CMD_MOVIEWB:
            Ux_PostEvent(NVTEVT_WIFI_EXE_MOVIE_WB, 1,par);
        break;
        case WIFIAPP_CMD_MOVIEFOV:
            Ux_PostEvent(NVTEVT_WIFI_EXE_MOVIE_FOV, 1,par);
        break;

        case WIFIAPP_CMD_MODECHANGE:
            Ux_PostEvent(NVTEVT_WIFI_EXE_MODE, 1,par);
        break;
        default:
            ret = -1;
            DBG_ERR("not suppor cmd %d\r\n",cmd);
        break;
    }
    return ret;
}
int UIAP_getData(char* path, char* argument, cyg_uint32 bufAddr, cyg_uint32* bufSize, char* mimeType, cyg_uint32 segmentCount)
{
    UINT32            len;
    char*             buf = (char*)bufAddr;
    FST_FILE          filehdl;
    char              pFilePath[64];
    UINT32            fileLen =*bufSize;
    UINT32            cmd,par;
    char*             pch;
    UINT32            ret =0;
    UINT32            writeXML = 0;

    DBG_ERR("Data2 path = %s, argument -> %s, mimeType= %s, bufsize= %d, segmentCount= %d\r\n",path,argument, mimeType, *bufSize, segmentCount);

    if(strncmp(argument,CMD_STR,strlen(CMD_STR))==0)
    {
        DBG_IND("match cmd = %s\r\n",argument+strlen(CMD_STR));
        sscanf(argument+strlen(CMD_STR),"%d",&cmd);
        DBG_IND("cmd = %d\r\n",cmd);

        pch=strchr(argument+strlen(CMD_STR),'&');
        if(pch)
        {
            DBG_IND("str %s\r\n",pch);
            if(strncmp(pch,PAR_STR,strlen(PAR_STR))==0)
            {
                DBG_IND("match par= %s\r\n",pch+strlen(PAR_STR));
                sscanf(pch+strlen(PAR_STR),"%d",&par);
                DBG_IND("par = %d\r\n",par);

            }
        }

        ret = UIAP_CmdExe(cmd,par,&writeXML);

        if(writeXML==0) //default return value xml
        {
            strcpy(mimeType,"text/xml");
            len = sprintf(buf,"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
            buf+=len;
            len = sprintf(buf,"<Function>\n<Cmd>%d</Cmd>\n<Status>%d</Status>\n</Function>",cmd,ret);
            buf+=len;
            *bufSize = (cyg_uint32)(buf)-bufAddr;
        }
        else if(writeXML==1) //playback file list xml
        {
            Hfs_PBGetData(path,argument,bufAddr,bufSize,mimeType,segmentCount);
        }
    }
    else
    {

        sprintf(pFilePath,"%s",HTML_PATH);  //html of all command list
        filehdl = FileSys_OpenFile(pFilePath,FST_OPEN_READ);
        if(filehdl)
        {
            // set the data mimetype
            strcpy(mimeType,"text/html");
            FileSys_ReadFile(filehdl, (UINT8 *)buf, &fileLen, 0,0);
            FileSys_CloseFile(filehdl);
            *bufSize = fileLen;
            *(buf+fileLen) ='\0';
        }
        else
        {
            strcpy(mimeType,"text/xml");
            len = sprintf(buf,"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
            buf+=len;
            len = sprintf(buf,"<Status>\n<Value>no %s</Value></Status>\n",HTML_PATH);
            buf+=len;
            *bufSize = (cyg_uint32)(buf)-bufAddr;

        }
    }
    return CYG_HFS_CB_GETDATA_RETURN_OK;
}

#endif
