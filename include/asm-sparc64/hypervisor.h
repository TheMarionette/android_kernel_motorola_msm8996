#ifndef _SPARC64_HYPERVISOR_H
#define _SPARC64_HYPERVISOR_H

/* Sun4v hypervisor interfaces and defines.
 *
 * Hypervisor calls are made via traps to software traps number 0x80
 * and above.  Registers %o0 to %o5 serve as argument, status, and
 * return value registers.
 *
 * There are two kinds of these traps.  First there are the normal
 * "fast traps" which use software trap 0x80 and encode the function
 * to invoke by number in register %o5.  Argument and return value
 * handling is as follows:
 *
 * -----------------------------------------------
 * |  %o5  | function number |     undefined     |
 * |  %o0  |   argument 0    |   return status   |
 * |  %o1  |   argument 1    |   return value 1  |
 * |  %o2  |   argument 2    |   return value 2  |
 * |  %o3  |   argument 3    |   return value 3  |
 * |  %o4  |   argument 4    |   return value 4  |
 * -----------------------------------------------
 *
 * The second type are "hyper-fast traps" which encode the function
 * number in the software trap number itself.  So these use trap
 * numbers > 0x80.  The register usage for hyper-fast traps is as
 * follows:
 *
 * -----------------------------------------------
 * |  %o0  |   argument 0    |   return status   |
 * |  %o1  |   argument 1    |   return value 1  |
 * |  %o2  |   argument 2    |   return value 2  |
 * |  %o3  |   argument 3    |   return value 3  |
 * |  %o4  |   argument 4    |   return value 4  |
 * -----------------------------------------------
 *
 * Registers providing explicit arguments to the hypervisor calls
 * are volatile across the call.  Upon return their values are
 * undefined unless explicitly specified as containing a particular
 * return value by the specific call.  The return status is always
 * returned in register %o0, zero indicates a successful execution of
 * the hypervisor call and other values indicate an error status as
 * defined below.  So, for example, if a hyper-fast trap takes
 * arguments 0, 1, and 2, then %o0, %o1, and %o2 are volatile across
 * the call and %o3, %o4, and %o5 would be preserved.
 *
 * If the hypervisor trap is invalid, or the fast trap function number
 * is invalid, HV_EBADTRAP will be returned in %o0.  Also, all 64-bits
 * of the argument and return values are significant.
 */

/* Trap numbers.  */
#define HV_FAST_TRAP		0x80
#define HV_MMU_MAP_ADDR_TRAP	0x83
#define HV_MMU_UNMAP_ADDR_TRAP	0x84
#define HV_TTRACE_ADDENTRY_TRAP	0x85
#define HV_CORE_TRAP		0xff

/* Error codes.  */
#define HV_EOK				0  /* Successful return            */
#define HV_ENOCPU			1  /* Invalid CPU id               */
#define HV_ENORADDR			2  /* Invalid real address         */
#define HV_ENOINTR			3  /* Invalid interrupt id         */
#define HV_EBADPGSZ			4  /* Invalid pagesize encoding    */
#define HV_EBADTSB			5  /* Invalid TSB description      */
#define HV_EINVAL			6  /* Invalid argument             */
#define HV_EBADTRAP			7  /* Invalid function number      */
#define HV_EBADALIGN			8  /* Invalid address alignment    */
#define HV_EWOULDBLOCK			9  /* Cannot complete w/o blocking */
#define HV_ENOACCESS			10 /* No access to resource        */
#define HV_EIO				11 /* I/O error                    */
#define HV_ECPUERROR			12 /* CPU in error state           */
#define HV_ENOTSUPPORTED		13 /* Function not supported       */
#define HV_ENOMAP			14 /* No mapping found             */
#define HV_ETOOMANY			15 /* Too many items specified     */

/* mach_exit()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MACH_EXIT
 * ARG0:	exit code
 * ERRORS:	This service does not return.
 *
 * Stop all CPUs in the virtual domain and place them into the stopped
 * state.  The 64-bit exit code may be passed to a service entity as
 * the domain's exit status.  On systems without a service entity, the
 * domain will undergo a reset, and the boot firmware will be
 * reloaded.
 *
 * This function will never return to the guest that invokes it.
 *
 * Note: By convention an exit code of zero denotes a successful exit by
 *       the guest code.  A non-zero exit code denotes a guest specific
 *       error indication.
 *
 */
#define HV_FAST_MACH_EXIT		0x00

/* Domain services.  */

/* mach_desc()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MACH_DESC
 * ARG0:	buffer
 * ARG1:	length
 * RET0:	status
 * RET1:	length
 * ERRORS:	HV_EBADALIGN	Buffer is badly aligned
 *		HV_ENORADDR	Buffer is to an illegal real address.
 *		HV_EINVAL	Buffer length is too small for complete
 *				machine description.
 *
 * Copy the most current machine description into the buffer indicated
 * by the real address in ARG0.  The buffer provided must be 16 byte
 * aligned.  Upon success or HV_EINVAL, this service returns the
 * actual size of the machine description in the RET1 return value.
 *
 * Note: A method of determining the appropriate buffer size for the
 *       machine description is to first call this service with a buffer
 *       length of 0 bytes.
 */
#define HV_FAST_MACH_DESC		0x01

/* mach_exit()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MACH_SIR
 * ERRORS:	This service does not return.
 *
 * Perform a software initiated reset of the virtual machine domain.
 * All CPUs are captured as soon as possible, all hardware devices are
 * returned to the entry default state, and the domain is restarted at
 * the SIR (trap type 0x04) real trap table (RTBA) entry point on one
 * of the CPUs.  The single CPU restarted is selected as determined by
 * platform specific policy.  Memory is preserved across this
 * operation.
 */
#define HV_FAST_MACH_SIR		0x02

/* mach_set_soft_state()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MACH_SET_SOFT_STATE
 * ARG0:	software state
 * ARG1:	software state description pointer
 * RET0:	status
 * ERRORS:	EINVAL		software state not valid or software state
 *				description is not NULL terminated
 *		ENORADDR	software state description pointer is not a
 *				valid real address
 *		EBADALIGNED	software state description is not correctly
 *				aligned
 *
 * This allows the guest to report it's soft state to the hypervisor.  There
 * are two primary components to this state.  The first part states whether
 * the guest software is running or not.  The second containts optional
 * details specific to the software.
 *
 * The software state argument is defined below in HV_SOFT_STATE_*, and
 * indicates whether the guest is operating normally or in a transitional
 * state.
 *
 * The software state description argument is a real address of a data buffer
 * of size 32-bytes aligned on a 32-byte boundary.  It is treated as a NULL
 * terminated 7-bit ASCII string of up to 31 characters not including the
 * NULL termination.
 */
#define HV_FAST_MACH_SET_SOFT_STATE	0x03
#define  HV_SOFT_STATE_NORMAL		 0x01
#define  HV_SOFT_STATE_TRANSITION	 0x02

/* mach_get_soft_state()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MACH_GET_SOFT_STATE
 * ARG0:	software state description pointer
 * RET0:	status
 * RET1:	software state
 * ERRORS:	ENORADDR	software state description pointer is not a
 *				valid real address
 *		EBADALIGNED	software state description is not correctly
 *				aligned
 *
 * Retrieve the current value of the guest's software state.  The rules
 * for the software state pointer are the same as for mach_set_soft_state()
 * above.
 */
#define HV_FAST_MACH_GET_SOFT_STATE	0x04

/* CPU services.
 *
 * CPUs represent devices that can execute software threads.  A single
 * chip that contains multiple cores or strands is represented as
 * multiple CPUs with unique CPU identifiers.  CPUs are exported to
 * OBP via the machine description (and to the OS via the OBP device
 * tree).  CPUs are always in one of three states: stopped, running,
 * or error.
 *
 * A CPU ID is a pre-assigned 16-bit value that uniquely identifies a
 * CPU within a logical domain.  Operations that are to be performed
 * on multiple CPUs specify them via a CPU list.  A CPU list is an
 * array in real memory, of which each 16-bit word is a CPU ID.  CPU
 * lists are passed through the API as two arguments.  The first is
 * the number of entries (16-bit words) in the CPU list, and the
 * second is the (real address) pointer to the CPU ID list.
 */

/* cpu_start()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CPU_START
 * ARG0:	CPU ID
 * ARG1:	PC
 * ARG1:	RTBA
 * ARG1:	target ARG0
 * RET0:	status
 * ERRORS:	ENOCPU		Invalid CPU ID
 *		EINVAL		Target CPU ID is not in the stopped state
 *		ENORADDR	Invalid PC or RTBA real address
 *		EBADALIGN	Unaligned PC or unaligned RTBA
 *		EWOULDBLOCK	Starting resources are not available
 *
 * Start CPU with given CPU ID with PC in %pc and with a real trap
 * base address value of RTBA.  The indicated CPU must be in the
 * stopped state.  The supplied RTBA must be aligned on a 256 byte
 * boundary.  On successful completion, the specified CPU will be in
 * the running state and will be supplied with "target ARG0" in %o0
 * and RTBA in %tba.
 */
#define HV_FAST_CPU_START		0x10

/* cpu_stop()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CPU_STOP
 * ARG0:	CPU ID
 * RET0:	status
 * ERRORS:	ENOCPU		Invalid CPU ID
 *		EINVAL		Target CPU ID is the current cpu
 *		EINVAL		Target CPU ID is not in the running state
 *		EWOULDBLOCK	Stopping resources are not available
 *		ENOTSUPPORTED	Not supported on this platform
 *
 * The specified CPU is stopped.  The indicated CPU must be in the
 * running state.  On completion, it will be in the stopped state.  It
 * is not legal to stop the current CPU.
 *
 * Note: As this service cannot be used to stop the current cpu, this service
 *       may not be used to stop the last running CPU in a domain.  To stop
 *       and exit a running domain, a guest must use the mach_exit() service.
 */
#define HV_FAST_CPU_STOP		0x11

