//--------------------------------------------------------------------------------------------------------------------//
//! @file serial-modbus.c
//!
//! @brief Entry point into example application using Quantum Framework.
//! @section DESCRIPTION
//! The following file is responsible for initializing the framework, starting the application state machines, and 
//! defining external framework dependencies. Finally, control is given to the framework to manage execution of the 
//! application.
//! @copyright     Hach Confidential
//!                Copyright(c) (2017)
//!                Hach
//!                All Rights Reserved
//! This product is protected by copyright and distributed under
//! Hach Software License, Version 1.0 (See accompanying file LICENSE_1.0.txt)
//--------------------------------------------------------------------------------------------------------------------//

#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#include <linux/fs.h>                               // File operations.
#include <linux/slab.h>                             // kmalloc(), kfree()
#include <linux/uaccess.h>                          // copy_(to,from)_user
#include <linux/gpio.h>                             // required to use gpiolib from kernel space
#include <linux/of_gpio.h>                          // of_get_named_gpio()

// Debug
#include <linux/delay.h>                            // msleep()
// Debug

// Register definitions.
#define URXD0 0x0                                   // Receiver Register
#define URTX0 0x40                                  // Transmitter Register
#define UCR1 0x80                                   // Control Register 1
#define UCR2 0x84                                   // Control Register 2
#define UCR3 0x88                                   // Control Register 3
#define UCR4 0x8c                                   // Control Register 4
#define UFCR 0x90                                   // FIFO Control Register
#define USR1 0x94                                   // Status Register 1
#define USR2 0x98                                   // Status Register 2
#define UESC 0x9c                                   // Escape Character Register
#define UTIM 0xa0                                   // Escape Timer Register
#define UBIR 0xa4                                   // BRM Incremental Register
#define UBMR 0xa8                                   // BRM Modulator Register
#define UBRC 0xac                                   // Baud Rate Count Register
#define IMX21_ONEMS 0xb0                            // One Millisecond register
#define IMX1_UTS 0xd0                               // UART Test Register on i.mx1
#define IMX21_UTS 0xb4                              // UART Test Register on all other i.mx

