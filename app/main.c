#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


#define CDEV_BUF_MAX    256
#define WLAN_SET_LEN    8
#define APP_NAME        "[APP/fasync] "

#define FASYNC_MSG_1    "3003"      // CUST_CMD_CONNECT_IND
#define FASYNC_MSG_2    "3017"      // CUST_CMD_START_AP_IND
#define FASYNC_MSG_LEN  2

struct rwnx_fasync_info
{
    int mem_status;
    char mem[CDEV_BUF_MAX];
};

struct wlan_settings
{
    unsigned int ip;
    unsigned int gw;
};

unsigned int str2uint(char * const str)
{
    char ch; int i;
    unsigned int value = 0;
    for(i=WLAN_SET_LEN-1; i>=0; i--) {
        ch = *(str + i);
        if('0'<=ch && ch<='9') {
            ch = ch - '0';
        } else if ('A'<=ch && ch<='F') {
            ch = ch - 'A' + 10;
        } else if ('a'<=ch && ch<='f') {
            ch = ch - 'a' + 10;
        }
        value |= (ch & 0xff) << (4 * (WLAN_SET_LEN-1-i));
    }
    return value;
}

char *fasync_msg[FASYNC_MSG_LEN] = {
    FASYNC_MSG_1,
    FASYNC_MSG_2
};

void analy_signal_msg(char *mem)
{
    char buff[64];
    struct wlan_settings wlan;
    // auto wlan-settings
    for (int i=0; i<FASYNC_MSG_LEN; i++) {
        char *ptr = strstr(mem, fasync_msg[i]);
        if (!ptr)
            continue;
        switch (i) {
            case 0:
            case 1:
                // set vnet0 ip
                ptr = strstr(mem, "ip");
                memcpy(buff, ptr+4, WLAN_SET_LEN);
                wlan.ip = str2uint(buff);
                sprintf(buff, "ifconfig vnet0 %d.%d.%d.%d",
                        (unsigned int)((wlan.ip >> 0 )&0xFF), (unsigned int)((wlan.ip >> 8 )&0xFF),
                        (unsigned int)((wlan.ip >> 16)&0xFF), (unsigned int)((wlan.ip >> 24)&0xFF));
                system(buff);

                // set vnet0 gw
                memcpy(buff, ptr+18, WLAN_SET_LEN);
                wlan.gw = str2uint(buff);
                sprintf(buff, "route add default gw %d.%d.%d.%d",
                        (unsigned int)((wlan.gw >> 0 )&0xFF), (unsigned int)((wlan.gw >> 8 )&0xFF),
                        (unsigned int)((wlan.gw >> 16)&0xFF), (unsigned int)((wlan.gw >> 24)&0xFF));
                system(buff);
                system("ifconfig");
                system("route -n");
                break;
            default:
                printf("default setting.\n");
        }
    }
}

int fd;
static void signal_handler(int signum)
{
    printf("signal_handler\r\n");
    int ret = 0;
    char data_buf[sizeof(struct rwnx_fasync_info)];
    struct rwnx_fasync_info *fsy_info = (struct rwnx_fasync_info *)data_buf;
    ret = read(fd, data_buf, sizeof(struct rwnx_fasync_info));
    if(ret < 0) {
        printf(APP_NAME "Read kernel-data fail\n");
    } else {
        printf(APP_NAME "%s\n", fsy_info->mem);
        fsy_info->mem_status = 0;
        ret = write(fd, &fsy_info->mem_status, sizeof(fsy_info->mem_status));
        if (ret < 0)
            printf(APP_NAME "Write kernel-data fail\n");

        analy_signal_msg(fsy_info->mem);
    }
}


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

void print_boot_time_us_v2(void) {
    // 1. 获取开机到现在的时长（精确到纳秒，转微秒）
    struct timespec boot_diff_ts;
    if (clock_gettime(CLOCK_BOOTTIME, &boot_diff_ts) == -1) {
        perror("clock_gettime CLOCK_BOOTTIME failed");
        return;
    }
    long long boot_diff_us = (long long)boot_diff_ts.tv_sec * 1000000 + boot_diff_ts.tv_nsec / 1000;

    // 2. 获取当前时间（精确到微秒）
    struct timeval now_tv;
    gettimeofday(&now_tv, NULL);
    long long now_us = (long long)now_tv.tv_sec * 1000000 + now_tv.tv_usec;

    // 3. 计算开机时间（微秒级）
    long long boot_us = now_us - boot_diff_us;
    time_t boot_sec = boot_us / 1000000;
    long boot_us_remain = boot_us % 1000000;

    // 4. 格式化输出
    struct tm boot_tm;
    localtime_r(&boot_sec, &boot_tm);

    char boot_time_str[64];
    snprintf(boot_time_str, sizeof(boot_time_str),
             "%04d-%02d-%02d %02d:%02d:%02d.%06ld",
             boot_tm.tm_year + 1900,
             boot_tm.tm_mon + 1,
             boot_tm.tm_mday,
             boot_tm.tm_hour,
             boot_tm.tm_min,
             boot_tm.tm_sec,
             boot_us_remain);

    printf("系统开机时间（含微秒）：%s\n", boot_time_str);
    printf("开机至今时长：%lld 微秒（%.6f 秒）\n", boot_diff_us, (double)boot_diff_us/1000000);
}

int main(int argc, char *argv[])
{
    print_boot_time_us_v2();
    int flag;
    char *filename = "/dev/aic_mcdev";
    fd = open(filename, O_RDWR);
    if(fd < 0) {
        printf(APP_NAME "Can't open cdev-file %s\r\n", filename);
        return -1;
    }
    printf(APP_NAME "Open cdev: %s\r\n", filename);

    signal(SIGIO, signal_handler);

    fcntl(fd, F_SETOWN, getpid());
    flag = fcntl(fd, F_GETFD);
    fcntl(fd, F_SETFL, flag|FASYNC);

    while(1)
       sleep(2);
    return 0;
}

