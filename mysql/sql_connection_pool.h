#include <string>
#include <mysql.h>
#include <list>
#include <iostream>
#include <string>

using namespace std;

class mysql_connection_pool{

public:
    void init(const string& address, const int& port, const string& username, const string& password, const string& databasename, int maxConn);

    MYSQL * get_mysql_conn();
    bool release_mysql_conn(MYSQL*);
    
    mysql_connection_pool();
    ~mysql_connection_pool();
    void destroy_mysql_connection_pool();

private:
    string address;     // 链接地址
    int port;           // 端口
    string username;    // 账户名
    string password;    // 密码
    string databasename;// 数据库名

    int maxConn;
    int curConn;
    int freeConn;
    list<MYSQL* > connList;     // 连接池
};