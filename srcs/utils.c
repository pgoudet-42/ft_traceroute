#include "../includes/ft_traceroute.h"

extern struct globglob global_info;

char   *malloc_safe(ssize_t len, unsigned char size) {
    char *buff;

    buff = malloc(size * len);
    if (buff == NULL)
        error_exit("Error: malloc failed\n", 12);
    return (buff);
}

float    calculate_time(struct timeval start, struct timeval end) {
    float res_start, res_end, res;

    res_start = end.tv_usec - start.tv_usec;
    res_end = end.tv_sec - start.tv_sec;
    res =  res_end * 1000 + res_start / 1000;
    return (res);
}

int error_exit(char *str, int exit_code) {
    printf(str);
    exit(exit_code);
}

int array_string_contains(char *str, char **array)  {
    int i = 0;

    while(array[i] != NULL) {
        if (ft_strcmp(str, array[i++]) == 0)
            return (1);
    }
    return (0);
}

void    help_option() {
    printf("\nUsage\n   ft_traceroute [options] \nOption:\n\
    -v                 verbose output\n");
}

char    *toBinary(int n, int len) {
    char* binary = (char*)malloc(sizeof(char) * len);
    int k = 0;
    for (unsigned i = (1 << (len - 1)); i > 0; i = i / 2) {
        binary[k++] = (n & i) ? '1' : '0';
    }
    binary[k] = '\0';
    return binary;
}



char *create_buff(void *datas, ssize_t size, char *str, ssize_t len) {
    char *tmp = (char *)datas;
    char *res;
    
    res = malloc_safe(size + len, 1);

    for (int i = 0; i < size + len; i++) {
        if (i < size)
            res[i] = tmp[i];
        else
            res[i] = str[i - size];
    }
    return (res);
}

void    init_hints(struct addrinfo *hints) {
    ft_memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_family = AF_INET;
    hints->ai_flags = 0;
    hints->ai_next = NULL;
    hints->ai_protocol = IPPROTO_ICMP;
    hints->ai_socktype = SOCK_RAW;
}

void signal_interrupt(__unused int sig) {
    close(global_info.fd);
    exit(130);
}

char *getaddr() {
    int n;
    struct ifreq ifr;
    char array[] = "eth0";
 
    n = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    ft_strlcpy(ifr.ifr_name , array , IFNAMSIZ - 1);
    ioctl(n, SIOCGIFADDR, &ifr);
    close(n);
    return (inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));
}

void    check_dest(int argc, char **argv) {
    char *dest;
    if (argc == 1) {
        help_option();
        exit(1);
    }

    if (argc > 2)
        error_exit("Error: Too much args\n", 1);
    else
        dest = argv[1];

    if (strcmp(dest, "255.255.255.255") == 0)
        error_exit("ft_traceroute: Operation not permitted\n", 1);
}

void    set_msghdr(struct msghdr *msg_received, char *buff1, char *buff2) {
    struct iovec iov;

    ft_memset(msg_received, 0, sizeof(*msg_received));
    ft_memset(&iov, 0, sizeof(iov));
    iov.iov_base = buff1;
    iov.iov_len  = 100;
    msg_received->msg_iov = &iov;
    msg_received->msg_iovlen = 1;
    msg_received->msg_control = buff2;
    msg_received->msg_controllen = 256;
	msg_received->msg_flags = 0;
}

struct addrinfo *get_host(char *address) {
    struct addrinfo hints, *res, *tmp;
    char host[256];
    int get_info_res;

    init_hints(&hints);
    if ((get_info_res = getaddrinfo(address, NULL, &hints, &res)) != 0) {
        printf("ft_traceroute: %s: %s\n", address, gai_strerror(get_info_res));
        exit(2);
    }
    for (tmp = res; tmp != NULL; tmp = tmp->ai_next) {
        if (tmp->ai_addr->sa_family == AF_INET) {
            inet_ntop(AF_INET, &res->ai_addr->sa_data[2], host, 256);
            break;
        }
    }
    return (res);
}

in_addr_t create_s_addr(char sa_data[14]) {
    in_addr_t tmp;
    tmp = (uint8_t)sa_data[5];
    tmp = (tmp << 8) + (uint8_t)sa_data[4];
    tmp = (tmp << 8) + (uint8_t)sa_data[3];
    tmp = (tmp << 8) + (uint8_t)sa_data[2];
    return (tmp);
}

char    *set_info_addr(struct sockaddr_in *sock_addr, char *address) {
    struct addrinfo *info_addr;
    char sa_data[14], *host;

    host = malloc(16);
    if (host == NULL)
        error_exit("Error: malloc failed\n", 12);
    bzero(sock_addr, sizeof(struct sockaddr_in));
    info_addr = get_host(address);
    ft_memcpy(sa_data, info_addr->ai_addr->sa_data, 14);
    snprintf(host, 16, "%hhu.%hhu.%hhu.%hhu", sa_data[2], sa_data[3], sa_data[4], sa_data[5]);
    freeaddrinfo(info_addr);

    sock_addr->sin_addr.s_addr = create_s_addr(sa_data);
    sock_addr->sin_family = AF_INET;
    sock_addr->sin_port = 0;
    return (host);
}

void set_globglob(char *address) {
    ft_memset(&global_info, 0, sizeof(struct globglob));
    global_info.destination = set_info_addr(&global_info.info_addr, address);
    global_info.max_hops = 30;
    global_info.reached = 0;
    global_info.failure_rate = 0;
    global_info.nb_failure = 0;
    global_info.nb_received = 0;
    global_info.fd = -1;
    memset(&global_info.probe_delay, 0, sizeof(struct delay));
    global_info.packet_size = 60;
}