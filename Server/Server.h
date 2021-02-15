//
// Created by YOMNA on 12/15/2020.
//

#ifndef ASSIGNMENT1_SERVER_H
#define ASSIGNMENT1_SERVER_H

#include <stdio.h>
#include <pthread.h>
#include <pthread.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;
#define BUFFER_SIZE 4800

class Server {
public:
    void init_server(int port);

private:
    int N = 10;

    void *request(int const &cs);

    vector<string> split(string str, string token);

    void get_request(string, int);

    void post_request(vector<string>, int);

    bool is_closed(int);

    int get_len(string);
};

#endif //ASSIGNMENT1_SERVER_H
