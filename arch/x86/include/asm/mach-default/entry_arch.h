/*
 * This file is designed to contain the BUILD_INTERRUPT specifications for
 * all of the extra named interrupt vectors used by the architecture.
 * Usually this is the Inter Process Interrupts (IPIs)
 */

/*
 * The following vectors are part of the Linux architecture, there
 * is no hardware IRQ pin equivalent for them, they are triggered
 * through the ICC by us (IPIs)
 */
#ifdef CONFIG_X86_SMP
BUILD_INTERRUPT(reschedule_interrupt,RESCHEDULE_VECTOR)
BUILD_INTERRUPT(invalidate_interrupt,INVALIDATE_TLB_VECTOR)
BUILD_INTERRUPT(call_function_interrupt,CALL_FUNCTION_VECTOR)
BUILD_INTERRUPT(call_function_single_interrupt,CALL_FUNCTION_SINGLE_VECTOR)
BUILD_INTERRUPT(irq_move_cleanup_interrupt,IRQ_MOVE_CLEANUP_VECTOR)
#endif

/*
 * every pentium local APIC has two 'local interrupts', with a
 * soft-definable vector attached to both interrupts, one of
 * which is a timer interrupt, the other one is error counter
 * overflow. Linux uses the local APIC timer interrupt to get
 * a much simpler SMP time architecture:
 */
#ifdef CONFIG_X86_LOCAL_APIC

BUILD_INTERRUPT(apic_timer_interrupt,LOCAL_TIMER_VECTOR)
BUILD_INTERRUPT(error_interrupt,ERROR_APIC_VECTOR)
BUILD_INTERRUPT(spurious_interrupt,SPURIOUS_APIC_VECTOR)

#ifdef CONFIG_PERF_COUNTERS
BUILD_INTERRUPT(perf_counter_interrupt, LOCAL_PERF_VECTOR)
#endif

#ifdef CONFIG_X86_MCE_P4THERMAL
BUILD_INTERRUPT(thermal_interrupt,THERMAL_APIC_VECTOR)
#endif

#endif
