#include "MediaRecAPI.h"
#include "UIMovieObj.h"
#include "UIMovieInfo.h"
#include "audio.h"

typedef struct
{
    UINT32  uiWidth;
    UINT32  uiHeight;
    UINT32  uiVidFrameRate;
    UINT32  uiTargetBitrate;
    UINT32  uiDispAspectRatio;
} MOVIE_SIZE_PARAM;

//Supporting FR 30 & 15
//static UINT32 uiMovieFRTable[2]={30,15};

static MOVIE_SIZE_PARAM g_MovieSizeTable[MOVIE_SIZE_ID_MAX] =
{
#if 1 // real sensor
    {1920,  1080,   30,     1450*1024,  VIDENC_DAR_DEFAULT  },  // 1080P, 30fps,  1450KB/sec, DAR default
    {1280,  720,    30,     800*1024,   VIDENC_DAR_DEFAULT  },  // 720P,  30fps,  800KB/sec, DAR default
    { 848,  480,    30,     400*1024,   VIDENC_DAR_DEFAULT  },  // WVGA,  30fps,  400KB/sec, DAR default
    { 640,  480,    30,     400*1024,   VIDENC_DAR_DEFAULT  },  // VGA,   30fps,  400KB/sec, DAR default
//  { 320,  240,    120,    600*1024,   VIDENC_DAR_DEFAULT  }   // QVGA,  120fps,  600KB/sec, DAR default
#else // FPGA
    {1920,  1080,   2,      120*1024,   VIDENC_DAR_DEFAULT  },  // 1080P, 2fps,    120KB/sec, DAR default
    {1280,  720,    2,      53*1024,    VIDENC_DAR_DEFAULT  },  // 720P,  2fps,     53KB/sec, DAR default
    { 854,  480,    3,      54*1024,    VIDENC_DAR_DEFAULT  },  // WVGA,  3fps,     54KB/sec, DAR default
    { 640,  480,    3,      40*1024,    VIDENC_DAR_DEFAULT  },  // VGA,   3fps,     40KB/sec, DAR default
    { 320,  240,    3,      20*1024,    VIDENC_DAR_DEFAULT  }   // QVGA,  3fps,     20KB/sec, DAR default
#endif
};

// 2-path image source
#if 0//(_SENSOR2_720P_)
static MOVIE_SIZE_PARAM g_MovieSizeTable_2p[2][MOVIE_SIZE_DUAL_ID_MAX] =
{
    {
    {1440,  1080,   30,     1200*1024,  VIDENC_DAR_16_9     },  // 1080P, 30fps,  1200KB/sec, DAR 16:9
    {1280,   720,   30,      800*1024,  VIDENC_DAR_DEFAULT  }   // 720P,  30fps,   800kB/sec, DAR default
    },

    {
    {1280,   720,   30,      800*1024,  VIDENC_DAR_DEFAULT  },  // 720P,  30fps,   800KB/sec, DAR default
    {1280,   720,   30,      800*1024,  VIDENC_DAR_DEFAULT  }   // 720P,  30fps,   800KB/sec, DAR default
    }
};
#else // sensor2 = D1
static MOVIE_SIZE_PARAM g_MovieSizeTable_2p[2][MOVIE_SIZE_DUAL_ID_MAX] =
{
    {
    {1920,  1080,   25,     1450*1024,  VIDENC_DAR_DEFAULT  },  // 1080P, 30fps,  1450KB/sec, DAR default
    {1280,   720,   30,      800*1024,  VIDENC_DAR_DEFAULT  }   // 720P,  30fps,   800kB/sec, DAR default
    },

    {
    //{640,    480,   30,      400*1024,  VIDENC_DAR_DEFAULT  },  // 480P,  30fps,   400KB/sec, DAR default
    //{640,    480,   30,      400*1024,  VIDENC_DAR_DEFAULT  }    // 480P,  30fps,   400KB/sec, DAR default
    {720,    480,   30,      400*1024,  VIDENC_DAR_DEFAULT  },  // 480P,  30fps,   400KB/sec, DAR default
    {720,    480,   30,      400*1024,  VIDENC_DAR_DEFAULT  }    // 480P,  30fps,   400KB/sec, DAR default
    }
};
#endif