/* cpu_yield()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CPU_YIELD
 * RET0:	status
 * ERRORS:	No possible error.
 *
 * Suspend execution on the current CPU.  Execution will resume when
 * an interrupt (device, %stick_compare, or cross-call) is targeted to
 * the CPU.  On some CPUs, this API may be used by the hypervisor to
 * save power by disabling hardware strands.
 */
#define HV_FAST_CPU_YIELD		0x12


/* cpu_qconf()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CPU_QCONF
 * ARG0:	queue
 * ARG1:	base real address
 * ARG2:	number of entries
 * RET0:	status
 * ERRORS:	ENORADDR	Invalid base real address
 *		EINVAL		Invalid queue or number of entries is less
 *				than 2 or too large.
 *		EBADALIGN	Base real address is not correctly aligned
 *				for size.
 *
 * Configure the given queue to be placed at the givem base real
 * address, with the given number of entries.  The number of entries
 * must be a power of 2.  The base real address must be aligned
 * exactly to match the queue size.  Each queue entry is 64 bytes
 * long, so for example a 32 entry queue must be aligned on a 2048
 * byte real address boundary.
 *
 * The specified queue is unconfigured is number of entries is given as zero.
 *
 * For the current version of this API service, the argument queue is defined
 * as follows:
 *	queue		description
 *	-----		-------------------------
 *	0x3c		cpu mondo queue
 *	0x3d		device mondo queue
 *	0x3e		resumable error queue
 *	0x3f		non-resumable error queue
 *
 * Note: The maximum number of entries for each queue for a specific cpu may
 *       be determined from the machine description.
 */
#define HV_FAST_CPU_QCONF		0x14
#define  HV_CPU_QUEUE_CPU_MONDO		 0x3c
#define  HV_CPU_QUEUE_DEVICE_MONDO	 0x3d
#define  HV_CPU_QUEUE_RES_ERROR		 0x3e
#define  HV_CPU_QUEUE_NONRES_ERROR	 0x3f

/* cpu_qinfo()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CPU_QINFO
 * ARG0:	queue
 * RET0:	status
 * RET1:	base real address
 * RET1:	number of entries
 * ERRORS:	EINVAL		Invalid queue
 *
 * Return the configuration info for the given queue.  The base real
 * address and number of entries of the defined queue are returned.
 * The queue argument values are the same as for cpu_qconf() above.
 *
 * If the specified queue is a valid queue number, but no queue has
 * been defined, the number of entries will be set to zero and the
 * base real address returned is undefined.
 */
#define HV_FAST_CPU_QINFO		0x15

/* cpu_mondo_send()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CPU_MONDO_SEND
 * ARG0-1:	CPU list
 * ARG2:	data real address
 * RET0:	status
 * ERRORS:	EBADALIGN	Mondo data is not 64-byte aligned or CPU list
 *				is not 2-byte aligned.
 *		ENORADDR	Invalid data mondo address, or invalid cpu list
 *				address.
 *		ENOCPU		Invalid cpu in CPU list
 *		EWOULDBLOCK	Some or all of the listed CPUs did not receive
 *				the mondo
 *		EINVAL		CPU list includes caller's CPU ID
 *
 * Send a mondo interrupt to the CPUs in the given CPU list with the
 * 64-bytes at the given data real address.  The data must be 64-byte
 * aligned.  The mondo data will be delivered to the cpu_mondo queues
 * of the recipient CPUs.
 *
 * In all cases, error or not, the CPUs in the CPU list to which the
 * mondo has been successfully delivered will be indicated by having
 * their entry in CPU list updated with the value 0xffff.
 */
#define HV_FAST_CPU_MONDO_SEND		0x42

/* cpu_myid()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CPU_MYID
 * RET0:	status
 * RET1:	CPU ID
 * ERRORS:	No errors defined.
 *
 * Return the hypervisor ID handle for the current CPU.  Use by a
 * virtual CPU to discover it's own identity.
 */
#define HV_FAST_CPU_MYID		0x16

/* cpu_state()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CPU_STATE
 * ARG0:	CPU ID
 * RET0:	status
 * RET1:	state
 * ERRORS:	ENOCPU		Invalid CPU ID
 *
 * Retrieve the current state of the CPU with the given CPU ID.
 */
#define HV_FAST_CPU_STATE		0x17
#define  HV_CPU_STATE_STOPPED		 0x01
#define  HV_CPU_STATE_RUNNING		 0x02
#define  HV_CPU_STATE_ERROR		 0x03

/* cpu_set_rtba()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CPU_SET_RTBA
 * ARG0:	RTBA
 * RET0:	status
 * RET1:	previous RTBA
 * ERRORS:	ENORADDR	Invalid RTBA real address
 *		EBADALIGN	RTBA is incorrectly aligned for a trap table
 *
 * Set the real trap base address of the local cpu to the given RTBA.
 * The supplied RTBA must be aligned on a 256 byte boundary.  Upon
 * success the previous value of the RTBA is returned in RET1.
 *
 * Note: This service does not affect %tba
 */
#define HV_FAST_CPU_SET_RTBA		0x18

/* cpu_set_rtba()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CPU_GET_RTBA
 * RET0:	status
 * RET1:	previous RTBA
 * ERRORS:	No possible error.
 *
 * Returns the current value of RTBA in RET1.
 */
#define HV_FAST_CPU_GET_RTBA		0x19

/* MMU services.
 *
 * Layout of a TSB description for mmu_tsb_ctx{,non}0() calls.
 */
#ifndef __ASSEMBLY__
struct hv_tsb_descr {
	unsigned short		pgsz_idx;
	unsigned short		assoc;
	unsigned int		num_ttes;	/* in TTEs */
	unsigned int		ctx_idx;
	unsigned int		pgsz_mask;
	unsigned long		tsb_base;
	unsigned long		resv;
};
#endif
#define HV_TSB_DESCR_PGSZ_IDX_OFFSET	0x00
#define HV_TSB_DESCR_ASSOC_OFFSET	0x02
#define HV_TSB_DESCR_NUM_TTES_OFFSET	0x04
#define HV_TSB_DESCR_CTX_IDX_OFFSET	0x08
#define HV_TSB_DESCR_PGSZ_MASK_OFFSET	0x0c
#define HV_TSB_DESCR_TSB_BASE_OFFSET	0x10
#define HV_TSB_DESCR_RESV_OFFSET	0x18

/* Page size bitmask.  */
#define HV_PGSZ_MASK_8K			(1 << 0)
#define HV_PGSZ_MASK_64K		(1 << 1)
#define HV_PGSZ_MASK_512K		(1 << 2)
#define HV_PGSZ_MASK_4MB		(1 << 3)
#define HV_PGSZ_MASK_32MB		(1 << 4)
#define HV_PGSZ_MASK_256MB		(1 << 5)
#define HV_PGSZ_MASK_2GB		(1 << 6)
#define HV_PGSZ_MASK_16GB		(1 << 7)

/* Page size index.  The value given in the TSB descriptor must correspond
 * to the smallest page size specified in the pgsz_mask page size bitmask.
 */
#define HV_PGSZ_IDX_8K			0
#define HV_PGSZ_IDX_64K			1
#define HV_PGSZ_IDX_512K		2
#define HV_PGSZ_IDX_4MB			3
#define HV_PGSZ_IDX_32MB		4
#define HV_PGSZ_IDX_256MB		5
#define HV_PGSZ_IDX_2GB			6
#define HV_PGSZ_IDX_16GB		7

/* MMU fault status area.
 *
 * MMU related faults have their status and fault address information
 * placed into a memory region made available by privileged code.  Each
 * virtual processor must make a mmu_fault_area_conf() call to tell the
 * hypervisor where that processor's fault status should be stored.
 *
 * The fault status block is a multiple of 64-bytes and must be aligned
 * on a 64-byte boundary.
 */
#ifndef __ASSEMBLY__
struct hv_fault_status {
	unsigned long		i_fault_type;
	unsigned long		i_fault_addr;
	unsigned long		i_fault_ctx;
	unsigned long		i_reserved[5];
	unsigned long		d_fault_type;
	unsigned long		d_fault_addr;
	unsigned long		d_fault_ctx;
	unsigned long		d_reserved[5];
};
#endif
#define HV_FAULT_I_TYPE_OFFSET	0x00
#define HV_FAULT_I_ADDR_OFFSET	0x08
#define HV_FAULT_I_CTX_OFFSET	0x10
#define HV_FAULT_D_TYPE_OFFSET	0x40
#define HV_FAULT_D_ADDR_OFFSET	0x48
#define HV_FAULT_D_CTX_OFFSET	0x50

#define HV_FAULT_TYPE_FAST_MISS	1
#define HV_FAULT_TYPE_FAST_PROT	2
#define HV_FAULT_TYPE_MMU_MISS	3
#define HV_FAULT_TYPE_INV_RA	4
#define HV_FAULT_TYPE_PRIV_VIOL	5
#define HV_FAULT_TYPE_PROT_VIOL	6
#define HV_FAULT_TYPE_NFO	7
#define HV_FAULT_TYPE_NFO_SEFF	8
#define HV_FAULT_TYPE_INV_VA	9
#define HV_FAULT_TYPE_INV_ASI	10
#define HV_FAULT_TYPE_NC_ATOMIC	11
#define HV_FAULT_TYPE_PRIV_ACT	12
#define HV_FAULT_TYPE_RESV1	13
#define HV_FAULT_TYPE_UNALIGNED	14
#define HV_FAULT_TYPE_INV_PGSZ	15
/* Values 16 --> -2 are reserved.  */
#define HV_FAULT_TYPE_MULTIPLE	-1

/* Flags argument for mmu_{map,unmap}_addr(), mmu_demap_{page,context,all}(),
 * and mmu_{map,unmap}_perm_addr().
 */
#define HV_MMU_DMMU			0x01
#define HV_MMU_IMMU			0x02
#define HV_MMU_ALL			(HV_MMU_DMMU | HV_MMU_IMMU)

