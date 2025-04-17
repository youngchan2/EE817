// Sleeping locks

#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"

void initsleeplock(struct sleeplock *lk, char *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
  lk->head = 0;
}

void acquiresleep(struct sleeplock *lk)
{
  // acquire(&lk->lk);
  // while (lk->locked)
  // {
  //   sleep(lk, &lk->lk);
  // }
  // lk->locked = 1;
  // lk->pid = myproc()->pid;
  // release(&lk->lk);

  struct proc *cur = myproc();

  acquire(&lk->lk);

  if (!lk->head)
  {
    lk->head = cur;
  }
  else
  {
    struct proc *p = lk->head;
    while (p->next)
    {
      p = p->next;
    }
    p->next = cur;
  }

  while (lk->locked || lk->head != cur)
  {
    sleep(cur, &lk->lk);
  }

  lk->head = cur->next;
  cur->next = 0;

  lk->locked = 1;
  lk->pid = cur->pid;
  release(&lk->lk);
}

void releasesleep(struct sleeplock *lk)
{
  // acquire(&lk->lk);
  // lk->locked = 0;
  // lk->pid = 0;
  // wakeup(lk);
  // release(&lk->lk);

  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  if (lk->head)
  {
    wakeup(lk->head);
  }
  release(&lk->lk);
}

int holdingsleep(struct sleeplock *lk)
{
  int r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  return r;
}
