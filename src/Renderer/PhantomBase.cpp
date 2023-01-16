//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <Windows.h>
#endif

#include "PhantomBase.h"
#include "PhantomManager.h"
#include "encrypt.h"

#ifndef WIN32_OS
    extern "C"{
        int Utf8ToWideOS(const char* text, int textlen, unsigned short* ret);
        int WideToUtf8OS(const unsigned short* text, int textlen, char* ret);
    };
#endif
	PHANTOM_API VOID MyLogDebug(const char* szText)//输出信息
	{
#ifdef WIN32_OS
		::OutputDebugStringA(szText);
		::OutputDebugStringA("\r\n");
#elif defined(IPHONE_OS)
        LogPrint("%s", szText);
#elif defined(ANDROID_OS)
        __android_log_print(ANDROID_LOG_INFO, "PhantomGameEngine", szText);
#endif
	}
	MyLogPrint	g_mylog = &MyLogDebug;
	PHANTOM_API void	SetLogPrint(MyLogPrint m)
	{
		g_mylog = m;
	}
	PHANTOM_API void    myLogPrint(const char* szText, ...)
	{
		char temp[10241];
#ifdef WIN32
		_vsnprintf(temp,10240,szText,(va_list)(&szText+1));
#else
        va_list args;
        va_start(args, szText);
        vsnprintf(temp, 10240, szText, args);
        va_end(args);
#endif
		if(g_mylog)
		{
			(*g_mylog)(temp);
#ifdef WIN32_OS
		::OutputDebugStringA(temp);
		::OutputDebugStringA("\r\n");
#endif
		}
		else
		{
#ifdef IPHONE_OS
			LogPrint("%s\n", temp);
#elif defined(ANDROID_OS)
		__android_log_print(ANDROID_LOG_INFO, "PhantomGameEngine", temp);
#elif defined(WIN32_OS)
		::OutputDebugStringA(temp);
		::OutputDebugStringA("\r\n");
#endif
		}
	}
namespace Phantom{
	
	TypeArray<cputime*>	cputime::s_tops;//最高级别的时钟
	cputime*			cputime::s_curr = 0;
	unsigned char		cputime::s_channelId = 0;
	cputime::cputime(const char* className, const char* funcName, int index, const char* file, int line)
	{
		memset(m_timeBegin, 0, sizeof(m_timeBegin));
		memset(m_timeFull, 0, sizeof(m_timeFull));
		memset(m_beginCount, 0, sizeof(m_beginCount));
		m_timeMax = 0;
		m_timeMin = 1.0f;
		m_className = className;
		NameT<512> files;
		files = file;
		NameT<512> f,ext;
		files.splitpath(0, &f, &ext);
		sprintf(m_funcName.t, "%s%d", funcName, index);
		m_fileName = f.t;
		m_fileName += ext.t;
		m_line = line;
		m_parent = 0;
		m_push = 0;
		m_pushCount		=	0;
		if(s_curr)
			s_curr->addchild(this);
		//else
		s_tops.push_back(this);
	}
	cputime::cputime(const char* className, const char* funcName, const char* file, int line)
	{
		memset(m_timeBegin, 0, sizeof(m_timeBegin));
		memset(m_timeFull, 0, sizeof(m_timeFull));
		memset(m_beginCount, 0, sizeof(m_beginCount));
		m_timeMax = 0;
		m_timeMin = 1.0f;
		m_className = className;
		NameT<512> files;
		files = file;
		NameT<512> f,ext;
		files.splitpath(0, &f, &ext);
		m_funcName = funcName;
		m_fileName = f.t;
		m_fileName += ext.t;
		m_line = line;
		m_parent = 0;
		m_push = 0;
		m_pushCount		=	0;
		if(s_curr)
			s_curr->addchild(this);
		//else
		s_tops.push_back(this);
	}
	void		cputime::begin()
	{
		if(m_pushCount==0)
		{
			m_push = s_curr;
			s_curr = this;
		}
		m_pushCount++;
		m_beginCount[s_channelId]++;
		m_timeBegin[s_channelId] = GetTickTime();
	}
	void		cputime::end()
	{
		double dE = GetTickTime() - m_timeBegin[s_channelId];
		m_timeFull[s_channelId] += dE;
		m_timeMax = getmax(dE, m_timeMax);
		m_timeMin = getmin(dE, m_timeMin);
		m_pushCount--;
		if(m_pushCount==0)
		{
			//assert(s_curr==this);
			s_curr = m_push;
			m_push = 0;
		}
	}
	void		cputime::addchild(cputime* c)
	{
		c->m_parent = this;
		m_childs.push_back(c);
	}


	Text	Text::format(const char* fmt, ...)
	{
		char temp[10241];
#ifdef WIN32
		_vsnprintf(temp,10240,fmt,(va_list)(&fmt+1));
#else
        va_list args;
        va_start(args, fmt);
        vsnprintf(temp, 10240, fmt, args);
        va_end(args);
#endif
		return Text(temp);// this->operator=(temp);
	}
	void	Text::add(const char* fmt, ...)
	{
		char temp[10241];
#ifdef WIN32
		_vsnprintf(temp,10240,fmt,(va_list)(&fmt+1));
#else
        va_list args;
        va_start(args, fmt);
        vsnprintf(temp, 10240, fmt, args);
        va_end(args);
#endif
		this->operator+=(temp);
	}