/* mmu_map_addr()
 * TRAP:	HV_MMU_MAP_ADDR_TRAP
 * ARG0:	virtual address
 * ARG1:	mmu context
 * ARG2:	TTE
 * ARG3:	flags (HV_MMU_{IMMU,DMMU})
 * ERRORS:	EINVAL		Invalid virtual address, mmu context, or flags
 *		EBADPGSZ	Invalid page size value
 *		ENORADDR	Invalid real address in TTE
 *
 * Create a non-permanent mapping using the given TTE, virtual
 * address, and mmu context.  The flags argument determines which
 * (data, or instruction, or both) TLB the mapping gets loaded into.
 *
 * The behavior is undefined if the valid bit is clear in the TTE.
 *
 * Note: This API call is for privileged code to specify temporary translation
 *       mappings without the need to create and manage a TSB.
 */

/* mmu_unmap_addr()
 * TRAP:	HV_MMU_UNMAP_ADDR_TRAP
 * ARG0:	virtual address
 * ARG1:	mmu context
 * ARG2:	flags (HV_MMU_{IMMU,DMMU})
 * ERRORS:	EINVAL		Invalid virtual address, mmu context, or flags
 *
 * Demaps the given virtual address in the given mmu context on this
 * CPU.  This function is intended to be used to demap pages mapped
 * with mmu_map_addr.  This service is equivalent to invoking
 * mmu_demap_page() with only the current CPU in the CPU list. The
 * flags argument determines which (data, or instruction, or both) TLB
 * the mapping gets unmapped from.
 *
 * Attempting to perform an unmap operation for a previously defined
 * permanent mapping will have undefined results.
 */

/* mmu_tsb_ctx0()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_TSB_CTX0
 * ARG0:	number of TSB descriptions
 * ARG1:	TSB descriptions pointer
 * RET0:	status
 * ERRORS:	ENORADDR		Invalid TSB descriptions pointer or
 *					TSB base within a descriptor
 *		EBADALIGN		TSB descriptions pointer is not aligned
 *					to an 8-byte boundary, or TSB base
 *					within a descriptor is not aligned for
 *					the given TSB size
 *		EBADPGSZ		Invalid page size in a TSB descriptor
 *		EBADTSB			Invalid associativity or size in a TSB
 *					descriptor
 *		EINVAL			Invalid number of TSB descriptions, or
 *					invalid context index in a TSB
 *					descriptor, or index page size not
 *					equal to smallest page size in page
 *					size bitmask field.
 *
 * Configures the TSBs for the current CPU for virtual addresses with
 * context zero.  The TSB descriptions pointer is a pointer to an
 * array of the given number of TSB descriptions.
 *
 * Note: The maximum number of TSBs available to a virtual CPU is given by the
 *       mmu-max-#tsbs property of the cpu's corresponding "cpu" node in the
 *       machine description.
 */
#define HV_FAST_MMU_TSB_CTX0		0x20

/* mmu_tsb_ctxnon0()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_TSB_CTXNON0
 * ARG0:	number of TSB descriptions
 * ARG1:	TSB descriptions pointer
 * RET0:	status
 * ERRORS:	Same as for mmu_tsb_ctx0() above.
 *
 * Configures the TSBs for the current CPU for virtual addresses with
 * non-zero contexts.  The TSB descriptions pointer is a pointer to an
 * array of the given number of TSB descriptions.
 *
 * Note: A maximum of 16 TSBs may be specified in the TSB description list.
 */
#define HV_FAST_MMU_TSB_CTXNON0		0x21

/* mmu_demap_page()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_DEMAP_PAGE
 * ARG0:	reserved, must be zero
 * ARG1:	reserved, must be zero
 * ARG2:	virtual address
 * ARG3:	mmu context
 * ARG4:	flags (HV_MMU_{IMMU,DMMU})
 * RET0:	status
 * ERRORS:	EINVAL			Invalid virutal address, context, or
 *					flags value
 *		ENOTSUPPORTED		ARG0 or ARG1 is non-zero
 *
 * Demaps any page mapping of the given virtual address in the given
 * mmu context for the current virtual CPU.  Any virtually tagged
 * caches are guaranteed to be kept consistent.  The flags argument
 * determines which TLB (instruction, or data, or both) participate in
 * the operation.
 *
 * ARG0 and ARG1 are both reserved and must be set to zero.
 */
#define HV_FAST_MMU_DEMAP_PAGE		0x22

/* mmu_demap_ctx()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_DEMAP_CTX
 * ARG0:	reserved, must be zero
 * ARG1:	reserved, must be zero
 * ARG2:	mmu context
 * ARG3:	flags (HV_MMU_{IMMU,DMMU})
 * RET0:	status
 * ERRORS:	EINVAL			Invalid context or flags value
 *		ENOTSUPPORTED		ARG0 or ARG1 is non-zero
 *
 * Demaps all non-permanent virtual page mappings previously specified
 * for the given context for the current virtual CPU.  Any virtual
 * tagged caches are guaranteed to be kept consistent.  The flags
 * argument determines which TLB (instruction, or data, or both)
 * participate in the operation.
 *
 * ARG0 and ARG1 are both reserved and must be set to zero.
 */
#define HV_FAST_MMU_DEMAP_CTX		0x23

/* mmu_demap_all()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_DEMAP_ALL
 * ARG0:	reserved, must be zero
 * ARG1:	reserved, must be zero
 * ARG2:	flags (HV_MMU_{IMMU,DMMU})
 * RET0:	status
 * ERRORS:	EINVAL			Invalid flags value
 *		ENOTSUPPORTED		ARG0 or ARG1 is non-zero
 *
 * Demaps all non-permanent virtual page mappings previously specified
 * for the current virtual CPU.  Any virtual tagged caches are
 * guaranteed to be kept consistent.  The flags argument determines
 * which TLB (instruction, or data, or both) participate in the
 * operation.
 *
 * ARG0 and ARG1 are both reserved and must be set to zero.
 */
#define HV_FAST_MMU_DEMAP_ALL		0x24

/* mmu_map_perm_addr()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_MAP_PERM_ADDR
 * ARG0:	virtual address
 * ARG1:	reserved, must be zero
 * ARG2:	TTE
 * ARG3:	flags (HV_MMU_{IMMU,DMMU})
 * RET0:	status
 * ERRORS:	EINVAL			Invalid virutal address or flags value
 *		EBADPGSZ		Invalid page size value
 *		ENORADDR		Invalid real address in TTE
 *		ETOOMANY		Too many mappings (max of 8 reached)
 *
 * Create a permanent mapping using the given TTE and virtual address
 * for context 0 on the calling virtual CPU.  A maximum of 8 such
 * permanent mappings may be specified by privileged code.  Mappings
 * may be removed with mmu_unmap_perm_addr().
 *
 * The behavior is undefined if a TTE with the valid bit clear is given.
 *
 * Note: This call is used to specify address space mappings for which
 *       privileged code does not expect to receive misses.  For example,
 *       this mechanism can be used to map kernel nucleus code and data.
 */
#define HV_FAST_MMU_MAP_PERM_ADDR	0x25

/* mmu_fault_area_conf()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_FAULT_AREA_CONF
 * ARG0:	real address
 * RET0:	status
 * RET1:	previous mmu fault area real address
 * ERRORS:	ENORADDR		Invalid real address
 *		EBADALIGN		Invalid alignment for fault area
 *
 * Configure the MMU fault status area for the calling CPU.  A 64-byte
 * aligned real address specifies where MMU fault status information
 * is placed.  The return value is the previously specified area, or 0
 * for the first invocation.  Specifying a fault area at real address
 * 0 is not allowed.
 */
#define HV_FAST_MMU_FAULT_AREA_CONF	0x26

/* mmu_enable()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_ENABLE
 * ARG0:	enable flag
 * ARG1:	return target address
 * RET0:	status
 * ERRORS:	ENORADDR		Invalid real address when disabling
 *					translation.
 *		EBADALIGN		The return target address is not
 *					aligned to an instruction.
 *		EINVAL			The enable flag request the current
 *					operating mode (e.g. disable if already
 *					disabled)
 *
 * Enable or disable virtual address translation for the calling CPU
 * within the virtual machine domain.  If the enable flag is zero,
 * translation is disabled, any non-zero value will enable
 * translation.
 *
 * When this function returns, the newly selected translation mode
 * will be active.  If the mmu is being enabled, then the return
 * target address is a virtual address else it is a real address.
 *
 * Upon successful completion, control will be returned to the given
 * return target address (ie. the cpu will jump to that address).  On
 * failure, the previous mmu mode remains and the trap simply returns
 * as normal with the appropriate error code in RET0.
 */
#define HV_FAST_MMU_ENABLE		0x27

/* mmu_unmap_perm_addr()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_UNMAP_PERM_ADDR
 * ARG0:	virtual address
 * ARG1:	reserved, must be zero
 * ARG2:	flags (HV_MMU_{IMMU,DMMU})
 * RET0:	status
 * ERRORS:	EINVAL			Invalid virutal address or flags value
 *		ENOMAP			Specified mapping was not found
 *
 * Demaps any permanent page mapping (established via
 * mmu_map_perm_addr()) at the given virtual address for context 0 on
 * the current virtual CPU.  Any virtual tagged caches are guaranteed
 * to be kept consistent.
 */
#define HV_FAST_MMU_UNMAP_PERM_ADDR	0x28

/* mmu_tsb_ctx0_info()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_TSB_CTX0_INFO
 * ARG0:	max TSBs
 * ARG1:	buffer pointer
 * RET0:	status
 * RET1:	number of TSBs
 * ERRORS:	EINVAL			Supplied buffer is too small
 *		EBADALIGN		The buffer pointer is badly aligned
 *		ENORADDR		Invalid real address for buffer pointer
 *
 * Return the TSB configuration as previous defined by mmu_tsb_ctx0()
 * into the provided buffer.  The size of the buffer is given in ARG1
 * in terms of the number of TSB description entries.
 *
 * Upon return, RET1 always contains the number of TSB descriptions
 * previously configured.  If zero TSBs were configured, EOK is
 * returned with RET1 containing 0.
 */
