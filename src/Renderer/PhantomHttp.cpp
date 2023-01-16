﻿//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <Windows.h>
#endif
#include "PhantomHttp.h"
#include "PhantomManager.h"

namespace Phantom{
	JsonValue&	JsonObject::NewValue()
	{
		m_numValues++;
		if(m_values.size() > m_numValues)
		{
			m_values[m_numValues-1].name.t[0] = 0;
			m_values[m_numValues-1].svalue.t[0] = 0;
			return m_values[m_numValues-1];
		}
		JsonValue v;
		m_values.push_back(v);
		return m_values[m_numValues - 1];
	}
	JsonObject::JsonObject(){
		m_numValues = 0;
		m_values.setGrow(5);
	}
	BOOL		JsonObject::exist(const char* szName)
	{
		for(int i=0;i<m_numValues;i++)
		{
			if(m_values[i].name == szName)
				return true;
		}
		return false;
	}
	JsonValue&	JsonObject::operator [](const char* szName)
	{
		for(int i=0;i<m_numValues;i++)
		{
			if(m_values[i].name == szName)
				return m_values[i];
		}
		JsonValue& v = NewValue();
		v.name = szName;
		return v;
	}
	JsonObject::~JsonObject(){
	}
	inline	char*	addtext(char* r, const char* t){
		int len = strlen(t);
		for(int i=0;i<=len;i++)
		{
			*r = t[i];
			if(*r=='\'' && *(r-1) != '\\')
				*r = '\"';
			if(i==len)
			{
				*r = '\"';
				return r;
			}
			r++;
		}
		return r;
	}
	JsonObject&	JsonObject::add(const char* szName, const char* szValue)
	{
		JsonValue& v = NewValue();
		v.name = szName;
		v = szValue;
		return *this;
	}
	JsonObject&	JsonObject::add(const char* szName, int nValue)
	{
		JsonValue& v = NewValue();
		v.name = szName;
		v = nValue;
		return *this;
	}
	VOID		JsonObject::BuildText(char* inret)
	{
		char* rets = inret;
		rets = addtext(rets, "{");
		for(int i=0;i<m_numValues;i++)
		{
			rets = addtext(rets, "\"");
			rets = addtext(rets, m_values[i].name.t);
			rets = addtext(rets, "\":\"");
			rets = addtext(rets, m_values[i].svalue.t);
			rets = addtext(rets, "\"");
			if(i < (int)m_values.size() - 1)
				rets = addtext(rets, ",");
		}
		rets = addtext(rets, "}");
		*rets = 0;
	}