	Text	Text::remove(char c) const
	{
		TypeArray<char> result;
		result.resize(texts.size());
		int len = 0;
		for (int i = 0; i < texts.size(); i++) {
			if (texts[i] != c) {
				result[len] = texts[i];
				len++;
			}
		}
		result.resize(len);
		return Text(result.GetPtr(), result.size());
	}
	bool	Text::isFirst(const char* c, int len) const
	{
		if (len > texts.size())
			return false;
		for (int i = 0; i < len; i++) {
			if (texts[i] != c[i])
				return false;
		}
		return true;
	}
	Text	Text::trim(char c) const
	{
		TypeArray<char> result;
		result.resize(texts.size());
		bool begin = false;
		int len = 0;
		for (int i = 0; i < texts.size(); i++) {
			if (!begin) {
				if (texts[i] != c) {
					begin = true;
				}
			}
			if (begin) {
				result[len] = texts[i];
				len++;
			}
		}
		for (int i = len - 1; i >= 0; i--) {
			if (texts[i] == c) {
				result[i] = 0;
				len--;
			}
			else
				break;
		}
		result.resize(len);
		return Text(result.GetPtr(), result.size());
	}
	Text	Text::remove(const char* c) const//去掉所有的相关c出现的位置，比如去掉\r\n中的\r
	{
		InText C(c);
		TypeArray<char> result;
		result.resize(texts.size());
		int len = 0;
		for (int i = 0; i < texts.size(); i++) {
			if (C.Valid(texts[i])) {
				len -= C.Length - 1;
				continue;
			}
			result[len] = texts[i];
			len++;
		}
		result.resize(len);
		return Text(result.GetPtr(), result.size());
	}
	Text	Text::replace(char c, char replace) const
	{
		TypeArray<char> result;
		result.resize(texts.size());
		for (int i = 0; i < texts.size(); i++) {
			if (texts[i] == c) {
				result[i] = replace;
			}
			else
				result[i] = texts[i];
		}
		return Text(result.GetPtr(), result.size());
	}
	Text	Text::replace(const char* c, const char* replace) const
	{
		InText C(c);
		InText R(replace);
		int len = 0;
		TypeArray<char> result;
		result.resize(texts.size());
		for (int i = 0; i < texts.size(); i++) {
			if (C.Valid(texts[i])) {
				len -= C.Length;
				if (result.size() <= (len + R.Length))
					result.resize(len + R.Length + 1024, 0);
				for (int j = 0; j < R.Length; j++) {
					result[len] = R.Name.t[j];
					len++;
				}
				continue;
			}
			if (result.size() <= len)
				result.resize(len + 1024, 0);
			result[len] = texts[i];
			len++;
		}
		result.resize(len);
		return Text(result.GetPtr(), result.size());
	}
	Text	Text::makeOne(char c) const
	{
		TypeArray<char> result;
		result.resize(texts.size());
		int len = 0;
		int count = 0;
		for (int i = 0; i < texts.size(); i++) {
			if (texts[i] == c) {
				if (count == 0) {
					result[len] = texts[i];
					len++;
				}
				count++;
			}
			else {
				count = 0;
				result[len] = texts[i];
				len++;
			}
		}
		result.resize(len);
		return Text(result.GetPtr(), result.size());
	}
	Text	Text::right(int count) const
	{
		if (texts.size() <= count)
			return Text("");
		TypeArray<char> result;
		result.resize(count);
		memcpy(result.GetPtr(), texts.constPtr() + (texts.size() - count), count);
		return Text(result.GetPtr(), result.size());
	}
	Text	Text::left(int count) const
	{
		if (texts.size() <= count)
			return Text("");
		TypeArray<char> result;
		result.resize(count);
		memcpy(result.GetPtr(), texts.constPtr(), count);
		return Text(result.GetPtr(), result.size());
	}
	Text	Text::mid(int from, int count) const
	{
		if (texts.size() <= (from+count))
			return Text("");
		TypeArray<char> result;
		result.resize(count);
		memcpy(result.GetPtr(), texts.constPtr() + from, count);
		return Text(result.GetPtr(), result.size());
	}
	Text	Text::remove(const char* b, const char* e, skip_symbol* skips) const
	{
		if (texts.size() == 0)
			return Text("");
		InText B(b);
		InText E(e);
		int len = 0;
		int begin = -1;
		TypeArray<char> v;
		v.resize(texts.size(), false);
		for (int i = 0; i < texts.size(); i++) {
			//if (bskip)
			{
				if (texts[i] == '\'')
					i = skip(texts, i + 1, 0, "\'");
				else if (texts[i] == '\"')
					i = skip(texts, i + 1, 0, "\"");
			}
			if (B.Valid(texts[i])) {
				len -= (B.Length - 1);
				begin = i + 1;
				int nEnd = skips?skip(texts, begin, *skips):skip(texts, begin, b, e);
				i = nEnd - 1;
			}
			else{// if (begin >= 0 && E.Valid(texts[i])) {
				v[len] = texts[i];
				len++;
				//int end = i - E.Count;
				//if (end <= begin)
				//	return Text("");
				//return Text(texts.constPtr() + begin, end - begin);
			}
		}
		v.resize(len);
		return Text(v.constPtr(), v.size());
	}
	int		Text::skip(const TypeArray<char>& texts, int index, Text::skip_symbol& skips) const
	{
		if (texts.size() == 0)
			return index;
		int len = 0;
		int begin = index;
		for (int i = index; i < texts.size(); i++) {
			if (skips.b == texts[i]) {
				i = skip(texts, i + 1, skips);
			}
			if(skips.e == texts[i]) {
				return (i + 1);// (i - E.Length + 1);
			}
		}
		return index;
	}
	int		Text::skip(const TypeArray<char>& texts, int index, const char* b, const char* e) const
	{
		if (texts.size() == 0)
			return index;
		InText B(b);
		InText E(e);
		int len = 0;
		int begin = index;
		for (int i = index; i < texts.size(); i++) {
			if (b) {
				if (B.Valid(texts[i])) {
					i = skip(texts, i + 1, b, e);
				}
			}
			if (E.Valid(texts[i])) {
				return (i + 1);// (i - E.Length + 1);
			}
		}
		return index;
	}
	Text	Text::inner(const char* b, const char* e, skip_symbol* skips) const
	{
		if (texts.size() == 0)
			return Text("");
		InText B(b);
		InText E(e);
		int len = 0;
		int begin = -1;
		for (int i = 0; i < texts.size(); i++) {
			//if (bskip)
			{
				if (texts[i] == '\'')
					i = skip(texts, i + 1, 0, "\'");
				else if (texts[i] == '\"')
					i = skip(texts, i + 1, 0, "\"");
			}
			if (B.Valid(texts[i])) {
				begin = i + 1;
				int nEnd = skips ? skip(texts, begin, *skips) : skip(texts, begin, b, e);
				nEnd -= E.Length;
				if(nEnd>begin)
					return Text(texts.constPtr() + begin, nEnd - begin);
				return Text("");
			}
		}
		return Text("");// texts.constPtr(), texts.size());
	}
	Text	Text::left(const char* b) const
	{
		if (texts.size() == 0)
			return Text("");
		InText B(b);
		int len = 0;
		bool begin = false;
		for (int i = 0; i < texts.size(); i++) {
			if (B.Valid(texts[i])) {
				if (i <= 0)
					return Text("");
				return Text(texts.constPtr(), i - B.Length + 1);
			}
		}
		return Text("");
	}
	Text	Text::right(const char* b) const
	{
		if (texts.size() == 0)
			return Text("");
		InText B(b);
		int len = 0;
		bool begin = false;
		for (int i = 0; i < texts.size(); i++) {
			if (B.Valid(texts[i])) {
				if (i <= 0)
					return Text("");
				return Text(texts.constPtr() + i + 1, texts.size() - (i + 1));
			}
		}
		return Text("");
	}

