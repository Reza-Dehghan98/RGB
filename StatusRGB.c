#include "R:\TahvilBahman\RGB\Src\StatusRGB.h"

/* private variables */
static const StatusRGB_Driver* rgbDriver;

static StatusRGB* lastRGB = STATUS_RGB_NULL;
    
#define __rgbs()            lastRGB
#define __next(RGB)         RGB = (RGB)->Previous

/*
ADD
SET pattern
Normal Mode
Handle
*/

#if STATUS_RGB_ACTIVE_STATE
		
	#define  __RGBWritePwmOff(RGB)   if ((RGB)->ActiveState){ \
                                                             rgbDriver->writePin((RGB)->Config, 0, 0, 0);\
                                                            }\
                                                            else { \
                                                            rgbDriver->writePin((RGB)->Config, ~0, ~0, ~0);\
                                                            }
		
    #define  __RGBWritePwm(RGB)   if ((RGB)->ActiveState) { \
                                                                rgbDriver->writePin((RGB)->Config, \
                                                                                    (RGB)->Pattern->Cycle[RGB->PatternIndex].Color.Red, \
                                                                                    (RGB)->Pattern->Cycle[RGB->PatternIndex].Color.Green, \
                                                                                    (RGB)->Pattern->Cycle[RGB->PatternIndex].Color.Blue);\
                                                          }\
                                                           else { \
                                                                rgbDriver->writePin((RGB)->Config, \
                                                                                    ~(RGB)->Pattern->Cycle[RGB->PatternIndex].Color.Red, \
                                                                                    ~(RGB)->Pattern->Cycle[RGB->PatternIndex].Color.Green, \
                                                                                    ~(RGB)->Pattern->Cycle[RGB->PatternIndex].Color.Blue);\
                                                          }
#else
    #define  __RGBWritePwm(RGB)         rgbDriver->writePin((RGB)->Config, \
                                                                        (RGB)->Pattern->Cycle[RGB->PatternIndex].Color.Red, \
																		(RGB)->Pattern->Cycle[RGB->PatternIndex].Color.Green, \
                                                                        (RGB)->Pattern->Cycle[RGB->PatternIndex].Color.Blue);
                                                          
    #define  __RGBWritePwmOff(RGB)      rgbDriver->writePin((RGB)->Config, 0, 0, 0);
                                                            
#endif


static const StatusRGB_SelectModeFn RGB_SELECT_MODE[2] = {
    RGB_NormalProcess,
    RGB_OnOffPulseProcess
};




StatusRGB_Timestamp  StatusRGB_handle(void) {
    StatusRGB_Timestamp timestamp = rgbDriver->getTimestamp();
    StatusRGB* pRGB = __rgbs();
    StatusRGB_Timestamp  nextTimestamp = 0;

     while (STATUS_RGB_NULL != pRGB) {
    #if STATUS_RGB_CHECK_ENABLE
        if(pRGB->Enabled){
    #endif

        if (timestamp >= pRGB->NextTick 
    #if STATUS_RGB_REPEAT
            && pRGB->PatternIndex < pRGB->Pattern->Len
    #endif
        ) {
            RGB_SELECT_MODE[pRGB->Mode](pRGB, timestamp);
            
            if (nextTimestamp <= pRGB->NextTick && nextTimestamp != 0) {
								nextTimestamp =  pRGB->NextTick;
            }        
        }
    #if STATUS_RGB_CHECK_ENABLE
        }
    #endif // STATUS_RGB_CHECK_ENABLE
        __next(pRGB);
    }
    return nextTimestamp ? nextTimestamp - timestamp : STATUS_RGB_IDLE_TIME;
}



StatusRGB_Timestamp RGB_NormalProcess (StatusRGB* rgb, StatusRGB_Timestamp time) {
    
    rgb->NextTick = time + rgb->Pattern->Cycle[rgb->PatternIndex].Time.Time;
    __RGBWritePwm(rgb);
    if (++rgb->PatternIndex >= rgb->Pattern->Len) {

    #if STATUS_RGB_REPEAT
        if (rgb->Repeat) {
    #endif
		rgb->PatternIndex = 0;
	#if STATUS_RGB_REPEAT
        }
    #endif
    }
}



