#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "semaphore.h"

void initsema(struct semaphore *s, int count)
{
    initlock(&s->lk, "semaphore");
    s->count = count;
}

int downsema(struct semaphore *s)
{
    acquire(&s->lk);
    while (s->count <= 0)
    {
        sleep(s, &s->lk);
    }
    s->count--;
    int ret_sema = s->count;
    release(&s->lk);
    return ret_sema;
}

int upsema(struct semaphore *s)
{
    acquire(&s->lk);
    s->count++;
    int ret_sema = s->count;
    wakeup(s);
    release(&s->lk);
    return ret_sema;
}
