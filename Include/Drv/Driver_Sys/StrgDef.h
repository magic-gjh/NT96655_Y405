/**
    Definitions for storage.

    Interface of storage (NAND, SDIO...)

    @file       StrgDef.h

    @ingroup    mIDrvStorage

    @note       Nothing

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/


#ifndef _STRGDEF_H
#define _STRGDEF_H

#include "Type.h"

/*==================================================================
* Storage Status information
*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  |1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|1|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|
*  |F|E|D|C|B|A|9|8|7|6|5|4|3|2|1|0|F|E|D|C|B|A|9|8|7|6|5|4|3|2|1|0|
*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   | | | | | | |   |                               | | | | | | | |
*   | | | | | | |   |                               | | | | | | | *--> 1 = Storage detached / 0 = Storage attached
*   | | | | | | |   |                               | | | | | | *----> 1 = Storage device error
*   | | | | | | |   |                               | | | | | *------> 1 = Storage full
*   | | | | | | |   |                               | | | | *--------> 1 = Host DMA error
*   | | | | | | |   |                               | | | *----------> 1 = Storage write fault
*   | | | | | | |   |                               | | *------------> 1 = Storage read fault
*   | | | | | | |   |                               | *--------------> 1 = Host is no buffer for i/o
*   | | | | | | |   |                               *----------------> 1 = Storage is processing
*   | | | | | | |   |
*   | | | | | | |   *------------------------------------------------> 1 = Media Changed. This flage is reported too upper system if the the media is changed.
*   | | | | | | |
*   | | | | | | *----------------------------------------------------> 1 = MS/MSPro card
*   | | | | | *------------------------------------------------------> 1 = SDRAM device
*   | | | | *--------------------------------------------------------> 1 = NOR Flash device
*   | | | *----------------------------------------------------------> 1 = NAND flash    // Dicky modified
*   | | *------------------------------------------------------------> 1 = SD card
*   | *--------------------------------------------------------------> 1 = SMC/xD card
*   *----------------------------------------------------------------> 1 = CF card
*=================================================================*/
#define STORAGE_READY           0x00000000      // Storage device is ready for read/write
#define STORAGE_DETACH          0x00000001      // Storage detached
#define STORAGE_DEVICE_ERROR    0x00000002      // Storage device error
#define STORAGE_FULL            0x00000004      // Storage full
#define STORAGE_DMA_ERROR       0x00000008      // DMA access error
#define STORAGE_WRITE_FAULT     0x00000010      // Storage write fault
#define STORAGE_READ_FAULT      0x00000020      // Storage read fault
#define STORAGE_HOST_NO_BUF     0x00000040      // Host is no buffer for i/o
#define STORAGE_PROCESSING      0x00000080      // Storage is processing
#define STORAGE_VOLTAGE_ERROR   0x00000100      // Voltage error
#define STORAGE_OPEN_FAIL       0x00000200      // Storage open/init fail
#define STORAGE_LOCK            0x00000400      // Storage device is locked
#define STORAGE_CFC             0x80000000      // CF card
#define STORAGE_SMC             0x40000000      // SMC/xD card
#define STORAGE_SDC             0x20000000      // SD card
#define STORAGE_NAND            0x10000000      // NAND flash
#define STORAGE_FLASH           0x08000000      // NOR Flash device
#define STORAGE_SDRAM           0x04000000      // SDRAM device
#define STORAGE_MSPro           0x02000000      // MS/MS_Pro device
#define STORAGE_SDIO            0x01000000      // SDIO device
#define STORAGE_CHANGED         0x00800000      // Media Change Flag. This flage is reported to upper system if the the media is changed.
//#define STORAGE_WRITEPROT       0x00400000      // Media Write Protect Flag. This flage is reported to upper system if the the media is write protected.
#define STORAGE_TYPE_MASK       0xFF000000      // MSB 7 bits for different storage type
#define STORAGE_STATUS_MASK     0x00FFFFFF      // LSB 24 bits for storage status

