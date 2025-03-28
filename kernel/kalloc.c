// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.
// TODO: reread the freelist related pagetable part
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct {
    struct spinlock lock;
    int refcnt_arr[(PHYSTOP - KERNBASE) / PGSIZE];
} refcnt;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&refcnt.lock, "refcnt");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
      refcnt_set((uint64)p, 1);
      kfree(p);
  }

}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  if(refcnt_add((uint64)pa, -1) > 0) {
      return;
  }

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);

}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk

  if(r)
    refcnt_set((uint64)r, 1);


  return (void*)r;
}

int refcnt_add(uint64 va, int add) {
    int pg_idx = (va - KERNBASE) / PGSIZE;
    acquire(&refcnt.lock);
    int res = refcnt.refcnt_arr[pg_idx];
    res += add;
    refcnt.refcnt_arr[pg_idx] = res;
    release(&refcnt.lock);
    return res;
}

void refcnt_set(uint64 va, int ref) {
    int pg_idx = (va - KERNBASE) / PGSIZE;
    acquire(&refcnt.lock);
    refcnt.refcnt_arr[pg_idx] = ref;
    release(&refcnt.lock);
}