#define HV_FAST_MMU_TSB_CTX0_INFO	0x29

/* mmu_tsb_ctxnon0_info()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_TSB_CTXNON0_INFO
 * ARG0:	max TSBs
 * ARG1:	buffer pointer
 * RET0:	status
 * RET1:	number of TSBs
 * ERRORS:	EINVAL			Supplied buffer is too small
 *		EBADALIGN		The buffer pointer is badly aligned
 *		ENORADDR		Invalid real address for buffer pointer
 *
 * Return the TSB configuration as previous defined by
 * mmu_tsb_ctxnon0() into the provided buffer.  The size of the buffer
 * is given in ARG1 in terms of the number of TSB description entries.
 *
 * Upon return, RET1 always contains the number of TSB descriptions
 * previously configured.  If zero TSBs were configured, EOK is
 * returned with RET1 containing 0.
 */
#define HV_FAST_MMU_TSB_CTXNON0_INFO	0x2a

/* mmu_fault_area_info()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMU_FAULT_AREA_INFO
 * RET0:	status
 * RET1:	fault area real address
 * ERRORS:	No errors defined.
 *
 * Return the currently defined MMU fault status area for the current
 * CPU.  The real address of the fault status area is returned in
 * RET1, or 0 is returned in RET1 if no fault status area is defined.
 *
 * Note: mmu_fault_area_conf() may be called with the return value (RET1)
 *       from this service if there is a need to save and restore the fault
 *	 area for a cpu.
 */
#define HV_FAST_MMU_FAULT_AREA_INFO	0x2b

/* Cache and Memory services. */

/* mem_scrub()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MEM_SCRUB
 * ARG0:	real address
 * ARG1:	length
 * RET0:	status
 * RET1:	length scrubbed
 * ERRORS:	ENORADDR	Invalid real address
 *		EBADALIGN	Start address or length are not correctly
 *				aligned
 *		EINVAL		Length is zero
 *
 * Zero the memory contents in the range real address to real address
 * plus length minus 1.  Also, valid ECC will be generated for that
 * memory address range.  Scrubbing is started at the given real
 * address, but may not scrub the entire given length.  The actual
 * length scrubbed will be returned in RET1.
 *
 * The real address and length must be aligned on an 8K boundary, or
 * contain the start address and length from a sun4v error report.
 *
 * Note: There are two uses for this function.  The first use is to block clear
 *       and initialize memory and the second is to scrub an u ncorrectable
 *       error reported via a resumable or non-resumable trap.  The second
 *       use requires the arguments to be equal to the real address and length
 *       provided in a sun4v memory error report.
 */
#define HV_FAST_MEM_SCRUB		0x31

/* mem_sync()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MEM_SYNC
 * ARG0:	real address
 * ARG1:	length
 * RET0:	status
 * RET1:	length synced
 * ERRORS:	ENORADDR	Invalid real address
 *		EBADALIGN	Start address or length are not correctly
 *				aligned
 *		EINVAL		Length is zero
 *
 * Force the next access within the real address to real address plus
 * length minus 1 to be fetches from main system memory.  Less than
 * the given length may be synced, the actual amount synced is
 * returned in RET1.  The real address and length must be aligned on
 * an 8K boundary.
 */
#define HV_FAST_MEM_SYNC		0x32

/* Time of day services.
 *
 * The hypervisor maintains the time of day on a per-domain basis.
 * Changing the time of day in one domain does not affect the time of
 * day on any other domain.
 *
 * Time is described by a single unsigned 64-bit word which is the
 * number of seconds since the UNIX Epoch (00:00:00 UTC, January 1,
 * 1970).
 */

/* tod_get()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_TOD_GET
 * RET0:	status
 * RET1:	TOD
 * ERRORS:	EWOULDBLOCK	TOD resource is temporarily unavailable
 *		ENOTSUPPORTED	If TOD not supported on this platform
 *
 * Return the current time of day.  May block if TOD access is
 * temporarily not possible.
 */
#define HV_FAST_TOD_GET			0x50

/* tod_set()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_TOD_SET
 * ARG0:	TOD
 * RET0:	status
 * ERRORS:	EWOULDBLOCK	TOD resource is temporarily unavailable
 *		ENOTSUPPORTED	If TOD not supported on this platform
 *
 * The current time of day is set to the value specified in ARG0.  May
 * block if TOD access is temporarily not possible.
 */
#define HV_FAST_TOD_SET			0x51

/* Console services */

/* con_getchar()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CONS_GETCHAR
 * RET0:	status
 * RET1:	character
 * ERRORS:	EWOULDBLOCK	No character available.
 *
 * Returns a character from the console device.  If no character is
 * available then an EWOULDBLOCK error is returned.  If a character is
 * available, then the returned status is EOK and the character value
 * is in RET1.
 *
 * A virtual BREAK is represented by the 64-bit value -1.
 *
 * A virtual HUP signal is represented by the 64-bit value -2.
 */
#define HV_FAST_CONS_GETCHAR		0x60

/* con_putchar()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_CONS_PUTCHAR
 * ARG0:	character
 * RET0:	status
 * ERRORS:	EINVAL		Illegal character
 *		EWOULDBLOCK	Output buffer currentl full, would block
 *
 * Send a character to the console device.  Only character values
 * between 0 and 255 may be used.  Values outside this range are
 * invalid except for the 64-bit value -1 which is used to send a
 * virtual BREAK.
 */
#define HV_FAST_CONS_PUTCHAR		0x61

/* Trap trace services.
 *
 * The hypervisor provides a trap tracing capability for privileged
 * code running on each virtual CPU.  Privileged code provides a
 * round-robin trap trace queue within which the hypervisor writes
 * 64-byte entries detailing hyperprivileged traps taken n behalf of
 * privileged code.  This is provided as a debugging capability for
 * privileged code.
 *
 * The trap trace control structure is 64-bytes long and placed at the
 * start (offset 0) of the trap trace buffer, and is described as
 * follows:
 */
#ifndef __ASSEMBLY__
struct hv_trap_trace_control {
	unsigned long		head_offset;
	unsigned long		tail_offset;
	unsigned long		__reserved[0x30 / sizeof(unsigned long)];
};
#endif
#define HV_TRAP_TRACE_CTRL_HEAD_OFFSET	0x00
#define HV_TRAP_TRACE_CTRL_TAIL_OFFSET	0x08

/* The head offset is the offset of the most recently completed entry
 * in the trap-trace buffer.  The tail offset is the offset of the
 * next entry to be written.  The control structure is owned and
 * modified by the hypervisor.  A guest may not modify the control
 * structure contents.  Attempts to do so will result in undefined
 * behavior for the guest.
 *
 * Each trap trace buffer entry is layed out as follows:
 */
#ifndef __ASSEMBLY__
struct hv_trap_trace_entry {
	unsigned char	type;		/* Hypervisor or guest entry?	*/
	unsigned char	hpstate;	/* Hyper-privileged state	*/
	unsigned char	tl;		/* Trap level			*/
	unsigned char	gl;		/* Global register level	*/
	unsigned short	tt;		/* Trap type			*/
	unsigned short	tag;		/* Extended trap identifier	*/
	unsigned long	tstate;		/* Trap state			*/
	unsigned long	tick;		/* Tick				*/
	unsigned long	tpc;		/* Trap PC			*/
	unsigned long	f1;		/* Entry specific		*/
	unsigned long	f2;		/* Entry specific		*/
	unsigned long	f3;		/* Entry specific		*/
	unsigned long	f4;		/* Entry specific		*/
};
#endif
#define HV_TRAP_TRACE_ENTRY_TYPE	0x00
#define HV_TRAP_TRACE_ENTRY_HPSTATE	0x01
#define HV_TRAP_TRACE_ENTRY_TL		0x02
#define HV_TRAP_TRACE_ENTRY_GL		0x03
#define HV_TRAP_TRACE_ENTRY_TT		0x04
#define HV_TRAP_TRACE_ENTRY_TAG		0x06
#define HV_TRAP_TRACE_ENTRY_TSTATE	0x08
#define HV_TRAP_TRACE_ENTRY_TICK	0x10
#define HV_TRAP_TRACE_ENTRY_TPC		0x18
#define HV_TRAP_TRACE_ENTRY_F1		0x20
#define HV_TRAP_TRACE_ENTRY_F2		0x28
#define HV_TRAP_TRACE_ENTRY_F3		0x30
#define HV_TRAP_TRACE_ENTRY_F4		0x38

/* The type field is encoded as follows.  */
#define HV_TRAP_TYPE_UNDEF		0x00 /* Entry content undefined     */
#define HV_TRAP_TYPE_HV			0x01 /* Hypervisor trap entry       */
#define HV_TRAP_TYPE_GUEST		0xff /* Added via ttrace_addentry() */

/* ttrace_buf_conf()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_TTRACE_BUF_CONF
 * ARG0:	real address
 * ARG1:	number of entries
 * RET0:	status
 * RET1:	number of entries
 * ERRORS:	ENORADDR	Invalid real address
 *		EINVAL		Size is too small
 *		EBADALIGN	Real address not aligned on 64-byte boundary
 *
 * Requests hypervisor trap tracing and declares a virtual CPU's trap
 * trace buffer to the hypervisor.  The real address supplies the real
 * base address of the trap trace queue and must be 64-byte aligned.
 * Specifying a value of 0 for the number of entries disables trap
 * tracing for the calling virtual CPU.  The buffer allocated must be
 * sized for a power of two number of 64-byte trap trace entries plus
 * an initial 64-byte control structure.
 * 
 * This may be invoked any number of times so that a virtual CPU may
 * relocate a trap trace buffer or create "snapshots" of information.
 *
 * If the real address is illegal or badly aligned, then trap tracing
 * is disabled and an error is returned.
 *
 * Upon failure with EINVAL, this service call returns in RET1 the
 * minimum number of buffer entries required.  Upon other failures
 * RET1 is undefined.
 */