// Macro for FAT
#define FAT_ROOT_SECTOR_NUM     32              // Sector number of root area (512 * 32) / 512

#define STRG_ER_TYPE_MASK           0xF0000000      //> MSB 4 bits for different storage type
#define STRG_ER_STATUS_MASK         0x0FFFFFFF      //> LSB 28 bits for storage error status

//#NT#2007/04/25#Ethan Wang -begin
//#add format callback
/*==========================
 * Callback Message ID
 *==========================*/
#define STRG_CB_ID_FORMAT           11              // format the the storage, ref: FST_CMD_FORMAT

//Extend command definition for ExtIOCtrl usage

#define MAX_STRG_EXT_CMD_COUNT      32

/**
    Storage object provided extend command
*/
typedef enum
{
    STRG_EXT_CMD_GLOBAL_START = 0x0,
    STRG_EXT_CMD_ABORT = STRG_EXT_CMD_GLOBAL_START, ///< General command for all storage object start
    STRG_EXT_CMD_FORMAT,                            ///< Format operation by storage provided format function
                                                    ///< Context is null:
                                                    ///< @code
                                                    ///  {
                                                    ///     if(strgObj->ExtIOCtrl(STRG_EXT_CMD_FORMAT, 0, 0) != E_OK)
                                                    ///     {
                                                    ///         ERROR;
                                                    ///     }
                                                    ///  }
                                                    ///  @endcode
    //STRG_EXT_CMD_FORMAT_ERASE,
    STRG_EXT_CMD_GLOBAL_END,                        ///< General command for all storage object end

    STRG_EXT_CMD_NAND_START,                        ///< Command for NAND storage object start
    //Add IO cmd for NAND here
    STRG_EXT_CMD_RW_CHK,                            ///< NAND r/w check for specific partition(take over nand_readwriteCheckFAT)
                                                    ///<    - @b UINT32 : address of buffer usage
                                                    ///<    - @b UINT32 : size of buffer usage (STRG_GET_MEMORY_SIZE_REQ at least)
                                                    ///< @note this command will keep original block data, so need buffer for temp data
                                                    ///< @code
                                                    ///  {
                                                    ///     if(strgObj->ExtIOCtrl(STRG_EXT_CMD_RW_CHK, (UINT32)pBuf, 0x100000) != E_OK)
                                                    ///     {
                                                    ///         ERROR;
                                                    ///     }
                                                    ///  }
                                                    ///  @endcode
    STRG_EXT_CMD_ERS_CHK,                           ///< NAND erase check for specific partition(take over nand_eraseCheckFAT)
                                                    ///< Context is null:
                                                    ///< @note  : Not erase once if block is defect block (status = 0x00), original data will be erased
                                                    ///< @code
                                                    ///  {
                                                    ///     if(strgObj->ExtIOCtrl(STRG_EXT_CMD_RW_CHK, 0, 0) != E_OK)
                                                    ///     {
                                                    ///         ERROR;
                                                    ///     }
                                                    ///  }
                                                    ///  @endcode
    STRG_EXT_CMD_READ_CHK,                          ///< NAND read check for specific partition(take over nand_readCheckAll)
                                                    ///<    - @b PSTRG_READ_CHK : read check related information
                                                    ///< @note for STRG_READ_CHK : will keep original data in specific nand block
                                                    ///< @code
                                                    ///  {
                                                    ///     STRG_READ_CHK  readChkParam;
                                                    ///     if(strgObj->ExtIOCtrl(STRG_EXT_CMD_READ_CHK, 0, 0) != E_OK)
                                                    ///     {
                                                    ///         ERROR;
                                                    ///     }
                                                    ///  }
                                                    ///  @endcode

    STRG_EXT_CMD_FORMAT_ERASE_FAT,                  ///< Erase FAT area before format operation => FormatErase()
                                                    ///< Context is null:

    STRG_EXT_CMD_WRITE_ONE_RESERVED_BLOCK,          ///< FW object write one reserved block
                                                    ///< - @b NAND_WRITERSVCFG * : struct of logic and physical block
                                                    ///< - @b UINT32 : address of buffer
                                                    ///< @note for NAND_WRITERSVCFG
                                                    ///< @code
                                                    ///  {
                                                    ///     NAND_WRITERSVCFG  nandWriteRsvCfg;
                                                    ///     nandWriteRsvCfg.uiLogicBlock = 4;
                                                    ///     nandWriteRsvCfg.uiPhysicalBlock = 4;
                                                    ///     strgObj->SetParam(STRG_SET_PARTITION_SIZE, 0, 0x500000);
                                                    ///     strgObj->SetParam(STRG_SET_LOADER_SECTOR_COUNT, 4, 0);
                                                    ///     strgObj->Open();
                                                    ///     if(strgObj->ExtIOCtrl(STRG_EXT_CMD_WRITE_ONE_RESERVED_BLOCK, (UINT32)&nandWriteRsvCfg, (UINT32)pBuf) == E_OK)
                                                    ///     {
                                                    ///         SUCCESS;
                                                    ///     }
                                                    ///     else
                                                    ///     {
                                                    ///         ERROR;
                                                    ///     }
                                                    ///     strgObj->Close();
                                                    ///  }
                                                    ///  @endcode
    STRG_EXT_CMD_NAND_END,                          ///< Command for NAND storage object end

    STRG_EXT_CMD_SPI_START,
    //Add IO cmd for SPI here
    STRG_EXT_CMD_SPI_CONTROLLER_ID,                 ///< spi flash driver to mount to specific SPI controller.
                                                    ///< Context (on param1) should be:
                                                    ///< - @b SPI_ID_1: SPI1 (default setting)
                                                    ///< - @b SPI_ID_2: SPI2
                                                    ///< - @b SPI_ID_3: SPI3
    STRG_EXT_CMD_SPI_INVALID_WRITE_REGION,          ///< Setup invalid write(program+erase) region.
                                                    ///< Context on param1 is starting address (unit: byte)
                                                    ///< Context on param2 is region length (unit: byte)
    STRG_EXT_CMD_SPI_INVALID_WRITE_CB,              ///< Install invalid write call back
                                                    ///< Context (on param1) should be: function pointer of SPIFLASH_INVALID_WRITE_CB.
    STRG_EXT_CMD_SPI_IDENTIFY_CB,                   ///< Install flash identify call back
                                                    ///< Context (on param1) should be: function pointer of SPIFLASH_IDENTIFY_CB

    STRG_EXT_CMD_SPI_END,
    STRG_EXT_CMD_SDIO_START,
    //Add IO cmd for SPI here
    STRG_EXT_CMD_SDIO_END,
    STRG_EXT_CMD_eMMC_START,
    //Add IO cmd for SPI here
    STRG_EXT_CMD_eMMC_RESET_CMD,
    STRG_EXT_CMD_eMMC_BOOT_PAR_ACES,
    STRG_EXT_CMD_eMMC_BOOT_PAR,
    STRG_EXT_CMD_eMMC_BOOT_BUS_WIDTH,
    STRG_EXT_CMD_eMMC_BOOT_ACK,
    STRG_EXT_CMD_eMMC_BOOT_MODE,
    //STRG_EXT_CMD_eMMC_ERSGRPDEF,
    STRG_EXT_CMD_eMMC_END,
    STRG_EXT_CMT_CNT,
}STRG_EXT_CMD;

