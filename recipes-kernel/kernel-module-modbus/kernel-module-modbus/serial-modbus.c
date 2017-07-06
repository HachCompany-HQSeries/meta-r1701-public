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

// Private device data.
struct private_data {
    struct platform_device* pdev;
	struct miscdevice miscdev;
    char* buffPtr;
    int buffMaxLen;
    int buffLen;
};
static unsigned int nextAvailDevId = 0;
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
                 const char __user* buf,            //!< [in] Buffer to user space containing data to be written. 
                 size_t lbuf,                       //!< [in] Size of the user space buffer.
                 loff_t *ppos)                      //!< [in, out] Position in buffer,
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
    if((*ppos + lbuf) > pData->buffMaxLen) {
        pr_info("Attempting to write outside of buffer range.\n");
        return -1;  
    }

    // Write the user data to the kernel buffer.
    nbytes = lbuf - copy_from_user(pData->buffPtr + *ppos, buf, lbuf);
    *ppos += nbytes;

	pr_info("modbus_dev_write() is called: nbytes=%d pos=%d.\n", nbytes, (int)*ppos);
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

    pr_info("modbus_dev_probe() is called %d.\n", nextAvailDevId);

    // Allocate memory for the private data, initialize it, and sets the owner and ops fields to point to the current
    // module and the proper file_operations table.
    pData = devm_kzalloc(&pdev->dev, sizeof(struct private_data), GFP_KERNEL);
    if(!pData) {
        pr_err("Failed to allocate memory for device structure.\n");
        return -ENOMEM;
    }
	pData->pdev = pdev;

	// misc device registration.
	pData->miscdev.minor = MISC_DYNAMIC_MINOR;
	pData->miscdev.name = kasprintf(GFP_KERNEL, "modbus_dev%d", nextAvailDevId);
	pData->miscdev.fops = &modbus_dev_fops;

    // To access private data in other parts of the driver, attach it to the pdev structure.
    platform_set_drvdata(pdev, pData);

    ret = misc_register(&pData->miscdev);
    if (ret != 0){ 
        pr_err("Call to misc_register failed with: %d.\n", ret); 
        return ret; 
    } 

    nextAvailDevId++;
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
    nextAvailDevId = 0;

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
