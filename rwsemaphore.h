struct rwsemaphore
{
    int readers;
    int writer_wait;
    int write_active;
    struct spinlock lk;

    struct proc *reader_head;
    struct proc *reader_tail;
    struct proc *writer_head;
    struct proc *writer_tail;
};