	bool	Text::load(const char* File)
	{
		IOFile f(File, "rb");
		if (!f)
			return false;
		unsigned char head[3];
		f.read(head, 3);
		if ((head[0] == 0xef && head[1] == 0xbb && head[2] == 0xbf))
		{
			if (f.size() <= 3)
				return true;
			texts.resize(f.size() - 3);
			f.read(texts.GetPtr(), f.size() - 3);
			return true;
		}
		f.seek(0, IOFile::Seek_Set);
		if (f.size() <= 0)
			return true;
		texts.resize(f.size());
		f.read(texts.GetPtr(), f.size());
		return true;
	}

	bool	Text::split(Array<Text>& resultLines, char Symbol, skip_symbol* skips) const
	{
		resultLines.clear();
		int nLast = 0;
		for (int i = 0; i < texts.size(); i++) {
			{
				if (texts[i] == '\"')
					i = skip(texts, i + 1, 0, "\"");
				else if (texts[i] == '\'')
					i = skip(texts, i + 1, 0, "\'");
				else if (skips && texts[i] == skips->b) {
					i = skip(texts, i + 1, *skips);
				}
				if (i >= texts.size())
					break;
			}
			if (texts[i] == Symbol) {
				//if (bskip)
				resultLines.push_back(Text(texts.constPtr() + nLast, i - nLast));
				nLast = i + 1;
			}
		}
		if (nLast < texts.size()) {
			resultLines.push_back(Text(texts.constPtr() + nLast, texts.size() - nLast));
		}
		return resultLines.size() > 1;
	}
	void	Text::breaks(Array<Text>& resultLines, const Array<Text>& inlines, char Symbol, skip_symbol* skips)
	{
		for (int i = 0; i < inlines.size(); i++) {
			Array<Text> r;
			if (inlines[i].split(r, Symbol, skips)){
				for (int j = 0; j < r.size(); j++) {
					resultLines += r[j];
				}
			}
			else {
				resultLines += inlines[i];
			}
		}
	}
	PHANTOM_API VOID			ReplaceAll(TypeArray<char>& ret, const char* szText, const char* szFinds, const char* szReplace){
		int nLen = strlen(szText);
		int nFind = strlen(szFinds);
		int nReplace = strlen(szReplace);
		const char* first = strstr(szText, szFinds);
		const char* last = szText;
		while(first){
			int len = first - last;
			int oldLen = ret.size();
			ret.resize(ret.size()+len+nReplace, 0);
			memcpy(ret.GetPtr()+oldLen, last, len);
			memcpy(ret.GetPtr()+oldLen+len, szReplace, nReplace);
			first = first + nFind;
			last = first;
			first = strstr(last, szFinds);
		}
		first = szText + nLen;
		int len = first - last;
		int oldLen = ret.size();
		ret.resize(ret.size()+len, 0);
		memcpy(ret.GetPtr()+oldLen, last, len);
	}
	PHANTOM_API const char* GetActionTypeName(int t)
	{
		switch(t)
		{
		case ActionType_Idle:
			return "Idle";
		case ActionType_Walk:
			return "Walk";
		case ActionType_Run:
			return "Run";
		case ActionType_Down:
			return "Down";
		case ActionType_Jump:
			return "Jump";
		case ActionType_Hit:
			return "Hit";
		case ActionType_Die:
			return "Die";
		case ActionType_Attack:
			return "Attack";
		case ActionType_Idle2:
			return "Idle2";
		case ActionType_Walk2:
			return "Walk2";
		case ActionType_Run2:
			return "Run2";
		case ActionType_Jump2:
			return "Jump2";
		case ActionType_Hit2:
			return "Hit2";
		case ActionType_Die2:
			return "Die2";
		case ActionType_Attack2:
			return "Attack2";
		case ActionType_Idle3:
			return "Idle3";
		case ActionType_Walk3:
			return "Walk3";
		case ActionType_Run3:
			return "Run3";
		case ActionType_Jump3:
			return "Jump3";
		case ActionType_Hit3:
			return "Hit3";
		case ActionType_Die3:
			return "Die3";
		case ActionType_Attack3:
			return "Attack3";
		case ActionType_Hide:
			return "Hide";
		case ActionType_Other1:
			return "Other1";
		case ActionType_Other2:
			return "Other2";
		case ActionType_Other3:
			return "Other3";
		case ActionType_Other4:
			return "Other4";
		case ActionType_Other5:
			return "Other5";
		case ActionType_Other6:
			return "Other6";
		case ActionType_Other7:
			return "Other7";
		case ActionType_Other8:
			return "Other8";
		case ActionType_Other9:
			return "Other9";
		case ActionType_Other10:
			return "Other10";
		}
		return "";
	}
	void							colorkey_data::from(const color4& c)
	{
		int ic[4];
		ic[0]	=	(int)(c.r * 255.0f);
		ic[1]	=	(int)(c.g * 255.0f);
		ic[2]	=	(int)(c.b * 255.0f);
		ic[3]	=	(int)(c.a * 255.0f);
		for(int i=0;i<4;i++)
		{
			if(ic[i] < 0)ic[i] = 0;
			if(ic[i] > 255)ic[i] = 255;
			color[i] = ic[i];
		}
	}
	void							colorkey_data::lerp(color4& out, colorkey_data& next, float fLerp)
	{
		float f2	=	(1.0f - fLerp);
		const float f255	=	1.0f / 255.0f;
		out.r		=	((float)next.color[0] * fLerp + (float)color[0] * f2) * f255;
		out.g		=	((float)next.color[1] * fLerp + (float)color[1] * f2) * f255;
		out.b		=	((float)next.color[2] * fLerp + (float)color[2] * f2) * f255;
		out.a		=	((float)next.color[3] * fLerp + (float)color[3] * f2) * f255;
	}
	void							colorkey_data::GetColor(color4& out)
	{
		const float f255	=	1.0f / 255.0f;
		out.r		=	(float)color[0] * f255;
		out.g		=	(float)color[1] * f255;
		out.b		=	(float)color[2] * f255;
		out.a		=	(float)color[3] * f255;
	}

