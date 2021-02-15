//
// Created by YOMNA on 12/15/2020.
//


#include "Client.h"

void Client::init_client(int port, string hostname, string requests) {
    struct sockaddr_in sin;
    struct hostent *hptr;
    char msg[80];
    int s = socket(AF_INET,SOCK_STREAM,0);
    // AF_INET --> IPv4, SOCK_STREAM --> Type for TCP, 0 --> for protocol " i have only one which is TCP
    if (s < 0){
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    } else {
        cout << "Socket created successfully";
    }
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port) // convert to tslsol 4bka
    hptr = gethostbyname(hostname)

    memcpy(&sin.sin_addr,hptr->h_addr,hptr->h_length);

    int ac = connect(s,(struct sockaddr *) &sin, sizeof(sin));
    if (ac < 0){
        perror("Failed to connect socket");
        exit(EXIT_FAILURE);
    } else {
        cout << "Socket connected successfully";
    }
    ///    requestes are in file
    string fpath = "pwd/Client/ClientFiles/" + requests;

    string response;

    ifstream file(fpath.c_str(), ifstream::ate | ifstream::binary);
    if (!file.is_open()) {
        cout << "error in file\n";
    } else {
        string line;
        vector<string> get_requests;
        while (getline(file, line)) {
            vector<string> request = split(line, " ");
            if (request[0] == "GET") {
                get_request(line,s);
            }

            if (request[0] == "POST") {
                post_request(line,s);
            }
        }
    }

//    request(requests);
//    printf("Enter the msg: ");
//    gets(msg);
//    write(s,msg,strlen(msg));
//    bzero((char *)&msg,sizeof(msg));
//    int n = read(s,msg,sizeof(msg));
//    printf("%d bytes : %s \n",n,msg);
    close(s);
}

void Client::get_request(string request, int cs) {
    cout << "Client: Sending get request\n";
    request += "\r\n\r\n";
    vector<string> r = split(request," ");
    if ((write(cs , (char* )request.c_str() , request.size())) < 0) {
        perror("failed to write to socket client->server");
    }
    string response;
    while (true) {
        vector<char> sr(4800, 0);
        int read_size = recv(cs, &sr[0], 4800, 0);
        if (read_size <= 0) {
            break;
        }
        response += string(sr.begin(), sr.begin() + std::min(read_size, 4800));
        if (response.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }

    vector<string> response_t = split(response, "\r\n");
    if (response_t[0].find("404 Not Found") != std::string::npos) {
        cout << r[1] << " Not Found\n";
        return;
    }
//    string content_len = boost::regex_replace(response_t[1], boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
//    int len = stoi(content_len);
    int len = get_len(response_t[1]);
    char * buffer;
    buffer = new char [len];
    memset(buffer, '\0', sizeof(buffer));
    cout << "getting data from server\n";

    while (len > 0) {
        int length_sent = read(cs, buffer, len-1);
        if (length_sent <=0 || length_sent >= len-1) {
            break;
        }
        buffer += length_sent;
        len -= length_sent;
    }

    cout << buffer << "\n";
    std::ofstream file("pwd/Client/ClientFiles/" + r[1], ios::out | ios::binary);
    file.write(buffer, strlen(buffer));
    file.close();
}

void Client::post_request(string request, int cs) {
    cout << "Sending post request --> " << request <<endl;
    int n;
//    char sendline[MAX_LEN];
    vector<string> fname = split(request, " ");
    string fpath = "pwd/Client/ClientFiles/" + fname[1];

    ifstream file(fpath.c_str(), ifstream::ate | ifstream::binary);
    if (!file.is_open()) {
        cout << "Error in file: cannot send it\n";
        return;
    }
    long len = file.tellg();

    request += ("\r\nContent-Length:" + to_string(len) + "\r\n\r\n");
//    request += "\r\n\r\n";
    int ac = write(cs , (char* )request.c_str() , request.size())
    if (ac < 0) {
        perror("failed to write to socket client->server");
    }

    string response;
    basic_string<char> response;
    while (true) {
        vector<char> server_response(4800, 0);
        int read_size = recv(cs, &server_response[0], 4800, 0);
        if (read_size <= 0) {
            break;
        }
        response += string(server_response.begin(), server_response.begin() + std::min(read_size, 4800));
        if (response.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }
    cout << "Response of server is: "<< response << "\n";
    char * buffer;
    file.seekg (0, ios::beg);
    buffer = new char [len];
    memset(buffer, '\0', sizeof(buffer));
    file.read (buffer, len);
    file.close();
    while (len  > 0){
        int length_sent = write(cs, buffer, len);
        if(length_sent == -1){
            return;
        }
        if(length_sent == 0){
            break;
        }
        buffer += length_sent;
        len -= length_sent;
    }
    cout << "Post request sent successfully\n";
}

vector<string> Client::split(string str, string token) {
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

int Client::get_len(string str) {
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