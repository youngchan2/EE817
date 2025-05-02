#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "rwsemaphore.h"

static void enqueue_reader(struct rwsemaphore *s, struct proc *p)
{
    p->next = 0;
    if (s->reader_tail)
    {
        s->reader_tail->next = p;
    }
    else
    {
        s->reader_head = p;
    }
    s->reader_tail = p;
}

static struct proc *dequeue_reader(struct rwsemaphore *s)
{
    struct proc *p = s->reader_head;
    if (p)
    {
        s->reader_head = p->next;
        if (!s->reader_head)
        {
            s->reader_tail = 0;
        }
        p->next = 0;
    }
    return p;
}

static void enqueue_writer(struct rwsemaphore *s, struct proc *p)
{
    p->next = 0;
    if (s->writer_tail)
    {
        s->writer_tail->next = p;
    }
    else
    {
        s->writer_head = p;
    }
    s->writer_tail = p;
}

static struct proc *dequeue_writer(struct rwsemaphore *s)
{
    struct proc *p = s->writer_head;
    if (p)
    {
        s->writer_head = p->next;
        if (!s->writer_head)
        {
            s->writer_tail = 0;
        }
        p->next = 0;
    }
    return p;
}

void initrwsema(struct rwsemaphore *s)
{
    initlock(&s->lk, "rwsema");
    s->readers = 0;
    s->writer_wait = 0;
    s->write_active = 0;
    s->reader_head = 0;
    s->reader_tail = 0;
    s->writer_head = 0;
    s->writer_tail = 0;
}

void downreadsema(struct rwsemaphore *s)
{
    acquire(&s->lk);
    while (s->write_active || s->writer_wait > 0)
    {
        struct proc *p = myproc();
        enqueue_reader(s, p);
        sleep(p, &s->lk);
    }
    s->readers++;
    release(&s->lk);
}

void upreadsema(struct rwsemaphore *s)
{
    acquire(&s->lk);
    s->readers--;
    if (s->readers == 0 && s->writer_wait > 0)
    {
        struct proc *p = dequeue_writer(s);
        if (p)
            wakeup(p);
    }
    release(&s->lk);
}

void downwritesema(struct rwsemaphore *s)
{
    acquire(&s->lk);
    s->writer_wait++;
    while (s->readers > 0 || s->write_active)
    {
        struct proc *p = myproc();
        enqueue_writer(s, p);
        sleep(p, &s->lk);
    }
    s->writer_wait--;
    s->write_active = 1;
    release(&s->lk);
}

void upwritesema(struct rwsemaphore *s)
{
    acquire(&s->lk);
    s->write_active = 0;
    if (s->writer_head)
    {
        struct proc *p = dequeue_writer(s);
        wakeup(p);
    }
    else
    {
        struct proc *p;
        while ((p = dequeue_reader(s)))
        {
            wakeup(p);
        }
    }
    release(&s->lk);
}
