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

#include <linux/cdev.h>
#include <linux/fs.h>                               // File operations.
#include <linux/device.h>                           // class_create(), device_create()
#include <linux/slab.h>                             // kmalloc(), kfree()
#include <linux/uaccess.h>                          // copy_(to,from)_user
#include <linux/platform_device.h>

// The following class and device names result in the creation of a device that appears in the file system
// at "/sys/class/modbus_class/modbus_dev".
static const char DEVICE_NAME[] = "modbus_dev";
static const char CLASS_NAME[] = "modbus_class";

static struct cdev c_dev;
static dev_t firstAssignedDevNum;                   // First device number being requested, major and minor.
#define firstOfMinorNum 0  
//static const unsigned int firstOfMinorNum = 0;  
static const unsigned int countOfMinorNum = 2;      // Range of contiguous minor numbers associated with this driver's 
                                                    // major.
static unsigned int nextAvailMinorNum = firstOfMinorNum;
static struct class* modbusClass = NULL;            // class struct pointer.
static struct device* modbusDevice = NULL;          // device struct pointer.

static const int DEFAULT_BUFFER_SIZE = 1024;        // Size of communication buffer to support half duplex modbus. 

// Private device data.
struct private_data {
    struct platform_device* pdev;
    struct cdev my_dev;
    char* buffPtr;
    int buffMaxLen;
    int buffLen;
};

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
    /*
    struct private_data* myPrivateData;

    // Allocate memory for private data and associate into file structure specific to this open instance.
    myPrivateData = kzalloc(sizeof(struct private_data), GFP_KERNEL);
    if(!myPrivateData) {
        pr_err("Failed to allocate memory for private data.\n");
        return -ENOMEM;
    }

    // Allocate modbus buffer.
    myPrivateData->buffMaxLen = DEFAULT_BUFFER_SIZE;
    myPrivateData->buffPtr = kzalloc((size_t)myPrivateData->buffMaxLen, GFP_KERNEL);
    myPrivateData->buffLen = 0;
    if(!myPrivateData->buffPtr) {
        pr_err("Failed to allocate buffer memory.\n");
        return -ENOMEM;
    }

    // Persist private data in file instance associated with this open.
    file->private_data = myPrivateData;

	pr_info("modbus_dev_open() is called.\n");
    */
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
    /*
    struct private_data* myPrivateData;
    
    // Access private data associated with the file structure specific to this open instance.
    myPrivateData = (struct private_data*)file->private_data;
    if(!myPrivateData) {
        pr_err("Failed to access memory for private data.\n");
        return -ENOMEM;
    }

    // Free resources.
    if(myPrivateData->buffPtr)
        kfree(myPrivateData->buffPtr);
    kfree(myPrivateData);
   
	pr_info("modbus_dev_close() is called.\n");
    */
	return 0;
}