	HttpRequest::HttpRequest()
	{
		m_hSocket = InvalidSocket;
		m_pJsonCallback	=	0;
		m_jsonCount	=	0;
		memset(m_szHost, 0, sizeof(m_szHost));
		memset(m_szPage, 0, sizeof(m_szPage));
		m_bFail	=	false;
		m_bUseThread	=	false;
		m_bRequestComplete = false;
		m_nResult = 0;
	}
	HttpRequest::~HttpRequest()
	{
		//g_manager.PopHttp(this);
		if(m_pJsonCallback)
			delete m_pJsonCallback;
		m_pJsonCallback=0;
		cancel();
	}
	VOID		HttpRequest::cancel()
	{
		m_thread.Stop();
		if(m_hSocket	!=	InvalidSocket)
		{
			closesocket(m_hSocket);
			m_hSocket	=	InvalidSocket;
		}
	}
	BOOL		HttpRequest::call(const char* szFormat, ...)//连接服务器,并发送一段json数据
	{
		if(!this->m_szHost[0])
		{
			return false;
		}
		m_nResult = 0;
		sprintf(m_requestText.t, "%s?code=", m_szPage);
		int num = m_requestText.size();
		char* szText = m_requestText.t + num;
		char buf[4096];
		va_list args;
		va_start(args, szFormat);
		vsnprintf(buf,4096,szFormat,args);
		va_end(args);
		int len = strlen(buf);
		for(int i=0;i<len;i++)
			if(buf[i]=='\'')
				buf[i] = '\"';
		//LogInfo("json:send(%s)", buf);
		if(!urlencode(szText, 4096, buf, len))
			return false;
		if(m_bUseThread)
		{
			m_bRequestComplete = false;
			this->m_thread.Start(this);
			return true;
		}
		this->OnThread();
		this->DoMessageQueue();
		return true;
	}
	BOOL		HttpRequest::request(const char* requestText)//连接服务器,并发送一段json数据
	{
		if(!this->m_szHost[0])
			return false;
		strcpy(m_requestText.t, requestText);
		if(m_bUseThread)
		{
			m_bRequestComplete = false;
			this->m_thread.Start(this);
			return true;
		}
		this->OnThread();
		this->DoMessageQueue();
		return true;
	}
	BOOL		HttpRequest::call(JsonObject& json)//连接服务器
	{
		if(!this->m_szHost[0])
			return false;
		if(json.size()>0){
		sprintf(m_requestText.t, "%s?code=", m_szPage);
#ifdef ANDROID_OS
		json["platform"] = PLATFORM_ANDROID;
#else
		json["platform"] = PLATFORM_OSX;
#endif
		int num = m_requestText.size();
		char* szText = m_requestText.t + num;
		char buf[4096];
		json.BuildText(buf);
		//LogInfo("json:send(%s)", buf);
		if(!urlencode(szText, 4096, buf))
			return false;
		}
		else
		{
			sprintf(m_requestText.t, m_szPage);
		}
		if(m_bUseThread)
		{
			m_bRequestComplete = false;
			this->m_thread.Start(this);
			return true;
		}
		this->OnThread();
		this->DoMessageQueue();
		return true;
	}
	BOOL		HttpRequest::RequestPage()	//需求某个页面执行
	{
		const int bufferSize = 8192;
		char szRequest[bufferSize];
		sprintf(szRequest, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: HTMLGET 1.0\r\n\r\n", m_requestText.t, this->m_szHost);
		send(this->m_hSocket, szRequest, strlen(szRequest), 0);
		//
		//LogInfo("REQ:%s", szRequest);
		memset(szRequest, 0, sizeof(szRequest));
		int tmpres;
		//
		int htmlstart = 0;
		char * htmlcontent;
		TypeArray<char> temp;
		temp.resize(0);
		this->m_nResult = 0;
		int tv = 3000;//10分钟内如果没有任何消息响应则默认是断开，自动关闭连接
		setsockopt(m_hSocket,	SOL_SOCKET,	SO_RCVTIMEO,	(char*)&tv,sizeof(int));
		while((tmpres = recv(m_hSocket, szRequest, bufferSize, 0)) > 0)
		{
			if(htmlstart == 0)
			{
				char* szcode = strstr(szRequest, "code:");
				if(szcode)
				{
					szcode += 5;
					if(szcode[0] == ' ')szcode++;
					m_nResult = atoi(szcode);
				}
				htmlcontent = strstr(szRequest, "\r\n\r\n");
				if(htmlcontent != NULL){
					htmlstart = 1;
					htmlcontent += 4;
					char* newcontent = strstr(htmlcontent, "{");
					if(newcontent)
						htmlcontent = newcontent;
				}
			}else{
				htmlcontent = szRequest;
			}
			if(htmlstart){
				int nInsert = strlen(htmlcontent);
				if(nInsert > 0){
					int nOldCount = temp.GetCount();
					temp.resize(nOldCount + nInsert);
					memcpy(temp.GetPtr() + nOldCount, htmlcontent, nInsert);
				}
			}
			memset(szRequest, 0, tmpres);
		}
		//m_lock.Lock();
		if(temp.size() > 0)
		{
			int oldSize = m_recvBuffers.size();
			m_recvBuffers.resize(oldSize + temp.size());
			memcpy(m_recvBuffers.GetPtr() + oldSize, temp.GetPtr(), temp.size());
		}
		//m_lock.Unlock();
		if(tmpres < 0)
		{
			return false;
		}
		closesocket(m_hSocket);
		m_hSocket	=	InvalidSocket;
		//
		m_bRequestComplete = true;
		return true;
	}
	VOID		ExplodeHost(const char* szJsonPHPUrl, char* szHost, char* szPage)
	{
		int len = strlen(szJsonPHPUrl);
		szHost[0] = 0;
		szPage[0] = 0;
		int nState = 0;
		int nLastBegin = 0;
		for(int i=0;i<len;i++)
		{
			if(szJsonPHPUrl[i]==':' && szJsonPHPUrl[i+1]=='/' && szJsonPHPUrl[i+2]=='/')
			{
				i+=2;
				nLastBegin = i;
				nState = 1;
			}
			else if(nState == 1 && szJsonPHPUrl[i] == '/')
			{
				strncpy(szHost, szJsonPHPUrl + nLastBegin + 1, (i-nLastBegin - 1));
				szHost[i-nLastBegin - 1] = 0;
				strcpy(szPage, szJsonPHPUrl + i);
				nState++;
				break;
			}
		}
		if(nState == 1)
		{
			strcpy(szHost, szJsonPHPUrl + nLastBegin);
		}
	}
	VOID		HttpRequest::Initialize(const char* szJsonPHPUrl)
	{
		ExplodeHost(szJsonPHPUrl, m_szHost, m_szPage);
	}
	BOOL		HttpRequest::Connect(const char* m_szUrl)//连接服务器
	{
		if(m_hSocket != InvalidSocket)
			cancel();
		WORD  wPort = 80;
		m_hSocket	=	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);//socket(m_info.af, m_info.type, m_info.protocol);//, NULL, 0, 0);
		if(m_hSocket == InvalidSocket)
			return false;
		int maxBuffer	=	1024 * 1024;
		int yes=0;
		setsockopt(m_hSocket,	SOL_SOCKET,	SO_REUSEADDR,	(char*)&yes,		sizeof(int));
		setsockopt(m_hSocket,	SOL_SOCKET,	SO_RCVBUF,		(char*)&maxBuffer,sizeof(int));
		setsockopt(m_hSocket,	SOL_SOCKET,	SO_SNDBUF,		(char*)&maxBuffer,sizeof(int));
		int tv = 3000;//10分钟内如果没有任何消息响应则默认是断开，自动关闭连接
		setsockopt(m_hSocket,	SOL_SOCKET,	SO_RCVTIMEO,	(char*)&tv,sizeof(int));

		//
		sockaddr_in addr;
		memset(&addr,0,sizeof(sockaddr_in));
		addr.sin_addr.s_addr		=	inet_addr( this->m_szHost );
		if( addr.sin_addr.s_addr == 0xffffffff)
		{
			hostent* ent = gethostbyname( m_szHost );
			if(ent == NULL)
			{
				SetLastSockError(SocketError_FailIP);
				closesocket(m_hSocket);
				m_hSocket = InvalidSocket;
				return false;
			}

			//addr.sin_addr.s_addr	=	((in_addr*)ent->h_addr_list)->s_addr;
			char ip[128];
			memset(ip, 0, sizeof(ip));
			//			if(inet_ntop(AF_INET, (void *)ent->h_addr_list[0], ip, iplen) == NULL)
			//			return false;
			in_addr** addr_list = (struct in_addr **)ent->h_addr_list;
			for(int i = 0; addr_list[i] != NULL; i++) {
				if(i > 0)strcat(ip, ",");
				strcat(ip, inet_ntoa(*addr_list[i]));
			}
			addr.sin_addr.s_addr		=	inet_addr( ip );
		}
		addr.sin_family					=	AF_INET;
		addr.sin_port					=	htons( wPort );
		int	nError	=	::connect(m_hSocket, (sockaddr*)&addr, sizeof(sockaddr_in));
		if(nError != 0)
		{
			closesocket(m_hSocket);
			m_hSocket	=	InvalidSocket;
			SetLastSockError(SocketError_FailConnect);
			return false;
		}
		return true;
	}

