//
//  gent_db.h
//  riser
//
//  Created by wyt on 13-3-24.
//  Copyright (c) 2013年 wyt. All rights reserved.
//

#ifndef riser_gent_db_h
#define riser_gent_db_h
#include <leveldb/db.h>
#include <leveldb/cache.h>
#include <leveldb/filter_policy.h>
#include "prefine.h"
#include "gent_command.h"

class GentDb
{
	static GentDb *intance_;
private:
	leveldb::DB* db;
    leveldb::Options options;
	string pathname;
	uint64_t key_num;
	CommLock key_num_lock;		
public:
	GentDb();
	~GentDb();
public:
    bool Init(string &err);
    bool Put(string &key, string &value);
    bool Put(string &key, const char *val, uint64_t len);
    bool Get(string &key,string &value);
    bool Del(string &key);
	uint64_t Count(const string &pre="");
	uint64_t Keys(vector<string> &outvec, const string &pre="*");
	uint64_t TotalSize();
	string &GetPath();
private:
	bool GetPathname(string &);
public:
	static GentDb *Instance();
	static void UnIntance();
};

#endif
