#ifndef _HTTP_CONN_H_
#define _HTTP_CONN_H_

#include <sys/epoll.h>
#include "./../mysql/sql_connection_pool.h"
#include "./log/log.h"
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

    enum LINE_STATUS{
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };

    enum CHECK_STATE{
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };


    void process();
    int improv;     // ????
    
public:
    http_conn(){}
    ~http_conn(){}

    void close_conn(bool read_close = true);
public:
    bool read_once();
    

    bool process_write(HTTP_CODE ret);

private:
    HTTP_CODE parse_request_line(char *text);
    HTTP_CODE process_read();
    LINE_STATUS parse_line();
    char* get_line(){return m_read_buf + m_start_line;}

private:
    int m_sockfd;
    
    

    CHECK_STATE m_check_state;
    

    int m_TRIGMode;
    int m_read_idx;
    char m_read_buf[READ_BUFFER_SIZE];
    int m_start_line;
    int m_checked_idx;

};


#endif