typedef enum
{
    STRG_EXT_CMD_EMMC_BOOT_PAR_ACES_DIS = 0,
    STRG_EXT_CMD_EMMC_BOOT_PAR_ACES_BA1,
    STRG_EXT_CMD_EMMC_BOOT_PAR_ACES_BA2,

    ENUM_DUMMY4WORD(STRG_EXT_CMD_EMMC_BOOT_PAR_ACES)
}STRG_EXT_CMD_EMMC_BOOT_PAR_ACES;

typedef enum
{
    STRG_EXT_CMD_EMMC_BOOT_PAR_DIS = 0,
    STRG_EXT_CMD_EMMC_BOOT_PAR_BA1_EN,
    STRG_EXT_CMD_EMMC_BOOT_PAR_BA2_EN,

    ENUM_DUMMY4WORD(STRG_EXT_CMD_EMMC_BOOT_PAR)
}STRG_EXT_CMD_EMMC_BOOT_PAR;


typedef enum
{
    STRG_EXT_CMD_EMMC_BOOT_ACK_DIS = 0,
    STRG_EXT_CMD_EMMC_BOOT_ACK_EN,

    ENUM_DUMMY4WORD(STRG_EXT_CMD_EMMC_BOOT_ACK)
}STRG_EXT_CMD_EMMC_BOOT_ACK;


