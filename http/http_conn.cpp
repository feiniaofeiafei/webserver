#include "http_conn.h"
#include <map>

map<string, string> user_info;

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

int http_conn::m_user_count = 0;