//--------------------------------------------------------------------------------------------------------------------//
//! This function provides 'out-of-bad' I/O control between user-space and this driver and is executed by the kernel 
//! when a user-space application invokes ioctl() on this device node and the system call then calls this entry point.
//! It is used to issue device-specific commands.
//! @retval int - 0: success
//
static long modbus_dev_ioctl(struct file* file, 
                             unsigned int cmd, 
                             unsigned long arg)
{
    /*
    struct private_data* myPrivateData;
    
    // Access private data associated with the file structure specific to this open instance.
    myPrivateData = (struct private_data*)file->private_data;
    if(!myPrivateData) {
        pr_err("Failed to access memory for private data.\n");
        return -ENOMEM;
    }

    // TODO:
    // 1. Add feature to modify size of modbus buffer.

	pr_info("modbus_dev_ioctl() is called. cmd = %d, arg = %ld\n", cmd, arg);
    */
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
    /*
    struct private_data* myPrivateData;
    int nbytes;
    
    // Access private data associated with the file structure specific to this open instance.
    myPrivateData = (struct private_data*)file->private_data;
    if((!myPrivateData)&&(myPrivateData->buffPtr)) {
        pr_err("Failed to access memory for private data.\n");
        return -ENOMEM;
    }

    // Validate starting position of read is within range of the packet length.
    if(*ppos > myPrivateData->buffMaxLen) {
        pr_info("Invalid starting position in entry point modbus_dev_read().\n");
        return -1;
    }
    
    // Check for end of file.
    if(*ppos == myPrivateData->buffMaxLen) {
        return 0;
    }

    // Debug - Block until the modbus packet is received and buffLen is set.
    myPrivateData->buffLen = 50;
    // Debug

    // Adjust lbuf based on the size of the modbus packet. i.e. there is no reason to read past end of packet.
    if((*ppos + lbuf) > myPrivateData->buffLen)
        lbuf = myPrivateData->buffLen - *ppos;

    nbytes = lbuf - copy_to_user(buf, myPrivateData->buffPtr + *ppos, lbuf);
    *ppos += nbytes;

	pr_info("modbus_dev_read() is called: nbytes=%d pos=%d.\n", nbytes, (int)*ppos);
    return nbytes;
    */
    return 0;
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
    /*
    struct private_data* myPrivateData;
    int nbytes;
    
    // Access private data associated with the file structure specific to this open instance.
    myPrivateData = (struct private_data*)file->private_data;
    if(!myPrivateData) {
        pr_err("Failed to access memory for private data.\n");
        return -ENOMEM;
    }

    // Verify write is within range of our buffer.
    if((*ppos + lbuf) > myPrivateData->buffMaxLen) {
        pr_info("Attempting to write outside of buffer range.\n");
        return -1;  
    }

    // Write the user data to the kernel buffer.
    nbytes = lbuf - copy_from_user(myPrivateData->buffPtr + *ppos, buf, lbuf);
    *ppos += nbytes;

	pr_info("modbus_dev_write() is called: nbytes=%d pos=%d.\n", nbytes, (int)*ppos);
    return nbytes;
    */
    return 0;
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
    /*
    int ret;
    dev_t nextAssignedDevNum;
    static int nextAvailMinorNum = firstOfMinorNum;
    struct private_data* pData;
    */

    pr_info("modbus_dev_probe() is called %d.\n", nextAvailMinorNum);

    modbusDevice = device_create(modbusClass, 
                     NULL, 
                     MKDEV(MAJOR(firstAssignedDevNum), nextAvailMinorNum), 
                     NULL, 
                     "%s%d", DEVICE_NAME, nextAvailMinorNum);
    if(IS_ERR(modbusDevice)) {
        //platform_driver_unregister(&my_platform_driver); 
        pr_info("Failed to create the device.\n");
        //class_destroy(modbusClass);
        //unregister_chrdev_region(firstAssignedDevNum, countOfMinorNum);
        return -1;
    }
    nextAvailMinorNum++;

    /*
	// Automatically allocate an available major and the range of minor numbers to go with it, return the first in
    // firstAssignedDevNum.
    if(nextAvailMinorNum == 0) {
        ret = alloc_chrdev_region(&firstAssignedDevNum, firstOfMinorNum, countOfMinorNum, DEVICE_NAME); 
        if (ret < 0){ 
            pr_err("Failed to allocate device region.\n"); 
            return ret; 
        }
    } 

    // Allocate memory for the private data, initialize it, and sets the owner and ops fields to point to the current
    // module and the proper file_operations table.
    pData = devm_kzalloc(&pdev->dev, sizeof(struct private_data), GFP_KERNEL);
    if(!pData) {
        pr_err("Failed to allocate memory for device structure.\n");
        return -ENOMEM;
    }
	pData->pdev = pdev;
    cdev_init(&pData->my_dev, &modbus_dev_fops);

    // To access private data in other parts of the driver, attach it to the pdev structure.
    platform_set_drvdata(pdev, pData);
    
    // Make the kernel aware of the driver.
    ret = cdev_add(&pData->my_dev, firstAssignedDevNum, countOfMinorNum);
    if(ret < 0) {
        pr_err("Unable to add cdev\n");
        unregister_chrdev_region(firstAssignedDevNum, countOfMinorNum);
        return ret;
    }

    // Register the device class.
    if(nextAvailMinorNum == 0) {
        modbusClass = class_create(THIS_MODULE, CLASS_NAME);
        if(IS_ERR(modbusClass)) {
            pr_err("Failed to register device class.\n");
            unregister_chrdev_region(firstAssignedDevNum, countOfMinorNum);
            return PTR_ERR(modbusClass);
        }
    }

    // Create the device and register a node in sysfs.
    nextAssignedDevNum = MKDEV(MAJOR(firstAssignedDevNum), nextAvailMinorNum);
	modbusDevice = device_create(modbusClass, NULL, nextAssignedDevNum, NULL, "%s%d", DEVICE_NAME,
                                                                                      nextAvailMinorNum++);
	if(IS_ERR(modbusDevice)) {
	    pr_info("Failed to create the device.\n");
	    class_destroy(modbusClass);
		unregister_chrdev_region(firstAssignedDevNum, countOfMinorNum);
	    return PTR_ERR(modbusDevice);
	}
   
    dev_info(modbusDevice, "Succeeded in registering character device %s, Major=%d, Minor=%d.\n", DEVICE_NAME,
                                                                                                  MAJOR(firstAssignedDevNum),
                                                                                                  nextAvailMinorNum);

    */
    return 0;
}

