#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_
#include "./../mysql/sql_connection_pool.h"
#include "./../http/http_conn.h"
#include "./../threadpool/threadpool.h"
#include <string>

const int MAX_FD = 65536;           //最大文件描述符

class webserver{
public:
    webserver();
    ~webserver();

    void server_init();
    void sql_pool_init();
    
    void thread_pool_init();

    
private:

    // 基础信息
    int m_conn_port;                    // 服务器连接端口
    int m_actor_model;                  // 事件处理机制

    http_conn* users;                   // 最大链接数

    // 数据库信息
    mysql_connection_pool* m_conn_pool; // 数据库链接池
    string m_username;                  // 数据库用户名
    string m_password;                  // 密码
    string m_database;                  // 数据库名
    int m_sql_port;                     // 数据库端口
    int m_sql_num;                      // 数据库连接池最大连接数

    // 线程池相关
    thread_pool<http_conn>* m_thread_pool;      // 线程池
    int m_thread_num;                           // 线程个数
};

#endif
