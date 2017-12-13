/*
 * gpio_sysfs_test.c
 *
 * Copyright (C) 2011 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: GPIO SYSFS test application
 *
 */

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define SYSFS_PWR_STATE		"/sys/power/state"
#define SYSFS_SUSPEND_TO_RAM	"mem"
/*
#define SYSFS_GPIO_BASE		"/sys/class/gpio/"
#define SYSFS_GPIO_EXPORT	SYSFS_GPIO_BASE "export"
#define SYSFS_GPIO_UNEXPORT	SYSFS_GPIO_BASE "unexport"
#define SYSFS_GPIO_OUT		"out"
#define SYSFS_GPIO_IN		"in"
*/
#define SYSFS_RTC_WAKEALARM	"/sys/class/rtc/rtc0/wakealarm"

//#define GPIO1			68
//#define GPIO2			74


static bool app_running = true;

/*
enum gpio_dir {
	GPIO_DIR_OUT,
	GPIO_DIR_IN,
	GPIO_DIR_OUT_HIGH,
	GPIO_DIR_OUT_LOW,
};
*/

size_t sysfs_write(const char *file, const char *buf, size_t len)
{
	int fd;
	size_t ret;

	fd = open(file, O_WRONLY | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "Error opening %s\n", file);
		return fd;
	}

	ret = write(fd, buf, len);
	if (ret == len) {
		ret = 0;
	} else {
		fprintf(stderr, "Error writing to %s, %s, %d (%d)\n",
			file, buf, len, ret);
	}

	close(fd);

	return ret;
}

int sysfs_suspend_to_ram(void)
{
	return sysfs_write(SYSFS_PWR_STATE, SYSFS_SUSPEND_TO_RAM, strlen(SYSFS_SUSPEND_TO_RAM));
}

int sysfs_rtc_alrm_in_n_seconds(unsigned int nsec)
{
	int ret;
	char *buf;

	ret = asprintf(&buf, "+%d", nsec);
	if (ret < 0) {
		fprintf(stdout, "%s: failed to allocate memory\n", __func__);
		return -ENOMEM;
	}

	ret = sysfs_write(SYSFS_RTC_WAKEALARM, buf, strlen(buf));
	free(buf);

	return ret;
}

/*
int sysfs_gpio_export(unsigned int gpio)
{
	int ret;
	char *buf;

	ret = asprintf(&buf, "%d", gpio);
	if (ret < 0) {
		fprintf(stdout, "%s: failed to allocate memory\n", __func__);
		return -ENOMEM;
	}

	ret = sysfs_write(SYSFS_GPIO_EXPORT, buf, strlen(buf));
	free(buf);

	return ret;
}

int sysfs_gpio_unexport(unsigned int gpio)
{
	int ret;
	char *buf;

	ret = asprintf(&buf, "%d", gpio);
	if (ret < 0) {
		fprintf(stdout, "Failed to allocate memory\n");
		return -ENOMEM;
	}

	ret = sysfs_write(SYSFS_GPIO_UNEXPORT, buf, strlen(buf));
	free(buf);

	return ret;
}

int sysfs_gpio_set_dir(unsigned int gpio, enum gpio_dir dir)
{
	int ret;
	char *buf;

	ret = asprintf(&buf, "%sgpio%d/direction", SYSFS_GPIO_BASE, gpio);
	if (ret < 0) {
		fprintf(stdout, "Failed to allocate memory\n");
		return -ENOMEM;
	}

	switch (dir) {
		case GPIO_DIR_OUT:
		case GPIO_DIR_OUT_HIGH:
		case GPIO_DIR_OUT_LOW:
			ret = sysfs_write(buf, SYSFS_GPIO_OUT, strlen(SYSFS_GPIO_OUT) + 1);
			break;
		case GPIO_DIR_IN:
			ret = sysfs_write(buf, SYSFS_GPIO_IN, strlen(SYSFS_GPIO_IN) + 1);
			break;
		default:
			ret = -EINVAL;
	}

	free(buf);

	return ret;
}

int sysfs_gpio_set_val(unsigned int gpio, unsigned int val)
{
	int ret;
	char *buf;

	ret = asprintf(&buf, "%sgpio%d/value", SYSFS_GPIO_BASE, gpio);
	if (ret < 0) {
		fprintf(stdout, "Failed to allocate memory\n");
		return -ENOMEM;
	}

	ret = sysfs_write(buf, val ? "1" : "0", 2);

	free(buf);

	return ret;
}

int gpio_init(unsigned int gpio, enum gpio_dir dir)
{
	int ret;

	ret = sysfs_gpio_export(gpio);
	if (ret)
		return ret;

	ret = sysfs_gpio_set_dir(gpio, dir);
	if (ret)
		return ret;

	switch (dir) {
	case GPIO_DIR_OUT_HIGH:
		ret = sysfs_gpio_set_val(gpio, 1);
		break;
	case GPIO_DIR_OUT_LOW:
		ret = sysfs_gpio_set_val(gpio, 0);
		break;
	default:
		break;
	}

	return ret;
}

int gpio_deinit(unsigned int gpio)
{
	return sysfs_gpio_unexport(gpio);
}
*/

