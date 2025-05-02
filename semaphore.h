struct semaphore
{
    int count;
    struct spinlock lk;
};