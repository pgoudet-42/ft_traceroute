#include "../includes/ft_traceroute.h"

extern struct globglob global_info;

char    *set_icmp_header(struct icmphdr  *icmp_hdr) {
    ssize_t final_size = 0;
    static uint16_t seq_num;

    icmp_hdr->type = 8;
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0x0;
    icmp_hdr->un.echo.id = htons(getpid());
    icmp_hdr->un.echo.sequence = htons(seq_num++);
    char *buff = create_buff((void *)icmp_hdr, sizeof(icmp_hdr), DATA, DATA_LEN);
    final_size = sizeof(struct icmphdr) + DATA_LEN;
    icmp_hdr->checksum = htons(checksum(buff, final_size));
    return (buff);
}

int connect_socket(unsigned char ttl) {
    int fd;
    struct timeval tv;

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    if ((fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) > 1) {
        if (setsockopt(fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == 1 ||
            setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) == -1) {
            perror("");
            error_exit("Error: setsockopt failed\n", 1);
        }
    }
    else {
        perror("");
        error_exit("Error: socket opening failed\n", 1);
    }
    return (fd);
}


void send_req(unsigned char ttl) {
    struct icmphdr icmp_hdr;
    char *buff;
    int res_sendto = 0;

    if (global_info.fd < 0)
        close(global_info.fd);
    global_info.fd = connect_socket(ttl);

    buff = set_icmp_header(&icmp_hdr);
    ft_memcpy(buff, &icmp_hdr, sizeof(icmp_hdr));
    ft_memcpy(&(buff[sizeof(struct icmphdr)]), DATA, DATA_LEN);
    if ((res_sendto = sendto(global_info.fd, buff, sizeof(struct icmphdr) + DATA_LEN, 
        0, (const struct sockaddr *)&global_info.info_addr, sizeof(struct sockaddr_in))) == -1) {
        perror("");
        error_exit("Error: sendto failed\n", 1);
    }
    gettimeofday(&global_info.probe_delay.start, NULL);
    free(buff);
}