void signal_handler(int sig)
{
	switch (sig) {
		case SIGHUP:
			printf("hangup signal catched\n");
			break;
		case SIGTERM:
			printf("terminate signal catched\n");
			break;
		case SIGINT:
			printf("Interrupt signal catched\n");
			break;
		default:
			printf("signal %d catched\n", sig);
			break;
	};

	app_running = false;
}

int main(int argc, char **argv, char **envp)
{
	int lcdAreaTest = 0;
	if(argc == 2)
		lcdAreaTest = atoi(argv[1]);
	int ret = 0;
	int fbfd = -1;
	int screensize = 0;
	char* fbp = (char*)NULL;
	//struct fb_var_screeninfo var_info;
	struct fb_fix_screeninfo fix_info;

	if(lcdAreaTest){
		system("modprobe mipdisplay-2-7-inch");
		fbfd = open("/dev/fb0", O_RDWR);
		if(fbfd < 0){
			printf("Unable to open Framebuffer driver!\n");
			return -1;
		}
		else{
			if(ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info)){
				close(fbfd);
				printf("Unable to read fixed screen information!\n");
				return -1;
			}

			/*
			if(ioctl(fbfd, FBIOGET_VSCREENINFO, &var_info)){
				close(fbfd);
				printf("Unable to read variable screen information!\n");
				return -1;
			}
			*/

			// Map fb to user mem.
			screensize = fix_info.smem_len;
			fbp = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
			if(fbp == NULL){
				close(fbfd);
				printf("FB memory map failed!\n");
				return -1;
			}
		}
	}

	// Installa signal handlers
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGHUP, signal_handler);

//	ret = gpio_init(GPIO1, GPIO_DIR_OUT_LOW);
//	if (ret < 0)
//		goto exit_0;

//	ret = gpio_init(GPIO2, GPIO_DIR_OUT_LOW);
//	if (ret < 0)
//		goto exit_1;

	while (app_running) {

		// Update display
		if(lcdAreaTest)
			memset(fbp, 0xFF, (size_t)(screensize*lcdAreaTest/100));

		ret = sysfs_rtc_alrm_in_n_seconds(1);
		if (!ret) {
			//sysfs_gpio_set_val(GPIO1, 0);
			//sysfs_gpio_set_val(GPIO1, 1);
			sysfs_suspend_to_ram();
			//sysfs_gpio_set_val(GPIO1, 0);
			//sysfs_gpio_set_val(GPIO1, 1);
		}
	}

//	gpio_deinit(GPIO2);
//exit_1:
//	gpio_deinit(GPIO1);
//exit_0:

	if(lcdAreaTest){
		munmap(fbp, screensize);
		close(fbfd);
		system("rmmod mipdisplay-2-7-inch");
	}

	return ret;
}