typedef enum
{
    STRG_EXT_CMD_EMMC_BOOT_BUS_WIDTH_1 = 0,
    STRG_EXT_CMD_EMMC_BOOT_BUS_WIDTH_4,
    STRG_EXT_CMD_EMMC_BOOT_BUS_WIDTH_8,

    ENUM_DUMMY4WORD(STRG_EXT_CMD_EMMC_BOOT_BUS_WIDTH)
}STRG_EXT_CMD_EMMC_BOOT_BUS_WIDTH;

typedef enum
{
    STRG_EXT_CMD_EMMC_BOOT_MODE_SINGLE_BACK = 0,
    STRG_EXT_CMD_EMMC_BOOT_MODE_SINGLE_HIGH,
    STRG_EXT_CMD_EMMC_BOOT_MODE_DUAL,

    ENUM_DUMMY4WORD(STRG_EXT_CMD_EMMC_BOOT_MODE)
}STRG_EXT_CMD_EMMC_BOOT_MODE;

/**
     STRG_EXT_CMD_READ_CHK related parameter

     read check related parameter configuration information
     @note for STRG_EXT_CMD_READ_CHK
*/
typedef struct
{
    UINT32  uiStartBlock;                   ///< Start block read check from
    UINT32  uiEndBlock;                     ///< End block read check to
    BOOL    bDumpAllBlock;                  ///< Dump all block status information or not
    BOOL    bMarkBadBlock;                  ///< Mark as bad block or not once encounter read error
} STRG_READ_CHK, *PSTRG_READ_CHK;



/*
    @name Old / New storage object switch flag
\n  For New storage object set as ENABLE

*/
//@{
#define NEW_STORAGE_OBJECT  ENABLE
//@}

/*==========================
   Error Status Definitions
  ==========================*/

// Capacity of Storage device
typedef struct
{
    UINT32  uiTotalCylinders;               // Total cylinders
    UINT32  uiTotalHeads;                   // Total heads
    UINT32  uiSectorsPerTrack;              // Sectors per track
    UINT32  uiTotalSectors;                 // Total sectors
    UINT32  uiBytesPerSector;               // Bytes per setors
} STRG_CAP, *PSTRG_CAP;

typedef struct
{
    UINT32  uiHeadNum;
    UINT32  uiSectorNum;
    UINT32  uiRelativeSector;
    UINT32  uiSectorPerCluster;
    UINT32  uiResvSec;
} STRG_FATINFO, *PSTRG_FATINFO;


