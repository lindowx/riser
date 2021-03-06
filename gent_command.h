//
//  gent_command.h
//  riser
//
//  Created by wyt on 13-1-27.
//  Copyright (c) 2013年 wyt. All rights reserved.
//

#ifndef riser_gent_command_h
#define riser_gent_command_h
#include "prefine.h"
//#include "gent_connect.h"
class GentConnect;

class CommandType
{
public:
	enum ct
	{
	 COMM_GET = 1,
 	 COMM_SET = 2,
 	 COMM_DEL = 3,
 	 COMM_QUIT = 4,
     COMM_STATS = 5,
	 COMM_REP = 6,
	 COMM_KEYS = 7,
	 COMM_MGET = 8,
	 COMM_EXISTS = 9,
	 COMM_PING = 10,
	};

};

class GentCommand
{
public:
    GentConnect *conn;
    //char *rbuf;
    //char *rcurr;
	//char *rcont;
    int rsize;
    int rbytes;
	int auth;	
public:
    GentCommand(GentConnect *c);
    virtual ~GentCommand();
public:
    virtual int Process(const char *rbuf, uint64_t size, string &outstr) = 0;
    virtual void Complete(string &outstr, const char *recont, uint64_t len) = 0;
    virtual GentCommand *Clone(GentConnect *) = 0;
	virtual bool Init(string &msg) = 0;
	virtual int ContinueProcess(const char *cbuf, uint64_t size, string &outstr);
	virtual void Reset();
    void SetAuth(int ah) {auth = ah;};
public:
    static const int READ_BUFFER_SIZE = 4096;
};


#endif