// UART Control Register Bit Fields.
#define URXD_DUMMY_READ (1<<16)
#define URXD_CHARRDY (1<<15)
#define URXD_ERR (1<<14)
#define URXD_OVRRUN (1<<13)
#define URXD_FRMERR (1<<12)
#define URXD_BRK (1<<11)
#define URXD_PRERR (1<<10)
#define URXD_RX_DATA (0xFF<<0)
#define UCR1_ADEN (1<<15)                           // Auto detect interrupt
#define UCR1_ADBR (1<<14)                           // Auto detect baud rate
#define UCR1_TRDYEN (1<<13)                         // Transmitter ready interrupt enable
#define UCR1_IDEN (1<<12)                           // Idle condition interrupt
#define UCR1_ICD_REG(x) (((x) & 3) << 10)           // idle condition detect
#define UCR1_RRDYEN (1<<9)                          // Recv ready interrupt enable
#define UCR1_RDMAEN (1<<8)                          // Recv ready DMA enable
#define UCR1_IREN (1<<7)                            // Infrared interface enable
#define UCR1_TXMPTYEN (1<<6)                        // Transimitter empty interrupt enable
#define UCR1_RTSDEN (1<<5)                          // RTS delta interrupt enable
#define UCR1_SNDBRK (1<<4)                          // Send break
#define UCR1_TDMAEN (1<<3)                          // Transmitter ready DMA enable
#define IMX1_UCR1_UARTCLKEN (1<<2)                  // UART clock enabled, i.mx1 only
#define UCR1_ATDMAEN (1<<2)                         // Aging DMA Timer Enable
#define UCR1_DOZE (1<<1)                            // Doze
#define UCR1_UARTEN (1<<0)                          // UART enabled
#define UCR2_ESCI (1<<15)                           // Escape seq interrupt enable
#define UCR2_IRTS (1<<14)                           // Ignore RTS pin
#define UCR2_CTSC (1<<13)                           // CTS pin control
#define UCR2_CTS (1<<12)                            // Clear to send
#define UCR2_ESCEN (1<<11)                          // Escape enable
#define UCR2_PREN (1<<8)                            // Parity enable
#define UCR2_PROE (1<<7)                            // Parity odd/even
#define UCR2_STPB (1<<6)                            // Stop
#define UCR2_WS (1<<5)                              // Word size
#define UCR2_RTSEN (1<<4)                           // Request to send interrupt enable
#define UCR2_ATEN (1<<3)                            // Aging Timer Enable
#define UCR2_TXEN (1<<2)                            // Transmitter enabled
#define UCR2_RXEN (1<<1)                            // Receiver enabled
#define UCR2_SRST (1<<0)                            // SW reset
#define UCR3_DTREN (1<<13)                          // DTR interrupt enable
#define UCR3_PARERREN (1<<12)                       // Parity enable
#define UCR3_FRAERREN (1<<11)                       // Frame error interrupt enable
#define UCR3_DSR (1<<10)                            // Data set ready
#define UCR3_DCD (1<<9)                             // Data carrier detect
#define UCR3_RI	 (1<<8)                             // Ring indicator
#define UCR3_ADNIMP (1<<7)                          // Autobaud Detection Not Improved
#define UCR3_RXDSEN (1<<6)                          // Receive status interrupt enable
#define UCR3_AIRINTEN (1<<5)                        // Async IR wake interrupt enable
#define UCR3_AWAKEN (1<<4)                          // Async wake interrupt enable
#define IMX21_UCR3_RXDMUXSEL (1<<2)                 // RXD Muxed Input Select
#define UCR3_INVT (1<<1)                            // Inverted Infrared transmission
#define UCR3_BPEN (1<<0)                            // Preset registers enable
#define UCR4_CTSTL_SHF 10                           // CTS trigger level shift
#define UCR4_CTSTL_MASK 0x3F                        // CTS trigger is 6 bits wide
#define UCR4_INVR (1<<9)                            // Inverted infrared reception
#define UCR4_ENIRI (1<<8)                           // Serial infrared interrupt enable
#define UCR4_WKEN (1<<7)                            // Wake interrupt enable
#define UCR4_REF16 (1<<6)                           // Ref freq 16 MHz
#define UCR4_IDDMAEN (1<<6)                         // DMA IDLE Condition Detected
#define UCR4_IRSC (1<<5)                            // IR special case
#define UCR4_TCEN (1<<3)                            // Transmit complete interrupt enable
#define UCR4_BKEN (1<<2)                            // Break condition interrupt enable
#define UCR4_OREN (1<<1)                            // Receiver overrun interrupt enable
#define UCR4_DREN (1<<0)                            // Recv data ready interrupt enable
#define UFCR_RXTL_SHF 0                             // Receiver trigger level shift
#define UFCR_DCEDTE (1<<6)                          // DCE/DTE mode select
#define UFCR_RFDIV (7<<7)                           // Reference freq divider mask
#define UFCR_RFDIV_REG(x) (((x) < 7 ? 6 - (x) : 6) << 7)
#define UFCR_TXTL_SHF 10                            // Transmitter trigger level shift
#define USR1_PARITYERR (1<<15)                      // Parity error interrupt flag
#define USR1_RTSS (1<<14)                           // RTS pin status
#define USR1_TRDY (1<<13)                           // Transmitter ready interrupt/dma flag
#define USR1_RTSD (1<<12)                           // RTS delta
#define USR1_ESCF (1<<11)                           // Escape seq interrupt flag
#define USR1_FRAMERR (1<<10)                        // Frame error interrupt flag
#define USR1_RRDY (1<<9)                            // Receiver ready interrupt/dma flag
#define USR1_AGTIM (1<<8)                           // Ageing timer interrupt flag
#define USR1_TIMEOUT (1<<7)                         // Receive timeout interrupt status
#define USR1_RXDS (1<<6)                            // Receiver idle interrupt flag
#define USR1_AIRINT (1<<5)                          // Async IR wake interrupt flag
#define USR1_AWAKE (1<<4)                           // Aysnc wake interrupt flag
#define USR2_ADET (1<<15)                           // Auto baud rate detect complete
#define USR2_TXFE (1<<14)                           // Transmit buffer FIFO empty
#define USR2_DTRF (1<<13)                           // DTR edge interrupt flag
#define USR2_IDLE (1<<12)                           // Idle condition
#define USR2_IRINT (1<<8)                           // Serial infrared interrupt flag
#define USR2_WAKE (1<<7)                            // Wake
#define USR2_RTSF (1<<4)                            // RTS edge interrupt flag
#define USR2_TXDC (1<<3)                            // Transmitter complete
#define USR2_BRCD (1<<2)                            // Break condition
#define USR2_ORE (1<<1)                             // Overrun error
#define USR2_RDR (1<<0)                             // Recv data ready
#define UTS_FRCPERR (1<<13)                         // Force parity error
#define UTS_LOOP (1<<12)                            // Loop tx and rx
#define UTS_TXEMPTY (1<<6)                          // TxFIFO empty
#define UTS_RXEMPTY (1<<5)                          // RxFIFO empty
#define UTS_TXFULL (1<<4)                           // TxFIFO full
#define UTS_RXFULL (1<<3)                           // RxFIFO full
#define UTS_SOFTRST (1<<0)                          // Software reset