StatusRGB_Timestamp RGB_OnOffPulseProcess(StatusRGB* rgb, StatusRGB_Timestamp time){
	rgb->NextTick = time + rgb->Pattern->Cycle[rgb->PatternIndex].Time.OnOff[!rgb->State]; 
	if(rgb->State){
		__RGBWritePwm(rgb);
	}
	else {
       __RGBWritePwmOff(rgb);		
	}
	rgb->State = !rgb->State;
	
    if (!rgb->State) {
        if (++rgb->PatternIndex >= rgb->Pattern->Len) {
            
        #if STATUS_RGB_REPEAT
            if (rgb->Repeat) {
        #endif
                rgb->PatternIndex = 0;
        #if STATUS_RGB_REPEAT
            }
        #endif
        }
    }
}



/**
 * @brief set RGB driver to USE
 * 
 * @param driver 
 */
void StatusRGB_init (const StatusRGB_Driver* driver) {
    rgbDriver = driver;
}



/**
 * @brief set RGB IO Port And Pin Config
 * 
 * @param rgb 
 * @param config 
 */
void StatusRGB_setConfig (StatusRGB* rgb, const StatusRGB_Config* config) {
    rgb->Config = config;
}



void StatusRGB_setPattern (StatusRGB* rgb, const StatusRGB_Pattern* pattern) {
    rgb->Pattern = pattern;
}




/**
 * @brief set RGB Handle Mode
 * 
 * @param rgb 
 * @param mode 
 */
void StatusRGB_setMode (StatusRGB* rgb, StatusRGB_Mode mode){
    rgb->Mode = mode;
}




/**
 * @brief tune OpenDrain Or PushPull 
 * 
 * @param rgb 
 * @param state 
 */
void StatusRGB_setActiveState (StatusRGB* rgb, StatusRGB_ActiveState state) {
    rgb->ActiveState = state;
}




/**
 * @brief set RGB Enable
 * 
 * @param rgb 
 * @param enabled 
 */
void StatusRGB_setEnabled (StatusRGB* rgb, uint8_t enabled) {
    rgb->Enabled = enabled;
}




/**
 * @brief set RGB Repeat
 * 
 * @param rgb 
 * @param repeat 
 */
void StatusRGB_setRepeat (StatusRGB* rgb, StatusRGB_RepeatMode repeat) {
    rgb->Repeat = repeat;
}





const StatusRGB_Config* StatusRGB_getConfig(StatusRGB* rgb) {
    return rgb->Config;
}




StatusRGB_ActiveState StatusRGB_getActiveState(StatusRGB* rgb) {
    return (StatusRGB_ActiveState) rgb->ActiveState;
}


uint8_t StatusRGB_isEnabled(StatusRGB* rgb) {
    return rgb->Enabled;
}


StatusRGB_RepeatMode StatusRGB_getRepeat(StatusRGB* rgb) {
    return rgb->Repeat;
}





StatusRGB_Status StatusRGB_add(StatusRGB* rgb, const StatusRGB_Config* config) {
    // check for null
    if (STATUS_RGB_NULL == rgb) {
        return StatusRGB_Status_Null;
    }
    // add new led to list
    rgb->State        = StatusRGB_RGBState_On;
    rgb->Pattern      = STATUS_RGB_PATTERN_NULL;
    rgb->PatternIndex = 0;
    rgb->NextTick     = 0;
    StatusRGB_setConfig(rgb, config);
    // init IOs

    // add rgb to linked list
    rgb->Previous = lastRGB;
    lastRGB       = rgb;

    rgb->Configured = 1;
    rgb->Enabled    = 1;
    return StatusRGB_Status_Ok;
}





StatusRGB_Status StatusRGB_remove(StatusRGB* remove) {

    StatusRGB* pRGB = lastRGB;
    
    if (remove == pRGB) {
        pRGB->Previous = remove->Previous;
        remove->Previous = STATUS_RGB_NULL;
        remove->Configured = 0;
        remove->Enabled = 0;
        return StatusRGB_Status_Ok;
    }
    while (STATUS_RGB_NULL != pRGB) {
        if (remove == pRGB->Previous) {
           
            // remove key dropped from link list
            pRGB->Previous = remove->Previous;
            remove->Previous = STATUS_RGB_NULL;
            remove->Configured = 0;
            remove->Enabled = 0;
            return StatusRGB_Status_Ok;
        }
        pRGB = pRGB->Previous;
    }

    return StatusRGB_Status_NotFound;
}



