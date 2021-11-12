#include <unistd.h>
#include "./mysql/sql_connection_pool.h"
#include "./config/config.h"
#include <iostream>
#include "./log/log.h"
#include "./timer/lst_timer.h"

using namespace std;

int main(int argc, char *argv[]){
    mysql_connection_pool msyql;

    config config;
    int m_close_log = 0;
    config.parse_arg(argc, argv);
    
    cout << "hello, this is myserver ..." << endl;

    util_timer* timer = new util_timer;
    sort_timer_list* timer_list = new sort_timer_list;
    timer_list->add_timer(timer);

    Log::get_instance()->init("./ServerLog.log", 0, 2000, 800000, 600);
    LOG_INFO("hello, this is log_info test%d",1);
    LOG_DEBUG("hello, this is log_debug test%d",2);
    LOG_WARN("hello, this is log_warn test%d",3);
    LOG_ERROR("hello, this is log_error test%d",4);
    sleep(2);
    
    return 0;
}
