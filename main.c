#include "device_file.h"
#include <linux/init.h>       /* module_init, module_exit */
#include <linux/module.h>     /* version info, MODULE_LICENSE, MODULE_AUTHOR, printk() */

MODULE_DESCRIPTION("Thanh Linux driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thanh Tran 516467");

/*===============================================================================================*/
static int thanh_driver_init(void)
{
    int result = 0;
    printk( KERN_NOTICE "Thanh-driver: Initialization started\n" );

    result = register_device();
    return result;
}

/*===============================================================================================*/
static void thanh_driver_exit(void)
{
    printk( KERN_NOTICE "Thanh-driver: Exiting\n" );
    unregister_device();
}

/*===============================================================================================*/
module_init(thanh_driver_init);
module_exit(thanh_driver_exit);