	void	matrix3x4::from(matrix4x4& mat)
	{
		int index = 0;
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<3;j++)
			{
				v[index]	=	mat.m[i][j];
				index++;
			}
		}
	}

	void	matrix3x4::get(matrix4x4& mat)
	{
		int index = 0;
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<3;j++)
			{
				mat.m[i][j]	=	v[index];
				index++;
			}
		}
	}

	void	matrix3x4::lerp(matrix4x4& mat, matrix3x4& next, float fLerp)
	{
		int index = 0;
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<3;j++)
			{
				mat.m[i][j]	=	(next.v[index] - v[index]) * fLerp + v[index];
				index++;
			}
		}
	}

	PHANTOM_API char * strlwr_t(char *s)
	{
		unsigned char *ucs = (unsigned char *) s;
		for ( ; *ucs != '\0'; ucs++)
		{
			*ucs = tolower(*ucs);
		}
		return s;
	}

	PHANTOM_API char * strupr_t(char *s)
	{
		unsigned char *ucs = (unsigned char *) s;
		for ( ; *ucs != '\0'; ucs++)
		{
			*ucs = toupper(*ucs);
		}
		return s;
	}

	PHANTOM_API int				u_strlen(const short_t* s1)
	{
		if(!s1)
			return 0;
		int c = 0;
		while(*s1)
		{
			c++;
			s1++;
		}
		return c;
	}
	PHANTOM_API const short_t*	u_strcpy(short_t* s1, const short_t* s2)
	{
		if(!s1 || !s2)
			return 0;
		while(*s2)
		{
			*s1	=	*s2;
			s1++;
			s2++;
		}
		*s1	=	0;
		return s1;
	}
	PHANTOM_API const short_t*	u_strncpy(short_t* s1, const short_t* s2, int length)
	{
		if(!s1 || !s2)
			return 0;
		while(*s2 && length > 0)
		{
			*s1	=	*s2;
			s1++;
			s2++;
			length --;
		}
		*s1	=	0;
		return s1;
	}
	PHANTOM_API const short_t*	u_strcat(short_t* s1, const short_t* s2)
	{
		if(!s1 || !s2)
			return 0;
		while(*s1)
			s1++;
		//
		while(*s2)
		{
			*s1	=	*s2;
			s1++;
			s2++;
		}
		*s1	=	0;
		return s1;
	}
	PHANTOM_API const short_t*	u_strlwr(short_t* s1)
	{
		while(*s1)
		{
			if(*s1 < 128)
				*s1 = tolower(*s1);
			s1++;
		}
		return s1;
	}
	PHANTOM_API int				u_strcmp(const short_t* s1, const short_t* s2)
	{
        int ret = 0 ;
        while( ! (ret = (int)(*s1 - *s2)) && *s2)
                ++s1, ++s2;
        if ( ret < 0 )
                ret = -1 ;
        else if ( ret > 0 )
                ret = 1 ;
        return( ret );
	}
	PHANTOM_API int				u_stricmp(const short_t* s1, const short_t* s2)
	{
        int ret = 0 ;
        while(*s1 && *s2)
		{
			short_t u1 = *s1;
			short_t u2 = *s2;
			if(u1 < 128)
				u1 = tolower(u1);
			if(u2 < 128)
				u2 = tolower(u2);
			ret = (int)u1 - (int)u2;
			if(ret != 0)
				break;
			++s1;
			++s2;
		}
		if(ret==0)
		{
			if(*s1==*s2)
				return 0;
			if(*s1)
				return -1;
			return 1;
		}
        if ( ret < 0 )
                ret = -1 ;
        else if ( ret > 0 )
                ret = 1 ;
        return( ret );
	}

	void*				CDMOperator::Move(void* dest, const void* src, int size)
	{
		return memmove(dest, src, size);
	}
	void*				CDMOperator::Copy(void* dest, const void* src, int size)
	{
		return memcpy(dest, src, size);
	}
	void*				CDMOperator::Allocate(int size)
	{
		return malloc(size);
	}
	void*				CDMOperator::Reallocate(void* ptr, int size)
	{
		return realloc(ptr, size);
	}
	void				CDMOperator::Free(void* ptr)
	{
		free(ptr);
	}
	void				CDMOperator::Fill(void* ptr, int v, int size)
	{
		memset(ptr, v, size);
	}
	PHANTOM_API VOID		RandomStart()	//随机数开始
	{
#ifdef WIN32_OS
		srand(GetTickCount());
#else
		timeval t2;
		gettimeofday(&t2, NULL);
		srand(t2.tv_sec);
#endif
	}
	void		GetLocalTime(Time& tr)
	{
#ifdef WIN32_OS
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		tr.hour		=	sys.wHour;
		tr.minute	=	sys.wMinute;
		tr.second	=	sys.wSecond;
		tr.year		=	sys.wYear;
		tr.month	=	sys.wMonth;
		tr.day		=	sys.wDay;
		LARGE_INTEGER curr;
		QueryPerformanceCounter(&curr);
		tr.microsecond	=	curr.LowPart;
#else
		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );
		tr.hour		=	now->tm_hour;
		tr.minute	=	now->tm_min;
		tr.second	=	now->tm_sec;
		tr.year		=	now->tm_year+1900;
		tr.month	=	now->tm_mon + 1;
		tr.day		=	now->tm_mday;
		//
		timeval t2;
		gettimeofday(&t2, NULL);
		tr.microsecond	=	t2.tv_usec;
