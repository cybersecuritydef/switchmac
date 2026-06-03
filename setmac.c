#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <net/if.h>
#include <sys/unistd.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>

#include "setmac.h"

static uint randoms(void){
    return (1 + rand() % 255);
}

static bool is_device_up(int sock, char *dev){
    struct ifreq device;
    if(dev != NULL && sock >= 0){
        memset(&device, '\0', sizeof(struct ifreq));
        snprintf(device.ifr_name, IFNAMSIZ, "%s", dev);
        if(ioctl(sock, SIOCGIFFLAGS, &device) != -1){
            if(device.ifr_flags & IFF_UP)
                return true;             
        }  
    }
    return false;
}

static int device_toggle(int sock, char *dev){    
    struct ifreq device;
    if(dev != NULL && sock >= 0){
        memset(&device, '\0', sizeof(struct ifreq));
        snprintf(device.ifr_name, IFNAMSIZ, "%s", dev);
        if(ioctl(sock, SIOCGIFFLAGS, &device) != -1){
            if(device.ifr_flags & IFF_UP)
                device.ifr_flags &= ~IFF_UP;
            else{
                device.ifr_flags |= IFF_UP;
                device.ifr_flags |= IFF_RUNNING; 
            }                
            if(ioctl(sock, SIOCSIFFLAGS, &device) != -1)            
                return EXIT_SUCCESS;
        }  
    }               
    return -1;
}

int setmac(char *mac, char *dev){
    int sock = 0;
    int i = 0;
    bool state_up = false;
    struct ifreq devmac;
    unsigned int tmp_mac[6];
    if(mac != NULL && dev != NULL){
        if(sscanf(mac, "%x:%x:%x:%x:%x:%x", &tmp_mac[0], &tmp_mac[1], &tmp_mac[2], &tmp_mac[3], &tmp_mac[4], &tmp_mac[5]) != 6)
            return -1;  
        if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) != -1){
            memset(&devmac, '\0', sizeof(struct ifreq));
            snprintf(devmac.ifr_name, IFNAMSIZ, "%s", dev);
            devmac.ifr_hwaddr.sa_family = ARPHRD_ETHER;        
            for(i = 0; i < 6; i++) 
                devmac.ifr_hwaddr.sa_data[i] = (char)(tmp_mac[i] & 0xFF);
            state_up = is_device_up(sock, dev);
            if(state_up)
                device_toggle(sock, dev);
            if(ioctl(sock, SIOCSIFHWADDR, &devmac) != -1){
                if(state_up)
                    device_toggle(sock, dev);
                close(sock);
                return EXIT_SUCCESS;
            }
            else{
                if(state_up)
                    device_toggle(sock, dev);
            }                
            close(sock);
        }
    }
    return -1;
}

int setmac_rand(char *dev){
    int sock = 0;
    int index = 0;
    bool state_up = false;
    struct ifreq devmac;
    srand(time(NULL));
    if(dev != NULL){
        if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) != -1){
            memset(&devmac, '\0', sizeof(struct ifreq));
            snprintf(devmac.ifr_name, IFNAMSIZ, "%s", dev);
            devmac.ifr_hwaddr.sa_family = ARPHRD_ETHER;
            for(index = 0; index < 6; ++index){
                if(index == 0)
                    devmac.ifr_hwaddr.sa_data[0] = (randoms() & 0xFC) | 0x02;
                else
                    devmac.ifr_hwaddr.sa_data[index] = randoms();
            }
            state_up = is_device_up(sock, dev);
            if(state_up)
                device_toggle(sock, dev);
            if(ioctl(sock, SIOCSIFHWADDR, &devmac) != -1){
                if(state_up)
                    device_toggle(sock, dev);
                close(sock);
                return EXIT_SUCCESS;
            }
            else{
                if(state_up)
                    device_toggle(sock, dev);
            }                
            close(sock);
        }   
    }    
    return -1;
}
