#include "../includes/ft_traceroute.h"

extern struct globglob global_info;


void    extract_values(char *buff, ssize_t size, struct reply_info *intel) {
    int offset = 0;

    offset = size - 32;
    intel->seq_num = buff[size - DATA_LEN - 1];
    if ((intel->addr = malloc(16)) == NULL)
        error_exit("Error: malloc failed\n", 12);
    memset(intel->addr, 0, 16);
    snprintf(intel->addr, 16, "%hhu.%hhu.%hhu.%hhu", (uint8_t)buff[size - 32 - (offset -12)],
        (uint8_t)buff[size - 32 - (offset - 13)],
        (uint8_t)buff[size - 32 - (offset - 14)],
        (uint8_t)buff[size - 32 - (offset - 15)]);
    intel->len_received = size;
    intel->ttl = 0;
}

void    handle_datas(struct reply_info *intel) {
    gettimeofday(&global_info.probe_delay.end, NULL);
    intel->time = calculate_time(global_info.probe_delay.start, global_info.probe_delay.end);
}

void receiv_and_display(struct msghdr msg_received, unsigned char ttl, int probe_nb) {
    int len_received = -1;
    struct reply_info intel;
    char *buff;

    len_received = recvmsg(global_info.fd, &msg_received, 0);
    buff = msg_received.msg_iov->iov_base;

    handle_datas(&intel);
    extract_values(buff, len_received, &intel);
    print_results(intel, ttl, probe_nb);
    if (ft_memcmp(global_info.destination, intel.addr, 16) == 0)
        global_info.reached = 1;
    else if (ft_memcmp(global_info.destination, "127.", 4) == 0 &&
            ft_memcmp(intel.addr, "127.", 4) == 0)
        global_info.reached = 1;
    free(intel.addr);
    intel.addr = NULL;
}


void  received_request(unsigned char ttl, int probe_nb) {
    struct msghdr msg_received;
    char buffer1[256], buffer2[256];
    set_msghdr(&msg_received, buffer1, buffer2);
    receiv_and_display(msg_received, ttl, probe_nb);
}