#endif
	}
	PHANTOM_API void		GetTime(int& hour, int& minute, int& second)	//返回时钟
	{
#ifdef WIN32_OS
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		hour	=	sys.wHour;
		minute	=	sys.wMinute;
		second	=	sys.wSecond;
#else
		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );
		hour = now->tm_hour;
		minute = now->tm_min;
		second = now->tm_sec;
#endif
	}
	PHANTOM_API double		GetTickTime()	//秒时间
	{
#ifdef WIN32_OS
		static BOOL bInit		=	false;
		static LARGE_INTEGER	win32Frequency;
		if(!bInit)
			QueryPerformanceFrequency(&win32Frequency);
		LARGE_INTEGER curr;
		QueryPerformanceCounter(&curr);
		return (double)curr.QuadPart / (double)win32Frequency.QuadPart;
#else
		timeval t2;
		gettimeofday(&t2, NULL);
		double ret	=	t2.tv_sec;      // sec to ms
		ret			+=	t2.tv_usec / 1000000.0;   // us to ms
		return ret;
#endif
	}
	PHANTOM_API double		RequestIntervalTime()
	{
		static BOOL bInit		=	false;
		double	dElapsedTime	=	0;
#ifdef WIN32_OS
		static LARGE_INTEGER	win32Frequency;
		static LARGE_INTEGER	lastTimer;
		if(!bInit)
		{
			QueryPerformanceFrequency(&win32Frequency);
			QueryPerformanceCounter(&lastTimer);
		}
		else
		{
			LARGE_INTEGER curr;
			QueryPerformanceCounter(&curr);
			dElapsedTime	= ((double)curr.QuadPart - (double)lastTimer.QuadPart) / (double)win32Frequency.QuadPart;
			lastTimer	=	curr;
		}
#else
		static timeval	lastTimer;
		if(!bInit)
		{
			gettimeofday(&lastTimer, NULL);
		}
		else
		{
			timeval t2;
			gettimeofday(&t2, NULL);
			dElapsedTime	=	(t2.tv_sec - lastTimer.tv_sec);      // sec to ms
			dElapsedTime	+=	(t2.tv_usec - lastTimer.tv_usec) / 1000000.0;   // us to ms
			lastTimer	=	t2;
		}
#endif
		bInit	=	true;
		return dElapsedTime;
	}
	PHANTOM_API VOID		AppSleep(INT millisecond)
	{
#ifdef WIN32_OS
		Sleep(millisecond);
#else
		usleep(millisecond * 1000);
#endif
	}
	//

	PHANTOM_API short_t utf8ToUnicode( const unsigned char *s, int *bytes )
	{
		*bytes	=	1;
		if ( ( 0x80 & s[0] ) == 0 )//一个字节的UTF-8 
		{
			return s[0];
		}
		short_t a, b;
		if ( ( 0xE0 & s[0] ) == 0xC0 )//两字节的UTF-8 
		{
			*bytes = 2;
			a = ( 0x1F & s[0] ) << 6;
			b = 0x3F & s[1];
			return a + b;
		}
		short_t c;
		if ( ( 0xF0 & s[0] ) == 0xE0 )//三字节的UTF-8 
		{
			*bytes = 3;
			a = ( 0x0F & s[0] ) << 12;
			b = ( 0x3F & s[1] ) << 6;
			c = 0x3F & s[2];
			return a + b + c;
		}
		return 0;
	}
	PHANTOM_API int	UnicodeToUtf8(short_t w, unsigned char* rets)
	{
        if (w >= 0x0000 && w <= 0x007f)
        {
            *rets = w;
            return 1;
        }
        else if (w >= 0x0080 && w <= 0x07ff)
        {
            *rets = 0xc0 | (w >> 6);
            rets ++;
            *rets = 0x80 | (w & (0xff >> 2));
            return 2;
        }
        //else if (w >= 0x0800 && w <= 0xffff)
		*rets = 0xe0 | (w >> 12);
		rets ++;
		*rets = 0x80 | (w >> 6 & 0x00ff);
		rets ++;
		*rets = 0x80 | (w & (0xff >> 2));
		return 3;
	}
	PHANTOM_API int				Utf8ToWide(const char* szUTF8, short_t* ret, int length)
	{
#ifdef WIN32
		int len		=	::MultiByteToWideChar(CP_UTF8, 0, szUTF8, strlen(szUTF8), (LPWSTR)ret, length);
		ret[len]	=	0;
		return len;
#else
		return Utf8ToWideOS(szUTF8, strlen(szUTF8), ret);
#endif
	}
	PHANTOM_API int				WideToUtf8(const short_t* szW, char* ret, int length)
	{
#ifdef WIN32
		int len		=	::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)szW, -1, ret, length, 0, 0);
		ret[len]	=	0;
		return len;
