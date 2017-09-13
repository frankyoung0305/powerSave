// UNIX IPC functionalities  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <errno.h>  
#include <fcntl.h>  
#include <string.h>  
// POSIX Message Queue  
#include <mqueue.h>  
  
// C++ header files  
#include <iostream>  
#include <sstream>  
#include <string>  
  
  
int receiveMsg(mqd_t *mqdes)  
{  
    unsigned int prio;   // priority of the message  
    struct mq_attr attr; // attribute  
    ssize_t numRead;     // size of message  
  
    if (mqdes == NULL)  
    {  
        std::cout << "mqdes == NULL" << std::endl;  
        exit(EXIT_FAILURE);  
    }  
  
    do  
    {  
        sleep(1);  
  
        std::cout << "Check for new message." << std::endl;  
  
        if (mq_getattr(*mqdes, &attr) == -1)  
        {  
            // this is an error  
            std::cout << "In receiveMsg(), get attribute error." << std::endl;  
            exit(EXIT_FAILURE);  
        }  
    }while (attr.mq_curmsgs == 0);  
  
    // allocate memory for the incoming messages  
    char * buffer = (char *)malloc(attr.mq_msgsize);  
    if (buffer == NULL)  
    {  
        std::cout << "In receiveMsg(), malloc error." << std::endl;  
        exit(EXIT_FAILURE);  
    }  
    memset(buffer,0,attr.mq_msgsize);  
    // test use  
//  memset(buffer,1,attr.mq_msgsize);  
  
    for (int i=0;i<attr.mq_curmsgs;++i)  
    {  
        numRead = mq_receive(*mqdes, buffer, attr.mq_msgsize, &prio);  
        if (numRead == -1)  
        {  
            std::cout << "In receiveMsg(), mq_receive() error." << std::endl;  
            exit(EXIT_FAILURE);  
        }  
  
        // print the message to the standard output  
        std::cout << "In receiveMsg(), buffer = "  
                  << buffer  
                  << ", with strlen(buffer) = "  
                  << strlen(buffer)  
                  << std::endl;  
  
        std::cout << "buffer = ";  
  
        for (int j=0;j<strlen(buffer);++j)  
        {  
            std::cout << (unsigned int)(buffer[j]) << " ";  
        }  
  
        std::cout << std::endl;  
    }  
  
    free(buffer);  
  
    return 0;  
}  
  
int main(int argc, char *argv[])  
{  
    // create the POSIX message queue  
    mqd_t mqdes;         // descriptor  
  
    mqdes = mq_open("/mymq", O_CREAT | O_RDWR | O_NONBLOCK, 0666, NULL);  
  
    if (mqdes == (mqd_t) -1)  
    {  
        std::cout << "Open MQ failed!" << std::endl;  
        exit(EXIT_FAILURE);  
    }  
  
    // begin to receive message  
    if (receiveMsg(&mqdes) != 0)  
    {  
        std::cout << "receiveMsg() returns error code." << std::endl;  
    }  
  
    mq_close(mqdes);  
    mq_unlink("/mymq");  
  
    exit(EXIT_SUCCESS);  
}
