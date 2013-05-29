/* 
** 
** Multi-port Voice Initialization 
** 
*/ 
#include "proslic.h"
#include "si_voice_datatypes.h"
#include "si_voice_ctrl.h"
#include "si_voice.h"
#include "si_voice.h"
#include "vdaa.h"
#include "custom/vdaa_constants.h"
#include "si3217x_constants.h"
#include "spi.h"
#include "timer.h"
//#include "../pcm_ctrl.h"

#define CONFIG_PCM_TS_START	1
 
#define NUMBER_OF_DEVICES 1 
#define CHAN_PER_DEVICE 1 
#define NUMBER_OF_CHAN (NUMBER_OF_DEVICES*CHAN_PER_DEVICE) 
#define VOICE_DEVICE_TYPE SI3217X_TYPE 

#include "slic_init.h"
chanState ports[NUMBER_OF_CHAN];  /* User¡¦s channel object, which has  
                                   ** a member defined as 
                                   ** SiVoiceChanType_ptr VoiceObj; 
                                   */ 
                                   
extern uInt8 Vdaa_GetHookStatus (vdaaChanType *pVdaa);    
int CustomerVoice_HWInit() 
{
	int i; 
ctrl_S  spiGciObj;              /* User¡¦s control interface object */ 
systemTimer_S timerObj;         /* User¡¦s timer object */ 

 
/* Define Voice control interface object */ 
SiVoiceControlInterfaceType *VoiceHWIntf; 
 
/* Define array of Voice device objects */ 
SiVoiceDeviceType *VoiceDevices[NUMBER_OF_DEVICES]; 
 
/* Define array of ProSLIC channel object pointers */ 
SiVoiceChanType_ptr arrayOfVoiceChans[NUMBER_OF_CHAN]; 

    ProSLIC_dbgRingCfg ringCfg; 
    uInt8 hsState;  
    /* 
    ** Step 1: (optional) 
    ** Initialize user¡¦s control interface and timer objects ¡V this 
    ** may already be done, if not, do it here  
    */ 
 
    /*  
    ** Step 2: (required) 
    ** Create Voice Control Interface Object 
    */  
    SiVoice_createControlInterface(&VoiceHWIntf); 
 
    /* 
    ** Step 3: (required) 
    ** Create Voice Device Objects 
    */ 
    for(i=0;i<NUMBER_OF_DEVICES;i++) 
    { 
        SiVoice_createDevice(&(VoiceDevices[i])); 
    } 
 
    /* 
    ** Step 4: (required) 
    ** Create and initialize Voice channel objects 
    ** Also initialize array pointers to user¡¦s proslic channel object 
    ** members to simplify initialization process. 
    */ 
    for(i=0;i<NUMBER_OF_CHAN;i++) 
    { 
        SiVoice_createChannel(&(ports[i].VoiceObj)); 
        SiVoice_SWInitChan(ports[i].VoiceObj,i,VOICE_DEVICE_TYPE, 
                           VoiceDevices[i/CHAN_PER_DEVICE],VoiceHWIntf); 
        arrayOfVoiceChans[i] = ports[i].VoiceObj; 
        SiVoice_setSWDebugMode(ports[i].VoiceObj,TRUE);  /* optional */ 
        //ProSLIC_setSWDebugMode(ports[i].ProObj,TRUE); /* optional */
    } 
 
    /* 
    ** Step 5: (required) 
    ** Establish linkage between host objects/functions and 
    ** API  
    */ 
    SiVoice_setControlInterfaceCtrlObj (VoiceHWIntf, &spiGciObj); 
    SiVoice_setControlInterfaceReset (VoiceHWIntf, ctrl_ResetWrapper); 
    SiVoice_setControlInterfaceWriteRegister (VoiceHWIntf, ctrl_WriteRegisterWrapper); 
    SiVoice_setControlInterfaceReadRegister (VoiceHWIntf, ctrl_ReadRegisterWrapper);   
    SiVoice_setControlInterfaceWriteRAM (VoiceHWIntf, ctrl_WriteRAMWrapper); 
    SiVoice_setControlInterfaceReadRAM (VoiceHWIntf, ctrl_ReadRAMWrapper); 
    SiVoice_setControlInterfaceTimerObj (VoiceHWIntf, &timerObj); 
    SiVoice_setControlInterfaceDelay (VoiceHWIntf, time_DelayWrapper); 
    SiVoice_setControlInterfaceTimeElapsed (VoiceHWIntf, time_TimeElapsedWrapper); 
    SiVoice_setControlInterfaceGetTime (VoiceHWIntf, time_GetTimeWrapper); 
    SiVoice_setControlInterfaceSemaphore (VoiceHWIntf, NULL); 
/* 
    ** Step 6: (system dependent) 
    ** Assert hardware Reset ¡V ensure VDD, PCLK, and FSYNC are present and stable 
    ** before releasing reset 
    */ 
    SiVoice_Reset(ports[0].VoiceObj);    
 
    /* 
    ** Step 7: (required) 
    ** Initialize channels (loading of general parameters, calibrations,  
    **   dc-dc powerup, etc.) 
    ** Note that VDAA channels are ignored by ProSLIC_Init and ProSLIC  
    ** channels are ignored by Vdaa_Init. 
    */ 
    ProSLIC_Init(arrayOfVoiceChans,NUMBER_OF_CHAN);
    for(i=0;i<NUMBER_OF_CHAN;i++)
	{
		if(arrayOfVoiceChans[i]->error!=0)
		{
			printk("ProSLIC_Init[%d] ERR=%d\n",i,arrayOfVoiceChans[i]->error);
			return 0;
		}
	}
#if 0
    Vdaa_Init(arrayOfVoiceChans,NUMBER_OF_CHAN); 
#endif	
 
    /* 
    ** Step 8: (design dependent) 
    ** Execute longitudinal balance calibration 
    ** or reload coefficients from factory LB cal 
    ** 
    ** Note: all batteries should be up and stable prior to  
    ** executing the lb cal 
    */ 
    ProSLIC_LBCal(arrayOfVoiceChans,NUMBER_OF_CHAN); 
 
     
    /* 
    ** Step 9: (design dependent) 
    ** Load custom configuration presets(generated using 
    ** ProSLIC API Config Tool) 
    */ 
    for(i=0;i<NUMBER_OF_CHAN;i++) 
    { 
        ProSLIC_DCFeedSetup(ports[i].VoiceObj,DCFEED_48V_20MA); 
        ProSLIC_RingSetup(ports[i].VoiceObj,RING_F20_45VRMS_0VDC_LPR); 
        ProSLIC_PCMSetup(ports[i].VoiceObj,PCM_DEFAULT_CONFIG); 
        ProSLIC_ZsynthSetup(ports[i].VoiceObj,ZSYN_600_0_0); 
        ProSLIC_ToneGenSetup(ports[i].VoiceObj,TONEGEN_FCC_DIAL); 
        ProSLIC_PCMTimeSlotSetup(ports[i].VoiceObj,CONFIG_PCM_TS_START+i*16,CONFIG_PCM_TS_START+i*16);
    #if 0    
        Vdaa_CountrySetup(ports[i].VoiceObj,COU_USA); 
        Vdaa_HybridSetup(ports[i].VoiceObj,HYB_600_0_0_500FT_24AWG); 
        Vdaa_PCMSetup(ports[i].VoiceObj,PCM_LINEAR16); 
 	Vdaa_PCMTimeSlotSetup(ports[i].VoiceObj,CONFIG_PCM_TS_START+i*16,CONFIG_PCM_TS_START+i*16);
    #endif
    } 
     
    /* 
    ** END OF TYPICAL INITIALIZATION  
    */ 
	for(i=0;i<NUMBER_OF_CHAN;i++)
	{
		ProSLIC_PCMStart(ports[i].VoiceObj);
	}	
	
    
     /*  
    ** EXAMPLE: Change ALL ProSLIC ports to FWD_ACTIVE linefeed state 
    */ 
	for(i=0;i<NUMBER_OF_CHAN;i++) 
	{ 
		ProSLIC_SetLinefeedStatus(ports[i].VoiceObj,LF_FWD_ACTIVE); 
	} 
	#if 0
	
	ProSLIC_SetDAAEnable(ports[1].VoiceObj, 1);
	for(i=0;i<NUMBER_OF_CHAN;i++)
	{
		Vdaa_PCMStart(ports[i].VoiceObj);
	}
	
	
	hsState = Vdaa_GetHookStatus(ports[1].VoiceObj);
	if(hsState == VDAA_ONHOOK) {
		Vdaa_SetHookStatus(ports[1].VoiceObj, VDAA_OFFHOOK); 
	}      
	
	hsState = Vdaa_PowerupLineside(ports[1].VoiceObj);     
	#endif
	
	//ProSLIC_PrintDebugData(ports[0].VoiceObj);
	//Vdaa_PrintDebugData(ports[1].VoiceObj);
	
	
	return 1;
} 

unsigned int Vdaa_ReadReg(int index)
{
	switch(index)
	{
	case 13:
	return Vdaa_ReadFDTStatus(ports[1].VoiceObj);
	default:
	return 0;
	}
	
}	
