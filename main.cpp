#include <unistd.h>
#include "./mysql/sql_connection_pool.h"
#include "./config/config.h"
#include <iostream>
#include "./log/log.h"

using namespace std;

int main(int argc, char *argv[]){
    mysql_connection_pool msyql;

    config config;
    int m_close_log = 0;
    config.parse_arg(argc, argv);
    
    cout << "hello, this is myserver ..." << endl;

    Log::get_instance()->init("./ServerLog", 0, 2000, 800000, 600);
    LOG_INFO("hello, this is log_info test%d",1);
    sleep(2);
    
    return 0;
}
