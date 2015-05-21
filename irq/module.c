/*  
 *  moudule.c - Demonstrates module documentation.
 */
#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */

#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/notifier.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/highmem.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#define DRIVER_AUTHOR "Peter Jay Salzman <p@dirac.org>"
#define DRIVER_DESC   "A sample driver"

wait_queue_head_t event_queue;

static struct task_struct *ptask;

/*
 * The mthread thread - touches the timestamp.
 */
static int mthread(void *unused)
{
    set_current_state(TASK_INTERRUPTIBLE);

    while (!kthread_should_stop()) {
        printk(KERN_INFO "-- %s %d\n", __func__, current->pid);
        msleep(100000);
    }

    __set_current_state(TASK_RUNNING);
    printk(KERN_INFO "-- %s exit\n", __func__);

    return 0;
}

static irqreturn_t xmodule_interrupt(int irq, void *dev_id)
{
    printk(KERN_INFO "-- %s [%d]\n", __func__, current->pid);
    return IRQ_HANDLED;
}

static int __init init_xmodule(void)
{
    int ret;

    printk(KERN_INFO "-- %s [%d]\n", __func__, current->pid);

    ptask = kthread_run(mthread, NULL, "mthread");
    if (IS_ERR(ptask)) {
        printk(KERN_INFO "Cannot create kthread\n");
        return 0;
    }

    /*
     * 1, irq 87 irqflags = IRQF_TRIGGER_LOW | IRQF_DISABLED | IRQF_SHARED
     * 2, ptask != NULL
     */

    ret = request_irq(87, xmodule_interrupt,
        IRQF_TRIGGER_LOW | IRQF_DISABLED | IRQF_SHARED, "xmodule", ptask);

    if (ret < 0)
        printk(KERN_INFO "request_irq error %d\n", ret);

    printk(KERN_INFO "Hello, world\n");

    return 0;
}

static void __exit cleanup_xmodule(void)
{
    if (ptask && ptask->state <= 0)
        kthread_stop(ptask); // Synchronous operation

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
MODULE_AUTHOR(DRIVER_AUTHOR);   /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC);    /* What does this module do */
