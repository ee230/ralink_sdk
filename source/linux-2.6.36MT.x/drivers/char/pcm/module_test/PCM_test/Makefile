PCM_DRIVER_TEST_DIR := .
obj-m +=$(PCM_DRIVER_TEST_DIR)/pcmdrivertest.o
KERNELDIR=$(KERNEL_DIR)
#mips_strip := mips-linux-gnu-strip 
#COMPLIER := mips-linux-gnu-gcc
Target := modules
all:
	make -C $(KERNELDIR) M=`pwd` modules
	#$(MAKE) -C $(KERNELDIR) M=$(PCM_DRIVER_TEST_DIR) modules
	
	$(STRIP) --strip-unneeded $(PCM_DRIVER_TEST_DIR)/pcmdrivertest.ko
	@date
clean:
	rm -rf $(PCM_DRIVER_TEST_DIR)/*.o $(PCM_DRIVER_TEST_DIR)/*.ko $(PCM_DRIVER_TEST_DIR)/*.mod.c
	rm -rf Module.symvers
	@date