// Private device data.
struct private_data {
    struct platform_device* pdev;
	struct miscdevice miscdev;
    char* buffPtr;
    int buffMaxLen;
    int buffLen;
    int gpioDE;
    int gpioRE;
	void __iomem* regs;
};
static const int DEFAULT_BUFFER_SIZE = 1024;        // Size of communication buffer to support half duplex modbus. 

//--------------------------------------------------------------------------------------------------------------------//
//! This callback function is executed by the kernel when a user-application makes a system call to open the file, or
//! device node, associated with this module within the virtual file system.
//
//! @retval int - 0: success
//
static int modbus_dev_open(struct inode* inode,     //!< [in] inode structure represent the file system item associated
                                                    //!< with this device node. It will contain the minor number to
                                                    //!< differentiate when more than one device is managed by this
                                                    //!< driver. There is only one inode structure pointing to a given
                                                    //!< device node, regardless of how many times it is opened.
                           struct file* file)       //!< [in] A unique file structure is supplied for each opened device
                                                    //!< node. Inside is private data field where information specific
                                                    //!< to this device node can be stored. When you operate on the file
                                                    //!< structure in other entry points, you can access the same private
                                                    //!< data associated with that device node.
{
    struct private_data* pData;

	pr_info("modbus_dev_open() is called.\n");

	// Get the private_data structure from file.
	if(file->private_data == NULL) {
		pr_err("File private_data is empty!\n");
		return -1;
	}
	pData = container_of(file->private_data, struct private_data, miscdev);
	if(pData == NULL) {
		pr_err("Could not retrieve miscdev structure.\n");
        return -1;
	}

    // Allocate modbus buffer.
    pData->buffMaxLen = DEFAULT_BUFFER_SIZE;
    pData->buffPtr = kzalloc((size_t)pData->buffMaxLen, GFP_KERNEL);
    pData->buffLen = 0;
    if(!pData->buffPtr) {
        pr_err("Failed to allocate buffer memory.\n");
        return -ENOMEM;
    }

	return 0;
}

//--------------------------------------------------------------------------------------------------------------------//
//! This callback function is executed by the kernel when a user-application makes a system call to close the file, or
//! device node, associated with this module within the virtual file system.
//
//! @retval int - 0: success
//
static int modbus_dev_close(struct inode* inode,    //!< [in] inode structure represent the file system item associated
                                                    //!< with this device node. It will contain the minor number to
                                                    //!< differentiate when more than one device is managed by this
                                                    //!< driver.
                            struct file* file)      //!< [in] A unique file structure is supplied for each opened device
                                                    //!< node. Inside is private data field where information specific
                                                    //!< to this device node can be stored. When you operate on the file
                                                    //!< structure in other entry points, you can access the same private
                                                    //!< data associated with that device node.
{
    struct private_data* pData;

	pr_info("modbus_dev_close() is called.\n");

	// Get the private_data structure from file.
	if(file->private_data == NULL) {
		pr_err("File private_data is empty!\n");
		return -1;
	}
	pData = container_of(file->private_data, struct private_data, miscdev);
	if(pData == NULL) {
		pr_err("Could not retrieve miscdev structure.\n");
        return -1;
	}

    // Free resources.
    if(pData->buffPtr) {
        kfree(pData->buffPtr);
    }

	return 0;
}

//--------------------------------------------------------------------------------------------------------------------//
//! This function provides 'out-of-band' I/O control between user-space and this driver and is executed by the kernel 
//! when a user-space application invokes ioctl() on this device node and the system call then calls this entry point.
//! It is used to issue device-specific commands.
//! @retval int - 0: success
//
static long modbus_dev_ioctl(struct file* file, 
                             unsigned int cmd, 
                             unsigned long arg)
{
    struct private_data* pData;

	pr_info("modbus_dev_ioctl() is called. cmd = %d, arg = %ld\n", cmd, arg);

	// Get the private_data structure from file.
	if(file->private_data == NULL) {
		pr_err("File private_data is empty!\n");
		return -1;
	}
	pData = container_of(file->private_data, struct private_data, miscdev);
	if(pData == NULL) {
		pr_err("Could not retrieve miscdev structure.\n");
        return -1;
	}

    // TODO:
    // 1. Add feature to modify size of modbus buffer.
    
	return 0;
}

