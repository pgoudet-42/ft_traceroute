#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <math.h>
#include "../libft/libft.h"

#ifndef FT_TRACEROUTE
#define FT_TRACEROUTE
#define DATA "0123456789ABCDEFGHIJKLMNOPQRSTUV"
#define DATA_LEN 32
# define __unused				__attribute__((unused))
# define __noreturn				__attribute__((noreturn))


struct reply_info {
    ssize_t         len_received;
    char            *addr;
    unsigned char   seq_num;
    unsigned char   ttl;
    float           time;
};

struct delay
{
    struct timeval  start;
    struct timeval  end;
    float           delay;
};

struct globglob {
    int                 fd;
    char                *destination;
    struct sockaddr_in  info_addr;
    struct delay        probe_delay;
    unsigned char       failure_rate;
    unsigned short int  nb_received;
    unsigned short int  nb_failure;
    unsigned char       reached;
    unsigned char       max_hops;
    unsigned char       hops;
    unsigned int        packet_size;
};

char        *malloc_safe(ssize_t len, unsigned char size);
char        *toBinary(int n, int len);
void        help_option();
int         array_string_contains(char *str, char **array);
int         error_exit(char *str, int exit_code);
char        *create_buff(void *datas, ssize_t size, char *str, ssize_t len);
void        init_hints(struct addrinfo *hints);
void        signal_interrupt(int sig);
char        *getaddr();
void        check_dest(int argc, char **argv);
float       calculate_time(struct timeval start, struct timeval end);
void        set_msghdr(struct msghdr *msg_received, char *buff1, char *buff2);
char        *set_info_addr(struct sockaddr_in *sock_addr, char *address);
void        set_globglob(char *address);

uint16_t checksum(char *data, ssize_t size);

int     connect_socket(unsigned char ttl);
void    send_req(unsigned char ttl);
void    received_request(unsigned char ttl, int probe_nb);

void    print_results(struct reply_info intel, unsigned char ttl, int probe_nb);
void    print_hexa_buffer(char *buff, int len);
void    print_hexa_buffer_received(char *buff, int len);
void    print_start();
#endif