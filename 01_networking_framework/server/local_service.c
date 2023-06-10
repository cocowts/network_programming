#include "local_service.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

void Service_Init(void)
{
    // 硬件模块初始化
}

const char *Service_GetDesc(void)
{
    return "Environment Service";
}

const char *Service_GetUsage(void)
{
    return "Illumination: Ill_Get\n"
           "Temperature: Tem_Get\n"
           "Humidity: Hum_Get\n"
           "Light: Lig_Get Lig_Set_On Lig_Set_Off\n";
}

SvrData Service_GetData(void)
{
    // 硬件模块数据读取

    SvrData ret = {188, 0.33, 35.6, 1};

    return ret;
}

int Service_SetLight(int on)
{
    int ret = 1;

    // led 状态设置
    printf("set light %d\n", on);

    return ret;
}