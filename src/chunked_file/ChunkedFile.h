#ifndef CHUNKED_FILE_H
#define CHUNKED_FILE_H

#include <ctime>
#include <unistd.h>
#include <string>
#include <cstring>

class ChunkedFile {
    static const char FILE_SEP = '/';

    FILE* file;
    const long time_split_sec;
    const std::string root;
    const std::string file_pref;
    const std::string file_ext;
    time_t time_last_update;
    
public:
    ChunkedFile(const char* root_dir, const char* file_pref, const char* file_ext, long time_split_sec) :
        file(nullptr),
        time_split_sec(time_split_sec),
        root(root_dir),
        file_pref(file_pref),
        file_ext(file_ext),
        time_last_update(0)
    {
    }

    ChunkedFile(const ChunkedFile&) = delete;
    ChunkedFile& operator=(const ChunkedFile&) = delete;
    
    ChunkedFile(ChunkedFile&&) = delete;
    ChunkedFile& operator=(ChunkedFile&&) = delete;
    
    ~ChunkedFile()
    {
        close();
    }

    int open()
    {
        if(file)
            return -1;
        
        time_t cur_time = time(NULL);
        struct tm local_time;
        localtime_r(&cur_time, &local_time);
        
        std::string current_dir = dir_name(local_time);
        std::string current_file = file_name(local_time);
        
        if(create_dir(current_dir) == 0){
            file = open_file(current_file);
            if(file){
                time_last_update = cur_time;
            }
        }
        
        return file == nullptr;
    }
    
    void close()
    {
        if(file)
            fclose(file);
        file = nullptr;
    }

    bool update()
    {
        time_t time_current = time(NULL);
        if (time_current - time_last_update >= time_split_sec || not same_day(time_current, time_last_update)) {
            close();
            return open() == 0;
        }
        return false;
    }
    
    operator FILE*() const
    {
        return file;
    }

private:
    
    std::string file_name(struct tm time) const noexcept
    {
        return dir_name(time) + FILE_SEP + file_pref + format_data_time(time) + "." + file_ext;
    }
    
    std::string dir_name(struct tm time) const noexcept
    {
        return root + FILE_SEP + format_data(time);
    }
    
    static int create_dir(std::string dir_name) noexcept
    {
        std::string cmd = "mkdir -p " + dir_name;
        return system(cmd.c_str());
    }
    
    static FILE* open_file(std::string file_name) noexcept
    {
        return fopen(file_name.c_str(), "a");
    }

    static std::string format_data(struct tm time) noexcept
    {
        constexpr unsigned BUF_SIZE = 64;
        char buffer[BUF_SIZE];

        snprintf(buffer, BUF_SIZE, "%04d-%02d-%02d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
        return std::string(buffer);
    }
    
    static std::string format_data_time(struct tm time) noexcept
    {
        constexpr unsigned BUF_SIZE = 64;
        char buffer[BUF_SIZE];

        snprintf(buffer, BUF_SIZE, "%04d-%02d-%02d_%02d-%02d-%02d",
            time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
            time.tm_hour, time.tm_min, time.tm_sec
            );
        return std::string(buffer);
    }
    
    static bool same_day(time_t first, time_t second)
    {
        struct tm fr;
        struct tm sc;
        
        localtime_r(&first, &fr);
        localtime_r(&second, &sc);
        
        return fr.tm_year == sc.tm_year && fr.tm_yday == sc.tm_yday;
    }
    
};

#endif /* CHUNKED_FILE_H */

