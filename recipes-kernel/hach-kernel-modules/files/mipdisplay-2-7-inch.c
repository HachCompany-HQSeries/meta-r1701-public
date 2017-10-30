//--------------------------------------------------------------------------------------------------------------------//
//! @file mipdisplay-2-7-inch.c
//!
//! @brief Entry point into kernel module for Sharp memory display framebuffer driver
//! @section DESCRIPTION
//! The following file is responsible for initializing and setting up framebuffer driver framework for Sharp memory  
//! display. This driver exports the framebuffer character driver ex. "/dev/fb1" for user space to use.
//! @copyright     Hach Confidential
//!                Copyright(c) (2017)
//!                Hach
//!                All Rights Reserved
//! This product is protected by copyright and distributed under
//! Hach Software License, Version 1.0 (See accompanying file LICENSE_1.0.txt)
//--------------------------------------------------------------------------------------------------------------------//

#include <linux/init.h>             	// macros used to mark up functions e.g. __init __exit
#include <linux/module.h>           	// core header for loading LKMs into the kernel
#include <linux/fs.h>               	// file_operations
#include <linux/slab.h>             	// kmalloc
#include <linux/uaccess.h>          	// copy_(to,from)_user
#include <linux/fb.h>               	// linux framebuffer operations
#include <linux/spi/spi.h>          	// linux spi bus operations
#include <linux/kthread.h>		// Using kthreads for the flashing functionality
#include <linux/delay.h>      		// Using this header for the msleep() function
#include <linux/of_gpio.h>		// convert DT based gpio handling to new OF API
#include <linux/gpio.h>			// linux gpio operations
#include <linux/platform_device.h>	// linux generic, centralized driver model operations
#include <linux/spinlock.h>
#include <linux/string.h>
#include <asm/io.h>

#define WIDTH			400
#define HEIGHT			240
#define BPP			1
#define SCREEN_SIZE 		((WIDTH * HEIGHT * BPP) / 8)

#define CMD_WR			0x80
#define CMD_CLR			0x20
#define PWRUP_DISP_DELAY_USEC	40

#define HIGH			1
#define LOW			0

/* Display Line Buffer structure  - 52 Bytes */
struct LineBuffer {	
	__u8 linenum;
	__u8 data[WIDTH/8];
	__u8 trailer;
}__attribute__((packed));

/* Entire frame SPI transfer structure - 1 + (52 * 240) + 1 + 30 = 12512 Bytes */
struct EntireFrameXferBuffer {
	__u8 cmd;
	struct LineBuffer linebuf[HEIGHT];
	__u8 trailer[31];
}__attribute__((packed));

/* MIP display data structure, holds all relevant data to operate the display */
struct mipdisplay_par {
	struct spi_device	*spi;
	struct fb_info 		*info;
	struct task_struct	*task;
	__u8			*videomem;
	__u8			*spi_xfer_buf;
	int			disp;
	int			extcomin;
	spinlock_t		dirty_lock;
	int			dirty_line_start;
	int			dirty_line_end;
};

static struct fb_fix_screeninfo mipdisplay_fix = {
	.id		= "LS027B7DH01",
	.type		= FB_TYPE_PACKED_PIXELS,
	.visual		= FB_VISUAL_MONO10,
	.xpanstep	= 0,
	.ypanstep	= 0,
	.ywrapstep	= 0,
	.line_length	= ((WIDTH*BPP)/8),
	.accel		= FB_ACCEL_NONE,
};

static struct fb_var_screeninfo mipdisplay_var = {
        .xres		= WIDTH,
	.yres		= HEIGHT,
	.xres_virtual	= WIDTH,
	.yres_virtual	= HEIGHT,
	.bits_per_pixel = BPP,
	.red    	= {0,1,0},
	.green  	= {0,1,0},
	.blue   	= {0,1,0},
	.transp 	= {0,0,0},
	.nonstd 	= 1,
	.left_margin 	= 0,
	.right_margin 	= 0,
	.upper_margin 	= 0,
	.lower_margin 	= 0,
	.vmode 		= FB_VMODE_NONINTERLACED,
};

static __u8 reverseByte(__u8 f_byteIn)
{
	f_byteIn = (f_byteIn & 0xF0) >> 4 | (f_byteIn & 0x0F) << 4;
	f_byteIn = (f_byteIn & 0xCC) >> 2 | (f_byteIn & 0x33) << 2;
	f_byteIn = (f_byteIn & 0xAA) >> 1 | (f_byteIn & 0x55) << 1;  
	return f_byteIn;
}