//--------------------------------------------------------------------------------------------------------------------//
//! This function reads from the device, returning the number of bytes read. 
//
//! @retval int - <0: error, 0: may mean end of device and not an error, >0: number of bytes read
//
static ssize_t modbus_dev_read(struct file* file,   //!< [in] Open instance of a file. Can be useful to take advantage
                                                    //!< of the private data field to access information specific to this
                                                    //!< minor number.
                               char __user* buf,    //!< [in] Buffer to user space containing data to be written. 
                               size_t lbuf,         //!< [in] Number of bytes to read.
                               loff_t *ppos)        //!< [in, out] Position in buffer,
{
    struct private_data* pData;
    int nbytes;

	// Get the private_data structure from file.
	if(file->private_data == NULL) {
		pr_err("File private_data is empty!\n");
		return -1;
	}
	pData = container_of(file->private_data, struct private_data, miscdev);
	if(pData == NULL) {
		pr_err("Could not retrieve miscdev structure.\n");
        return -1;
	}

    // Validate starting position of read is within range of the packet length.
    if(*ppos > pData->buffMaxLen) {
        pr_info("Invalid starting position in entry point modbus_dev_read().\n");
        return -1;
    }
    
    // Check for end of file.
    if(*ppos == pData->buffMaxLen) {
        return 0;
    }

    // Debug - Block until the modbus packet is received and buffLen is set.
    pData->buffLen = 50;
    // Debug

    // Adjust lbuf based on the size of the modbus packet. i.e. there is no reason to read past end of packet.
    if((*ppos + lbuf) > pData->buffLen)
        lbuf = pData->buffLen - *ppos;

    nbytes = lbuf - copy_to_user(buf, pData->buffPtr + *ppos, lbuf);
    *ppos += nbytes;

	pr_info("modbus_dev_read() is called: nbytes=%d pos=%d.\n", nbytes, (int)*ppos);
    return nbytes;
}

//--------------------------------------------------------------------------------------------------------------------//
//! This function writes to the device, returning the number of bytes witten.
//
//! @retval int - <0: error, 0: may mean end of device and not an error, >0: number of bytes written.
//
static ssize_t
modbus_dev_write(struct file* file,                 //!< [in] Open instance of a file. Can be useful to take advantage
                                                    //!< of the private data field to access information specific to this
                                                    //!< minor number.
                 const char __user* ubuf,           //!< [in] Buffer in user space containing data to be written. 
                 size_t size,                       //!< [in] Size of the user space buffer.
                 loff_t* off)                       //!< [in, out] Position in buffer,
{
    struct private_data* pData;
    int nbytes;

	// Get the private_data structure from file.
	if(file->private_data == NULL) {
		pr_err("File private_data is empty!\n");
		return -1;
	}
	pData = container_of(file->private_data, struct private_data, miscdev);
	if(pData == NULL) {
		pr_err("Could not retrieve miscdev structure.\n");
        return -1;
	}

    // Verify write is within range of our buffer.
    if((*off + size) > pData->buffMaxLen) {
        pr_info("Attempting to write outside of buffer range.\n");
        return -1;  
    }

    // Write the user data to the kernel buffer.
    nbytes = size - copy_from_user(pData->buffPtr + *off, ubuf, size);
    *off += nbytes;

	pr_info("modbus_dev_write() is called: nbytes=%d pos=%d.\n", nbytes, (int)*off);
    return nbytes;
}

// Define the entry points, or callback functions, implemented by this driver when a system call is made from user-space.
static const struct file_operations modbus_dev_fops = {
    .owner = THIS_MODULE,                           // When this device driver is opened, the kernel will poke the owner
                                                    // field and increment the modules' reference count, so the module 
                                                    // doesn't unload when using it.
    .read = modbus_dev_read,
    .write = modbus_dev_write,    
    .open = modbus_dev_open,
	.release = modbus_dev_close,
	.unlocked_ioctl = modbus_dev_ioctl,
};