//--------------------------------------------------------------------------------------------------------------------//
//! This function is called when platform driver is unloaded.
//
//! @retval int -
//
static int __exit modbus_dev_remove(struct platform_device* pdev)
{
	pr_info("modbus_dev_remove() is called.\n");

    /*
    // Remove the device from the system.
    device_destroy(modbusClass, firstAssignedDevNum);

    // Unregister the device class.
	class_unregister(modbusClass);

    // Remove the device class.
	class_destroy(modbusClass);
    
    // Unregister the device numbers and remove the association with device numbers.
    unregister_chrdev_region(firstAssignedDevNum, countOfMinorNum);

    */
    return 0;
}

// Declare a list of devices support by this driver. The kernel will use this table to bind this driver to any matching 
// devices represented in the device tree when the module is loaded.
static const struct of_device_id my_of_ids[] = {
    { .compatible = "hach,serial-modbus"},
	{},
};
MODULE_DEVICE_TABLE(of, my_of_ids);

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

// Register our platform 'device' driver with the platform 'bus' driver.
//module_platform_driver(my_platform_driver);


static int __init modbus_dev_init(void)
{
    int ret;
    //int minorNum;

    pr_info("modbus_dev_init() is called.\n");
    nextAvailMinorNum = firstOfMinorNum;
    //ret = platform_driver_probe(&my_platform_driver, &modbus_dev_probe);
    //ret = platform_driver_register(&my_platform_driver);
    //if (ret < 0){ 
    //    pr_err("Call to platform_driver_probe failed with: %d.\n", ret); 
    //    return ret; 
    //} 

    // Automatically allocate an available major and the range of minor numbers to go with it, return the first in
    // firstAssignedDevNum.
    ret = alloc_chrdev_region(&firstAssignedDevNum, firstOfMinorNum, countOfMinorNum, DEVICE_NAME); 
    if (ret < 0){
        platform_driver_unregister(&my_platform_driver); 
        pr_err("Failed to allocate device region.\n"); 
        return ret; 
    } 

    // Register the device class.
    modbusClass = class_create(THIS_MODULE, CLASS_NAME);
    if(IS_ERR(modbusClass)) {
        platform_driver_unregister(&my_platform_driver); 
        pr_err("Failed to register device class.\n");
        unregister_chrdev_region(firstAssignedDevNum, countOfMinorNum);
        return PTR_ERR(modbusClass);
    }

    // Create the device and register a node in sysfs.
    /*
    for(minorNum=MINOR(firstAssignedDevNum); minorNum<countOfMinorNum; minorNum++) {
        if(device_create(modbusClass, 
                         NULL, 
                         MKDEV(MAJOR(firstAssignedDevNum), MAJOR(firstAssignedDevNum) + minorNum), 
                         NULL, 
                         "%s%d", DEVICE_NAME, minorNum) == NULL) {
            platform_driver_unregister(&my_platform_driver); 
            pr_info("Failed to create the device.\n");
            class_destroy(modbusClass);
            unregister_chrdev_region(firstAssignedDevNum, countOfMinorNum);
            return PTR_ERR(modbusDevice);
        }
	}
    */

    cdev_init(&c_dev, &modbus_dev_fops);
    
    // Make the kernel aware of the driver.
    ret = cdev_add(&c_dev, firstAssignedDevNum, countOfMinorNum);
    if(ret < 0) {
        platform_driver_unregister(&my_platform_driver); 
        pr_err("Unable to add cdev.\n");
        //for(minorNum=MINOR(firstAssignedDevNum); minorNum<countOfMinorNum; minorNum++) {
        //    device_destroy(modbusClass, MKDEV(MAJOR(firstAssignedDevNum), MAJOR(firstAssignedDevNum) + minorNum));
        //}
	    class_unregister(modbusClass);
        unregister_chrdev_region(firstAssignedDevNum, countOfMinorNum);
        return ret;
    }

    ret = platform_driver_register(&my_platform_driver);
    if (ret < 0){ 
        pr_err("Call to platform_driver_probe failed with: %d.\n", ret); 
        return ret; 
    } 
    return 0;
}

static void __exit modbus_dev_exit(void)
{
    int minorNum;

	pr_info("modbus_dev_exit() is called.\n");

    platform_driver_unregister(&my_platform_driver); 

    cdev_del(&c_dev);

    // Remove the device from the system.
    for(minorNum=MINOR(firstAssignedDevNum); minorNum<countOfMinorNum; minorNum++) {
        device_destroy(modbusClass, MKDEV(MAJOR(firstAssignedDevNum), MAJOR(firstAssignedDevNum) + minorNum));
    }

    // Unregister the device class.
	class_unregister(modbusClass);

    // Remove the device class.
	class_destroy(modbusClass);
    
    // Unregister the device numbers and remove the association with device numbers.
    unregister_chrdev_region(firstAssignedDevNum, countOfMinorNum);
}

module_init(modbus_dev_init);
module_exit(modbus_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tim Higgins");
MODULE_DESCRIPTION("Modbus RTU implementation.");
