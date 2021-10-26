#include "webserver.h"

webserver::webserver(){
    users = new http_conn[MAX_FD];

}


webserver::~webserver(){
    delete[] users;
}

void webserver::server_init(){


}

void webserver::sql_pool_init(){
    m_conn_pool = mysql_connection_pool::get_instance();
    m_conn_pool -> init("localhost", 3306, m_username, m_password, m_database, m_sql_num);

    // 初始化数据库读取表
    users -> init_mysql_result(m_conn_pool);
}



void webserver::thread_pool_init(){
    m_thread_pool = new thread_pool<http_conn>(m_actor_model, m_conn_pool, m_thread_num);

}