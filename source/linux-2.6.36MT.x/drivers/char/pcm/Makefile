EXTRA_CFLAGS+=-DSLIC_SILICON
EXTRA_CFLAGS+=-DPCM_LOOPBACK_TEST

EXTRA_CFLAGS += -Idrivers/char/pcm/pcm_slic/proslic_api/inc -Idrivers/char/pcm/pcm_slic/proslic_api/custom
EXTRA_CFLAGS += -Idrivers/char/pcm/pcm_slic/proslic_api/example_drivers/ralink/proslic_ppt_mb
EXTRA_CFLAGS += -Idrivers/char/pcm/pcm_slic/proslic_api/example_drivers/ralink/timer
			
#CONFIG_SLIC_SI322X := y
ifeq ($(CONFIG_SLIC_SI322X),y)
EXTRA_CFLAGS += -DSI322X
endif

#CONFIG_SLIC_SI3217X := y
ifeq ($(CONFIG_SLIC_SI3217X),y)
EXTRA_CFLAGS += -DSI3217X
endif

CUSTOM_DIR = ./proslic_api/custom
CUSTOM_OBJ = \
			$(CUSTOM_DIR)/vdaa_constants.o

ifeq ($(CONFIG_SLIC_SI322X),y)
CUSTOM_OBJ += $(CUSTOM_DIR)/si3226_constants.o
CUSTOM_OBJ += $(CUSTOM_DIR)/si3226_patch_C_FLBK_2008NOV20.o
CUSTOM_OBJ += $(CUSTOM_DIR)/si3226_patch_D_FLBK_2008NOV20.o
CUSTOM_OBJ += $(CUSTOM_DIR)/si3226_patch_E_FLBK_2010JAN06.o
#CUSTOM_OBJ += $(CUSTOM_DIR)/si3226_patch_C_QCUK_2008NOV20.o
#CUSTOM_OBJ += $(CUSTOM_DIR)/si3226_patch_D_QCUK_2008NOV20.o
CUSTOM_OBJ += $(CUSTOM_DIR)/si3226_patch_E_FLBK_2010JAN06.o
endif

ifeq ($(CONFIG_SLIC_SI321X),y)
CUSTOM_OBJ += $(CUSTOM_DIR)/si321x_constants.o 
endif

ifeq ($(CONFIG_SLIC_SI324X),y)
CUSTOM_OBJ += $(CUSTOM_DIR)/si324x_constants.o 
CUSTOM_OBJ += $(CUSTOM_DIR)/si324x_patch_D_2008JUN23.o 
endif

ifeq ($(CONFIG_SLIC_SI3217X),y)
CUSTOM_OBJ += $(CUSTOM_DIR)/si3217x_FLBK_constants.o
#CUSTOM_OBJ += $(CUSTOM_DIR)/si3217x_patch_B_BB_2009SEP22.o
CUSTOM_OBJ += $(CUSTOM_DIR)/si3217x_patch_B_FB_2009SEP22.o
#CUSTOM_OBJ += $(CUSTOM_DIR)/si3217x_BKBT_constants.o
CUSTOM_OBJ += $(CUSTOM_DIR)/vdaa_constants.o
endif
			
SRC_DIR = ./proslic_api/src
SRC_OBJ = $(SRC_DIR)/si_voice_version.o $(SRC_DIR)/vdaa.o \
			$(SRC_DIR)/proslic.o $(SRC_DIR)/si_voice.o

ifeq ($(CONFIG_SLIC_SI322X),y)
SRC_OBJ += $(SRC_DIR)/si3226_intf.o 
endif

ifeq ($(CONFIG_SLIC_SI324X),y)
SRC_OBJ += $(SRC_DIR)/si324x_intf.o 
endif

ifeq ($(CONFIG_SLIC_SI321X),y)
SRC_OBJ += $(SRC_DIR)/si321x_intf.o 
endif

#ifeq ($(CONFIG_SLIC_SI322X),y)			
RALINK_DIR = ./proslic_api/example_drivers/ralink/proslic_ppt_mb/si3226
#endif

ifeq ($(CONFIG_SLIC_SI324X),y)			
RALINK_DIR = ./proslic_api/example_drivers/ralink/proslic_ppt_mb/si324x
endif

ifeq ($(CONFIG_SLIC_SI321X),y)			
RALINK_DIR = ./proslic_api/example_drivers/ralink/proslic_ppt_mb/si321x
endif

ifeq ($(CONFIG_SLIC_SI3217X),y)
SRC_OBJ += $(SRC_DIR)/si3217x_intf.o
SRC_OBJ += $(SRC_DIR)/si_voice.o  
endif


RALINK_OBJ = $(RALINK_DIR)/proslic_spi_api.o
TIMER_DIR = ./proslic_api/example_drivers/ralink/timer
RALINK_OBJ += $(TIMER_DIR)/proslic_timer_intf_linux.o ./proslic_api/fxsfxo_init.o ./proslic_api/slic_init.o


PCM_DRIVER_DIR := .
obj-m +=$(PCM_DRIVER_DIR)/ralink_pcm.o
$(PCM_DRIVER_DIR)/ralink_pcm-objs := $(PCM_DRIVER_DIR)/pcmdriver.o \
						$(PCM_DRIVER_DIR)/slic_ctrl.o	\
						$(PCM_DRIVER_DIR)/_pcm.o
#$(SRC_OBJ) $(CUSTOM_OBJ) $(RALINK_OBJ)
	
clean:
	rm -rf $(PCM_DRIVER_DIR)/*.o $(PCM_DRIVER_DIR)/*.ko $(PCM_DRIVER_DIR)/*.mod.c
	rm -f Module.symvers
	@date
