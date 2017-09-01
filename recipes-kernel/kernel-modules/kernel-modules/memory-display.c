#include <linux/init.h>             // macros used to mark up functions e.g. __init __exit
#include <linux/module.h>           // core header for loading LKMs into the kernel
#include <linux/fs.h>               // file_operations
#include <linux/slab.h>             // kmalloc
#include <linux/cdev.h>             // cdev utilities
#include <linux/uaccess.h>          // copy_(to,from)_user
#include <linux/fb.h>               // linux framebuffer operations
#include <linux/spi/spi.h>          // linux spi bus operations
#include <linux/gpio.h>             // Required for the GPIO functions
#include <linux/workqueue.h>        // work queue handling for linux
#include <linux/mm.h>               // linux virtual memory manager primitives
#include <linux/platform_device.h>

#define MYDEV_NAME "memdisplay"
#define WIDTH   400
#define HEIGHT  240
#define BPP     1
#define FPS     4       // Start with 4 times a second

static int memdisplay_open(struct inode *inode, struct file *file)
{
    pr_info("OPENING device: %s\n\n", MYDEV_NAME);
    return 0;
}

static int memdisplay_release(struct inode *inode, struct file *file)
{
    pr_info("CLOSING device: %s\n\n", MYDEV_NAME);
    return 0;
}

static ssize_t memdisplay_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
    return 0;
}

static ssize_t memdisplay_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos)
{
    return 0;
}

static const struct file_operations memdisplay_fops = {
    .owner = THIS_MODULE,
    .read = memdisplay_read,
    .write = memdisplay_write,
    .open = memdisplay_open,
    .release = memdisplay_release,
};

static int memdisplay_spi_probe(struct spi_device *spi){

    pr_info("%s - SPI PROBE function called\n", MYDEV_NAME);
    return 0;
}

static int memdisplay_spi_remove(struct spi_device *spi){

    pr_info("%s - SPI REMOVE function called\n", MYDEV_NAME);
    return 0;
}

static int memdisplay_probe_pdev(struct platform_device *pdev){

    pr_info("%s - Platform device PROBE function called\n", MYDEV_NAME);
    return 0;
}

static int memdisplay_remove_pdev(struct platform_device *pdev){

    pr_info("%s - Platform device REMOVE function called\n", MYDEV_NAME);
    return 0;
}

static const struct of_device_id dt_ids[] = {
    { .compatible = "sharp,ls027b7dh01" },
    {},
};

MODULE_DEVICE_TABLE(of, dt_ids);

static struct spi_driver memdisplay_spi_driver = {
	.driver = {
		.name   = MYDEV_NAME,
		.owner  = THIS_MODULE,
        .of_match_table = of_match_ptr(dt_ids),
	},
	.probe  = memdisplay_spi_probe,
	.remove = memdisplay_spi_remove,
};

static struct platform_driver memdisplay_platform_driver = {
	.driver = {
		.name   = MYDEV_NAME,
		.owner  = THIS_MODULE,
        .of_match_table = of_match_ptr(dt_ids),
	},
	.probe  = memdisplay_probe_pdev,
	.remove = memdisplay_remove_pdev,
};  

static int __init memdisplay_init(void) 
{
    int ret;

    ret = spi_register_driver(&memdisplay_spi_driver);
    if(ret < 0){
        pr_err(KERN_ERR "Failed to register memory display driver: %d", ret);
        return ret;
    }

    ret = platform_driver_register(&memdisplay_platform_driver);
    if(ret < 0){
        pr_err(KERN_ERR "Failed to register memory display platform driver: %d", ret);
        return ret;
    }
    else{
        pr_info("%s - __INIT function called successfully\n", MYDEV_NAME);
        return ret;
    }
}

static void __exit memdisplay_exit(void)
{
	spi_unregister_driver(&memdisplay_spi_driver);
	platform_driver_unregister(&memdisplay_platform_driver);
    pr_info("%s - __EXIT function called successfully\n", MYDEV_NAME);
}

module_init(memdisplay_init);
module_exit(memdisplay_exit);

MODULE_DESCRIPTION("Sharp Memory LCD Module Driver");
MODULE_AUTHOR("Devang Patel");
MODULE_LICENSE("GPL v2");