	inline BYTE toHex(BYTE x){
		return x > 9 ? x -10 + 'A': x + '0';
	}
	inline BYTE fromHex(BYTE x){
		return isdigit(x) ? x-'0' : x-'A'+10;
	}

	bool	urlencode(char* ret, int bufferLength, const char* sIn, int len)
	{
		if(len == 0)
			len = strlen(sIn);
		int index = 0;
		for( int ix = 0; ix < len; ix++ )
		{
			BYTE buf[4];
			memset( buf, 0, 4 );
			if( isalnum( (BYTE)sIn[ix] ) )
			{
				buf[0] = sIn[ix];
			}
			else
			{
				buf[0] = '%';
				buf[1] = toHex( (BYTE)sIn[ix] >> 4 );
				buf[2] = toHex( (BYTE)sIn[ix] % 16);
			}
			for(int i=0;i<4;i++)
				if(buf[i]!=0)
				{
					if(index >= bufferLength)
						return false;
					ret[index++] = buf[i];
				}
		}
		ret[index++] = 0;
		return true;
	};

	char*	urldecode(char* inOut)
	{
		int len = strlen(inOut);
		int out = 0;
		for( int ix = 0; ix < len; ix++ )
		{
			BYTE ch = 0;
			if(inOut[ix]=='%')
			{
				ch = (fromHex(inOut[ix+1])<<4);
				ch |= fromHex(inOut[ix+2]);
				ix += 2;
			}
			else if(inOut[ix] == '+')
			{
				ch = ' ';
			}
			else if(inOut[ix] == '\\' && inOut[ix+1] == '/')
			{
				ch = '/';
				ix ++;
			}
			else
			{
				ch = inOut[ix];
			}
			inOut[out++] = ch;
		}
		inOut[out++] = 0;
		return inOut;
	}
	int			ParseValue(NameT<64>& name, char* value, int valueLen, const char* szRecv, int nBegin, int nLength)
	{
		if(szRecv[nBegin] == '\"')
		{
			int idx = 0;
			nBegin++;
			while(nBegin<nLength)
			{
				if(szRecv[nBegin-1]!='\\' && szRecv[nBegin] == '\"')
				{
					name.t[idx] = 0;
					break;
				}
				if(idx >= name.Length)
					return -1;
				name.t[idx++] = szRecv[nBegin];
				nBegin++;
			}
			nBegin++;
			if(szRecv[nBegin]!=':')
				return -1;
			nBegin++;
			idx = 0;
			if(szRecv[nBegin-1]!='\\' && szRecv[nBegin] == '\"')
			{
				nBegin++;
				while(nBegin<nLength)
				{
					if(szRecv[nBegin-1]!='\\' && szRecv[nBegin] == '\"')
					{
						value[idx++] = 0;
						urldecode(value);
						return nBegin;
					}
					if(idx >= valueLen)
						return -1;
					value[idx++] = szRecv[nBegin];
					nBegin++;
				}
			}
			else
			{
				while(nBegin<nLength)
				{
					if(szRecv[nBegin] == ',' || szRecv[nBegin] == '}')
					{
						value[idx++] = 0;
						return nBegin-1;
					}
					if(idx >= valueLen)
						return -1;
					value[idx++] = szRecv[nBegin];
					nBegin++;
				}
			}
			return -1;
		}
		return -1;
	}
	//基础接口
	VOID		HttpRequest::DoMessageQueue()
	{
		//m_lock.Lock();
		if(m_bFail){
			m_bFail = false;
			if(m_pJsonCallback)
				this->m_pJsonCallback->OnJsonAnalyseFail("connect", "连接服务器失败\n请检查网络是否稳定\n或者联系 http://www.aixspace.com");
		}
		if(m_bRequestComplete && this->m_recvBuffers.size() > 0)
		{
			int index = 0;
			while(index<m_recvBuffers.size())
			{
				if(m_recvBuffers[index] == '{')
					break;
				else
					index++;
            }
			if(index==m_recvBuffers.size())
			{
				if(m_pJsonCallback)
					this->m_pJsonCallback->OnJsonAnalyseFail("request", m_recvBuffers.GetPtr());
				return;
			}

			if(m_recvBuffers[index] != '{')//如果返回的值是有问题的则不能被调用
			{
				if(m_pJsonCallback)
					this->m_pJsonCallback->OnJsonAnalyseFail("request", m_recvBuffers.GetPtr());
			}
			else
			{
				int nBegin = 0;
				const char* szRecv = m_recvBuffers.GetPtr();
				int length = m_recvBuffers.size();
				int chBegin = 0;
				NameT<64> name;
				while(nBegin < length)
				{
					char ch = szRecv[nBegin];
					if(ch == '{')
					{
					}
					else if(ch == '}')
					{
						
					}
					else if(szRecv[nBegin-1]!='\\' && ch == '\"')
					{
						int ret = ParseValue(name, m_jsonCode.t,m_jsonCode.Length, szRecv, nBegin, length);
						if(ret < 0)
						{
							this->m_pJsonCallback->OnJsonAnalyseFail("len>65535", m_recvBuffers.GetPtr());
							return;
						}
						if(name != "count")//如果不是数量
						{
							urldecode(m_jsonCode.t);
							if(!OnParseJson(m_jsonCode.t))
							{
								return;
							}
							//到这里运行解析代码,每一个value都是单独的json代码
						}
						nBegin = ret;
					}
					nBegin++;
				}
			}
			m_recvBuffers.resize(0);
		}
		//m_lock.Unlock();
	}
	BOOL		HttpRequest::OnParseJson(const char* szText)
	{
		//LogInfo("json:%s", szText);
		int length = strlen(szText);
		int nBegin = 0;
		m_tempJson.Reset();
		while(nBegin < length)
		{
			const char* begin = szText + nBegin;
			char ch = *begin;
			if(ch == '{')
			{
			}
			else if(ch == '}')
			{
			}
			else if(ch == '\"')
			{
				JsonValue& v = m_tempJson.NewValue();
				int ret = ParseValue(v.name, v.svalue.t, v.svalue.Length, szText, nBegin, length);
				if(ret < 0)
				{
					if(m_pJsonCallback)
						this->m_pJsonCallback->OnJsonAnalyseFail("one>2048", szText);
					return false;
				}
				nBegin = ret;
			}
			nBegin++;
		}
		if(m_tempJson.size()>0 && m_pJsonCallback)
			m_pJsonCallback->OnJsonAnalyse(m_tempJson);
		return true;
	}
	VOID		HttpRequest::OnThread()
	{
		m_bFail	=	false;
		if(!Connect(m_szHost))
		{
			m_bFail	=	true;
			return;
		}
		RequestPage();
	}
	AuthHttpRequest::AuthHttpRequest()
	{
		m_bRequest	=	false;
		m_pJsonCallback	=	this;
	}
	AuthHttpRequest::~AuthHttpRequest()
	{
		m_pJsonCallback	=	0;
	}
	VOID		AuthHttpRequest::OnThread()
	{
		m_bFail	=	false;
		if(!Connect(m_szHost))
		{
			m_bFail	=	true;
			m_bRequest = 1;
			return;
		}
		RequestPage();
		DoMessageQueue();
		m_bRequest	=	false;
	}
	VOID		AuthHttpRequest::OnJsonAnalyse(JsonObject& obj)
	{
	}
	VOID		AuthHttpRequest::OnJsonAnalyseFail(const char* type, const char* szFailText)
	{

	}

