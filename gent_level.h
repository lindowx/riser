//
//  gent_command.h
//  riser
//
//  Created by wyt on 13-1-27.
//  Copyright (c) 2013年 wyt. All rights reserved.
//

#ifndef riser_gent_event_h
#define riser_gent_event_h

#include "gent_command.h"
#include "gent_connect.h"

typedef struct token_s {
    char *value; 
    size_t length;
} token_t;

class CommandType
{
public:
	enum ct
	{
	 COMM_GET = 1,
 	 COMM_SET = 2,
 	 COMM_DEL = 3,
 	 COMM_QUIT = 4,
	};

};


class GentLevel : public GentCommand
{
  // char   *rcurr;  /** but if we parsed some already, this is where we stopped */
   
   char   *wbuf;
   char   *wcurr;
   int    wsize;
   int    wbytes;
   /** which state to go into after finishing current write */
   //void   *write_and_free; /** free this memory after finishing writing */
   
   char   *ritem;  /** when we read in an item's value, it goes here */
   int    rlbytes;
   int 	  remains;	
   
   int 	  commandtype; 
   string keystr;
   string content;    
public:
    GentLevel(GentConnect *c);
    ~GentLevel();
private:
   size_t TokenCommand(char *command, token_t *tokens, const size_t max_tokens);
   int ParseCommand();
   int ParseCommand2(const string &str);
   uint8_t Split(const string &str, const string &delimit, vector<string> &v);
   int CommandWord(string &data);
   void AssignVal(token_t *tokens);
    void ProcessGet(string &);
public:
   int Process(const char *rbuf, uint64_t size, string &outstr);	
   void Complete(string &outstr, const char *);
   int GetStatus();
};


#endif
