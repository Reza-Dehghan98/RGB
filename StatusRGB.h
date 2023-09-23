/*In the Name of God*/
/**
 * @file StatusRGB.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef _STATUS_RGB_H_
#define _STATUS_RGB_H_



#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h> 

/**/



#define STATUS_RGB_CHECK_ENABLE         0
#define STATUS_RGB_REPEAT               0
#define STATUS_RGB_IDLE_TIME            100
#define STATUS_RGB_ACTIVE_STATE         1


typedef void*     StatusRGB_Tim;
typedef uint16_t  StatusRGB_Channel;
typedef uint32_t  StatusRGB_Timestamp;
typedef uint8_t   StatusRGB_LenType;
typedef uint8_t   StatusRGB_Value; 



#define  RGB_RED_COLOR           {255, 0,     0}
#define  RGB_ORANGE_COLOR        {255, 165,   0}
#define  RGB_BLUE_COLOR    	     {0,   0,   255}
#define  RGB_GREEN_COLOR    	 {0,   255,   0}
#define  RGB_YELLOW_COLOR    	 {255, 255,   0}
#define  RGB_PURPLE_COLOR    	 {255, 0,   255}
#define  RGB_CYAN_COLOR    	     {0,   255, 255}


/*
typedef enum {
 RGB_RedColor    		= 0XFF0000,
 RGB_OrangeColor        = 0XFFA500,
 RGB_YellowColor    	= 0XFFFF00,
 RGB_GreenColor     	= 0X008000, 
 RGB_BlueColor    		= 0X0000FF,
 RGB_CyanColor    		= 0X00FFFF,
 RGB_PurpleColor        = 0X800080,
} RGB_RainBowColor;
*/

typedef enum {
    StatusRGB_Status_Ok           = 0,
    StatusRGB_Status_NoSpace      = 1,
    StatusRGB_Status_Null         = 2,
    StatusRGB_Status_NotFound     = 3,
} StatusRGB_Status;


/**
 * @brief for PullDown and OpenDrain 
 */
typedef enum {
    StatusRGB_ActiveState_Low     = 0,
    StatusRGB_ActiveState_High    = 1,
} StatusRGB_ActiveState;


/**
 * @brief for active Repeat Mode
 */
typedef enum {
    StatusRGB_Repeat_Off         = 0,
    StatusRGB_Repeat_On          = 1,
} StatusRGB_RepeatMode;



/**
 * @brief 
 */
typedef enum {
    StatusRGB_RGBState_On        = 0,
    StatusRGB_RGBState_Off       = 1,
} StatusRGB_RGBState; 



typedef struct {
    StatusRGB_Tim               Tim; 
    StatusRGB_Channel           Channel;
} StatusRGB_PWMConfig;



typedef union {
    StatusRGB_PWMConfig         RGBConfig[3];
    struct{
        StatusRGB_PWMConfig     Red;
        StatusRGB_PWMConfig     Green;
        StatusRGB_PWMConfig     Blue;
    };
} StatusRGB_Config;



typedef union {                 
    StatusRGB_Value             Colors[3];
    struct{
        StatusRGB_Value         Red;
        StatusRGB_Value         Green;
        StatusRGB_Value         Blue;
    };
} StatusRGB_Color;



typedef union{
    uint32_t                    Time;
	uint16_t                    OnOff[2];
    struct{
        uint16_t                On;
        uint16_t                Off;
    };
} StatusRGB_CycleTime;


#define  RGB_ON_OFF_TIME(A,B)   {.On = A, .Off = B}


typedef struct {
    StatusRGB_Color            Color;
    StatusRGB_CycleTime        Time;
} StatusRGB_Cycle;


#define  STATUS_RGB_LEN(ARR)            (sizeof(ARR) / sizeof(ARR[0]))
#define  STATUS_RGB_PATTERN(Cycle)      {(const StatusRGB_Cycle*)(Cycle), STATUS_RGB_LEN(Cycle)}


