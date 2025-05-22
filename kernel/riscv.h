#ifndef RISCV_H
#define RISCV_H

// Interrupt Enable Bits (must be visible even in assembly context)
#define SIE_SEIE (1L << 9) // Supervisor External Interrupt Enable
#define SIE_STIE (1L << 5) // Supervisor Timer Interrupt Enable
#define SIE_SSIE (1L << 1) // Supervisor Software Interrupt Enable
#define MIE_STIE (1L << 5) // Machine-mode Timer Interrupt Enable

#define SATP_SV39 (8L << 60)
#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)(pagetable)) >> 12))

#include "types.h"

#ifndef __ASSEMBLER__

// ----------------- Page Table Types -----------------
typedef uint64 pte_t;
typedef uint64 *pagetable_t; // 512 PTEs

// ----------------- Hart Identification -----------------
static inline uint64 r_mhartid() {
  uint64 x;
  asm volatile("csrr %0, mhartid" : "=r"(x));
  return x;
}

// ----------------- mstatus and sstatus Flags -----------------
#define MSTATUS_MPP_MASK (3L << 11)
#define MSTATUS_MPP_M     (3L << 11)
#define MSTATUS_MPP_S     (1L << 11)
#define MSTATUS_MPP_U     (0L << 11)
#define MSTATUS_MIE       (1L << 3)

#define SSTATUS_SPP       (1L << 8)
#define SSTATUS_SPIE      (1L << 5)
#define SSTATUS_UPIE      (1L << 4)
#define SSTATUS_SIE       (1L << 1)
#define SSTATUS_UIE       (1L << 0)

// ----------------- PTE Flags -----------------
#define PTE_V     (1L << 0)
#define PTE_R     (1L << 1)
#define PTE_W     (1L << 2)
#define PTE_X     (1L << 3)
#define PTE_U     (1L << 4)
#define PTE_LAZY  (1L << 10) // Lazy allocation flag

#if defined(LAB_MMAP) || defined(LAB_PGTBL)
#define PTE_LEAF(pte) (((pte)&PTE_R) | ((pte)&PTE_W) | ((pte)&PTE_X))
#endif

// ----------------- CSR Access -----------------
static inline uint64 r_mstatus()        { uint64 x; asm volatile("csrr %0, mstatus" : "=r"(x)); return x; }
static inline void   w_mstatus(uint64 x){ asm volatile("csrw mstatus, %0" : : "r"(x)); }

static inline void   w_mepc(uint64 x)   { asm volatile("csrw mepc, %0" : : "r"(x)); }
static inline uint64 r_sstatus()        { uint64 x; asm volatile("csrr %0, sstatus" : "=r"(x)); return x; }
static inline void   w_sstatus(uint64 x){ asm volatile("csrw sstatus, %0" : : "r"(x)); }

static inline uint64 r_sip()            { uint64 x; asm volatile("csrr %0, sip" : "=r"(x)); return x; }
static inline void   w_sip(uint64 x)    { asm volatile("csrw sip, %0" : : "r"(x)); }

static inline uint64 r_sie()            { uint64 x; asm volatile("csrr %0, sie" : "=r"(x)); return x; }
static inline void   w_sie(uint64 x)    { asm volatile("csrw sie, %0" : : "r"(x)); }

static inline uint64 r_mie()            { uint64 x; asm volatile("csrr %0, mie" : "=r"(x)); return x; }
static inline void   w_mie(uint64 x)    { asm volatile("csrw mie, %0" : : "r"(x)); }

static inline void   w_sepc(uint64 x)   { asm volatile("csrw sepc, %0" : : "r"(x)); }
static inline uint64 r_sepc()           { uint64 x; asm volatile("csrr %0, sepc" : "=r"(x)); return x; }

static inline uint64 r_medeleg()        { uint64 x; asm volatile("csrr %0, medeleg" : "=r"(x)); return x; }
static inline void   w_medeleg(uint64 x){ asm volatile("csrw medeleg, %0" : : "r"(x)); }

static inline uint64 r_mideleg()        { uint64 x; asm volatile("csrr %0, mideleg" : "=r"(x)); return x; }
static inline void   w_mideleg(uint64 x){ asm volatile("csrw mideleg, %0" : : "r"(x)); }

