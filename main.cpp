#include "./mysql/sql_connection_pool.h"
#include "./config/config.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]){
    mysql_connection_pool msyql;
    config config;
    config.parse_arg(argc, argv);
    cout << "hello, this is myserver ..." << endl;
    return 0;
}