# xv6 Demand Paging Extension

This project extends the MIT xv6 operating system with **demand paging** support. The modification implements lazy allocation of memory pages and updates various kernel components to support page faults and virtual memory management.

## 📚 Features Implemented

### 🔁 1. Lazy Memory Allocation (Demand Paging)
- Memory pages are not allocated immediately on process creation or memory growth.
- Instead, **pages are allocated on demand** when a page fault occurs due to accessing an unmapped virtual address.

### ⚙️ 2. Page Fault Handling in `trap.c`
- The kernel now handles `scause` codes 13 and 15 (load/store page faults).
- On fault, if the page is marked as lazily mapped (`PTE_LAZY`), a new physical page is allocated and mapped to the faulting virtual address.
- If the address is not lazy-mapped or allocation fails, the process is marked as killed.

### 🧠 3. New Page Table Flag: `PTE_LAZY`
- A new flag `PTE_LAZY` is added to indicate pages that should be lazily allocated on first access.

### 📦 4. Modified `uvmcopy` → `uvmcopy_lazy`
- During `fork`, instead of eagerly copying all memory pages, only the page table entries are copied with `PTE_LAZY` set.
- Physical memory is shared until a page is accessed, at which point it's allocated privately.

### 🧩 5. Updated `mappages` in `vm.c`
- Validates inputs and sets appropriate flags (including `PTE_LAZY` when needed).
- Now supports mapping virtual addresses to physical addresses with lazy allocation logic.

## 📁 Modified Files

- `kernel/vm.c`: Added support for lazy mappings in `mappages`.
- `kernel/trap.c`: Added page fault handler for demand paging.
- `kernel/proc.c`: Modified `fork` to use `uvmcopy_lazy`.
- `kernel/riscv.h`: Added `PTE_LAZY` flag for page table entries.
- `kernel/defs.h`: Declared `uvmcopy_lazy`.

## 💻 How to Run

1. Build the xv6 kernel:
   ```bash
   make qemu