//--------------------------------------------------------------------------------------------------------------------//
//! This function is called when the platform device driver matches its "compatible of_device_id" entry with the 
//! "compatible property" of the DT device node. 
//
//! @retval int -
//
static int modbus_dev_probe(struct platform_device* pdev)
{
    struct private_data* pData;
    int ret;
    struct resource* res;

    // Debug
    int loopCt;
    // Debug

    pr_info("modbus_dev_probe() is called.\n");

    // Obtain the first memory resource from the device tree, in this case, the register memory map of the UART. 
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		pr_err("Unable to obtain platform memory resource.\n");
		return -EINVAL;
	}
    pr_info("res->start = 0x%08lx\n", (long unsigned int)res->start);
    pr_info("res->end = 0x%08lx\n", (long unsigned int)res->end);

    // Allocate memory for the private data, initialize it, and sets the owner and ops fields to point to the current
    // module and the proper file_operations table.
    pData = devm_kzalloc(&pdev->dev, sizeof(struct private_data), GFP_KERNEL);
    if(!pData) {
        pr_err("Failed to allocate memory for device structure.\n");
        return -ENOMEM;
    }
	pData->pdev = pdev;

    // Map physical memory associated with the resource to kernel virtual address space.
	pData->regs = devm_ioremap_resource(&pdev->dev, res);
	if(!pData->regs) {
		dev_err(&pdev->dev, "Cannot remap registers.\n");
		return -ENOMEM;
	}

    // Configure gpio for RS485 drive enable (DE).
    pData->gpioDE = of_get_named_gpio(pData->pdev->dev.of_node, "gpio-DE", 0);
    if(pData->gpioDE < 0) {
        pr_err("Failed to get named gpio-DE from device tree.\n");
        return -ENODEV;
    }
    if(!gpio_is_valid(pData->gpioDE)) {
        pr_err("Invalid DE gpio.\n");
        return -ENODEV;
    }
    if(devm_gpio_request(&pdev->dev, pData->gpioDE, "tim")) {
        pr_err("Request for DE gpio failed.\n");
        return -ENODEV;
    }
    gpio_direction_output(pData->gpioDE, 0);
    gpio_export(pData->gpioDE, 0);

    // Configure gpio for RS485 receive enable (DE).
    pData->gpioRE = of_get_named_gpio(pData->pdev->dev.of_node, "gpio-RE", 0);
    if(pData->gpioRE < 0) {
        pr_err("Failed to get named gpio-DE from device tree.\n");
        return -ENODEV;
    }
    if(!gpio_is_valid(pData->gpioRE)) {
        pr_err("Invalid DE gpio.\n");
        return -ENODEV;
    }
    if(devm_gpio_request(&pdev->dev, pData->gpioRE, "tim")) {
        pr_err("Request for DE gpio failed.\n");
        return -ENODEV;
    }
    gpio_direction_output(pData->gpioRE, 0);
    gpio_export(pData->gpioRE, 0);

    // Debug
    for(loopCt=0; loopCt<5; loopCt++) {
        gpio_set_value(pData->gpioRE, loopCt%2);
        msleep(250);
    }
    // Debug

	// misc device registration.
	pData->miscdev.minor = MISC_DYNAMIC_MINOR;
	pData->miscdev.name = kasprintf(GFP_KERNEL, "mbdev-%x", res->start);
	pData->miscdev.fops = &modbus_dev_fops;

    // To access private data in other parts of the driver, attach it to the pdev structure.
    platform_set_drvdata(pdev, pData);

    ret = misc_register(&pData->miscdev);
    if (ret != 0){ 
        pr_err("Call to misc_register failed with: %d.\n", ret); 
        return ret; 
    } 

	return 0;
}

//--------------------------------------------------------------------------------------------------------------------//
//! This function is called when platform driver is unloaded.
//
//! @retval int -
//
static int modbus_dev_remove(struct platform_device* pdev)
{
    struct private_data* pData;

	pr_info("modbus_dev_remove() is called.\n");

	pData = platform_get_drvdata(pdev);
	if(pData == NULL) {
		pr_err("Platform private_data is empty!\n");
		return -ENODEV;
	}
	misc_deregister(&pData->miscdev);
	kfree(pData->miscdev.name);

	return 0;
}

// Declare a list of devices support by this driver. The kernel will use this table to bind this driver to any matching 
// devices represented in the device tree when the module is loaded.
static const struct of_device_id my_of_ids[] = {
    { .compatible = "hach,serial-modbus"},
	{},
};

// Define platform driver structure.
static struct platform_driver my_platform_driver = {
	.probe = modbus_dev_probe,
	.remove = modbus_dev_remove,
	.driver = {
		.name = "serial-modbus",
		.of_match_table = my_of_ids,
		.owner = THIS_MODULE,
	}
};
module_platform_driver(my_platform_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tim Higgins");
MODULE_DESCRIPTION("Modbus RTU implementation.");
