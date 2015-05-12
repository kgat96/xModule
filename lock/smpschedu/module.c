/*  
 *  moudule.c - Demonstrates module documentation.
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#define DRIVER_AUTHOR "Peter Jay Salzman <p@dirac.org>"
#define DRIVER_DESC   "A sample driver"

static int __init init_xmodule(void)
{
	printk(KERN_INFO "Hello, world\n");
	return 0;
}

static void __exit cleanup_xmodule(void)
{
	printk(KERN_INFO "Goodbye, world\n");
}

module_init(init_xmodule);
module_exit(cleanup_xmodule);
/* 
 * Get rid of taint message by declaring code as GPL. 
 */
MODULE_LICENSE("GPL");

/*
 * Or with defines, like this:
 */
MODULE_AUTHOR(DRIVER_AUTHOR);	/* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC);	/* What does this module do */