#else
		return WideToUtf8OS(szW, u_strlen(szW), ret);
#endif
	}

	PHANTOM_API VOID	AsciiToUtf8(TypeArray<char>& ret, const char* szIn)
	{
#ifdef WIN32
		int len = strlen(szIn);
		wchar_t* buf = new wchar_t[len*2+1];
		len			=	::MultiByteToWideChar(CP_ACP, 0, szIn, -1, (LPWSTR)buf, len*2+1);
		buf[len]	=	0;
		ret.resize(len*2+1);
		len			=	::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)buf, -1, ret.GetPtr(), len*2+1, 0, 0);
		ret.resize(len);
		delete[] buf;
#else
		int len = strlen(szIn);
		ret.resize(len);
		strcpy(ret.GetPtr(), szIn);
#endif
	}
	PHANTOM_API VOID	Utf8ToAscii(TypeArray<char>& ret, const char* szIn)
	{
#ifdef WIN32
		int len = strlen(szIn);
		wchar_t* buf = new wchar_t[len*2+1];
		len		=	::MultiByteToWideChar(CP_UTF8, 0, szIn, -1, (LPWSTR)buf, len*2+1);
		buf[len]	=	0;
		ret.resize(len*2+1);
		len			=	::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)buf, -1, ret.GetPtr(), len*2+1, 0, 0);
		ret.resize(len);
		delete[] buf;
#else
		int len = strlen(szIn);
		ret.resize(len);
		strcpy(ret.GetPtr(), szIn);
#endif
	}
/*
	int				ShortToWide(const short_t* szUTF8, wchar_t* ret, int length)
	{
		int len = u_strlen(szUTF8);
		for(int i=0;i<=len;i++)
			ret[i] = szUTF8[i];
		return len;
	}
	int				WidtToShort(const wchar_t* szW, short_t* ret, int length)
	{
		int len = wcslen(szW);
		for(int i=0;i<=len;i++)
			ret[i] = szW[i];
		return len;
	}
*/
	PHANTOM_API int							Utf8ToW(const char* szUTF8, short_t* ret, int length)
	{
		int len		=	strlen(szUTF8);
#ifdef WIN32
		len		=	::MultiByteToWideChar(CP_ACP, 0, szUTF8, -1, (LPWSTR)ret, length);
		ret[len]	=	0;
		return len;
#else
//#ifdef ANDROID_OS
		int index	=	0;
		for(int i=0;i<len;i++)
		{
			int n = 0;
			ret[index++] = utf8ToUnicode((const unsigned char*)szUTF8 + i, &n);
			i	+=	(n - 1);
			if(index >= length)
			{
				ret[index - 1] = 0;
				return index;
			}
		}
		ret[index] = 0;
		return index;
#endif
//#else
//		static TypeArray<short_t> wcs;
//		wcs.resize(len * 2);
//		::mbstowcs(wcs.GetPtr(), szUTF8, wcs.size());
//		for(int i=0;i<length;i++)
//		{
//			ret[i] = wcs[i];
//			if(i >= wcs.size())
//				return wcs.size();
//		}
//		ret[length - 1] = 0;
//		return length - 1;
//#endif
	}
	PHANTOM_API int							WToUtf8(const short_t* szW, char* ret, int length)
	{
#ifdef WIN32
		int len		=	::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szW, -1, ret, length, 0, 0);
		ret[len]	=	0;
		return len;
#else
		int len		=	u_strlen(szW);
		int index	=	0;
		for(int i=0;i<len;i++)
		{
			unsigned char ch[8];
			int n = UnicodeToUtf8(szW[i], ch);
			for(int j=0;j<n;j++)
			{
				ret[index++] = ch[j];
				if(index >= length)
				{
					ret[length - 1] = 0;
					return length;
				}
			}
		}
		ret[index] = 0;
		return index;
