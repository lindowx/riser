//
//  gent_command.h
//  riser
//
//  Created by wyt on 13-1-27.
//  Copyright (c) 2013年 wyt. All rights reserved.
//

#ifndef riser_gent_filter_h
#define riser_gent_filter_h

#include "gent_command.h"
#include "gent_connect.h"

typedef struct token_sf {
    char *value; 
    size_t length;
} token_f;

class CommandTypeFilter
{
public:
	enum ct
	{
	 COMM_GET = 1,
 	 COMM_SET = 2,
 	 COMM_DEL = 3,
 	 COMM_QUIT = 4,
     COMM_STATS = 5,
	};

};


class GentFilter : public GentCommand
{
   uint64_t  rlbytes;
   uint32_t  remains;	
   
   uint8_t commandtype; 
   string keystr;
   string content;
   string commandstr;
public:
    GentFilter(GentConnect *c=NULL);
    ~GentFilter();
private:
   size_t TokenCommand(char *command, token_f *tokens, const size_t max_tokens);
   int ParseCommand(const string &str);
   uint8_t Split(const string &str, const string &delimit, vector<string> &v);
   int CommandWord();
   void AssignVal(token_f *tokens);
   void ProcessGet(string &);
    void ProcessSet(string &outstr, const string &cont);
   void ProcessStats(string &);
public:
   int Process(const char *rbuf, uint64_t size, string &outstr);	
   void Complete(string &outstr, const char *, uint64_t);
   GentCommand *Clone(GentConnect *);
   int GetStatus();
   bool Init(string &msg);
};


#endif