/**
    @name Storage device type name
\n  Identify each storage device type

*/
//@{
typedef enum
{
    STRG_TYPE_NAND = 0x0,               ///< NAND flash
    STRG_TYPE_SDIO,                     ///< SDIO1 device
    STRG_TYPE_SDIO2,                    ///< SDIO2 device
    STRG_TYPE_SPIFLASH,                 ///< SPI flash
    STRG_TYPE_SDRAM,                    ///< SDRAM
    STRG_TYPE_EMMC,                     ///< EMMC card
    STRG_TYPE_USB_READER,               ///< USB Card Reader
    STRG_TYPE_RAMDISK,                  ///< Ram Disk

    STRG_TYPE_CNT,
    ENUM_DUMMY4WORD(STRG_TYPE)
} STRG_TYPE;
//@}


/**
    @name Storage object partition ID
\n  For device driver provided access object

    @note for nand_getStorageObject(),sdio_getStorageObject(), spi_getStorageObject
*/
//@{
typedef enum
{
    STRG_OBJ_FW_RSV1,                  ///< Firmware reserved area 1
    STRG_OBJ_FW_RSV2,                  ///< Firmware reserved area 2
    STRG_OBJ_FW_RSV3,                  ///< Firmware reserved area 3

    STRG_OBJ_FAT1 = 0x10,              ///< FAT area 1
    STRG_OBJ_FAT2,                     ///< FAT area 2
    STRG_OBJ_FAT3,                     ///< FAT area 3

    STRG_OBJ_PSTORE1 = 0x20,           ///< Pstore area 1
    STRG_OBJ_PSTORE2,                  ///< Pstore area 2
    STRG_OBJ_PSTORE3,                  ///< Pstore area 2
    STRG_OBJ_PSTORE4,                  ///< Pstore area 2
    STRG_OBJ_PSTORE5,                  ///< Pstore area 2


    ENUM_DUMMY4WORD(STRG_OBJ_ID)
} STRG_OBJ_ID;
//@}



