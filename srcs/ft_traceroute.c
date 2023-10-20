#include "../includes/ft_traceroute.h"

struct globglob global_info;



void    ft_traceroute() {
    unsigned char ttl = 0;

    while (ttl < 30 && global_info.reached == 0 && global_info.failure_rate < 80)
    {
        ttl++;
        for (int i = 0; i < 3; i++) {
            send_req(ttl);
            received_request(ttl, i);
        }
    }
    write(1, "\n", 1);
    
}


int main(int argc, char **argv) {
    check_dest(argc, argv);
    signal(SIGINT, signal_interrupt);

    set_globglob(argv[1]);
    print_start(global_info);

    if (array_string_contains("--help", argv) == 1)
        help_option();
    else
        ft_traceroute();
    return (0);
    
}