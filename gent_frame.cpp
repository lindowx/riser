/*
 * gent_frame.cpp
 *
 *  Created on: 2012-2-19
 *      Author: wyt
 */
#include "prefine.h"
#include "gent_event.h"
#include "gent_frame.h"
#include "gent_app_mgr.h"
#include "gent_thread.h"
#include "gent_level.h"
#include "gent_redis.h"
 #include "gent_repl.h"

GentFrame *GentFrame::instance_ = NULL;

GentFrame *GentFrame::Instance() {
	if(NULL == instance_) {
		instance_ = new GentFrame();
	}
	return instance_;
}

void GentFrame::Unstance() {
	if(instance_ != NULL) {
		delete instance_;
	}
}

GentFrame::GentFrame() {
	//msg_.Resize(MAX_CONNECT);
	master_msg_.Resize(MAX_CONNECT);
}


GentFrame::~GentFrame() {
	Destory();
}

int GentFrame::Init(struct riserserver *server, const char *configfile)
{
    if(access(configfile, 0) == -1) {
        string str(configfile);
		INFO(GentLog::ERROR,str+" not exist, start fail");
		return false;
    }
    config.Parse(string(configfile,strlen(configfile)));
	if(!GentLog::setfd(config["log_file"])) {
		INFO(GentLog::ERROR,"open "+config["log_file"]+" error");
		return false;
	}
	GentLog::setLevel(config["log_level"]);
	s = server;
	//config info
    string msg;
    if(config["type"] == "" || config["type"] == "leveldb"){
        //GentLevel *p;
        //REGISTER_COMMAND(p, GentLevel);
        GentRedis *p;
        REGISTER_COMMAND(p, GentRedis);
		if(!p->Init(msg))
        {
			INFO(GentLog::ERROR, "database initialize failed");
            return false;
        }
    }
    GentRepMgr::Instance("master")->Init();
	/*
	else if(config["type"] == "filter") {
		GentFilter *p;
		REGISTER_COMMAND(p, GentFilter);
		if(!p->Init(msg))              
		{                              
    		return false;              
		}                              
	}
	*/
    return true;

}

int GentFrame::Socket() {
	int listenfd;
	int flags = 1;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd == -1) {
		cout << "ERROR socket create failed." << endl;	
		return -1;
	}
	if( setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(int)) == -1) {
		cout << "ERROR set socket option failed." << endl;
		return -1;
	}
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&flags, sizeof(flags));
    
    
    struct linger so_linger={0,0};
    setsockopt(listenfd, SOL_SOCKET, SO_LINGER, (void *)&so_linger, sizeof(so_linger));
    setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flags, sizeof(flags));

    
	if ((flags = fcntl(listenfd, F_GETFL, 0)) < 0 ||
	        fcntl(listenfd, F_SETFL, flags | O_NONBLOCK) < 0) {
		perror("setting nonblock failed.");	
	    close(listenfd);
	    return -1;
	}
	return listenfd;
}

int GentFrame::ServerSocket(int port)  {
	int fd;
    if(port == -1) {
        port = atoi(config["port"].c_str());
    }
	s->port = port;
	if((fd = Socket()) == -1) {
		return -1;
	}
	snprintf(s->server_id,sizeof(s->server_id),"%s_%d",SERVER_ID.c_str(),port);
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if(config["host"] == "") {
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}else{
		addr.sin_addr.s_addr = inet_addr(config["host"].c_str());
	}
	char str[50]={0};
	if(bind(fd,(struct sockaddr *)&addr,sizeof(addr)) == -1 ) {
		sprintf(str, "bind port %d failed",port);
		INFO(GentLog::ERROR, str);
		close(fd);
		return -1;
	}
	if(listen(fd,1024) == -1) {
		INFO(GentLog::ERROR, "listen failed.");	
		close(fd);
		return -1;
	}
	sprintf(str, "bind port %d sucess",port);
	INFO(GentLog::INFO, str);
	return fd;
}

int GentFrame::Run(int port) {
    int fd = ServerSocket(port);
	if(fd <= 0) {
		return -1;
	}
	GentAppMgr::Instance();
	int num = (config["threads"] == "")?1:atoi(config["threads"].c_str());
	for(int i=1; i<=num; i++) {
		GENT_MSG_ITEM mitem;
		msg_[i] = mitem;
		msg_[i].Resize(MAX_CONNECT);
	}
	//master_msg_.Resize(MAX_CONNECT);
	GentThread::Intance()->init(fd, num);
	//启动线程
	GentThread::Intance()->Start();
    return 0;
}

int GentFrame::Register(int key, GentBasic *app) {
	MODULE_MAP::iterator mit = modules_.find(key);
	if(mit != modules_.end()) {
		return 0;
	}
	modules_[key] = app;
	GentAppMgr::Instance()->Register(key,app);
	return 0;
}

void GentFrame::Destory() {
	MODULE_MAP::iterator it;
	for(it = modules_.begin();it!=modules_.end(); ++it) {
		//delete it->second;
		//it->second = NULL;
	}
}

int GentFrame::GetModule(GentBasic *&app, int cmd) {
	MODULE_MAP::iterator mit = modules_.find(cmd);
	if(mit == modules_.end()) {
		return -1;
	}
	app = modules_[cmd];
	return 0;
}