#define HV_FAST_TTRACE_BUF_CONF		0x90

/* ttrace_buf_info()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_TTRACE_BUF_INFO
 * RET0:	status
 * RET1:	real address
 * RET2:	size
 * ERRORS:	None defined.
 *
 * Returns the size and location of the previously declared trap-trace
 * buffer.  In the event that no buffer was previously defined, or the
 * buffer is disabled, this call will return a size of zero bytes.
 */
#define HV_FAST_TTRACE_BUF_INFO		0x91

/* ttrace_enable()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_TTRACE_ENABLE
 * ARG0:	enable
 * RET0:	status
 * RET1:	previous enable state
 * ERRORS:	EINVAL		No trap trace buffer currently defined
 *
 * Enable or disable trap tracing, and return the previous enabled
 * state in RET1.  Future systems may define various flags for the
 * enable argument (ARG0), for the moment a guest should pass
 * "(uint64_t) -1" to enable, and "(uint64_t) 0" to disable all
 * tracing - which will ensure future compatability.
 */
#define HV_FAST_TTRACE_ENABLE		0x92

/* ttrace_freeze()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_TTRACE_FREEZE
 * ARG0:	freeze
 * RET0:	status
 * RET1:	previous freeze state
 * ERRORS:	EINVAL		No trap trace buffer currently defined
 *
 * Freeze or unfreeze trap tracing, returning the previous freeze
 * state in RET1.  A guest should pass a non-zero value to freeze and
 * a zero value to unfreeze all tracing.  The returned previous state
 * is 0 for not frozen and 1 for frozen.
 */
#define HV_FAST_TTRACE_FREEZE		0x93

/* ttrace_addentry()
 * TRAP:	HV_TTRACE_ADDENTRY_TRAP
 * ARG0:	tag (16-bits)
 * ARG1:	data word 0
 * ARG2:	data word 1
 * ARG3:	data word 2
 * ARG4:	data word 3
 * RET0:	status
 * ERRORS:	EINVAL		No trap trace buffer currently defined
 *
 * Add an entry to the trap trace buffer.  Upon return only ARG0/RET0
 * is modified - none of the other registers holding arguments are
 * volatile across this hypervisor service.
 */

/* Core dump services.
 *
 * Since the hypervisor viraulizes and thus obscures a lot of the
 * physical machine layout and state, traditional OS crash dumps can
 * be difficult to diagnose especially when the problem is a
 * configuration error of some sort.
 *
 * The dump services provide an opaque buffer into which the
 * hypervisor can place it's internal state in order to assist in
 * debugging such situations.  The contents are opaque and extremely
 * platform and hypervisor implementation specific.  The guest, during
 * a core dump, requests that the hypervisor update any information in
 * the dump buffer in preparation to being dumped as part of the
 * domain's memory image.
 */

/* dump_buf_update()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_DUMP_BUF_UPDATE
 * ARG0:	real address
 * ARG1:	size
 * RET0:	status
 * RET1:	required size of dump buffer
 * ERRORS:	ENORADDR	Invalid real address
 *		EBADALIGN	Real address is not aligned on a 64-byte
 *				boundary
 *		EINVAL		Size is non-zero but less than minimum size
 *				required
 *		ENOTSUPPORTED	Operation not supported on current logical
 *				domain
 *
 * Declare a domain dump buffer to the hypervisor.  The real address
 * provided for the domain dump buffer must be 64-byte aligned.  The
 * size specifies the size of the dump buffer and may be larger than
 * the minimum size specified in the machine description.  The
 * hypervisor will fill the dump buffer with opaque data.
 *
 * Note: A guest may elect to include dump buffer contents as part of a crash
 *       dump to assist with debugging.  This function may be called any number
 *       of times so that a guest may relocate a dump buffer, or create
 *       "snapshots" of any dump-buffer information.  Each call to
 *       dump_buf_update() atomically declares the new dump buffer to the
 *       hypervisor.
 *
 * A specified size of 0 unconfigures the dump buffer.  If the real
 * address is illegal or badly aligned, then any currently active dump
 * buffer is disabled and an error is returned.
 *
 * In the event that the call fails with EINVAL, RET1 contains the
 * minimum size requires by the hypervisor for a valid dump buffer.
 */
#define HV_FAST_DUMP_BUF_UPDATE		0x94

/* dump_buf_info()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_DUMP_BUF_INFO
 * RET0:	status
 * RET1:	real address of current dump buffer
 * RET2:	size of current dump buffer
 * ERRORS:	No errors defined.
 *
 * Return the currently configures dump buffer description.  A
 * returned size of 0 bytes indicates an undefined dump buffer.  In
 * this case the return address in RET1 is undefined.
 */
#define HV_FAST_DUMP_BUF_INFO		0x95

/* Device interrupt services.
 *
 * Device interrupts are allocated to system bus bridges by the hypervisor,
 * and described to OBP in the machine description.  OBP then describes
 * these interrupts to the OS via properties in the device tree.
 *
 * Terminology:
 *
 *	cpuid		Unique opaque value which represents a target cpu.
 *
 *	devhandle	Device handle.  It uniquely identifies a device, and
 *			consistes of the lower 28-bits of the hi-cell of the
 *			first entry of the device's "reg" property in the
 *			OBP device tree.
 *
 *	devino		Device interrupt number.  Specifies the relative
 *			interrupt number within the device.  The unique
 *			combination of devhandle and devino are used to
 *			identify a specific device interrupt.
 *
 *			Note: The devino value is the same as the values in the
 *			      "interrupts" property or "interrupt-map" property
 *			      in the OBP device tree for that device.
 *
 *	sysino		System interrupt number.  A 64-bit unsigned interger
 *			representing a unique interrupt within a virtual
 *			machine.
 *
 *	intr_state	A flag representing the interrupt state for a given
 *			sysino.  The state values are defined below.
 *
 *	intr_enabled	A flag representing the 'enabled' state for a given
 *			sysino.  The enable values are defined below.
 */

#define HV_INTR_STATE_IDLE		0 /* Nothing pending */
#define HV_INTR_STATE_RECEIVED		1 /* Interrupt received by hardware */
#define HV_INTR_STATE_DELIVERED		2 /* Interrupt delivered to queue */

#define HV_INTR_DISABLED		0 /* sysino not enabled */
#define HV_INTR_ENABLED			1 /* sysino enabled */

/* intr_devino_to_sysino()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_INTR_DEVINO2SYSINO
 * ARG0:	devhandle
 * ARG1:	devino
 * RET0:	status
 * RET1:	sysino
 * ERRORS:	EINVAL		Invalid devhandle/devino
 *
 * Converts a device specific interrupt number of the given
 * devhandle/devino into a system specific ino (sysino).
 */
#define HV_FAST_INTR_DEVINO2SYSINO	0xa0

/* intr_getenabled()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_INTR_GETENABLED
 * ARG0:	sysino
 * RET0:	status
 * RET1:	intr_enabled (HV_INTR_{DISABLED,ENABLED})
 * ERRORS:	EINVAL		Invalid sysino
 *
 * Returns interrupt enabled state in RET1 for the interrupt defined
 * by the given sysino.
 */
#define HV_FAST_INTR_GETENABLED		0xa1

/* intr_setenabled()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_INTR_SETENABLED
 * ARG0:	sysino
 * ARG1:	intr_enabled (HV_INTR_{DISABLED,ENABLED})
 * RET0:	status
 * ERRORS:	EINVAL		Invalid sysino or intr_enabled value
 *
 * Set the 'enabled' state of the interrupt sysino.
 */
#define HV_FAST_INTR_SETENABLED		0xa2

/* intr_getstate()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_INTR_GETSTATE
 * ARG0:	sysino
 * RET0:	status
 * RET1:	intr_state (HV_INTR_STATE_*)
 * ERRORS:	EINVAL		Invalid sysino
 *
 * Returns current state of the interrupt defined by the given sysino.
 */
#define HV_FAST_INTR_GETSTATE		0xa3

/* intr_setstate()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_INTR_SETSTATE
 * ARG0:	sysino
 * ARG1:	intr_state (HV_INTR_STATE_*)
 * RET0:	status
 * ERRORS:	EINVAL		Invalid sysino or intr_state value
 *
 * Sets the current state of the interrupt described by the given sysino
 * value.
 *
 * Note: Setting the state to HV_INTR_STATE_IDLE clears any pending
 *       interrupt for sysino.
 */
#define HV_FAST_INTR_SETSTATE		0xa4

/* intr_gettarget()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_INTR_GETTARGET
 * ARG0:	sysino
 * RET0:	status
 * RET1:	cpuid
 * ERRORS:	EINVAL		Invalid sysino
 *
 * Returns CPU that is the current target of the interrupt defined by
 * the given sysino.  The CPU value returned is undefined if the target
 * has not been set via intr_settarget().
 */
#define HV_FAST_INTR_GETTARGET		0xa5

/* intr_settarget()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_INTR_SETTARGET
 * ARG0:	sysino
 * ARG1:	cpuid
 * RET0:	status
 * ERRORS:	EINVAL		Invalid sysino
 *		ENOCPU		Invalid cpuid
 *
 * Set the target CPU for the interrupt defined by the given sysino.
 */
#define HV_FAST_INTR_SETTARGET		0xa6

