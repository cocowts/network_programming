#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#include "response_task.h"
#include "service_task.h"

int main(void)
{
    int ret;
    pthread_t tid1;
    pthread_t tid2;

    ret = pthread_create(&tid1, NULL, Response_Task, NULL);
    if (ret != 0) {
        printf("[dt4sw] Failed to create response task!\n");
    }

    ret = pthread_create(&tid2, NULL, Service_Task, NULL);
    if (ret != 0) {
        printf("[dt4sw] Failed to create service task!\n");
    }

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);

    return 0;
}