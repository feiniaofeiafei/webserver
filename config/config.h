#ifndef _CONFIG_H_
#define _CONFIG_H_
class config{

public:
    config();
    ~config();

    void parse_arg(int argc, char* argv[]);
private:
    int port;       // server默认端口
    int sql_num;    // 数据库链接池数量
};


#endif