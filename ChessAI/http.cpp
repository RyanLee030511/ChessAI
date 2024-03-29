#include "pch.h"
#include "http.h"


boolean http::open(std::string url)
{
    this->url = url;

    //初始化套接字
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return false;
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return false;
    }
    SOCKADDR_IN serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    serv_addr.sin_addr.S_un.S_addr = inet_addr("59.56.97.206");


    char* pstr;
    hostent *phost = gethostbyname("www.chessdb.cn");
    printf("offical name: %s\n", phost->h_name);
    int i = 0;
    for (pstr = phost->h_aliases[i]; pstr != NULL; pstr = phost->h_aliases[++i])
    {
        printf("alias: %s\n", pstr);
    }
    if (phost->h_addrtype == AF_INET)
    {
        printf("address type: ipv4\n");
    }
    else if (phost->h_addrtype == AF_INET6)
    {
        printf("address type: ipv6\n");
    }
    i = 0;
    for (pstr = phost->h_addr_list[0]; pstr != NULL; pstr = phost->h_addr_list[++i])
    {
        u_long temp;
        temp = *(u_long*)pstr;
        in_addr in;
        in.S_un.S_addr = temp;
        printf("%s\n", inet_ntoa(in));
    }
 

    


    //memcpy(&serv_addr.sin_addr, gethostbyname("www.chessdb.cn")->h_addr, 4); //获取域名对应的第一个ip
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        return false;
    }
    //设置延时
    int timeout = 8000;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));


    return true;
}

std::string http::get()
{
    call("GET", "");
    return std::string();
}

boolean http::call(std::string method, std::string data)
{
    std::stringstream request;
    request << method << " " << this->url << " HTTP/1.1\r\n";
    request << "Host: " << domain << "\r\n";
    if (data!="")
    {
        request << "Content-Length:" << data.length() << "\r\n";
    }
    if (this->Cookie != "")
    {
        request << this->Cookie + "\r\n";
    }
    request << "\r\n";
    request << data;
    
    std::string requestStr = request.str();
    if (send(sock, requestStr.c_str(), requestStr.size(), 0) <= 0 ) {
        return false;
    }

    char buf[1024]{0};
    std::string recvStr;

        recv(sock, buf, sizeof(buf) - sizeof(buf), 0);
        recvStr += buf;
        printf("buf:%s", buf);
    
    closesocket(sock);

    
}
