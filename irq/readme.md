```
<Mips IRQ generic>
handle_int (arch/mips/kernel/genex.S)
--> plat_irq_dispatch (arch/xxx/irq.c)
     --> do_IRQ (arch/mips/kernel/irq.c)
         --> generic_handle_irq (kernel/irq/irqdesc.c)
             --> generic_handle_irq_desc (include/linux/irqdesc.h)
                --> *irq_desc.handle_irq () "handle_level_irq"
    --> handle_level_irq (kernel/irq/chip.c)
      --> handle_irq_event (kernel/irq/handle.c)
         --> handle_irq_event_percpu (kernel/irq/handle.c)
            --> action->handler(irq, action->dev_id); (kernel/irq/handle.c) (over)

<Mips IRQ reguest>
request_irq (include/linux/interrupt.h)
  --> request_threaded_irq (kernel/irq/manage.c)
     --> __setup_irq (kernel/irq/manage.c)
```

```
sharing the irq among several devices

1, irqflags |= IRQF_SHARED
2, same irqflags
3, mast have dev_id

/*
 * Sanity-check: shared interrupts must pass in a real dev-ID,
 * otherwise we'll have trouble later trying to figure out
 * which interrupt is which (messes up the interrupt freeing
 * logic etc).
 */
if ((irqflags & IRQF_SHARED) && !dev_id)
    return -EINVAL;

/*
 * Can't share interrupts unless both agree to and are
 * the same type (level, edge, polarity). So both flag
 * fields must have IRQF_SHARED set and the bits which
 * set the trigger type must match. Also all must
 * agree on ONESHOT.
 */
if (!((old->flags & new->flags) & IRQF_SHARED) ||
    ((old->flags ^ new->flags) & IRQF_TRIGGER_MASK) ||
    ((old->flags ^ new->flags) & IRQF_ONESHOT)) {
    old_name = old->name;
    goto mismatch;
}

```



