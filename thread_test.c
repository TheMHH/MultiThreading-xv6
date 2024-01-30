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

void test1()
{
    printf(1, "test 1 started\n");

    int arr[] = {5, 4, 3, 2, 1};
    int pid = thread_create(&test1_func, (void *) arr, (void *) 0);

    thread_join(pid);
    for (int i = 0; i < 5; i++)
    {
        if (test1_arr[i] != 6)
        {
            printf(1, "test 1 failed\n");
            return;
        }
    }
    printf(1, "test 1 passed\n");
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

void test2()
{
    printf(1, "test 2 started\n");

    int pid1 = thread_create(&test2_func, (void *)"Thread1", (void *) 0);
    int pid2 = thread_create(&test2_func, (void *)"Thread2", (void *) 0);
    thread_join(pid2);
    thread_join(pid1);

    if (thread_join(pid1) != -1 || thread_join(pid2) != -1)
    {
        printf(1, "test 2 failed\n");
        return;
    }
    printf(1, "test 2 passed\n");

}

int main(int argc, char *argv[]) {

    // test 1
    // test create thread 
    // change value and test
    test1();


    // test 2
    // test join
    // join twice a thread and check
    test2();

    // test 3
    // test lock
    // increament a number in two threads and check for final value
    // test3();

    // test 4
    // check fork and thread
    // fork a process and create 10 thread in one the fork shouldnt end after all threads
    // test4();

    exit();
}
