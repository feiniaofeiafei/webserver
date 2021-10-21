#ifndef _CONFIG_H_
#define _CONFIG_H_
class config{

public:
    config();
    ~config();

    void parse_arg(int argc, char* argv[]);
};


#endif