/* PCI IO services.
 *
 * See the terminology descriptions in the device interrupt services
 * section above as those apply here too.  Here are terminology
 * definitions specific to these PCI IO services:
 *
 *	tsbnum		TSB number.  Indentifies which io-tsb is used.
 *			For this version of the specification, tsbnum
 *			must be zero.
 *
 *	tsbindex	TSB index.  Identifies which entry in the TSB
 *			is used.  The first entry is zero.
 *
 *	tsbid		A 64-bit aligned data structure which contains
 *			a tsbnum and a tsbindex.  Bits 63:32 contain the
 *			tsbnum and bits 31:00 contain the tsbindex.
 *
 *	io_attributes	IO attributes for IOMMU mappings.  One of more
 *			of the attritbute bits are stores in a 64-bit
 *			value.  The values are defined below.
 *
 *	r_addr		64-bit real address
 *
 *	pci_device	PCI device address.  A PCI device address identifies
 *			a specific device on a specific PCI bus segment.
 *			A PCI device address ia a 32-bit unsigned integer
 *			with the following format:
 *
 *				00000000.bbbbbbbb.dddddfff.00000000
 *
 *			Use the HV_PCI_DEVICE_BUILD() macro to construct
 *			such values.
 *
 *	pci_config_offset
 *			PCI configureation space offset.  For conventional
 *			PCI a value between 0 and 255.  For extended
 *			configuration space, a value between 0 and 4095.
 *
 *			Note: For PCI configuration space accesses, the offset
 *			      must be aligned to the access size.
 *
 *	error_flag	A return value which specifies if the action succeeded
 *			or failed.  0 means no error, non-0 means some error
 *			occurred while performing the service.
 *
 *	io_sync_direction
 *			Direction definition for pci_dma_sync(), defined
 *			below in HV_PCI_SYNC_*.
 *
 *	io_page_list	A list of io_page_addresses, an io_page_address is
 *			a real address.
 *
 *	io_page_list_p	A pointer to an io_page_list.
 *
 *	"size based byte swap" - Some functions do size based byte swapping
 *				 which allows sw to access pointers and
 *				 counters in native form when the processor
 *				 operates in a different endianness than the
 *				 IO bus.  Size-based byte swapping converts a
 *				 multi-byte field between big-endian and
 *				 little-endian format.
 */

#define HV_PCI_MAP_ATTR_READ		0x01
#define HV_PCI_MAP_ATTR_WRITE		0x02

#define HV_PCI_DEVICE_BUILD(b,d,f)	\
	((((b) & 0xff) << 16) | \
	 (((d) & 0x1f) << 11) | \
	 (((f) & 0x07) <<  8))

#define HV_PCI_SYNC_FOR_DEVICE		0x01
#define HV_PCI_SYNC_FOR_CPU		0x02

/* pci_iommu_map()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_IOMMU_MAP
 * ARG0:	devhandle
 * ARG1:	tsbid
 * ARG2:	#ttes
 * ARG3:	io_attributes
 * ARG4:	io_page_list_p
 * RET0:	status
 * RET1:	#ttes mapped
 * ERRORS:	EINVAL		Invalid devhandle/tsbnum/tsbindex/io_attributes
 *		EBADALIGN	Improperly aligned real address
 *		ENORADDR	Invalid real address
 *
 * Create IOMMU mappings in the sun4v device defined by the given
 * devhandle.  The mappings are created in the TSB defined by the
 * tsbnum component of the given tsbid.  The first mapping is created
 * in the TSB i ndex defined by the tsbindex component of the given tsbid.
 * The call creates up to #ttes mappings, the first one at tsbnum, tsbindex,
 * the second at tsbnum, tsbindex + 1, etc.
 *
 * All mappings are created with the attributes defined by the io_attributes
 * argument.  The page mapping addresses are described in the io_page_list
 * defined by the given io_page_list_p, which is a pointer to the io_page_list.
 * The first entry in the io_page_list is the address for the first iotte, the
 * 2nd for the 2nd iotte, and so on.
 *
 * Each io_page_address in the io_page_list must be appropriately aligned.
 * #ttes must be greater than zero.  For this version of the spec, the tsbnum
 * component of the given tsbid must be zero.
 *
 * Returns the actual number of mappings creates, which may be less than
 * or equal to the argument #ttes.  If the function returns a value which
 * is less than the #ttes, the caller may continus to call the function with
 * an updated tsbid, #ttes, io_page_list_p arguments until all pages are
 * mapped.
 *
 * Note: This function does not imply an iotte cache flush.  The guest must
 *       demap an entry before re-mapping it.
 */
#define HV_FAST_PCI_IOMMU_MAP		0xb0

/* pci_iommu_demap()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_IOMMU_DEMAP
 * ARG0:	devhandle
 * ARG1:	tsbid
 * ARG2:	#ttes
 * RET0:	status
 * RET1:	#ttes demapped
 * ERRORS:	EINVAL		Invalid devhandle/tsbnum/tsbindex
 *
 * Demap and flush IOMMU mappings in the device defined by the given
 * devhandle.  Demaps up to #ttes entries in the TSB defined by the tsbnum
 * component of the given tsbid, starting at the TSB index defined by the
 * tsbindex component of the given tsbid.
 *
 * For this version of the spec, the tsbnum of the given tsbid must be zero.
 * #ttes must be greater than zero.
 *
 * Returns the actual number of ttes demapped, which may be less than or equal
 * to the argument #ttes.  If #ttes demapped is less than #ttes, the caller
 * may continue to call this function with updated tsbid and #ttes arguments
 * until all pages are demapped.
 *
 * Note: Entries do not have to be mapped to be demapped.  A demap of an
 *       unmapped page will flush the entry from the tte cache.
 */
#define HV_FAST_PCI_IOMMU_DEMAP		0xb1

/* pci_iommu_getmap()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_IOMMU_GETMAP
 * ARG0:	devhandle
 * ARG1:	tsbid
 * RET0:	status
 * RET1:	io_attributes
 * RET2:	real address
 * ERRORS:	EINVAL		Invalid devhandle/tsbnum/tsbindex
 *		ENOMAP		Mapping is not valid, no translation exists
 *
 * Read and return the mapping in the device described by the given devhandle
 * and tsbid.  If successful, the io_attributes shall be returned in RET1
 * and the page address of the mapping shall be returned in RET2.
 *
 * For this version of the spec, the tsbnum component of the given tsbid
 * must be zero.
 */
#define HV_FAST_PCI_IOMMU_GETMAP	0xb2

/* pci_iommu_getbypass()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_IOMMU_GETBYPASS
 * ARG0:	devhandle
 * ARG1:	real address
 * ARG2:	io_attributes
 * RET0:	status
 * RET1:	io_addr
 * ERRORS:	EINVAL		Invalid devhandle/io_attributes
 *		ENORADDR	Invalid real address
 *		ENOTSUPPORTED	Function not supported in this implementation.
 *
 * Create a "special" mapping in the device described by the given devhandle,
 * for the given real address and attributes.  Return the IO address in RET1
 * if successful.
 */
#define HV_FAST_PCI_IOMMU_GETBYPASS	0xb3

/* pci_config_get()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_CONFIG_GET
 * ARG0:	devhandle
 * ARG1:	pci_device
 * ARG2:	pci_config_offset
 * ARG3:	size
 * RET0:	status
 * RET1:	error_flag
 * RET2:	data
 * ERRORS:	EINVAL		Invalid devhandle/pci_device/offset/size
 *		EBADALIGN	pci_config_offset not size aligned
 *		ENOACCESS	Access to this offset is not permitted
 *
 * Read PCI configuration space for the adapter described by the given
 * devhandle.  Read size (1, 2, or 4) bytes of data from the given
 * pci_device, at pci_config_offset from the beginning of the device's
 * configuration space.  If there was no error, RET1 is set to zero and
 * RET2 is set to the data read.  Insignificant bits in RET2 are not
 * guarenteed to have any specific value and therefore must be ignored.
 *
 * The data returned in RET2 is size based byte swapped.
 *
 * If an error occurs during the read, set RET1 to a non-zero value.  The
 * given pci_config_offset must be 'size' aligned.
 */
#define HV_FAST_PCI_CONFIG_GET		0xb4

/* pci_config_put()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_CONFIG_PUT
 * ARG0:	devhandle
 * ARG1:	pci_device
 * ARG2:	pci_config_offset
 * ARG3:	size
 * ARG4:	data
 * RET0:	status
 * RET1:	error_flag
 * ERRORS:	EINVAL		Invalid devhandle/pci_device/offset/size
 *		EBADALIGN	pci_config_offset not size aligned
 *		ENOACCESS	Access to this offset is not permitted
 *
 * Write PCI configuration space for the adapter described by the given
 * devhandle.  Write size (1, 2, or 4) bytes of data in a single operation,
 * at pci_config_offset from the beginning of the device's configuration
 * space.  The data argument contains the data to be written to configuration
 * space.  Prior to writing, the data is size based byte swapped.
 *
 * If an error occurs during the write access, do not generate an error
 * report, do set RET1 to a non-zero value.  Otherwise RET1 is zero.
 * The given pci_config_offset must be 'size' aligned.
 *
 * This function is permitted to read from offset zero in the configuration
 * space described by the given pci_device if necessary to ensure that the
 * write access to config space completes.
 */
#define HV_FAST_PCI_CONFIG_PUT		0xb5

/* pci_peek()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_PEEK
 * ARG0:	devhandle
 * ARG1:	real address
 * ARG2:	size
 * RET0:	status
 * RET1:	error_flag
 * RET2:	data
 * ERRORS:	EINVAL		Invalid devhandle or size
 *		EBADALIGN	Improperly aligned real address
 *		ENORADDR	Bad real address
 *		ENOACCESS	Guest access prohibited
 *
 * Attempt to read the IO address given by the given devhandle, real address,
 * and size.  Size must be 1, 2, 4, or 8.  The read is performed as a single
 * access operation using the given size.  If an error occurs when reading
 * from the given location, do not generate an error report, but return a
 * non-zero value in RET1.  If the read was successful, return zero in RET1
 * and return the actual data read in RET2.  The data returned is size based
 * byte swapped.
 *
 * Non-significant bits in RET2 are not guarenteed to have any specific value
 * and therefore must be ignored.  If RET1 is returned as non-zero, the data 
 * value is not guarenteed to have any specific value and should be ignored.
 *
 * The caller must have permission to read from the given devhandle, real
 * address, which must be an IO address.  The argument real address must be a
 * size aligned address.
 *
 * The hypervisor implementation of this function must block access to any
 * IO address that the guest does not have explicit permission to access.
 */
