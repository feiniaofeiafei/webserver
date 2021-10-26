#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <pthread.h>
#include "log.h"
using namespace std;

Log::Log() {
    m_count = 0;
    m_async = false;
}
Log::~Log() {
    if(m_fp!=nullptr){
        fclose(m_fp);
    }
}
bool Log::init(const char *file_name, int close_log, int log_buf_size, int split_lines, int max_queue_size) {
    if (max_queue_size>0) {
        m_async = true;
        m_log_queue = new block_queue<string>(max_queue_size);
        pthread_t tid;
        pthread_create(&tid,NULL,flush_log_thread,NULL);
    }

    m_close_log = close_log;
    m_log_buf_size = log_buf_size;
    m_buf = new char[m_log_buf_size];
    memset(m_buf,'\0',m_log_buf_size);
    m_split_lines = split_lines;

    time_t t = time(NULL);
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;

    const char *p = strrchr(file_name, '/');
    char log_full_name[256] = {0};

    if (p == NULL) {
        snprintf(log_full_name, 255, "%d_%02d_%02d_%s", my_tm.tm_year+1900, my_tm.tm_mon + 1, my_tm.tm_mday, file_name);
    } else {
        strcpy(m_log_name, p + 1);
        strncpy(m_dir_name, file_name, p - file_name + 1);
        snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s", m_dir_name, my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, m_log_name);
    }
    
    m_today = my_tm.tm_mday;
    m_fp = fopen(log_full_name,"a");
    if(m_fp==nullptr) {
        return false;
    }
    return true;
}
void Log::write_log(int level, const char* format, ...) {
    struct timeval now = {0,0};
    gettimeofday(&now,NULL);
    time_t t = now.tv_sec;
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;

    char s[16] = {0};
    switch(level){
        case 0 :{
            strcpy(s, "[Debug]:");
            break;
        }
        case 1 :{
            strcpy(s, "[Info]:");
            break;
        }
        case 2 :{
            strcpy(s, "[Warn]:");
            break;          
        }
        case 3 :{
            strcpy(s, "[Error]:");
            break;
        }
        default:{
            strcpy(s, "[Info]:");
            break;        
        }
    }
    m_mutex.lock();
    m_count++;

    if (m_today != my_tm.tm_mday || m_count % m_split_lines == 0) //everyday log
    {
        
        char new_log[256] = {0};
        fflush(m_fp);
        fclose(m_fp);
        char tail[16] = {0};
       
        snprintf(tail, 16, "%d_%02d_%02d_", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday);
       
        if (m_today != my_tm.tm_mday)
        {
            snprintf(new_log, 255, "%s%s%s", m_dir_name, tail, m_log_name);
            m_today = my_tm.tm_mday;
            m_count = 0;
        }
        else
        {
            snprintf(new_log, 255, "%s%s%s.%lld", m_dir_name, tail, m_log_name, m_count / m_split_lines);
        }
        m_fp = fopen(new_log, "a");
    }
 
    m_mutex.unlock();


    va_list valst;
    va_start(valst,format);
    
    string log_record;
    m_mutex.lock();
    //向log_record中写入数据
    int n = snprintf(m_buf, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",
                    my_tm.tm_year+1900, my_tm.tm_mon+1, my_tm.tm_mday,
                    my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, now.tv_usec, s);
    int m = vsnprintf(m_buf+n, m_log_buf_size-1, format, valst);
    m_buf[n+m] = '\n';
    m_buf[n+m+1] = '\0';
    log_record = m_buf;
    m_mutex.unlock();
    
    if (m_async && !m_log_queue->full()) {
        m_log_queue->push(log_record);    
    }else{
        m_mutex.lock();
        fputs(log_record.c_str(),m_fp);
        m_mutex.unlock();
    }
    va_end(valst);
}

void Log::flush(void) {
    m_mutex.lock();
    fflush(m_fp);
    m_mutex.unlock();
}