static int mipdisplay_toggle_extcomin_sig(void* arg)
{
	struct mipdisplay_par *par = (struct mipdisplay_par*)arg;

	while(!kthread_should_stop()){
		gpio_set_value(par->extcomin, HIGH);
		msleep(100);
		gpio_set_value(par->extcomin, LOW);
		msleep(900);
	};

	return 0;
}

static int mipdisplay_On(struct mipdisplay_par *par)
{
	gpio_set_value(par->disp, HIGH);
	usleep_range(PWRUP_DISP_DELAY_USEC, PWRUP_DISP_DELAY_USEC+10);
	gpio_set_value(par->extcomin, HIGH);
	usleep_range(PWRUP_DISP_DELAY_USEC, PWRUP_DISP_DELAY_USEC+10);

	par->task = kthread_run(mipdisplay_toggle_extcomin_sig, par, "EXTCOMIN_kthrd");
	if(IS_ERR(par->task)){
		pr_err(KERN_ERR "%s - Failed to kthread to toggle EXTCOMIN signal!\n", __func__);
		return PTR_ERR(par->task);
	}

	return 0;
}

static void mipdisplay_Off(struct mipdisplay_par *par)
{
	kthread_stop(par->task);
	gpio_set_value(par->extcomin, LOW);
	gpio_set_value(par->disp, LOW);
}

static void mipdisplay_invalidate(struct fb_info *info, int y, int height)
{
	struct mipdisplay_par *par = info->par;

	if(y == -1){
		y = 0;
		height = info->var.yres - 1;
	}

	/* Mark display lines/area as dirty */
	spin_lock(&par->dirty_lock);
	if (y < par->dirty_line_start)
		par->dirty_line_start = y;
	if (y + height - 1 > par->dirty_line_end)
		par->dirty_line_end = y + height - 1;
	spin_unlock(&par->dirty_lock);

	schedule_delayed_work(&info->deferred_work, info->fbdefio->delay);
}

static ssize_t mipdisplay_write(struct fb_info *info, const char __user *buf, size_t count, loff_t *ppos)
{
	ssize_t ret;	
	ret = fb_sys_write(info, buf, count, ppos);

	/* TODO: only mark changed area update all for now */
	mipdisplay_invalidate(info, -1, 0);

	return ret;
}

static void mipdisplay_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
	sys_fillrect(info, rect);
	mipdisplay_invalidate(info, rect->dy, rect->height);
}

static void mipdisplay_copyarea(struct fb_info *info, const struct fb_copyarea *area)
{
	sys_copyarea(info, area);
	mipdisplay_invalidate(info, area->dy, area->height);
}

static void mipdisplay_imageblit(struct fb_info *info, const struct fb_image *image)
{
	sys_imageblit(info, image);
	mipdisplay_invalidate(info, image->dy, image->height);
}

static void mipdisplay_update(struct fb_info *info, struct list_head *pagelist)
{
	struct page *page;
	struct LineBuffer *line;
	struct mipdisplay_par *par = info->par;
	unsigned long index;
	int width, height, dirty_line_start, dirty_line_end;
	int y_low = 0, y_high = 0;
	size_t totalXferBytes;

	spin_lock(&par->dirty_lock);
	dirty_line_start = par->dirty_line_start;
	dirty_line_end = par->dirty_line_end;

	/* set display line markers as clean */
	par->dirty_line_start = par->info->var.yres - 1;
	par->dirty_line_end = 0;
	spin_unlock(&par->dirty_lock);

	/* walk the written page list and find out the memory change in lines */
	list_for_each_entry(page, pagelist, lru){
		index = page->index << PAGE_SHIFT;
		y_low = index / info->fix.line_length;
		y_high = (index + PAGE_SIZE - 1) / info->fix.line_length;
		if (y_high > info->var.yres - 1)
			y_high = info->var.yres - 1;
		if (y_low < dirty_line_start)
			dirty_line_start = y_low;
		if (y_high > dirty_line_end)
			dirty_line_end = y_high;
	}

	/* line and length boundary checks */
	if (dirty_line_start > dirty_line_end) {
		pr_warning(KERN_WARNING "%s - start_line=%d is larger than end_line=%d. Shouldn't happen, will do full display update\n",
			__func__, dirty_line_start, dirty_line_end);
		dirty_line_start = 0;
		dirty_line_end = par->info->var.yres - 1;
	}
	if ((dirty_line_start > par->info->var.yres - 1) || (dirty_line_end > par->info->var.yres - 1)) {
		pr_warning(KERN_WARNING "%s - start_line = %d or end_line = %d is larger than max = %d. Shouldn't happen, will do full display update\n",
			__func__, dirty_line_start, dirty_line_end, par->info->var.yres - 1);
		dirty_line_start = 0;
		dirty_line_end = par->info->var.yres - 1;
	}

	// CMD + Total Lines to transfer + trailer and make total size 32 bytes align for proper SPI DMA transfer
	totalXferBytes = sizeof(char) +
			(sizeof(struct LineBuffer) * (dirty_line_end - dirty_line_start + 1)) +
			sizeof(char);
	if(totalXferBytes % 32)
		totalXferBytes += (32 - (totalXferBytes % 32));

	memset(par->spi_xfer_buf, 0, totalXferBytes);
	par->spi_xfer_buf[0] = CMD_WR;
	line = (struct LineBuffer*)&par->spi_xfer_buf[1];
	for(height = dirty_line_start; height <= dirty_line_end; height++){
		line->linenum = reverseByte(height + 1);
		for(width = 0; width < par->info->fix.line_length; width++)
			line->data[width] = (0xFF ^ par->videomem[(height * par->info->fix.line_length) + width]);
		line++;
	}

	// Write all Black
	if(spi_write(par->spi, &par->spi_xfer_buf[0], totalXferBytes) < 0)
		pr_err(KERN_ERR "%s:%s spi_write failed to update display buffer\n", __func__, par->info->fix.id);
}

