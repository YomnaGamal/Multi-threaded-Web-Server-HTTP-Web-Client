//
// Created by YOMNA on 12/15/2020.
//


#include "Server.h"

void Server::init_server(int port) {

    struct sockaddr_in sin;
    char msg[80];
    int s = socket(AF_INET, SOCK_STREAM,
                   0);// AF_INET --> IPv4, SOCK_STREAM --> Type for TCP, 0 --> for protocol " i have only one which is TCP
    if (s == 0) {
        perror("failed to create socket");
        exit(EXIT_FAILURE);
        close(s);
    } else {
        cout << "Socket created successfully\n";
    }
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port); // convert to tslsol 4bka
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    int b = bind(s, (struct sockaddr *) &sin, sizeof(sin));
    if (b < 0) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
        close(s);
    } else {
        cout << "Bind done successfully";
    }
    if (listen(s, N)) {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
        close(s);
    } else {
        cout << "Socket listens successfully";
    }
    while (1) {
        int cs = accept(s, (struct sockaddr *) &sin, &(sizeof(sin)));
        if (cs < 0) {
            perror("Failed to accept connection");
            break;
        } else {
            cout << "connection accepted successfully";
        }
        /// creating thread for each connection
        pthread_t thread;
        pthread_create(&thread, NULL, request, cs);
    }
    close(s);
}

void *Server::request(int const &cs) {

    cout << "Server: client connected\n";
    while (true) {
        string request;
        if (is_closed(cs)) {
            break;
        }
        vector<char> server_response(BUFFER_SIZE, 0);
        int read_size = recv(cs, &server_response[0], BUFFER_SIZE, 0);
        if (read_size <= 0) {
            break;
        }
        request += string(server_response.begin(), server_response.begin() + std::min(read_size, BUFFER_SIZE));
        if (request.find("\r\n\r\n") != std::string::npos) {
            vector<string> all_requests = split(request, "\r\n\r\n");
            for (int i = 0; i < all_requests.size(); i++) {
                cout << request + "\n";
                if (all_requests[i].empty()) {
                    continue;
                }
                cout << "serving request \n";
                vector<string> request_t = split(all_requests[i], "\r\n");
                if (all_requests[i].find("POST") != std::string::npos) {
                    post_request(request_t, cs);
                } else if (all_requests[i].find("GET") != std::string::npos) {
                    get_request(request_t[0], cs);
                }
            }
        }
    }
    cout << "connection ended with client\n";
    return nullptr;
}

bool Server::is_closed(int sock) {
    char x;
    interrupted:
    ssize_t r = ::recv(sock, &x, 1, MSG_DONTWAIT | MSG_PEEK);
    if (r < 0) {
        switch (errno) {
            case EINTR:
                goto interrupted;
            case EAGAIN:
                break; /* empty rx queue */
            case ETIMEDOUT:
                break; /* recv timeout */
            case ENOTCONN:
                break; /* not connected yet */
            default:
                throw (errno);
        }
    }
    return r == 0;
}

vector<string> Server::split(string str, string token) {
    vector<string> result;
    while (str.size()) {
        int index = str.find(token);
        if (index != string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0)result.push_back(str);
        } else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

void Server::get_request(string request, int cs) {
    cout << "Handling get request -->" << request << endl;
    vector<string> fname = split(request, " ");
    string fpath = "pwd/Server/ServerFiles/" + fname[1];

    string response;

    ifstream file(fpath.c_str(), ifstream::ate | ifstream::binary);
    if (!file.is_open()) {
        cout << "error in file, 404\n";
        response = "HTTP/1.1 404 Not Found\r\n";
        write(cs, (char *) response.c_str(), response.size());
        return;
    } else {
        response = "HTTP/1.1 200 OK\r\n";

        long len = file.tellg();
        char *buffer = new char[len];
        memset(buffer, '\0', sizeof(buffer));

        response += ("Content-Length: " + to_string(len - 1) + "\r\n\r\n");
        write(cs, (char *) response.c_str(), response.size());

        file.seekg(0, ios::beg);
        file.read(buffer, len);
        file.close();
        cout << "file opened & read successfully";
        while (len > 0) {
            int length_sent = write(cs, buffer, len);
            if (length_sent <= 0) {
                break;
            }
            buffer += length_sent;
            len -= length_sent;
        }
        cout << "file sent successfully";
    }
}

void Server::post_request(vector<string> request, int cs) {
    cout << "Handling post request -->" << request << endl;
    string response = "HTTP/1.1 200 OK\r\n\r\n";
    write(cs, (char *) response.c_str(), response.size());

    vector<string> fname = split(request[0], " ");
    std::ofstream file("pwd/Server/ServerFiles/" + fname[1], ios::out | ios::binary);

    if (!file.is_open()) {
        cout << "error in file\n";
    }

//    string content_len = boost::regex_replace(request[1], boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
//    int len = stoi(content_len);
    int len = get_len(request[1]);
    char *buffer = new char[len];
    memset(buffer, '\0', sizeof(buffer));

    while (len > 0) {
        int length_get = read(cs, buffer, len - 1);
        if (length_get >= len - 1 || length_get <= 0) {
            break;
        }
        buffer += length_get;
        len -= length_get;
    }
    file.write(buffer, strlen(buffer));
    file.close();
    cout << "file got successfully\n";
    cout << "file content: " << buffer << endl;
}

int Server::get_len(string str) {
    stringstream ss;
    ss << str;
    string temp;
    int found;
    while (!ss.eof()) {
        ss >> temp;
        if (stringstream(temp) >> found){
            return found;
        }
        temp = "";
    }
    return 0;
}
