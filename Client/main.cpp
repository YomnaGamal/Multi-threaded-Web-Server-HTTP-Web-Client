//
// Created by YOMNA on 12/15/2020.
//

#include "Client.h"

int main(int argc, char* argv[]) {
    cout << "Client starts\n";
    Client Client;
    string requests = "/requests1";
    Client.init_client(atoi(argv[2]),argv[1],requests);
    return 0;
}