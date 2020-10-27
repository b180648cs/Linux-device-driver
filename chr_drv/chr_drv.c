#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>
//for file operations
#include<linux/fs.h>
//for making character device available
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/kdev_t.h>
//for giving user access to read and write from kernal-space to user-space and vice-versa
#include<linux/uaccess.h>
dev_t dev=0;
#define DEVICENAME "vi_device"
//initialize character driver
static struct cdev my_cdev;
static struct class *dev_class;
uint8_t *kernel_buffer;
MODULE_LICENSE("GPL");

static int device_open(struct inode *, struct file *);    // function for opening the device driver
static int device_close(struct inode *, struct file *);   // function for closing the device driver
static ssize_t device_read(struct file *, char __user*, size_t, loff_t *);            // return signed integer value as this is ssize_t so it will take care of 32 / 64 bit system we don't need to take care of that
static ssize_t device_write(struct file *, const char __user*, size_t, loff_t *);

struct file_operations fops = { /* these are the file operations provided by our driver */
    .owner = THIS_MODULE, /*prevents unloading when operations are in use*/
    .open = device_open,  /*to open the device*/
    .write = device_write, /*to write to the device*/
    .read = device_read, /*to read the device*/
    .release = device_close, /*to close the device*/
};
#define mem_size 1024
static int device_open(struct inode *inode, struct file *file){
	/*creating physical memory to open device*/
	if((kernel_buffer=kmalloc(mem_size,GFP_KERNEL))==0){
		printk(KERN_INFO "Cannot allocate the memory..\n");
		return -1;
	}
	printk(KERN_INFO "Device file opened..\n");
	return 0;
} 
static int device_close(struct inode *inode, struct file *file){
	kfree(kernel_buffer);
	printk(KERN_INFO "Device file closed..\n");
	return 0;
}
static ssize_t device_read(struct file *file, char __user *buf, size_t len, loff_t *off){
	copy_to_user(buf,kernel_buffer,mem_size);
	printk(KERN_INFO "Data read :finished..\n");
	return mem_size;
}
static ssize_t device_write(struct file *file,const char __user *buf, size_t len, loff_t *off){
	copy_from_user(kernel_buffer,buf,len);
	printk(KERN_INFO "Data is written successfully..\n");
	return len;
}


static int my_initial(void){
	printk(KERN_INFO "My Module has started.. using function %s\n",__FUNCTION__);
	if((alloc_chrdev_region(&dev,0,1,DEVICENAME))<0){
		printk(KERN_INFO "failed to allocate major number\n");
		return -1;
	}
	printk(KERN_INFO "Major:%d Minor:%d\n",MAJOR(dev),MINOR(dev));
	/* creating cdev structure*/
	cdev_init(&my_cdev,&fops);
	/*adding character device to the system*/
	if((cdev_add(&my_cdev,dev,1))<0){
		printk(KERN_INFO "Cannot add the device to the system..\n");
		goto unregister;
	}
	/* create struct class*/
		if((dev_class=class_create(THIS_MODULE,"my_class"))==NULL){
			printk(KERN_INFO "Cannot create the strut class..\n");
			goto unregister;
		}
	/* creating device*/
		if((device_create(dev_class,NULL,dev,NULL,DEVICENAME))==NULL){
			printk(KERN_INFO "Cannot create the device..\n");
			goto destroy;
		}
		printk(KERN_INFO "Device driver is inserted successfully..\n");
		return 0;

destroy:
		class_destroy(dev_class);


unregister:
		unregister_chrdev_region(dev,1);
		return -1;

}

static void my_exit(void){
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev,1);
	printk(KERN_INFO "Device driver is removed successfully..\n");
	printk(KERN_INFO "My Module has terminated.. using function %s\n",__FUNCTION__);
}


module_init(my_initial);
module_exit(my_exit);


MODULE_AUTHOR("Ritik");
MODULE_DESCRIPTION("Character Device Driver");