#define HV_FAST_PCI_PEEK		0xb6

/* pci_poke()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_POKE
 * ARG0:	devhandle
 * ARG1:	real address
 * ARG2:	size
 * ARG3:	data
 * ARG4:	pci_device
 * RET0:	status
 * RET1:	error_flag
 * ERRORS:	EINVAL		Invalid devhandle, size, or pci_device
 *		EBADALIGN	Improperly aligned real address
 *		ENORADDR	Bad real address
 *		ENOACCESS	Guest access prohibited
 *		ENOTSUPPORTED	Function is not supported by implementation
 *
 * Attempt to write data to the IO address given by the given devhandle,
 * real address, and size.  Size must be 1, 2, 4, or 8.  The write is
 * performed as a single access operation using the given size. Prior to
 * writing the data is size based swapped.
 *
 * If an error occurs when writing to the given location, do not generate an
 * error report, but return a non-zero value in RET1.  If the write was
 * successful, return zero in RET1.
 *
 * pci_device describes the configuration address of the device being
 * written to.  The implementation may safely read from offset 0 with
 * the configuration space of the device described by devhandle and
 * pci_device in order to guarantee that the write portion of the operation
 * completes
 *
 * Any error that occurs due to the read shall be reported using the normal
 * error reporting mechanisms .. the read error is not suppressed.
 *
 * The caller must have permission to write to the given devhandle, real
 * address, which must be an IO address.  The argument real address must be a
 * size aligned address.  The caller must have permission to read from
 * the given devhandle, pci_device cofiguration space offset 0.
 *
 * The hypervisor implementation of this function must block access to any
 * IO address that the guest does not have explicit permission to access.
 */
#define HV_FAST_PCI_POKE		0xb7

/* pci_dma_sync()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_DMA_SYNC
 * ARG0:	devhandle
 * ARG1:	real address
 * ARG2:	size
 * ARG3:	io_sync_direction
 * RET0:	status
 * RET1:	#synced
 * ERRORS:	EINVAL		Invalid devhandle or io_sync_direction
 *		ENORADDR	Bad real address
 *
 * Synchronize a memory region described by the given real address and size,
 * for the device defined by the given devhandle using the direction(s)
 * defined by the given io_sync_direction.  The argument size is the size of
 * the memory region in bytes.
 *
 * Return the actual number of bytes synchronized in the return value #synced,
 * which may be less than or equal to the argument size.  If the return
 * value #synced is less than size, the caller must continue to call this
 * function with updated real address and size arguments until the entire
 * memory region is synchronized.
 */
#define HV_FAST_PCI_DMA_SYNC		0xb8

/* PCI MSI services.  */

#define HV_MSITYPE_MSI32		0x00
#define HV_MSITYPE_MSI64		0x01

#define HV_MSIQSTATE_IDLE		0x00
#define HV_MSIQSTATE_ERROR		0x01

#define HV_MSIQ_INVALID			0x00
#define HV_MSIQ_VALID			0x01

#define HV_MSISTATE_IDLE		0x00
#define HV_MSISTATE_DELIVERED		0x01

#define HV_MSIVALID_INVALID		0x00
#define HV_MSIVALID_VALID		0x01

#define HV_PCIE_MSGTYPE_PME_MSG		0x18
#define HV_PCIE_MSGTYPE_PME_ACK_MSG	0x1b
#define HV_PCIE_MSGTYPE_CORR_MSG	0x30
#define HV_PCIE_MSGTYPE_NONFATAL_MSG	0x31
#define HV_PCIE_MSGTYPE_FATAL_MSG	0x33

#define HV_MSG_INVALID			0x00
#define HV_MSG_VALID			0x01

/* pci_msiq_conf()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSIQ_CONF
 * ARG0:	devhandle
 * ARG1:	msiqid
 * ARG2:	real address
 * ARG3:	number of entries
 * RET0:	status
 * ERRORS:	EINVAL		Invalid devhandle, msiqid or nentries
 *		EBADALIGN	Improperly aligned real address
 *		ENORADDR	Bad real address
 *
 * Configure the MSI queue given by the devhandle and msiqid arguments,
 * and to be placed at the given real address and be of the given
 * number of entries.  The real address must be aligned exactly to match
 * the queue size.  Each queue entry is 64-bytes long, so f.e. a 32 entry
 * queue must be aligned on a 2048 byte real address boundary.  The MSI-EQ
 * Head and Tail are initialized so that the MSI-EQ is 'empty'.
 *
 * Implementation Note: Certain implementations have fixed sized queues.  In
 *                      that case, number of entries must contain the correct
 *                      value.
 */
#define HV_FAST_PCI_MSIQ_CONF		0xc0

/* pci_msiq_info()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSIQ_INFO
 * ARG0:	devhandle
 * ARG1:	msiqid
 * RET0:	status
 * RET1:	real address
 * RET2:	number of entries
 * ERRORS:	EINVAL		Invalid devhandle or msiqid
 *
 * Return the configuration information for the MSI queue described
 * by the given devhandle and msiqid.  The base address of the queue
 * is returned in ARG1 and the number of entries is returned in ARG2.
 * If the queue is unconfigured, the real address is undefined and the
 * number of entries will be returned as zero.
 */
#define HV_FAST_PCI_MSIQ_INFO		0xc1

/* pci_msiq_getvalid()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSIQ_GETVALID
 * ARG0:	devhandle
 * ARG1:	msiqid
 * RET0:	status
 * RET1:	msiqvalid	(HV_MSIQ_VALID or HV_MSIQ_INVALID)
 * ERRORS:	EINVAL		Invalid devhandle or msiqid
 *
 * Get the valid state of the MSI-EQ described by the given devhandle and
 * msiqid.
 */
#define HV_FAST_PCI_MSIQ_GETVALID	0xc2

/* pci_msiq_setvalid()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSIQ_SETVALID
 * ARG0:	devhandle
 * ARG1:	msiqid
 * ARG2:	msiqvalid	(HV_MSIQ_VALID or HV_MSIQ_INVALID)
 * RET0:	status
 * ERRORS:	EINVAL		Invalid devhandle or msiqid or msiqvalid
 *				value or MSI EQ is uninitialized
 *
 * Set the valid state of the MSI-EQ described by the given devhandle and
 * msiqid to the given msiqvalid.
 */
#define HV_FAST_PCI_MSIQ_SETVALID	0xc3

/* pci_msiq_getstate()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSIQ_GETSTATE
 * ARG0:	devhandle
 * ARG1:	msiqid
 * RET0:	status
 * RET1:	msiqstate	(HV_MSIQSTATE_IDLE or HV_MSIQSTATE_ERROR)
 * ERRORS:	EINVAL		Invalid devhandle or msiqid
 *
 * Get the state of the MSI-EQ described by the given devhandle and
 * msiqid.
 */
#define HV_FAST_PCI_MSIQ_GETSTATE	0xc4

/* pci_msiq_getvalid()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSIQ_GETVALID
 * ARG0:	devhandle
 * ARG1:	msiqid
 * ARG2:	msiqstate	(HV_MSIQSTATE_IDLE or HV_MSIQSTATE_ERROR)
 * RET0:	status
 * ERRORS:	EINVAL		Invalid devhandle or msiqid or msiqstate
 *				value or MSI EQ is uninitialized
 *
 * Set the state of the MSI-EQ described by the given devhandle and
 * msiqid to the given msiqvalid.
 */
#define HV_FAST_PCI_MSIQ_SETSTATE	0xc5

/* pci_msiq_gethead()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSIQ_GETHEAD
 * ARG0:	devhandle
 * ARG1:	msiqid
 * RET0:	status
 * RET1:	msiqhead
 * ERRORS:	EINVAL		Invalid devhandle or msiqid
 *
 * Get the current MSI EQ queue head for the MSI-EQ described by the
 * given devhandle and msiqid.
 */
#define HV_FAST_PCI_MSIQ_GETHEAD	0xc6

/* pci_msiq_sethead()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSIQ_SETHEAD
 * ARG0:	devhandle
 * ARG1:	msiqid
 * ARG2:	msiqhead
 * RET0:	status
 * ERRORS:	EINVAL		Invalid devhandle or msiqid or msiqhead,
 *				or MSI EQ is uninitialized
 *
 * Set the current MSI EQ queue head for the MSI-EQ described by the
 * given devhandle and msiqid.
 */
#define HV_FAST_PCI_MSIQ_SETHEAD	0xc7

/* pci_msiq_gettail()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSIQ_GETTAIL
 * ARG0:	devhandle
 * ARG1:	msiqid
 * RET0:	status
 * RET1:	msiqtail
 * ERRORS:	EINVAL		Invalid devhandle or msiqid
 *
 * Get the current MSI EQ queue tail for the MSI-EQ described by the
 * given devhandle and msiqid.
 */
#define HV_FAST_PCI_MSIQ_GETTAIL	0xc8

/* pci_msi_getvalid()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSI_GETVALID
 * ARG0:	devhandle
 * ARG1:	msinum
 * RET0:	status
 * RET1:	msivalidstate
 * ERRORS:	EINVAL		Invalid devhandle or msinum
 *
 * Get the current valid/enabled state for the MSI defined by the
 * given devhandle and msinum.
 */
#define HV_FAST_PCI_MSI_GETVALID	0xc9

/* pci_msi_setvalid()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSI_SETVALID
 * ARG0:	devhandle
 * ARG1:	msinum
 * ARG2:	msivalidstate
 * RET0:	status
 * ERRORS:	EINVAL		Invalid devhandle or msinum or msivalidstate
 *
 * Set the current valid/enabled state for the MSI defined by the
 * given devhandle and msinum.
 */
