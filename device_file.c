#include "device_file.h"
#include <linux/fs.h>       /* file stuff */
#include <linux/kernel.h>   /* printk() */
#include <linux/errno.h>    /* error codes */
#include <linux/module.h>   /* THIS_MODULE */
#include <linux/cdev.h>     /* char device stuff */
#include <linux/uaccess.h>  /* copy_to_user() */
#include <linux/slab.h>     /* kmalloc() and kfree() */

static const char g_s_Hello_World_string[] = "Hello world from kernel mode!\n\0";
static const ssize_t g_s_Hello_World_size = sizeof(g_s_Hello_World_string);

static struct class *device_class;
static struct device *device;
static struct cdev cdev; // Add this line
#define MAX_BUFFER_SIZE 1024
#define DEVICE_FILE_NAME "/dev/thanh_driver"
/*===============================================================================================*/
static ssize_t device_file_read(
    struct file *file_ptr,
    char __user *user_buffer,
    size_t count,
    loff_t *position)
{
    printk(KERN_NOTICE "Thanh-driver: Device file is read at offset = %i, read bytes count = %u\n",
           (int)*position,
           (unsigned int)count);

    if (*position >= g_s_Hello_World_size)
        return 0;

    if (*position + count > g_s_Hello_World_size)
        count = g_s_Hello_World_size - *position;

    if (copy_to_user(user_buffer, g_s_Hello_World_string + *position, count) != 0)
        return -EFAULT;

    *position += count;
    return count;
}

/*===============================================================================================*/
static ssize_t device_file_write(struct file *file_ptr, const char __user *user_buffer, size_t count, loff_t *position)
{
    char *kernel_buffer;
    ssize_t ret = 0;

    printk(KERN_NOTICE "Thanh-driver: Device file is written at offset = %lld, write bytes count = %zu\n",
           *position, count);

    // Limit the buffer size if necessary
    if (count > MAX_BUFFER_SIZE) {
        count = MAX_BUFFER_SIZE;
    }

    kernel_buffer = kmalloc(count, GFP_KERNEL);
    if (!kernel_buffer) {
        ret = -ENOMEM;
        goto out;
    }

    if (copy_from_user(kernel_buffer, user_buffer, count)) {
        ret = -EFAULT;
        goto free_buffer;
    }

    // Process the data in the kernel buffer as needed

    ret = count; // Return the number of bytes written

free_buffer:
    kfree(kernel_buffer);

out:
    if (ret < 0) {
        printk(KERN_WARNING "Thanh-driver: Failed to write to device file: %zd\n", ret);
    }

    return ret;
}
/*===============================================================================================*/
static struct file_operations thanh_driver_fops = {
    .owner = THIS_MODULE,
    .read = device_file_read,
    .write = device_file_write,
};

static int device_file_major_number = 0;
static const char device_name[] = "thanh_driver";

/*===============================================================================================*/
int register_device(void)
{
    int result = 0;

    printk(KERN_NOTICE "Thanh-driver: register_device() is called.\n");

    device_class = class_create(THIS_MODULE, device_name);
    if (IS_ERR(device_class))
    {
        printk(KERN_WARNING "Thanh-driver: can't create device class\n");
        return PTR_ERR(device_class);
    }

    result = alloc_chrdev_region(&device_file_major_number, 0, 1, device_name);
    if (result < 0)
    {
        printk(KERN_WARNING "Thanh-driver: can't allocate major number\n");
        return result;
    }

    device = device_create(device_class, NULL, device_file_major_number, NULL, device_name);
    if (IS_ERR(device))
    {
        class_destroy(device_class);
        unregister_chrdev_region(device_file_major_number, 1);
        printk(KERN_WARNING "Thanh-driver: can't create device\n");
        return PTR_ERR(device);
    }

    cdev_init(&cdev, &thanh_driver_fops);
    cdev_add(&cdev, device_file_major_number, 1);

    printk(KERN_NOTICE "Thanh-driver: registered character device with major number = %i and minor numbers 0...255\n",
           device_file_major_number);

    return 0;
}

/*===============================================================================================*/
void unregister_device(void)
{
    printk(KERN_NOTICE "Thanh-driver: unregister_device() is called\n");
    cdev_del(&cdev);
    device_destroy(device_class, device_file_major_number);
    class_destroy(device_class);
    unregister_chrdev_region(device_file_major_number, 1);
}
