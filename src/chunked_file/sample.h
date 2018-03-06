#ifndef CHUNKED_FILE_SAMPLE_H
#define CHUNKED_FILE_SAMPLE_H

#include "ChunkedFile.h"

int chunked_file_sample(int, char**)
{
    ChunkedFile cfile("tmp", "pref", "log", 10);
    if(cfile.open()){
        printf("cannot create file");
        return -1;
    }
    
    unsigned data_to_write = 0;
    
    while(1){
        fprintf(cfile, "%u\n", data_to_write++);
        fflush(cfile);
        cfile.update();
        
        sleep(1);
    }
    std::cout << "<---- the end of main() ---->\n";
    return 0;
}

#endif /* CHUNKED_FILE_SAMPLE_H */