#endif
	}


	PHANTOM_API char	dec(unsigned char ch)
	{
		const unsigned char s_decVector[] = "0123456789ABCDEF";
		if(ch < 16)
			return s_decVector[ch];
		return 0;
	}
	PHANTOM_API unsigned char	hex(char ch)
	{
		if(ch >= '0' && ch <= '9')
			return ch - '0';
		if(ch >= 'A' && ch <= 'F')
			return ch - 'A' + 10;
		return 0;
	}
	PHANTOM_API VOID	HexEncode(std::vector<unsigned char>& ret, const char* szText)
	{
		std::string str = szText;
		strupr_t((char*)str.c_str());
		int len	=	str.size();
		for(int i=0;i<len; i+=2)
		{
			ret.push_back((hex(str[i]) * 16 + hex(str[i + 1])));
		}
	}
	PHANTOM_API int	HexDecode(char* ret, unsigned char* val, int length)
	{
		int count	=	0;
		for(int i=0;i<length;i++)
		{
			unsigned char ch	=	val[i];
			ret[count++]		=	dec(ch / 16);
			ret[count++]		=	dec(ch % 16);
		}
		ret[count++] = 0;
		return count;
	}

	HttpGUID::HttpGUID()
	{
		Generate();
	}
	HttpGUID::~HttpGUID()
	{
	}
	VOID	HttpGUID::ToText(char* szText)//返回一串文本
	{
		HexDecode(szText, (unsigned char*)this, sizeof(HttpGUID));
	}
	VOID	HttpGUID::FromText(const char* szText)//从一串文本
	{
		std::vector<unsigned char> rets;
		HexEncode(rets, szText);
		if(rets.size() == sizeof(HttpGUID))
			memcpy(this, &rets.at(0), rets.size());
	}
	VOID	HttpGUID::Generate()	//创建一个GUID
	{
		Time tm;
		GetLocalTime(tm);
		Data1[0] = (tm.year - 2000);
		Data1[1] = tm.month;
		Data1[2] = tm.day;
		Data1[3] = tm.hour;
		Data1[4] = tm.second;
		Data1[5] = tm.minute;
		Data2 = tm.microsecond;
		Data3 = rand();
		ucSystem	= 0;
		ucRoleID	= 0;
#ifdef ANDROID_OS
		ucSystem	= Flags_Android;
#elif defined(IPHONE_OS)
		ucSystem	= Flags_IPhone;
#endif
	}


	//数据指针管理器
	class	CPtrManager
	{
	public:
		CPtrManager();
		~CPtrManager();
		BOOL					IsExistPtr(const PtrID& id);
		Base*			GetPtr(const PtrID& id);
	protected:
		PtrID					Push(Base* ptr);
		VOID					Pop(const PtrID& id);
		friend class			Base;
	protected:
		TypeArray<Ptr>		m_pointers;
		TypeArray<int>		m_pointersFree;
		int						m_nID;
	};

	CPtrManager::CPtrManager()
	{
		m_pointers.setGrow(1000);
		m_pointersFree.setGrow(1000);
		m_nID	=	1;
	}
	CPtrManager::~CPtrManager()
	{
	}

	PtrID					CPtrManager::Push(Base* autoPtr)
	{
		if(m_pointersFree.size() > 0)
		{
			int newSize = m_pointersFree.size()-1;
			int index = m_pointersFree[newSize];
			assert(m_pointers[index].id == 0);
			m_pointersFree.resize(newSize);
			m_pointers[index].id		=	m_nID++;
			m_pointers[index].ptr		=	autoPtr;
			return PtrID(index, m_pointers[index].id);
		}
		//for(int i=0;i<m_pointers.size();i++)
		//{
		//	if(m_pointers[i].id == 0)
		//	{
		//		m_pointers[i].id		=	m_nID++;
		//		m_pointers[i].ptr		=	autoPtr;
		//		return PtrID(i, m_pointers[i].id);
		//	}
		//}
		Ptr p;
		p.id	=	m_nID++;
		p.ptr	=	autoPtr;
		int index	=	m_pointers.size();
		m_pointers.push_back(p);
		return PtrID(index, m_pointers[index].id);
	}
	VOID					CPtrManager::Pop(const PtrID& id)
	{
		if(id.index >= m_pointers.size())
			return;
		assert(m_pointers[id.index].id == id.id);
		if(m_pointers[id.index].id != id.id)
			return;
		int oldSize = m_pointersFree.size();
		m_pointersFree.resize(oldSize+1);
		m_pointersFree[oldSize] = id.index;
		m_pointers[id.index].id			=	0;
		m_pointers[id.index].ptr		=	0;
	}

	Base*				CPtrManager::GetPtr(const PtrID& id)
	{
		if(id.index >= m_pointers.size())
			return 0;
		if(m_pointers[id.index].id != id.id)//   ？？？ID不一致是咋回事?
			return 0;
		return m_pointers[id.index].ptr;
	}

	BOOL					CPtrManager::IsExistPtr(const PtrID& id)
	{
		if(id.index >= m_pointers.size())
			return false;
		if(m_pointers[id.index].id != id.id)
			return false;
		return true;
	}

	CPtrManager*	g_pPtrManager	=	0;
	PHANTOM_API CPtrManager*	GetPtrManager()
	{
		if(!g_pPtrManager)
			g_pPtrManager	=	new CPtrManager();
		return g_pPtrManager;
	}
	PHANTOM_API int						GetBaseContext(const PtrID& id)
	{
		if(g_pPtrManager)
		{
			Base*  b = g_pPtrManager->GetPtr(id);
			if(!b)return 0;
			return b->m_context;
		}
		return 0;
	}
	PHANTOM_API VOID					SetBaseContext(const PtrID& id, int n)
	{
		if(g_pPtrManager)
		{
			Base*  b = g_pPtrManager->GetPtr(id);
			if(b)
				b->m_context	=	n;
		}
	}
	PHANTOM_API Base*			GetBasePtr(const PtrID& id)
	{
		if(g_pPtrManager)
			return g_pPtrManager->GetPtr(id);
		return 0;
	}
	Base::Base(){
		m_baseid	=	GetPtrManager()->Push(this);
	}
	Base::~Base(){
		assert(m_baseid.id > 0 && m_baseid.index >= 0);
		GetPtrManager()->Pop(m_baseid);
	}
	GenuineGUID				g_phantomGUID;
	PHANTOM_API VOID					SetPhantomGUID(unsigned int* guid)//设置幻影授权码，写入授权码之后很多操作才能正确运作，包括读取资源操作
	{
		memcpy(&g_phantomGUID, guid, sizeof(g_phantomGUID));
	}

	IOFile::IOFile()
	{
		m_pFile = 0;
	}
	IOFile::IOFile(const char* File, const char* Mode)
	{
		m_pFile = fopen(File, Mode);
	}
	IOFile::~IOFile()
	{
		close();
	}
	bool						IOFile::readall(TypeArray<char>& result)
	{
		int64 len = size();
		if (len > 0) {
			result.resize(len, 0);
			read(result.GetPtr(), len);
		}
		return len > 0;
	}
	int64						IOFile::write(const void* t, int64 tLength)
	{
		CPUTime(IOFile);
		if (!m_pFile)
			return 0;
		return fwrite(t, tLength, 1, m_pFile);
	}
	int64						IOFile::read(void* t, int64 tLength)
	{
		CPUTime(IOFile);
		if (!m_pFile)
			return 0;
		return fread(t, tLength, 1, m_pFile);
	}
	int64						IOFile::seek(int64 numSeek, int seekType)
	{
		CPUTime(IOFile);
		if (!m_pFile)
			return 0;
		return fseek(m_pFile, numSeek, seekType);
	}
	int64						IOFile::tell()
	{
		CPUTime(IOFile);
		if (!m_pFile)
			return 0;
		return ftell(m_pFile);
	}
	int64						IOFile::size()
	{
		CPUTime(IOFile);
		if (!m_pFile)
			return 0;
		int64 pos = ftell(m_pFile); // get current file pointer
		fseek(m_pFile, 0, SEEK_END); // seek to end of file
		int64 size = ftell(m_pFile); // get current file pointer
		fseek(m_pFile, pos, SEEK_SET);
		return size;
	}
	BOOL					IOFile::eof()
	{
		CPUTime(IOFile);
		if (!m_pFile)
			return 0;
		return feof(m_pFile);
	}
	void					IOFile::close()
	{
		CPUTime(IOFile);
		if (!m_pFile)
			return;
		fclose(m_pFile);
		m_pFile = 0;
	}

	skip_w::skip_w(StreamBase* s){
		m_stream = s;
		m_seek = m_stream->tell();
		m_stream->write(&m_filePointer, sizeof(int));
	}
	char	skip_w::end(){
		if(!m_stream)
			return false;
		int skip_type = 0xD25F0821;
		m_filePointer = m_stream->tell();
		m_stream->seek(m_seek, 0);
		m_stream->write(&m_filePointer, sizeof(int));
		m_stream->seek(m_filePointer, 0);
		m_stream->write(&skip_type, sizeof(int));
		return true;
	}
	skip_w::~skip_w(){
	}
	skip_r::skip_r(){
		m_stream = 0;
	}
	void	skip_r::begin(StreamBase* s){
		m_stream = s;
		m_stream->read(&m_filePointer, sizeof(int));
	}
	char	skip_r::end(){
		if(!m_stream)
			return false;
		int currPointer = m_stream->tell();
		m_stream->seek(m_filePointer);
		int skip_type = 0xD25F0821;
		m_stream->read(&skip_type, sizeof(int));
		if(skip_type != 0xD25F0821)
		{
			assert(skip_type != 0xD25F0821);
			return 0;
		}
		if(currPointer <  m_filePointer)//如果当前指针小于需要的指针则表示目前处于版本过低状态
		{
			LogInfo("system->error:%s", utf8("你的引擎版本过低，将会丢失某些功能，请更新到最新版本"));
			return 2;
		}
		//如果当前指针和读取的指针一致则返回1，就是相同版本
		return 1;
	}
	skip_r::~skip_r(){
	}
	VOID	ThrowError()
	{
		assert(false);
		g_manager.ExitGame();
	}
	VOID	DisableColor(color4& c) {
		float f = c.r * 0.11 + c.g * 0.59 + c.b * 0.30;
		c.setRGBA(f, f, f, c.a);
	}
};

MyHttpLoadFile	g_httpLoad = 0;
PHANTOM_API VOID	SetMyLoadFile(MyHttpLoadFile f)
{
	g_httpLoad = f;
}

PHANTOM_API VOID	InitMyFile(const char* szFile, BOOL bDownloadNow, BOOL bInMemory)
{
	if(g_httpLoad)
		(*g_httpLoad)(szFile, bDownloadNow, bInMemory);
}

#ifdef WIN32_OS
PHANTOM_API const char* utf8(const char* szIn) {
	static Phantom::TypeArray<char> temps[6];
	static int usage = 0;
	usage++;
	usage %= 6;
	int len = strlen(szIn);
	temps[usage].resize(len * 3);
	wchar_t wbuf[1024];
	len = ::MultiByteToWideChar(CP_ACP, 0, szIn, len, wbuf, 1024);
	wbuf[len] = 0;
	len = ::WideCharToMultiByte(CP_UTF8, 0, wbuf, len, temps[usage].GetPtr(), len * 3, 0, 0);
	temps[usage].resize(len);
	return temps[usage].GetPtr();
}
#endif
