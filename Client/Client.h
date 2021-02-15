//
// Created by YOMNA on 12/15/2020.
//

#ifndef ASSIGNMENT1_CLIENT_H
#define ASSIGNMENT1_CLIENT_H
#include <stdio.h>
#include <netdb.h>
//#include <Winsock2.h>
#include <string.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

class Client {
public:
    void init_client(int port,string hostname,string requests);

private:
    void get_request(string, int);

    void post_request(string, int);

    vector<string> split(string str, string token);

//    int N = 10;
//    static void * handle_request(void *);
//    void accept_connections(int server_soscket);
    int get_len(string str);
};

#endif //ASSIGNMENT1_CLIENT_H
