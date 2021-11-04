#include "http_conn.h"
#include <map>

map<string, string> user_info;


void modfd(int epollfd, int fd, int env, int TRIGMode){
    epoll_event event;
    event.data.fd = fd;
    if(1 == TRIGMode){
        
    }else{

    }
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void removefd(int epollfd, int fd){
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

void http_conn::init_mysql_result(mysql_connection_pool* conn_pool){
    MYSQL* sql = nullptr;
    connectionRAII mysql_con(&sql, conn_pool);

    if(mysql_query(sql, "select username, passwd from user")){
        mysql_error(sql);
    }

    // 结果记录
    MYSQL_RES* result = mysql_store_result(sql);

    // 记录列数
    int num_fields = mysql_num_fields(result);

    // 返回所有字段结构的数组
    MYSQL_FIELD* fields = mysql_fetch_fields(result);

    while(MYSQL_ROW row = mysql_fetch_row(result)){
        string temp1(row[0]);
        string temp2(row[1]);
        user_info[temp1] = temp2;
    }

}

int http_conn::m_epollfd = -1;
int http_conn::m_user_count = 0;


void http_conn::process(){
    HTTP_CODE read_ret = process_read();
    if(read_ret == NO_REQUEST){
        modfd(http_conn::m_epollfd, m_sockfd, EPOLLIN, m_TRIGMode);
        return;
    }
    bool write_ret = process_write(read_ret);
    if(!write_ret){
        close_conn();
    }
    modfd(m_epollfd, m_sockfd, EPOLLOUT, m_TRIGMode);
}

http_conn::HTTP_CODE http_conn::process_read(){
    return NO_REQUEST;
}

bool http_conn::process_write(HTTP_CODE ret){
    return true;
}

bool http_conn::read_once(){
    return true;
}


void http_conn::close_conn(bool real_close){
    if(real_close && (m_sockfd != -1)){
        printf("close %d \n", m_sockfd);
        removefd(m_epollfd, m_sockfd);
        m_sockfd = -1;
        m_user_count--;
    }
}


