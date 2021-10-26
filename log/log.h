#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include "block_queue.h"

using namespace std;

class Log {
public:
    static Log* get_instance() {
        static Log instance;
        return &instance;
    }
    static void* flush_log_thread(void* args) {
        Log::get_instance()->async_write_log();
    }
    bool init(const char *file_name, int close_log, int log_buf_size = 8192, int split_lines = 5000000, int max_queue_size = 0);

    void write_log(int level, const char *format, ...);

    void flush(void);
private:
    Log();
    ~Log();
    //功能是：当阻塞队列中有元素时，从队列中取出元素写文件
    void* async_write_log() {
        string log_record;
        while(true == m_log_queue->pop(log_record)) {
            m_mutex.lock();
            fputs(log_record.c_str(), m_fp);
            m_mutex.unlock();
        }
    }

    FILE* m_fp;
    block_queue<string> *m_log_queue;
    locker m_mutex;
    char m_dir_name[128];
    char m_log_name[128];
    long long m_count; //日志当前行数
    int m_split_lines; //日志分隔行数（配置值）
    int m_log_buf_size;
    int m_close_log;
    bool m_async;
    char* m_buf;
    int m_today;
};

#define LOG_DEBUG(format, ...) if(0 == m_close_log) {Log::get_instance()->write_log(0, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_INFO(format, ...) if(0 == m_close_log) {Log::get_instance()->write_log(1, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_WARN(format, ...) if(0 == m_close_log) {Log::get_instance()->write_log(2, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_ERROR(format, ...) if(0 == m_close_log) {Log::get_instance()->write_log(3, format, ##__VA_ARGS__); Log::get_instance()->flush();}

#endif

