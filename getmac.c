#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <net/if.h>
#include <sys/unistd.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>

int getmac(char *mac_addr, size_t size_buf, char *dev){
    int sock;
    struct ifreq mac;
    if(dev != NULL && mac_addr != NULL && size_buf > 0){
        if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) != -1){
            snprintf(mac.ifr_name, IFNAMSIZ, "%s", dev);
            if(ioctl(sock, SIOCGIFHWADDR, &mac) != -1){
                snprintf(mac_addr, size_buf, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", (u_char)mac.ifr_hwaddr.sa_data[0] & 0xFF,
                                                                            (u_char)mac.ifr_hwaddr.sa_data[1] & 0xFF,
                                                                            (u_char)mac.ifr_hwaddr.sa_data[2] & 0xFF,
                                                                            (u_char)mac.ifr_hwaddr.sa_data[3] & 0xFF,
                                                                            (u_char)mac.ifr_hwaddr.sa_data[4] & 0xFF,
                                                                            (u_char)mac.ifr_hwaddr.sa_data[5] & 0xFF);
                close(sock);
                return EXIT_SUCCESS;
            }
            else
                close(sock);
        }
    }    
    return -1;
}
