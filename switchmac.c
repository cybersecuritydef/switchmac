#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>

#include "getmac.h"
#include "setmac.h"

void help(void){
    printf("OPTIONS:\n\t");
    printf("-r\tRandom change mac address\n\t");
    printf("-g\tGet mac address\n\t");
    printf("-s\tSet mac address\n\t");
    printf("-h\tUsing help\n");
    printf("EXAMPLES:\n\t");
    printf("switchmac -r eth0\n\t");
    printf("switchmac -s 50:2a:14:af:b5:A4 eth0\n");
}

int main(int argc, char **argv){
    int opt = 0;
    opterr = false;
    char mode = '\0';
    char *mac = NULL;
    char *dev = NULL;
    char mac_addr[MAC_SIZE];
     while((opt = getopt(argc, argv, "rgs:h")) != -1){
        switch(opt){
            case 'r' :
            case 'g' :
                mode = opt;
                break;
            case 's' :
                mode = 's';
                mac = optarg;
                break;
            case 'h':
                help();
                return EXIT_SUCCESS;
            default :
                fprintf(stderr, "Invalid argument\n");
                return EXIT_FAILURE;
        }
    }
     if(optind < argc)
        dev = argv[optind];
    if (dev == NULL && mode != '\0') {
        fprintf(stderr, "Error: Network device interface not specified.\n");
        help();
        return EXIT_FAILURE;
    }
    switch(mode){
        case 'r':
            if(getmac(mac_addr, MAC_SIZE, dev) != -1 && setmac_rand(dev) != -1){
                printf("Current mac address %s: %s\n", dev, mac_addr);
                getmac(mac_addr, MAC_SIZE, dev);
                printf("New mac address     %s: %s\n", dev, mac_addr);
                return EXIT_SUCCESS;
            }
            else
                perror("Set random mac failed");
            return EXIT_FAILURE;
        case 's':
            if(getmac(mac_addr, MAC_SIZE, dev) != -1 && setmac(mac, dev) != -1){
                printf("Current mac address %s: %s\n", dev, mac_addr);
                printf("New mac address     %s: %s\n", dev, mac);
                return EXIT_SUCCESS;
            }
            else
                perror("Set static mac failed");
            return EXIT_FAILURE;
        case 'g':
            if(getmac(mac_addr, MAC_SIZE, dev) != -1) {
                printf("Current mac address %s: %s\n", dev, mac_addr);
                return EXIT_SUCCESS;
            }
            else
                perror("Get mac failed");
            return EXIT_FAILURE;
        default:
            help();
            break;
    }
    return 0;
}
