/*
 * Copyright (C) 2015 Kage Shen <kgat96@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
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
#include <linux/mm.h>
#include <linux/cdev.h>

#define DRIVER_AUTHOR "Kage Shen <kgat96@gmail.com>"
#define DRIVER_DESC   "A sample driver"

#define KSTR_DEF "Hello world from kernel virtual space"

static struct task_struct *ptask;

static struct cdev *pcdev;
static dev_t ndev;
static struct page *pg;
static struct timer_list timer;

static void timer_func(unsigned long data)
{
    printk(KERN_INFO "%s %s\n", __func__, (char *)data);
    timer.expires = jiffies + HZ * 10;
    add_timer(&timer);
}

static int demo_inf(struct inode *nd, struct file *fp)
{
    return 0;   
}

static int demo_mmap(struct file *fp, struct vm_area_struct *vm)
{
    int err = 0;
    unsigned long start = vm->vm_start;
    unsigned long size = vm->vm_end - vm->vm_start;

    err = remap_pfn_range(vm, start, vm->vm_pgoff, size, vm->vm_page_prot);

    return err;
}

static struct file_operations mmap_fops = 
{
    .owner = THIS_MODULE,
    .open = demo_inf,
    .release = demo_inf,
    .mmap = demo_mmap,
};

/*
 * The mthread thread - touches the timestamp.
 */
static int mthread(void *unused)
{
    set_current_state(TASK_INTERRUPTIBLE);

    while (!kthread_should_stop()) {
        msleep(100);
    }

    __set_current_state(TASK_RUNNING);
    printk(KERN_INFO "-- %s exit\n", __func__);

    return 0;
}

static int __init init_xmodule(void)
{
    int err = 0;
    char *kstr;

    printk(KERN_INFO "-- %s [%d]\n", __func__, current->pid);

    ptask = kthread_run(mthread, NULL, "mthread");
    if (IS_ERR(ptask)) {
        printk(KERN_INFO "Cannot create kthread\n");
        return 0;
    }

    // allocate a page in HIGH_MEM area
    pg = alloc_pages(GFP_HIGHUSER, 0);
    if(!pg)
        return -ENOMEM;
    SetPageReserved(pg);
    //page comes from the highmem area,
    //so need to call kmap to establish the mapping mechanism
    kstr = (char *)kmap(pg);
    strcpy(kstr, KSTR_DEF);
    printk(KERN_INFO "kpa = 0x%x, kernel string = %s\n", page_to_phys(pg), kstr);

    pcdev = cdev_alloc();
    if (!pcdev) {
        err = -ENOMEM;
        goto cdev_err;
    }

    cdev_init(pcdev, &mmap_fops);
    err = alloc_chrdev_region(&ndev, 0, 1, "mmap_dev");
    if(err)
        goto region_err;

    printk("major = %d, minor = %d\n", MAJOR(ndev), MINOR(ndev));

    pcdev->owner = THIS_MODULE;
    err = cdev_add(pcdev, ndev, 1);
    if(err)
        goto add_err;
    
    init_timer(&timer);
    timer.function = timer_func;
    timer.data = (unsigned int)kstr;
    timer.expires = jiffies + HZ*10;
    add_timer(&timer);
    goto done;

add_err:
    unregister_chrdev_region(ndev, 1);
region_err:
    cdev_del(pcdev);
cdev_err:
    kunmap(pg);
    ClearPageReserved(pg);
done:
    return err;
}

static void __exit cleanup_xmodule(void)
{
    if (ptask && ptask->state <= 0)
        kthread_stop(ptask); // Synchronous operation

    del_timer_sync(&timer);
    cdev_del(pcdev);
    unregister_chrdev_region(ndev, 1);
    kunmap(pg);
    ClearPageReserved(pg);
    __free_pages(pg, 0);

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
