#ifndef CLIENT_H
#define CLIENT_H

class Client{
    public:
        //Called by driver, should spawn off worker threads to do work
        virtual void run() = 0;
    private:
        //Function that is parallelized by run function
        virtual void work() = 0;
};

#endif
