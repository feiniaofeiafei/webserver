#include "config.h"
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

config::config(){
    this -> port = 9036;
    this -> sql_num = 8;
}


config::~config(){
}


void config::parse_arg(int argc, char*argv[]){
    const char* str = "p:s:";
    int opt;
    while((opt = (getopt(argc, argv, str))) != -1){
        switch(opt){
            case 'p' :{
                this -> port = atoi(optarg);
                break;
            }
            case 's' :{
                this -> sql_num = atoi(optarg);
                break;
            }
            default:{
                std::cout << "parameter parsing error" << std::endl;
                break;
            }
        }
    }
}