static struct fb_ops mipdisplay_ops = {
        .owner 		= THIS_MODULE,
	.fb_read	= fb_sys_read,
        .fb_write	= mipdisplay_write,
	.fb_fillrect	= mipdisplay_fillrect,
        .fb_copyarea	= mipdisplay_copyarea,
	.fb_imageblit	= mipdisplay_imageblit,
};

static struct fb_deferred_io mipdisplay_defio = {
	.delay	= HZ/10,
	.deferred_io = &mipdisplay_update,
};

static int mipdisplay_spi_probe(struct spi_device *spi)
{
	int ret;
	struct mipdisplay_par *privdata;
	struct device_node *np = spi->dev.of_node;	

	privdata = (struct mipdisplay_par*)kzalloc(sizeof(struct mipdisplay_par), GFP_KERNEL);
	if(privdata == NULL){
		pr_err(KERN_ERR "%s: MIP display private data allication failed!\n", __func__);
		return -ENOMEM;
	}
 
	privdata->info = framebuffer_alloc(sizeof(struct mipdisplay_par), &spi->dev);
	if(privdata->info == NULL){
		pr_err(KERN_ERR "%s: Framebuffer allocation failed!\n", __func__);
		ret = -ENOMEM;
		goto free_privdata;
	}

	privdata->videomem = vzalloc(PAGE_ALIGN(SCREEN_SIZE));
	if(privdata->videomem == NULL){
		pr_err(KERN_ERR "%s: Video memory allocation failed!\n", __func__);
		ret = -ENOMEM;
		goto free_fb_alloc;
	}

	privdata->info->par 		= privdata;
	privdata->info->screen_base 	= (char __iomem*)privdata->videomem;
	privdata->info->screen_size 	= SCREEN_SIZE;
	privdata->info->fbops		= &mipdisplay_ops;
	privdata->info->fix		= mipdisplay_fix;
	privdata->info->fix.smem_start	= (unsigned long)privdata->videomem;
	privdata->info->fix.smem_len	= SCREEN_SIZE;
	privdata->info->var		= mipdisplay_var;
	privdata->info->fbdefio		= &mipdisplay_defio;
	privdata->info->pseudo_palette	= NULL;
	privdata->info->flags		= FBINFO_FLAG_DEFAULT | FBINFO_VIRTFB;

	fb_deferred_io_init(privdata->info);
	 
	ret = register_framebuffer(privdata->info);
	if(ret < 0){
		pr_err(KERN_ERR "%s: FB registration failed!\n", __func__);
		goto free_video_mem;
	}

	spin_lock_init(&privdata->dirty_lock);

	privdata->disp		= of_get_named_gpio(np, "gpios", 0);
	privdata->extcomin	= of_get_named_gpio(np, "gpios", 1);
	ret = gpio_request_one(of_get_named_gpio(np, "gpios", 0), GPIOF_OUT_INIT_LOW, "DISP_PIN");
	if(ret < 0){
		pr_err(KERN_ERR "%s: Failed to request GPIO for DISP control line, return code # %d\n", __func__, ret);
		goto free_unreg_fb;
	}
	ret = gpio_request_one(of_get_named_gpio(np, "gpios", 1), GPIOF_OUT_INIT_LOW, "EXTCOMIN_PIN");
	if(ret < 0){
		pr_err(KERN_ERR "%s: Failed to request GPIO for EXTCOMIN control line, return code # %d\n", __func__, ret);
		goto free_disp_pin;
	}

	ret = mipdisplay_On(privdata);
	if(ret < 0){
		pr_err(KERN_ERR "%s: Failed to turn on display!\n", __func__);
		goto free_extcomin_pin;
	}

	privdata->spi_xfer_buf = (__u8*)kzalloc(sizeof(struct EntireFrameXferBuffer), GFP_DMA);
	if(privdata->spi_xfer_buf == NULL){
		pr_err(KERN_ERR "%s: SPI transfer buffer allocation failed!\n", __func__);
		goto spi_xfer_buf_failed;
	}

	spi_set_drvdata(spi, privdata);
	privdata->spi = spi;
	privdata->spi_xfer_buf[0] = CMD_CLR;
	ret = spi_write(spi, (const void*)privdata->spi_xfer_buf, 2);
	if(ret < 0){
		pr_err(KERN_ERR "%s: LCD CLR command write failed!\n", __func__);
		goto init_failed;
	}

	pr_info("%s - MIP Display FB SYSFS driver initialized!\n", __func__);

	return 0;

init_failed:
	kfree(privdata->spi_xfer_buf);

spi_xfer_buf_failed:
	mipdisplay_Off(privdata);
	spi_set_drvdata(spi, NULL);

free_extcomin_pin:
	gpio_free(privdata->extcomin);

free_disp_pin:
	gpio_free(privdata->disp);

free_unreg_fb:
	unregister_framebuffer(privdata->info);

free_video_mem:
	fb_deferred_io_cleanup(privdata->info);
	vfree(privdata->videomem);

free_fb_alloc:
	framebuffer_release(privdata->info);

free_privdata:
	kfree(privdata);

	return ret;
}