typedef struct {
    const StatusRGB_Cycle*      Cycle;
    StatusRGB_LenType           Len;
} StatusRGB_Pattern;


/**
 * @brief RGB Operation Mode
 */
typedef enum {
    StatusRGB_Mode_Normal        	 = 0,
	StatusRGB_Mode_PulseOnOff    	 = 1,
} StatusRGB_Mode;



/**
 * @brief pre Defined
 */
struct  __StatusRGB;
typedef struct __StatusRGB StatusRGB;



/**
 * @brief FunctionPointers 
 */
typedef void (*StatusRGB_WritePinFn)(const StatusRGB_Config* config, StatusRGB_Value r, StatusRGB_Value g, StatusRGB_Value b);
typedef StatusRGB_Timestamp (*StatusRGB_GetTimestampFn)(void); 

typedef StatusRGB_Timestamp (*StatusRGB_SelectModeFn)  (StatusRGB* rgb, StatusRGB_Timestamp time);



#define STATUS_RGB_NULL                     ((StatusRGB*) 0)
#define STATUS_RGB_PATTERN_NULL             ((StatusRGB_Pattern*) 0)
#define STATUS_RGB_CONFIG_NULL              ((StatusRGB_Config*) 0)


typedef struct {
    StatusRGB_GetTimestampFn    getTimestamp;
    StatusRGB_WritePinFn        writePin;
} StatusRGB_Driver;



typedef struct __StatusRGB {
    struct __StatusRGB*         Previous;
	
    StatusRGB_Config*           Config;          /**< RGB Config Io Port And Pin */
    StatusRGB_Pattern*          Pattern;         /**< RGB Pattern */
    StatusRGB_Mode              Mode;            /**< RGB Mode */
    StatusRGB_Timestamp         NextTick;        /**< next Tick time */
    StatusRGB_LenType           PatternIndex;    /**< pattern length */
    uint8_t                     Enabled     : 1; /**< enabled RGB */
    uint8_t                     Configured  : 1; /**< configured */
    uint8_t                     Repeat      : 1; /**< repeat mode */
    uint8_t                     ActiveState : 1; /**< active state */
    uint8_t                     State       : 1; /**< state */
    uint8_t                     Reserved    : 3; /**< reserved */
} StatusRGB;





/**  Function protoType **/
StatusRGB_Timestamp  StatusRGB_handle (void);
void StatusRGB_init (const StatusRGB_Driver* driver);
void StatusRGB_setConfig  (StatusRGB* rgb, const StatusRGB_Config* config);
void StatusRGB_setPattern (StatusRGB* rgb, const StatusRGB_Pattern* pattern);
void StatusRGB_setMode (StatusRGB* rgb, StatusRGB_Mode mode);
const StatusRGB_Config* StatusRGB_getConfig (StatusRGB* rgb);


void StatusRGB_setActiveState(StatusRGB* rgb, StatusRGB_ActiveState state);
StatusRGB_ActiveState StatusRGB_getActiveState(StatusRGB* rgb);


#if STATUS_RGB_CHECK_ENABLE
void StatusRGB_setEnabled(StatusRGB* rgb, uint8_t enabled);
uint8_t StatusRGB_isEnabled(StatusRGB* rgb);
#endif


#if STATUS_RGB_REPEAT
void StatusRGB_setRepeat(StatusRGB* rgb, StatusRGB_RepeatMode repeat);
StatusRGB_RepeatMode StatusRGB_getRepeat(StatusRGB* rgb);
#endif


StatusRGB_Timestamp RGB_NormalProcess     (StatusRGB* rgb, StatusRGB_Timestamp time);
StatusRGB_Timestamp RGB_OnOffPulseProcess (StatusRGB* rgb, StatusRGB_Timestamp time);


StatusRGB_Status StatusRGB_remove(StatusRGB* remove);
StatusRGB_Status StatusRGB_add(StatusRGB* rgb, const StatusRGB_Config* config);

#ifdef __cplusplus
}
#endif

#endif