#define HV_FAST_PCI_MSI_SETVALID	0xca

/* pci_msi_getmsiq()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSI_GETMSIQ
 * ARG0:	devhandle
 * ARG1:	msinum
 * RET0:	status
 * RET1:	msiqid
 * ERRORS:	EINVAL		Invalid devhandle or msinum or MSI is unbound
 *
 * Get the MSI EQ that the MSI defined by the given devhandle and
 * msinum is bound to.
 */
#define HV_FAST_PCI_MSI_GETMSIQ		0xcb

/* pci_msi_setmsiq()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSI_SETMSIQ
 * ARG0:	devhandle
 * ARG1:	msinum
 * ARG2:	msitype
 * ARG3:	msiqid
 * RET0:	status
 * ERRORS:	EINVAL		Invalid devhandle or msinum or msiqid
 *
 * Set the MSI EQ that the MSI defined by the given devhandle and
 * msinum is bound to.
 */
#define HV_FAST_PCI_MSI_SETMSIQ		0xcc

/* pci_msi_getstate()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSI_GETSTATE
 * ARG0:	devhandle
 * ARG1:	msinum
 * RET0:	status
 * RET1:	msistate
 * ERRORS:	EINVAL		Invalid devhandle or msinum
 *
 * Get the state of the MSI defined by the given devhandle and msinum.
 * If not initialized, return HV_MSISTATE_IDLE.
 */
#define HV_FAST_PCI_MSI_GETSTATE	0xcd

/* pci_msi_setstate()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSI_SETSTATE
 * ARG0:	devhandle
 * ARG1:	msinum
 * ARG2:	msistate
 * RET0:	status
 * ERRORS:	EINVAL		Invalid devhandle or msinum or msistate
 *
 * Set the state of the MSI defined by the given devhandle and msinum.
 */
#define HV_FAST_PCI_MSI_SETSTATE	0xce

/* pci_msg_getmsiq()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSG_GETMSIQ
 * ARG0:	devhandle
 * ARG1:	msgtype
 * RET0:	status
 * RET1:	msiqid
 * ERRORS:	EINVAL		Invalid devhandle or msgtype
 *
 * Get the MSI EQ of the MSG defined by the given devhandle and msgtype.
 */
#define HV_FAST_PCI_MSG_GETMSIQ		0xd0

/* pci_msg_setmsiq()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSG_SETMSIQ
 * ARG0:	devhandle
 * ARG1:	msgtype
 * ARG2:	msiqid
 * RET0:	status
 * ERRORS:	EINVAL		Invalid devhandle, msgtype, or msiqid
 *
 * Set the MSI EQ of the MSG defined by the given devhandle and msgtype.
 */
#define HV_FAST_PCI_MSG_SETMSIQ		0xd1

/* pci_msg_getvalid()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSG_GETVALID
 * ARG0:	devhandle
 * ARG1:	msgtype
 * RET0:	status
 * RET1:	msgvalidstate
 * ERRORS:	EINVAL		Invalid devhandle or msgtype
 *
 * Get the valid/enabled state of the MSG defined by the given
 * devhandle and msgtype.
 */
#define HV_FAST_PCI_MSG_GETVALID	0xd2

/* pci_msg_setvalid()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_PCI_MSG_SETVALID
 * ARG0:	devhandle
 * ARG1:	msgtype
 * ARG2:	msgvalidstate
 * RET0:	status
 * ERRORS:	EINVAL		Invalid devhandle or msgtype or msgvalidstate
 *
 * Set the valid/enabled state of the MSG defined by the given
 * devhandle and msgtype.
 */
#define HV_FAST_PCI_MSG_SETVALID	0xd3

/* Performance counter services.  */

#define HV_PERF_JBUS_PERF_CTRL_REG	0x00
#define HV_PERF_JBUS_PERF_CNT_REG	0x01
#define HV_PERF_DRAM_PERF_CTRL_REG_0	0x02
#define HV_PERF_DRAM_PERF_CNT_REG_0	0x03
#define HV_PERF_DRAM_PERF_CTRL_REG_1	0x04
#define HV_PERF_DRAM_PERF_CNT_REG_1	0x05
#define HV_PERF_DRAM_PERF_CTRL_REG_2	0x06
#define HV_PERF_DRAM_PERF_CNT_REG_2	0x07
#define HV_PERF_DRAM_PERF_CTRL_REG_3	0x08
#define HV_PERF_DRAM_PERF_CNT_REG_3	0x09

/* get_perfreg()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_GET_PERFREG
 * ARG0:	performance reg number
 * RET0:	status
 * RET1:	performance reg value
 * ERRORS:	EINVAL		Invalid performance register number
 *		ENOACCESS	No access allowed to performance counters
 *
 * Read the value of the given DRAM/JBUS performance counter/control register.
 */
#define HV_FAST_GET_PERFREG		0x100

/* set_perfreg()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_SET_PERFREG
 * ARG0:	performance reg number
 * ARG1:	performance reg value
 * RET0:	status
 * ERRORS:	EINVAL		Invalid performance register number
 *		ENOACCESS	No access allowed to performance counters
 *
 * Write the given performance reg value to the given DRAM/JBUS
 * performance counter/control register.
 */
#define HV_FAST_SET_PERFREG		0x101

/* MMU statistics services.
 *
 * The hypervisor maintains MMU statistics and privileged code provides
 * a buffer where these statistics can be collected.  It is continually
 * updated once configured.  The layout is as follows:
 */
#ifndef __ASSEMBLY__
struct hv_mmu_statistics {
	unsigned long immu_tsb_hits_ctx0_8k_tte;
	unsigned long immu_tsb_ticks_ctx0_8k_tte;
	unsigned long immu_tsb_hits_ctx0_64k_tte;
	unsigned long immu_tsb_ticks_ctx0_64k_tte;
	unsigned long __reserved1[2];
	unsigned long immu_tsb_hits_ctx0_4mb_tte;
	unsigned long immu_tsb_ticks_ctx0_4mb_tte;
	unsigned long __reserved2[2];
	unsigned long immu_tsb_hits_ctx0_256mb_tte;
	unsigned long immu_tsb_ticks_ctx0_256mb_tte;
	unsigned long __reserved3[4];
	unsigned long immu_tsb_hits_ctxnon0_8k_tte;
	unsigned long immu_tsb_ticks_ctxnon0_8k_tte;
	unsigned long immu_tsb_hits_ctxnon0_64k_tte;
	unsigned long immu_tsb_ticks_ctxnon0_64k_tte;
	unsigned long __reserved4[2];
	unsigned long immu_tsb_hits_ctxnon0_4mb_tte;
	unsigned long immu_tsb_ticks_ctxnon0_4mb_tte;
	unsigned long __reserved5[2];
	unsigned long immu_tsb_hits_ctxnon0_256mb_tte;
	unsigned long immu_tsb_ticks_ctxnon0_256mb_tte;
	unsigned long __reserved6[4];
	unsigned long dmmu_tsb_hits_ctx0_8k_tte;
	unsigned long dmmu_tsb_ticks_ctx0_8k_tte;
	unsigned long dmmu_tsb_hits_ctx0_64k_tte;
	unsigned long dmmu_tsb_ticks_ctx0_64k_tte;
	unsigned long __reserved7[2];
	unsigned long dmmu_tsb_hits_ctx0_4mb_tte;
	unsigned long dmmu_tsb_ticks_ctx0_4mb_tte;
	unsigned long __reserved8[2];
	unsigned long dmmu_tsb_hits_ctx0_256mb_tte;
	unsigned long dmmu_tsb_ticks_ctx0_256mb_tte;
	unsigned long __reserved9[4];
	unsigned long dmmu_tsb_hits_ctxnon0_8k_tte;
	unsigned long dmmu_tsb_ticks_ctxnon0_8k_tte;
	unsigned long dmmu_tsb_hits_ctxnon0_64k_tte;
	unsigned long dmmu_tsb_ticks_ctxnon0_64k_tte;
	unsigned long __reserved10[2];
	unsigned long dmmu_tsb_hits_ctxnon0_4mb_tte;
	unsigned long dmmu_tsb_ticks_ctxnon0_4mb_tte;
	unsigned long __reserved11[2];
	unsigned long dmmu_tsb_hits_ctxnon0_256mb_tte;
	unsigned long dmmu_tsb_ticks_ctxnon0_256mb_tte;
	unsigned long __reserved12[4];
};
#endif

/* mmustat_conf()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMUSTAT_CONF
 * ARG0:	real address
 * RET0:	status
 * RET1:	real address
 * ERRORS:	ENORADDR	Invalid real address
 *		EBADALIGN	Real address not aligned on 64-byte boundary
 *		EBADTRAP	API not supported on this processor
 *
 * Enable MMU statistic gathering using the buffer at the given real
 * address on the current virtual CPU.  The new buffer real address
 * is given in ARG1, and the previously specified buffer real address
 * is returned in RET1, or is returned as zero for the first invocation.
 *
 * If the passed in real address argument is zero, this will disable
 * MMU statistic collection on the current virtual CPU.  If an error is
 * returned then no statistics are collected.
 *
 * The buffer contents should be initialized to all zeros before being
 * given to the hypervisor or else the statistics will be meaningless.
 */
#define HV_FAST_MMUSTAT_CONF		0x102

/* mmustat_info()
 * TRAP:	HV_FAST_TRAP
 * FUNCTION:	HV_FAST_MMUSTAT_INFO
 * RET0:	status
 * RET1:	real address
 * ERRORS:	EBADTRAP	API not supported on this processor
 *
 * Return the current state and real address of the currently configured
 * MMU statistics buffer on the current virtual CPU.
 */
#define HV_FAST_MMUSTAT_INFO		0x103

/* Function numbers for HV_CORE_TRAP.  */
#define HV_CORE_VER			0x00
#define HV_CORE_PUTCHAR			0x01
#define HV_CORE_EXIT			0x02

#endif /* !(_SPARC64_HYPERVISOR_H) */