/**
    @name Storage object get param event

\n  For specific storage object get param event
*/
//@{
typedef enum
{
    STRG_GET_MEMORY_SIZE_REQ,           ///< Obtain storage temporary memory usage requirement(Unit : byte)
                                        ///< Context is :
                                        ///< - @b UINT32 * : address of return memory requirement
                                        ///< @code
                                        ///     UINT32          uiParam1
                                        ///     if(strgObj->GetParam(STRG_GET_MEMORY_SIZE_REQ, (UINT32)&uiParam1, 0) != E_OK)
                                        ///     {
                                        ///         ERROR;
                                        ///     }
                                        ///     uiParam1 => soecific storage requirement size; (byte unit)
                                        ///  }
                                        ///  @endcode

    STRG_GET_CAP_TAB,                   ///< Obtain storage capacity struct
                                        ///< Context is :
                                        ///< - @b UINT32 * : address of return cap struct address
                                        ///< @code
                                        ///  {
                                        ///     STRG_CAP        Cap;
                                        ///     if(strgObj->GetParam(STRG_GET_CAP_TAB, (UINT32)&Cap, 0) != E_OK)
                                        ///     {
                                        ///         ERROR;
                                        ///     }
                                        ///     debug_err(("Capacity uiTotalCylinders     = 0x%08x\r\n", Cap.uiTotalCylinders));
                                        ///  }
                                        ///  @endcode

    STRG_GET_SECTOR_SIZE,               ///< Obtain device sector size (unit : bytes), if device not implement, use 512 bytes as default usage
                                        ///<    - @b UINT32 * : address of return access size
                                        ///< @code
                                        ///     UINT32          uiParam1
                                        ///     if(strgObj->GetParam(STRG_GET_BEST_ACCESS_SIZE, (UINT32)&uiParam1, 0) != E_OK)
                                        ///     {
                                        ///         ERROR;
                                        ///     }
                                        ///     uiParam1 => ACCESS size; (byte unit)
                                        ///  }
                                        ///  @endcode

    STRG_GET_BEST_ACCESS_SIZE,          ///< Obtain device best access size boundary (Unit : bytes, Ex : NAND : block size is recommend)
                                        ///<    - @b UINT32 * : address of return access size
                                        ///< @code
                                        ///     UINT32          uiParam1
                                        ///     if(strgObj->GetParam(STRG_GET_BEST_ACCESS_SIZE, (UINT32)&uiParam1, 0) != E_OK)
                                        ///     {
                                        ///         ERROR;
                                        ///     }
                                        ///     uiParam1 => ACCESS size; (byte unit)
                                        ///  }
                                        ///  @endcode

    STRG_GET_DEVICE_PHY_SIZE,           ///< Obtain spefific storage device total size (capacity) (Unit : bytes)
                                        ///< Context is :
                                        ///< - @b UINT32 * : address of return device size
                                        ///< @code
                                        ///     UINT32          uiParam1
                                        ///     if(strgObj->GetParam(STRG_GET_DEVICE_PHY_SIZE, (UINT32)&uiParam1, 0) != E_OK)
                                        ///     {
                                        ///         ERROR;
                                        ///     }
                                        ///     uiParam1 => device size; (byte unit)
                                        ///  }
                                        ///  @endcode

    STRG_GET_FAT_TABLE_PARAM,           ///< Obtain FAT relative param such as ucHeadNum, ucSectorNum, ucRelativeSector, uiSecPerCluster
                                        ///< Context is :
                                        ///< - @b UINT32    : Total Sectors
                                        ///<   @b UINT32 *  : address of return PSTRG_FATINFO

    STRG_GET_LOADER_SECTOR_COUNT,       ///< Context is :
                                        ///< - @b UINT32 * : address of return device size
                                        ///< @code
                                        ///     UINT32          uiParam1
                                        ///     if(strgObj->GetParam(STRG_GET_LOADER_SECTOR_COUNT, (UINT32)&uiParam1, 0) != E_OK)
                                        ///     {
                                        ///         ERROR;
                                        ///     }
                                        ///     uiParam1 => loader sector count; (byte unit)
                                        ///  }
                                        ///  @endcode
    STRG_GET_DEVICE_ID,                 ///< Storage object obtain device ID
                                        ///< - @b UINT32 * : address of ID
                                        ///< @code
                                        ///  {
                                        ///     UINT32 nandID;
                                        ///     if(strgObj->GetParam(STRG_GET_DEVICE_ID, (UINT32)&nandID, 0) == E_OK)
                                        ///     {
                                        ///         SUCCESS;
                                        ///     }
                                        ///     else
                                        ///     {
                                        ///         ERROR;
                                        ///     }
                                        ///  }
                                        ///  @endcode
    ENUM_DUMMY4WORD(STRG_GET_PARAM_EVT)
}STRG_GET_PARAM_EVT;
//@}


/**
    @name Storage object set param event

\n  For specific storage object set param event
*/
//@{
typedef enum
{
    STRG_SET_MEMORY_REGION,             ///< Set storage temporary memory address
                                        ///< Context are :
                                        ///< - @b UINT32 : starting address of memory usage requirement
                                        ///< - @b UINT32 : memory size usage requirement(Unit:bytes)
    STRG_SET_PARTITION_SIZE,            ///< Set storage object partition size, use STRG_GET_DEVICE_PHY_SIZE to get device capacity first
                                        ///< Context are :
                                        ///< - @b UINT32 : address of start address (Unit : bytes, need STRG_GET_SECTOR_SIZE alignment)
                                        ///< - @b UINT32 : size of partition        (Unit : bytes, need STRG_GET_SECTOR_SIZE alignment)
    STRG_SET_PARTITION_RSV_SIZE,        ///< Set storage object partition reserved size, subset of STRG_SET_PARTITION_SIZE
                                        ///< Context are :
                                        ///< - @b UINT32 : size of partition        (Unit : bytes, need STRG_GET_BEST_ACCESS_SIZE alignment, if not aligned, driver will align automatically)
    STRG_SET_BOOT_SCCESS,               ///<

    STRG_SET_LOADER_SECTOR_COUNT,       ///< Context are : Region of loader area

    ENUM_DUMMY4WORD(STRG_SET_PARAM_EVT)
}STRG_SET_PARAM_EVT;
//@}


