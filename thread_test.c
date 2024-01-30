#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "user.h"
#include "stat.h"
#include "fcntl.h"
#include "x86.h"


// test1 data
int test1_arr[] = {1, 2, 3, 4, 5};

void test1_func(void *arg1, void *arg2)
{
    int *arr = (int *) arg1;
    sleep(30);
    for (int i = 0; i < 5; i++)
    {
        test1_arr[i] += arr[i];
    }
    exit();
}

int test1()
{
    printf(1, "\n\ntest 1 started\n");

    int arr[] = {5, 4, 3, 2, 1};
    int pid = thread_create(&test1_func, (void *) arr, (void *) 0);

    thread_join(pid);
    for (int i = 0; i < 5; i++)
    {
        if (test1_arr[i] != 6)
        {
            printf(1, "test 1 failed\n");
            return 0;
        }
    }
    printf(1, "test 1 passed\n");
    return 1;
}


// test2 data

void test2_func(void *arg1, void *arg2)
{
    char *thread_name = (char *)arg1;
    int cnt = 0;
    for (int i = 0; i < 1000; i++) // busy waiting
    {
        cnt++;
    }    
    printf(1, "%s finished\n", thread_name);
    exit();
}

int test2()
{
    printf(1, "\n\ntest 2 started\n");

    int pid1 = thread_create(&test2_func, (void *)"Thread1", (void *) 0);
    int pid2 = thread_create(&test2_func, (void *)"Thread2", (void *) 0);
    thread_join(pid2);
    thread_join(pid1);

    if (thread_join(pid1) != -1 || thread_join(pid2) != -1)
    {
        printf(1, "test 2 failed\n");
        return 0;
    }
    printf(1, "test 2 passed\n");
    return 1;
}



// test3 data

lock_t test3_lock;
int test3_counter;

void test3_func(void *arg1, void *arg2)
{
    printf(1, "runnig Thread %d \n", *(int *) arg1);
    for (int i = 0; i < 1000; i++)
    {
        lock_acquire(&test3_lock);
        test3_counter++;
        // printf(1, "\ncounter is %d\n", counter);
        lock_release(&test3_lock);
    }
    exit();
}

int test3()
{
    printf(1, "\n\ntest 3 started\n");
    lock_init(&test3_lock); 
    test3_counter = 0;
    int t1 = 1;
    int t2 = 2;
    int pid1 = thread_create(&test3_func, &t1, &t1);
    int pid2 = thread_create(&test3_func, &t2, &t2);

    thread_join(pid1);
    thread_join(pid2);

    if (test3_counter != 2000)
    {
        printf(1, "test 3 failed\n");
        return 0;
    }

    printf(1, "test 3 passed\n");
    return 1;
}


// test4 data
int test4_cnt = 0;
int test4_thread_cnts[] = {0, 0, 0, 0, 0};

void test4_func(void *arg1, void *arg2)
{
    int thread_id = *(int *) arg1;
    for (int i = 0; i < 10000; i++)
    {
        test4_thread_cnts[thread_id]++;
    }
    exit();
}

int test4()
{
    printf(1, "\n\ntest 4 started\n");
    int pid1 = fork();
    if (pid1 == 0)
    {
        for (int i = 0; i < 10000; i++)
        {
            test4_cnt++;
        }
        exit();
    }

    int thread_pid[] = {0, 0, 0, 0, 0};
    int thread_no[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++)
    {
        thread_pid[i] = thread_create(&test4_func, (void *)(thread_no + i), (void *) 0);
    }
    if (wait() == -1)
    {
        printf(1, "fork failed");
        return 0;
    }
    int cnt = 0;
    for (int i = 0; i < 5; i++)
    {
        if (test4_thread_cnts[i] == 10000)
            cnt++;
    }
    for (int i = 0; i < 5; i++)
    {
        thread_join(thread_pid[i]);
    }
    if (cnt > 1)
    {
        printf(1, "scheduler is not fair\n");
        return 0;
    }
    printf(1, "test 4 passed\n");
    return 1;
}

int main(int argc, char *argv[]) {

    int stat = 1;

    // test 1
    // test create thread 
    // change value and test
    stat = stat & test1();


    // test 2
    // test join
    // join twice a thread and check
    stat = stat & test2();


    // test 3
    // test lock
    // increament a number in two threads and check for final value
    stat = stat & test3();

    // test 4
    // check fork and thread
    // fork a process and create 10 thread in one the fork shouldnt end after all threads
    stat = stat & test4();


    if (stat == 0)
    {
        printf(1, "\ntest failed\n");
        exit();
    }
    printf(1, "\nAll Tests passed successfully\n");
    exit();
}
