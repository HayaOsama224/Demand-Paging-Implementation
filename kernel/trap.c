#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct spinlock tickslock;
uint ticks;

extern char trampoline[], uservec[], userret[];

// in kernelvec.S, calls kerneltrap().
void kernelvec();

extern int devintr();

void trapinit(void) {
  initlock(&tickslock, "time");
}

// set up to take exceptions and traps while in the kernel.
void trapinithart(void) {
  w_stvec((uint64)kernelvec);
}

void usertrap(void) {
  int which_dev = 0;

  if ((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();
  p->trapframe->epc = r_sepc();

  if (r_scause() == 8) {
    if (killed(p)) exit(-1);
    p->trapframe->epc += 4;
    intr_on();
    syscall();
  } else if ((r_scause() & 0xFFF) == 13 || (r_scause() & 0xFFF) == 15) {
    uint64 va = PGROUNDDOWN(r_stval());
    int loaded = 0;

    for (int i = 0; i < p->lazy_count; i++) {
      struct lazy_segment *seg = &p->lazy_segs[i];

      if (va >= seg->va_start && va < seg->va_start + seg->memsz) {
        char *mem = kalloc();
        if (mem == 0) {
          p->killed = 1;
          return;
        }

        memset(mem, 0, PGSIZE);

        uint offset_in_segment = va - seg->va_start;
        uint file_offset = seg->file_offset + offset_in_segment;
        uint to_read = PGSIZE;

        if (offset_in_segment < seg->filesz) {
          if (offset_in_segment + PGSIZE > seg->filesz)
            to_read = seg->filesz - offset_in_segment;

          ilock(seg->ip);
          if (readi(seg->ip, 0, (uint64)mem, file_offset, to_read) != to_read) {
            iunlock(seg->ip);
            kfree(mem);
            p->killed = 1;
            return;
          }
          iunlock(seg->ip);
        }

        if (mappages(p->pagetable, va, PGSIZE, (uint64)mem,
                     flags2perm(seg->flags) | PTE_U) != 0) {
          kfree(mem);
          p->killed = 1;
        }

        loaded = 1;
        break;
      }
    }

    if (!loaded) p->killed = 1;
  } else if ((which_dev = devintr()) != 0) {
    // handled by devintr
  } else {
    printf("usertrap(): unexpected scause 0x%lx pid=%d\n", r_scause(), p->pid);
    printf("            sepc=0x%lx stval=0x%lx\n", r_sepc(), r_stval());
    setkilled(p);
  }

  if (killed(p)) exit(-1);

  if (which_dev == 2) yield();

  usertrapret();
}

void usertrapret(void) {
  struct proc *p = myproc();
  intr_off();

  uint64 trampoline_uservec = TRAMPOLINE + (uservec - trampoline);
  w_stvec(trampoline_uservec);

  p->trapframe->kernel_satp = r_satp();
  p->trapframe->kernel_sp = p->kstack + PGSIZE;
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();

  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP;
  x |= SSTATUS_SPIE;
  w_sstatus(x);

  w_sepc(p->trapframe->epc);
  uint64 satp = MAKE_SATP(p->pagetable);

  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((void (*)(uint64))trampoline_userret)(satp);
}

void kerneltrap() {
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();

  if ((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if (intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if ((which_dev = devintr()) == 0) {
    printf("scause=0x%lx sepc=0x%lx stval=0x%lx\n", scause, r_sepc(), r_stval());
    panic("kerneltrap");
  }

  if (which_dev == 2 && myproc() != 0)
    yield();

  w_sepc(sepc);
  w_sstatus(sstatus);
}

void clockintr() {
  if (cpuid() == 0) {
    acquire(&tickslock);
    ticks++;
    wakeup(&ticks);
    release(&tickslock);
  }

  w_stimecmp(r_time() + 1000000);
}

int devintr() {
  uint64 scause = r_scause();

  if (scause == 0x8000000000000009L) {
    int irq = plic_claim();
    if (irq == UART0_IRQ) uartintr();
    else if (irq == VIRTIO0_IRQ) virtio_disk_intr();
    else if (irq) printf("unexpected interrupt irq=%d\n", irq);
    if (irq) plic_complete(irq);
    return 1;
  } else if (scause == 0x8000000000000005L) {
    clockintr();
    return 2;
  } else {
    return 0;
  }
}