static inline void   w_stvec(uint64 x)  { asm volatile("csrw stvec, %0" : : "r"(x)); }
static inline uint64 r_stvec()          { uint64 x; asm volatile("csrr %0, stvec" : "=r"(x)); return x; }

static inline uint64 r_stimecmp()       { uint64 x; asm volatile("csrr %0, 0x14d" : "=r"(x)); return x; }
static inline void   w_stimecmp(uint64 x){ asm volatile("csrw 0x14d, %0" : : "r"(x)); }

static inline uint64 r_menvcfg()        { uint64 x; asm volatile("csrr %0, 0x30a" : "=r"(x)); return x; }
static inline void   w_menvcfg(uint64 x){ asm volatile("csrw 0x30a, %0" : : "r"(x)); }

static inline void   w_pmpcfg0(uint64 x){ asm volatile("csrw pmpcfg0, %0" : : "r"(x)); }
static inline void   w_pmpaddr0(uint64 x){ asm volatile("csrw pmpaddr0, %0" : : "r"(x)); }

static inline void   w_satp(uint64 x)   { asm volatile("csrw satp, %0" : : "r"(x)); }
static inline uint64 r_satp()           { uint64 x; asm volatile("csrr %0, satp" : "=r"(x)); return x; }

static inline uint64 r_scause()         { uint64 x; asm volatile("csrr %0, scause" : "=r"(x)); return x; }
static inline uint64 r_stval()          { uint64 x; asm volatile("csrr %0, stval" : "=r"(x)); return x; }

static inline void   w_mcounteren(uint64 x){ asm volatile("csrw mcounteren, %0" : : "r"(x)); }
static inline uint64 r_mcounteren()     { uint64 x; asm volatile("csrr %0, mcounteren" : "=r"(x)); return x; }

static inline uint64 r_time()           { uint64 x; asm volatile("csrr %0, time" : "=r"(x)); return x; }

// ----------------- Stack/Register Access -----------------
static inline uint64 r_sp() { uint64 x; asm volatile("mv %0, sp" : "=r"(x)); return x; }
static inline uint64 r_fp() { uint64 x; asm volatile("mv %0, s0" : "=r"(x)); return x; }
static inline uint64 r_tp() { uint64 x; asm volatile("mv %0, tp" : "=r"(x)); return x; }
static inline void   w_tp(uint64 x)     { asm volatile("mv tp, %0" : : "r"(x)); }
static inline uint64 r_ra()             { uint64 x; asm volatile("mv %0, ra" : "=r"(x)); return x; }

// ----------------- TLB -----------------
static inline void sfence_vma() {
  asm volatile("sfence.vma zero, zero");
}

// ----------------- Interrupt Utils -----------------
static inline void intr_on()  { w_sstatus(r_sstatus() | SSTATUS_SIE); }
static inline void intr_off() { w_sstatus(r_sstatus() & ~SSTATUS_SIE); }
static inline int  intr_get() { return (r_sstatus() & SSTATUS_SIE) != 0; }

#endif // __ASSEMBLER__

// ----------------- Paging Macros -----------------
#define PGSIZE     4096
#define PGSHIFT    12

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) ((a) & ~(PGSIZE-1))

#define PA2PTE(pa)     ((((uint64)(pa)) >> 12) << 10)
#define PTE2PA(pte)    (((pte) >> 10) << 12)
#define PTE_FLAGS(pte) ((pte) & 0x3FF)

#define PXMASK         0x1FF
#define PXSHIFT(level) (PGSHIFT + (9 * (level)))
#define PX(level, va)  ((((uint64)(va)) >> PXSHIFT(level)) & PXMASK)

#define MAXVA          (1L << (9 + 9 + 9 + 12 - 1))

#ifdef LAB_PGTBL
#define SUPERPGSIZE     (2 * (1 << 20)) // 2MB
#define SUPERPGROUNDUP(sz) (((sz) + SUPERPGSIZE - 1) & ~(SUPERPGSIZE - 1))
#endif

#endif // RISCV_H

