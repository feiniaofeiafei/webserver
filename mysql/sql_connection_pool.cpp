#include "sql_connection_pool.h"


mysql_connection_pool* mysql_connection_pool::get_instance(){
    static mysql_connection_pool conn_pool;
    return &conn_pool;
}

// 构造函数
mysql_connection_pool::mysql_connection_pool(){
    this -> curConn = 0;
    this -> freeConn = 0;
}


// 析构函数
mysql_connection_pool::~mysql_connection_pool(){
    destroy_mysql_connection_pool();
}


void mysql_connection_pool::destroy_mysql_connection_pool(){
    if(connList.size() > 0){
        for(auto it = connList.begin(); it != connList.end(); ++it){
            MYSQL* con = *it;
            mysql_close(con);
        }
        this -> curConn = 0;
        this -> freeConn = 0;
        this -> connList.clear();
    }
}

void mysql_connection_pool::init(const string& address, const int& port, const string& username, const string& password, const string& databasename, int maxConn){
    this -> address = address;
    this -> port = port;
    this -> username = username;
    this -> password = password;
    this -> databasename = databasename;

    this -> maxConn = maxConn;

    for(int i = 0; i < maxConn; i++){
        MYSQL* conn = nullptr;
        conn = mysql_init(conn);

        if(conn == nullptr){
            std::cout << "mysql_connection_pool connection init has error ..." << std::endl;
            exit(-1);
        }

        conn = mysql_real_connect(conn, this -> address.c_str(), 
                this -> username.c_str(), this -> password.c_str(), 
                this -> databasename.c_str(), this -> port, nullptr, 0);
        
        if(conn == nullptr){
            std::cout << "mysql_connection_pool connection init has error ..." << std::endl;
            exit(-1);
        }

        this -> connList.push_back(conn);
    }
    this -> freeConn = maxConn;
}

// 从连接池中获得一个链接
MYSQL* mysql_connection_pool::get_mysql_conn(){
    MYSQL* sqlCon = nullptr;
    if(0 == this -> connList.size()){
        return nullptr;
    }
    
    sqlCon = connList.front();
    connList.pop_front();
    --this -> freeConn;
    ++this -> curConn;

    return sqlCon;
}

// 释放当前使用的链接 归还到连接池中
bool mysql_connection_pool::release_mysql_conn(MYSQL* sqlCon){
    if(sqlCon == nullptr){
        return false;
    }

    // 因为是释放，等于直接加入到链接池中即可
    this -> connList.push_back(sqlCon);
    ++this -> freeConn;
    --this -> curConn;


    return true;
}


connectionRAII::connectionRAII(MYSQL** conn, mysql_connection_pool* conn_pool){
    *conn = conn_pool -> get_mysql_conn();

    this -> connRAII = *conn;
    this -> poolRAII = conn_pool;
}

connectionRAII::~connectionRAII(){
    this -> poolRAII -> release_mysql_conn(this -> connRAII);
}