	class	HttpRequestOnce: public HttpRequest, public IJsonAnalyseCallback
	{
	public:
		Name				m_file;
		void*			m_param;
		fnHttpCallback	m_callback;
		HttpRequestOnce()
		{
			m_param	=	0;
			m_callback = 0;
			m_bRequest	=	0;
			m_pJsonCallback	=	this;
		}
		~HttpRequestOnce()
		{
			m_pJsonCallback	=	0;
		}
		VOID		OnThread()
		{
			m_bFail	=	false;
			if(!Connect(m_szHost))
			{
				m_bFail	=	true;
				m_bRequest	=	1;
				return;
			}
			RequestPage();
			m_bRequest	=	1;
		}

		VOID		OnJsonAnalyseFail(const char* type, const char* szFailText)
		{
			if(m_callback)
			{
				JsonObject obj;
				(*m_callback)(m_file, obj, szFailText, m_param);
			}
		}
		VOID		OnJsonAnalyse(JsonObject& obj)
		{
			if(m_callback)
				(*m_callback)(m_file, obj, 0, m_param);
		}
		BOOL		m_bRequest;
	};
    
	char	g_szServerIP[128] ={0};
	const char*	GetServerIP(){
		return g_szServerIP;
	}
	void	SetServerIP(const char* ip)
	{
        memset(g_szServerIP, 0, 128);
		strncpy(g_szServerIP, ip, 127);
	}
	VOID		RequestIP(fnHttpCallback cb, JsonObject& params, const char* szHome, void* param)
	{
		HttpRequestOnce* once = new HttpRequestOnce();
		once->m_bRequest	=	0;
		char buf[512];
		strcpy(buf, szHome);
		strcat(buf, "request.php");
		once->m_file = buf;
		once->Initialize(buf);
		once->m_callback = cb;
		once->m_param = param;
		once->SetUseThread(false);
		once->call(params);
		delete once;
	}
	TypeArray<HttpRequestOnce*>	g_httpRequest;
	VOID		Request(const char* infile, JsonObject& params, fnHttpCallback cb, BOOL bUseThread, void* param)
	{
		HttpRequestOnce* once = new HttpRequestOnce();
		if(bUseThread)
		{
			g_httpRequest.push_back(once);
		}
		once->m_bRequest	=	0;
		char buf[512];
		strcpy(buf, g_szServerIP);//
		strcat(buf, infile);
		strcat(buf, ".php");
		once->m_file = infile;
		once->Initialize(buf);
		once->m_callback = cb;
		once->m_param = param;
		once->SetUseThread(bUseThread);
		//LogInfo("Request:(%s)(%s)", buf, once->m_szHost);
		if(!once->m_szHost[0])
		{
			if(cb)
			{
				Name name = infile;
				JsonObject obj;
				(*cb)(name, obj, "网络故障,请检查网络是否完好,或者幻影官方是否可以正常访问", once->m_param);
			}
		}
		once->call(params);
		if(!bUseThread)
			delete once;
	}
	struct DownloadFileInfo
	{
		BOOL		complete;
		NameT<256>	request;
		NameT<256>	docFile;
		PDownloadComplte callback;
		BOOL		bMemory;
		BOOL		bThread;
	};
	TypeArray<DownloadFileInfo*>	g_completes;//完成列表
	VOID		OnDownloadFrame()
	{
		for(int i=0;i<g_httpRequest.size();i++)
		{
			if(g_httpRequest[i]->m_bRequest == 1){
				g_httpRequest[i]->DoMessageQueue();
				delete g_httpRequest[i];
				g_httpRequest.eraseByIndex(i);
				i--;
			}
		}
		for(int i=0;i<g_completes.size();i++)
		{
			if(g_completes[i]->complete != 0)
			{
				if(g_completes[i]->callback)
					(*g_completes[i]->callback)(g_completes[i]->request.t, g_completes[i]->docFile.t, g_completes[i]->complete < 0, 0, 0);
				delete g_completes[i];
				g_completes.eraseByIndex(i, 1, true);
			}
		}
	}
	//
	VOID		DownloadProc(void* parameter)
	{
		DownloadFileInfo* info = (DownloadFileInfo*)parameter;
		//
		char szHost[256], szPage[256];
		ExplodeHost(info->request.t, szHost, szPage);
		Socket	hSocket;
		WORD  wPort = 80;
		hSocket	=	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(hSocket == InvalidSocket)
		{
			info->complete = -1;
			if(!info->bThread)
				delete info;
			return;
		}
		int maxBuffer	=	1024 * 1024;
		int yes=0;
		setsockopt(hSocket,	SOL_SOCKET,	SO_REUSEADDR,	(char*)&yes,		sizeof(int));
		setsockopt(hSocket,	SOL_SOCKET,	SO_RCVBUF,		(char*)&maxBuffer,sizeof(int));
		setsockopt(hSocket,	SOL_SOCKET,	SO_SNDBUF,		(char*)&maxBuffer,sizeof(int));
		//
		sockaddr_in addr;
		memset(&addr,0,sizeof(sockaddr_in));
		addr.sin_addr.s_addr		=	inet_addr( szHost );
		if( addr.sin_addr.s_addr == 0xffffffff)
		{
			hostent* ent = gethostbyname( szHost );
			if(ent == NULL)
			{
				closesocket(hSocket);
				hSocket = InvalidSocket;
				info->complete = -1;
				if(!info->bThread)
					delete info;
				return;
			}
			char ip[128];
			memset(ip, 0, sizeof(ip));
			in_addr** addr_list = (struct in_addr **)ent->h_addr_list;
			for(int i = 0; addr_list[i] != NULL; i++) {
				if(i > 0)strcat(ip, ",");
				strcat(ip, inet_ntoa(*addr_list[i]));
			}
			addr.sin_addr.s_addr		=	inet_addr( ip );
		}
		addr.sin_family					=	AF_INET;
		addr.sin_port					=	htons( wPort );
		int	nError	=	::connect(hSocket, (sockaddr*)&addr, sizeof(sockaddr_in));
		if(nError != 0)
		{
			info->complete = -1;
			closesocket(hSocket);
			if(!info->bThread)
				delete info;
			return;
		}
		const int bufferSize = 8192;
		char szRequest[bufferSize];
		sprintf(szRequest, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: HTMLGET 1.0\r\nContent-Type: multipart/byteranges\r\nAccept: \r\n\r\n", szPage, szHost);
		send(hSocket, szRequest, strlen(szRequest), 0);
		memset(szRequest, 0, sizeof(szRequest));
		int tmpres;
		int htmlstart = 0;
		char * htmlcontent;
		TypeArray<char> temp;
		temp.resize(0);
		while((tmpres = recv(hSocket, szRequest, bufferSize, 0)) > 0)
		{
			int length = tmpres;
			if(htmlstart == 0)
			{
				htmlcontent = strstr(szRequest, "\r\n\r\n");
				if(htmlcontent != NULL){
					if(strstr(htmlcontent, "404"))
					{
						info->complete = -1;
						if(!info->bThread)
							delete info;
						return;
					}
					htmlstart = 1;
					htmlcontent += 4;
					length = tmpres - (htmlcontent - szRequest);
				}
			}
			else
				htmlcontent = szRequest;
			if(htmlstart){
				if(length > 0){
					int nOldCount = temp.GetCount();
					temp.resize(nOldCount + length);
					memcpy(temp.GetPtr() + nOldCount, htmlcontent, length);
				}
			}
		}
		closesocket(hSocket);
		if(temp.size() > 0)
		{
			if(info->docFile.t[0]){
				StreamBase* s = 0;
				if(!info->bMemory)
					s = create_s(info->docFile.t);
				else
					s = create_mem_s(info->docFile.t, temp.size());
				if(s)
				{
					s->write(temp.GetPtr(), temp.size());
					close_s(&s);
				}
			}
			info->complete = 1;
			if(!info->bThread)
			{
				if(info->callback)
					(*info->callback)(info->request.t, info->docFile.t, false, temp.GetPtr(), temp.size());
			}
		}
		else
			info->complete = -1;
		if(!info->bThread)
			delete info;
	}
	BOOL		downloadFile(const char* requestText, const char* indocFile, PDownloadComplte callback, BOOL bUseThread, BOOL bInMemory)
	{
		DownloadFileInfo* info = new DownloadFileInfo();
		info->request = requestText;
		info->docFile = indocFile;
		info->callback = callback;
		info->complete = false;
		info->bMemory = bInMemory;
		info->bThread = bUseThread;
		if(bUseThread){
			g_completes.push_back(info);
			createThread(DownloadProc, (void*)info);
		}
		else
		{
			DownloadProc(info);
		}
		return true;
	}

};
