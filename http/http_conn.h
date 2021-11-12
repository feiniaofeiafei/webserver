#ifndef _HTTP_CONN_H_
#define _HTTP_CONN_H_

#include <sys/epoll.h>
#include "./../mysql/sql_connection_pool.h"
#include <sys/unistd.h>
#include <errno.h>
#include <sys/socket.h>

class http_conn{
public:
    static const int READ_BUFFER_SIZE = 2048;
    void init_mysql_result(mysql_connection_pool* conn_pool);
    static int m_epollfd;
    static int m_user_count;
    MYSQL* mysql;
    int m_state;   

    enum HTTP_CODE{
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };

    void process();
    int improv;     // ????
    
public:
    http_conn(){}
    ~http_conn(){}

    void close_conn(bool read_close = true);
public:
    bool read_once();


private:
    int m_sockfd;
    HTTP_CODE process_read();
    bool process_write(HTTP_CODE ret);

    int m_TRIGMode;
    int m_read_idx;
    char m_read_buf[READ_BUFFER_SIZE];

};


#endif