static int mipdisplay_spi_remove(struct spi_device *spi)
{
	struct mipdisplay_par *privdata = (struct mipdisplay_par*)spi_get_drvdata(spi);
	spi_set_drvdata(spi, NULL);

	if(privdata){	
		fb_deferred_io_cleanup(privdata->info);

		mipdisplay_Off(privdata);

		gpio_free(privdata->disp);
		gpio_free(privdata->extcomin);

		if(privdata->info){
			unregister_framebuffer(privdata->info);
			vfree(privdata->videomem);
			framebuffer_release(privdata->info);
		}

		kfree(privdata->spi_xfer_buf);
		kfree(privdata);
	}
	else{
		pr_warn(KERN_WARNING "%s - Private data structure pointer is NULL!!\n", __func__);
	}	

	pr_info("%s - MIP Display FB SYSFS driver removed!\n", __func__);
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int mipdisplay_suspend(struct device *dev)
{
	struct mipdisplay_par *par;
	struct spi_device *spi = to_spi_device(dev);
	par = (struct mipdisplay_par*)spi_get_drvdata(spi);
	mipdisplay_Off(par);
	return 0;
}

static int mipdisplay_resume(struct device *dev)
{
	struct mipdisplay_par *par;
	struct spi_device *spi = to_spi_device(dev);
	par = (struct mipdisplay_par*)spi_get_drvdata(spi);
	return mipdisplay_On(par);
}

static SIMPLE_DEV_PM_OPS(mipdisplay_pm_ops, mipdisplay_suspend, mipdisplay_resume);

#define MIPDISPLAY_PM_OPS (&mipdisplay_pm_ops)
#else
#define MIPDISPLAY_PM_OPS NULL
#endif

static const struct of_device_id dt_ids[] = {
    { .compatible = "sharp,ls027b7dh01" },
    {},
};
MODULE_DEVICE_TABLE(of, dt_ids);

static struct spi_driver mipdisplay_spi_driver = {
	.driver = {
		.name   = "Sharp-LS027B7DH01",
		.owner  = THIS_MODULE,
		.pm	= MIPDISPLAY_PM_OPS,
		.of_match_table = of_match_ptr(dt_ids),
		.suppress_bind_attrs = true,
	},
	.probe  = mipdisplay_spi_probe,
	.remove = mipdisplay_spi_remove,
};
module_spi_driver(mipdisplay_spi_driver);

MODULE_ALIAS("spi:sharp,ls027b7dh01");
MODULE_DESCRIPTION("Sharp Memory LCD Driver");
MODULE_AUTHOR("Devang Patel");
MODULE_LICENSE("GPL");