typedef enum
{
    STRG_FAT_TBL_HEADNUM,
    STRG_FAT_TBL_SECTORNUM,
    STRG_FAT_TBL_RELATIVESECTOR,
    STRG_FAT_TBL_SECPERCLUSTER,
    STRG_FAT_TBL_RESERVEDSECTOR,
    STRG_FAT_TBL_PARAM_CNT,
    ENUM_DUMMY4WORD(STRG_FAT_TBL_PARAM)

}STRG_FAT_TBL_PARAM;

typedef enum
{
    STRG_FAT_SIZE_0MB_4MB,
    STRG_FAT_SIZE_4MB_8MB,
    STRG_FAT_SIZE_8MB_16MB,
    STRG_FAT_SIZE_16MB_32MB,
    STRG_FAT_SIZE_32MB_64MB,
    STRG_FAT_SIZE_64MB_128MB,
    STRG_FAT_SIZE_128MB_256MB,
    STRG_FAT_SIZE_256MB_512MB,
    STRG_FAT_SIZE_512MB_1024MB,
    STRG_FAT_SIZE_1024MB_2048MB,
    STRG_FAT_SIZE_2048MB_4096MB,
    STRG_FAT_SIZE_4096MB_8192MB,
    STRG_FAT_SIZE_8192MB_16384MB,
    STRG_FAT_SIZE_16384MB_32768MB,
    STRG_FAT_TOTAL_SIZE_CNT,
    ENUM_DUMMY4WORD(STRG_FAT_PATAM_SIZE_TBL)
}STRG_FAT_PATAM_SIZE_TBL;


/**
    @name New storage object interface

    For new version storage device driver
*/
//@{
typedef struct
{
    UINT32      uiStrgType;                                         ///< Storage object identify
    UINT32      uiStrgStatus;                                       ///< Storage object status
    ER          (*Lock)(void);                                      ///< Storage object access right occupy
    ER          (*Unlock)(void);                                    ///< Storage object access right release
    ER          (*Open)(void);                                      ///< Storage object open
    ER          (*WrSectors)(INT8 *, UINT32, UINT32);               ///< Storage object write sector
                                                                    ///< Context are :
                                                                    ///< - @b INT8 * : Buffer be write from
                                                                    ///< - @b UINT32 : starting sector(logic sector)
                                                                    ///< - @b UINT32 : total write sector count
    ER          (*RdSectors)(INT8 *, UINT32, UINT32);               ///< Storage object read sector(logic sector)
                                                                    ///< Context are :
                                                                    ///< - @b INT8 * : Buffer be read to
                                                                    ///< - @b UINT32 : starting sector(logic sector)
                                                                    ///< - @b UINT32 : total write sector count
    ER          (*Close)(void);                                     ///< Storage object close
    ER          (*OpenMemBus)(void);                                ///< Open Storage bus
    ER          (*CloseMemBus)(void);                               ///< Close Storage bus
    ER          (*SetParam)(STRG_SET_PARAM_EVT, UINT32, UINT32);    ///< Set Param
    ER          (*GetParam)(STRG_GET_PARAM_EVT, UINT32, UINT32);    ///< Get Param
    ER          (*ExtIOCtrl)(STRG_EXT_CMD, UINT32, UINT32);         ///< Extend IO command
} STORAGE_OBJ, *PSTORAGE_OBJ;
//@}
#if (NEW_STORAGE_OBJECT == ENABLE)
#define STRG_TAB        STORAGE_OBJ     //For compatiable
#define PSTRG_TAB       PSTORAGE_OBJ    //For compatiable

#endif
#endif
