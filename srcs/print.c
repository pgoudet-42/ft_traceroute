#include "../includes/ft_traceroute.h"

extern struct globglob global_info;

void    print_hexa_buffer(char *buff, int len) {
    for (int i = 0; i < len; i++) {
        if (i == 20 || i == 28)
            printf("\n");
        printf("%hhx ", buff[i]);
    }
    printf("\n");
}

void    print_hexa_buffer_received(char *buff, int len) {
    for (int i = 0; i < len; i++) {
        if (i == 28 || i == 20)
            printf("\n");
        printf("%hhx ", buff[i]);
    }
    printf("\n");
}

void    print_replies(struct reply_info intel, unsigned char ttl, int probe_nb, char *addr) {
    static unsigned char validate;

    global_info.nb_received++;
    if (probe_nb == 0) {
        validate = 0;
        printf("\n%hhi ", ttl);
    }

    if (errno == 11) {
        printf(" *");
        global_info.nb_failure ++;
        global_info.failure_rate = (global_info.nb_failure / global_info.nb_received) * 100;
    }
    else if (validate == 0){
        printf(" %s (%s) %.3f ms", intel.addr, intel.addr, intel.time);
        validate = 1;
    }
    else {
        if (ft_strcmp(addr, intel.addr) == 0) 
            printf(" %.3f ms", intel.time);
        else
            printf(" %s %.3f ms", intel.addr, intel.time);
    }

}

void    print_results(struct reply_info intel, unsigned char ttl, int probe_nb) {
    static char *addr;

    print_replies(intel, ttl, probe_nb, addr);

    if (addr != NULL) {
        free(addr);
        addr = NULL;
    }
    addr = ft_strdup(intel.addr);
    fflush(stdout);
    errno = 0;
}

void    print_reply_info(struct reply_info reply) {
    printf("reply len_received = >%ld<\n", reply.len_received);
    printf("reply host");
    printf("reply addr");
    print_hexa_buffer(reply.addr, 16);
    printf("reply error = >%u<\n", reply.seq_num);
    printf("reply error = >%u<\n", reply.ttl);
    printf("reply error = >%f<\n", reply.time);
}

void    print_globglob() {
    printf("globglob fd = >%d<\n", global_info.fd);
    printf("globglob destination = \n");
    print_hexa_buffer(global_info.destination, 16);
    printf("globglob info_addr = >%p<\n", &global_info.info_addr);
    printf("globglob prob delay= >%f<\n", global_info.probe_delay.delay);
    printf("globglob failure_rate = >%u<\n", global_info.failure_rate);
    printf("globglob reqched = >%u<\n", global_info.reached);
    printf("globglob max_hops = >%u<\n", global_info.max_hops);
    printf("globglob hops = >%u<\n", global_info.hops);
    printf("globglob packet_size = >%u<\n", global_info.packet_size);
}

void    print_start() {
    printf("ft_traceroute to %s (%s), %u hops max, %d byte packets\n", \
    global_info.destination, global_info.destination, global_info.max_hops, global_info.packet_size);
}

