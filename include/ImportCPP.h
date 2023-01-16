#pragma once
#include <string>
#include <vector>
#include <map>
#include "PhantomBase.h"
#include "DexianExec.h"

namespace Phantom {

	typedef NameT<128>		ImportName;
	struct ExpLine{
		Text			Line;
		Text			Desc;
	};
	class ImportSection{
	public:
		ImportSection();
		~ImportSection();
		ImportSection(const char* Text, int nBegin, int nCount = -1);
		bool							FromToEnd(const char* Text, int BeginIndex, int TextCount, const char* Find, const char* MustNotExist);
		//所有Begin，End出现的段落全部分割，Begin和End之间的段落也分割 , char CodeSymbol = ';'/*C++中是代码分割段；*/);
		//ImportSection*					CreateSplit(const char* Begin ,const char* End);
		TypeArray<ImportSection*>&		GetLines() { return Lines; }
		bool							SplitLine(char Symbol = '\n', bool bAddSymbol = false);//根据字符分割
		void							SplitBreak(char Symbol);//如果行内有相关的字符继续断开，插入式断开，比如{,};等等符号
		void							TrimNothingLine();//过滤掉所有空行
		void							TrimLine();//去掉行首行尾空格
		void							TrimDescription(const char* c);
		void							MergeLines();
		void							RemoveAllLines();
		Text							text;
	protected:
		ImportSection*					AddLine(const char* Text, int BeginIndex, int Count);
		//int								SplitToEnd(const char* Text, int nIndex, int nLength, const char* Begin, const char* End);
		int								BeginIndex, Count;
		TypeArray<ImportSection*>		Lines;
		friend class	ImportCPP;
	};

	class ImportCPP
	{
	public:
		ImportCPP();
		virtual ~ImportCPP();
	public:
		typedef bool (ImportCPP::*ProcImport)(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent);
		struct TypeCB : public Name {
			ProcImport	cb;
			TypeCB() { cb = 0; }
			TypeCB(const char* name, ProcImport _cb):
			Name(name){
				cb = _cb;
			}
		};
	public:
		DexianProgramLanguage*	Import(const char* File);//从文件导入.h文件
	protected:
		struct PropV {
			Name		name, value;
		};
		Name			propText(Array<PropV>& props, const char* Name);
		DexianProgramLanguage*	ImportFile(const char* File);//从文件导入.h文件
		bool			ImportNext(const char* File, const Text& text, DexianProgramLanguage* system, DexianStruct* parent);
		//bool	ImportNextC(const char* File, const char* EXP, ImportSection& Sec, int& Index);
		NameT<512>	m_Path, m_File;
		//
		bool			CPP_Type(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent);
		bool			CPP_Enum(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent);
		bool			CPP_Struct(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent);
		bool			CPP_Method(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent);
		bool			CPP_Class(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent);
		bool			CPP_Event(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent);
	};

};
