#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/module.h> 

#include "cls_io.h"

extern int ra_classifier_init(void);
extern void ra_classifier_release(void);
extern int classifier_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

#ifdef  CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
static	devfs_handle_t devfs_handle;
#endif

static int clsdrv_major =  235;
#if defined(CONFIG_SYSFS_DEPRECATED)
static struct class *clsmodule_class; 
#endif


struct file_operations classifier_fops = {
    ioctl:      classifier_ioctl,
};

int __init ra_classifier_init_module(void)
{
	printk("Insert Ralink Classifier Module\n");
	return ra_classifier_init();
}

void ra_classifier_cleanup_module(void)
{
	ra_classifier_release();
	printk("Remove Ralink Classifier Module\n");
	return;
}


int reg_classifier_ioctl(void)
{
#ifdef  CONFIG_DEVFS_FS
    if(devfs_register_chrdev(clsdrv_major, RA_CLASSIFIER_DEVNAME , &classifier_fops)) {
		printk(KERN_WARNING " cls: can't create device node - %s\n",RA_CLASSIFIER_DEVNAME);
		return -EIO;
    }

    devfs_handle = devfs_register(NULL, RA_CLASSIFIER_DEVNAME, DEVFS_FL_DEFAULT, clsdrv_major, 0, 
	    S_IFCHR | S_IRUGO | S_IWUGO, &classifier_fops, NULL);
#else
    int result=0;
    result = register_chrdev(clsdrv_major, RA_CLASSIFIER_DEVNAME, &classifier_fops);
    if (result < 0) {
		printk(KERN_WARNING "cls: can't get major %d\n",clsdrv_major);
        return result;
    }

    if (clsdrv_major == 0) {
		clsdrv_major = result; /* dynamic */
    }
#endif
#if defined(CONFIG_SYSFS_DEPRECATED)
	clsmodule_class=class_create(THIS_MODULE, RA_CLASSIFIER_DEVNAME);
	if (IS_ERR(clsmodule_class)) 
		return -EFAULT;
	device_create(clsmodule_class, NULL, MKDEV(clsdrv_major, 0), RA_CLASSIFIER_DEVNAME); 
#endif	
    return 0;
}



void unreg_classifier_ioctl(void)
{

#ifdef  CONFIG_DEVFS_FS
    devfs_unregister_chrdev(clsdrv_major, RA_CLASSIFIER_DEVNAME);
    devfs_unregister(devfs_handle);
#else
    unregister_chrdev(clsdrv_major, RA_CLASSIFIER_DEVNAME);
#endif
#if defined(CONFIG_SYSFS_DEPRECATED)
	device_destroy(clsmodule_class,MKDEV(clsdrv_major, 0));
	class_destroy(clsmodule_class);
#endif	 

}

EXPORT_SYMBOL(reg_classifier_ioctl);
EXPORT_SYMBOL(unreg_classifier_ioctl);

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,12)
MODULE_PARM (clsdrv_major, "i");
#else
module_param (clsdrv_major, int, 0);
#endif

module_init(ra_classifier_init_module);
module_exit(ra_classifier_cleanup_module);

MODULE_DESCRIPTION("Ralink Network Classifier Module");
MODULE_AUTHOR("Qwert");
#if defined(CONFIG_SYSFS_DEPRECATED)
MODULE_LICENSE("GPL");
#else
MODULE_LICENSE("Proprietary");
#endif
