//
// Created by Yomna on 12/15/2020.
//
#include "Server.h"

//socket = IP + port // 3nwan el 4baka
// example argv = server portnum
int main(int argc, char *argv[]) {
    cout << "Server starts\n";
    Server Server;
    Server.init_server(atoi(argv[1]));
    return 0;
}