#if 0
static UINT32 uiMovieQuaTable[3]=
{
    MOVOBJ_REC_QUALITY_BEST,
    MOVOBJ_REC_QUALITY_NORMAL,
    MOVOBJ_REC_QUALITY_DRAFT
};
#endif

#if 0
UINT32 GetMovieQualityValue(UINT32 uiIndex)
{
    return uiMovieQuaTable[uiIndex];
}
#endif

// 1-path image source
UINT32 GetMovieSizeWidth(UINT32 uiIndex)
{
    return g_MovieSizeTable[uiIndex].uiWidth;
}
UINT32 GetMovieSizeHeight(UINT32 uiIndex)
{
    return g_MovieSizeTable[uiIndex].uiHeight;
}
UINT32 GetMovieFrameRate(UINT32 uiIndex)
{
    return g_MovieSizeTable[uiIndex].uiVidFrameRate;
}
UINT32 GetMovieTargetBitrate(UINT32 uiSize, UINT32 uiQuality)
{
    // [TO DO]: set different target bit rate according to quality setting (if necessary)
    return g_MovieSizeTable[uiSize].uiTargetBitrate;
}
UINT32 GetMovieDispAspectRatio(UINT32 uiIndex)
{
    return g_MovieSizeTable[uiIndex].uiDispAspectRatio;
}

// 2-path image source
UINT32 GetMovieSizeWidth_2p(UINT32 uiPath, UINT32 uiIndex)
{
    return g_MovieSizeTable_2p[uiPath][uiIndex].uiWidth;
}
UINT32 GetMovieSizeHeight_2p(UINT32 uiPath, UINT32 uiIndex)
{
    return g_MovieSizeTable_2p[uiPath][uiIndex].uiHeight;
}
UINT32 GetMovieFrameRate_2p(UINT32 uiPath, UINT32 uiIndex)
{
    return g_MovieSizeTable_2p[uiPath][uiIndex].uiVidFrameRate;
}
UINT32 GetMovieTargetBitrate_2p(UINT32 uiPath, UINT32 uiIndex)
{
    return g_MovieSizeTable_2p[uiPath][uiIndex].uiTargetBitrate;
}
UINT32 GetMovieDispAspectRatio_2p(UINT32 uiPath, UINT32 uiIndex)
{
    return g_MovieSizeTable_2p[uiPath][uiIndex].uiDispAspectRatio;
}

UINT32 GetMovieAudioVolumeValue(UINT32 uiIndex)
{
    UINT32 audVol = AUDIO_VOL_7;
    switch (uiIndex)
    {
        case MOVIE_AUDIO_VOL_MUTE:
            audVol = 0;
            break;
        case MOVIE_AUDIO_VOL_0:
            audVol=10;
            break;
        case MOVIE_AUDIO_VOL_1:
            audVol=15;
            break;
        case MOVIE_AUDIO_VOL_2:
            audVol=35;
            break;
        case MOVIE_AUDIO_VOL_3:
            audVol=50;
            break;
        case MOVIE_AUDIO_VOL_4:
            audVol=65;
            break;
        case MOVIE_AUDIO_VOL_5:
            audVol=70;
            break;
        case MOVIE_AUDIO_VOL_6:
            audVol=90;
            break;
        case MOVIE_AUDIO_VOL_7:
        default:
            audVol=100;
            break;
    }
    return audVol;
}

UINT32 Get_MovieFormatValue(UINT32 uhIndex)
{
    switch(uhIndex)
    {
        case WIFI_RTSP_LIVEVIEW:
            return MEDIAREC_LIVEVIEW;
        case WIFI_RTSP_REC:
            return MEDIAREC_AUD_VID_BOTH;
        default:
            return MEDIAREC_AUD_VID_BOTH;
    }
}
