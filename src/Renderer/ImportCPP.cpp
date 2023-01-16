#include "ImportCPP.h"

namespace Phantom {

	ImportSection::ImportSection() {
		BeginIndex = Count = 0;
	}
	ImportSection::~ImportSection()
	{
		for (int i = 0; i < Lines.size(); i++) {
			delete Lines[i];
		}
	}
	void	ImportSection::TrimDescription(const char* c)
	{
		int clen = strlen(c);
		for (int i = 0; i < Lines.size(); i++) {
			if (Lines[i]->text.isFirst(c, clen)) {
				delete Lines[i];
				Lines.eraseByIndex(i, 1);
				i--;
			}
		}
	}
	void	ImportSection::TrimLine()//去掉行首行尾空格
	{
		for (int i = 0; i < Lines.size(); i++) {
			Lines[i]->text = Lines[i]->text.trim(' ');
		}
	}
	bool	ImportSection::FromToEnd(const char* Text, int BeginIndex, int TextCount, const char* Find, const char* MustNotExist)
	{
		InText E(Find);
		InText M(MustNotExist);
		text.texts.resize(1024);
		for (int i = BeginIndex; i < TextCount; i++) {
			if (M.Valid(Text[i])) {
				return false;
			}
			else if (E.Valid(Text[i])) {
				int nLen = i - BeginIndex;
				if (nLen > 0) {
					text.texts.resize(nLen, 0);
					memcpy(text.texts.GetPtr(), Text + i, nLen);
					return true;
				}
				return false;
			}
		}
		return false;
	}
	ImportSection::ImportSection(const char* Text, int nBegin, int nCount)
		:ImportSection()
	{
		if (nCount <= 0) {
			nCount = strlen(Text) - nBegin;
		}
		if (nCount > 0) {
			text.texts.resize(nCount);
			memcpy(text.texts.GetPtr(), Text + nBegin, nCount);
		}
	}
	ImportSection*	ImportSection::AddLine(const char* Text, int _BeginIndex, int _Count)
	{
		if (_Count <= 0)
			return 0;
		ImportSection* C = new ImportSection();
		C->BeginIndex = _BeginIndex;
		C->Count = _Count;
		C->text.texts.resize(C->Count);
		memcpy(C->text.texts.GetPtr(), Text + C->BeginIndex, C->Count);
		Lines.push_back(C);
		return C;
	}
	void	ImportSection::RemoveAllLines() {
		for (int i = 0; i < Lines.size(); i++) {
			delete Lines[i];
		}
		Lines.clear();
	}
	bool	ImportSection::SplitLine(char Symbol, bool bAddSymbol) {
		int nLast = 0;
		for (int i = 0; i < text.texts.size(); i++) {
			if (text.texts[i] == Symbol) {
				AddLine(text.texts.GetPtr(), nLast, i - nLast);
				nLast = i + 1;
				if (bAddSymbol) {
					AddLine(text.texts.GetPtr(), i, 1);
				}
			}
		}
		if (nLast < text.texts.size()) {
			AddLine(text.texts.GetPtr(), nLast, text.texts.size() - nLast);
		}
		return (nLast > 0);
	}
	void				ImportSection::SplitBreak(char Symbol)
	{
		for (int i = 0; i < Lines.size(); i++) {
			if (Lines[i]->SplitLine(Symbol, false)) {
				ImportSection* S = Lines[i];
				Lines.eraseByIndex(i, 1);
				for (int j = 0; j < S->Lines.size(); j++) {
					S->Lines[j]->BeginIndex += S->BeginIndex;
					Lines.push_back(&S->Lines[j], 1, i + j);
				}
				i += S->Lines.size() - 1;
				S->Lines.clear();
				delete S;
			}
		}
	}
	void				ImportSection::MergeLines()
	{
		int nLast = 0;
		for (int i = 0; i < Lines.size(); i++) {
			int nSize = Lines[i]->text.size();
			if (nSize > 0) {
				text.texts.resize(nLast + nSize, false);
				memcpy(text.texts.GetPtr() + nLast, Lines[i]->text.str(), nSize);
				nLast += nSize;
			}
		}
	}
	void				ImportSection::TrimNothingLine()
	{
		for (int i = 0; i < Lines.size(); i++) {
			if (Lines[i]->text.texts.size() == 0 || Lines[i]->text.texts[0] == ' ' || Lines[i]->text.texts[0] == '\t') {
				delete Lines[i];
				Lines.eraseByIndex(i, 1);
				i--;
			}
		}
	}
	ImportCPP::ImportCPP()
	{
	}
	ImportCPP::~ImportCPP()
	{
	}

	DexianProgramLanguage*	ImportCPP::Import(const char* File)
	{
		NameT<512> F = File;
		NameT<512> Ext;
		F.splitpath(&m_Path, &m_File, &Ext);
		m_File += Ext;
		return ImportFile(File);
	}
	DexianProgramLanguage*	ImportCPP::ImportFile(const char* File)
	{
		Text S;
		if (!S.load(File))
		{
			LogError("Import (%s) Load Failure\n", File);
			return 0;
		}
		int nIndex = 0;
		//printf("%s\n", S.str());
		S = S.remove('\r');
		S = S.replace('\t', ' ');
		S = S.remove("//", "\n");
		S = S.remove('\n');
		S = S.remove(' ');
		//printf("%s\n", S.str());
		//S.remove("/*", "*/");
		ImportSection IS;
		IS.text = S;
		IS.SplitLine();
		//S.SplitBreak('{');
		//S.SplitBreak('}');
		IS.TrimNothingLine();
		IS.TrimLine();
		IS.TrimDescription("//");
		IS.MergeLines();
		IS.RemoveAllLines();
		DexianProgramLanguage* s = new DexianProgramLanguage();
		if (!ImportNext(m_File, IS.text, s, s)) {
			delete s;
			return 0;
		}
		return s;
	}
	Name	ImportCPP::propText(Array<PropV>& props, const char* name)
	{
		for (int i = 0; i < props.size(); i++) {
			if (props[i].name == name) {
				return props[i].value;
			}
		}
		return Name("");
	}
	bool	ImportCPP::CPP_Type(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent)
	{
		DexianType* de = new DexianType(s, Parent);
		Text name = text.left(":");
		Text right = text.right(":");
		Text cnname = right.left("(");
		Text::skip_symbol ss('(', ')');
		Text minner = right.inner("(", ")", &ss);
		Array<Text> props;
		minner.split(props, ',', &ss);
		Array<PropV> pp;
		for (int i = 0; i < props.size(); i++) {
			PropV v;
			Array<Text> ps;
			props[i].split(ps, '=');
			v.name = ps[0];
			if(ps.size()>1)
				v.value = ps[1];
			pp += v;
		}
		de->EditableFromText(propText(pp, "editor"));
		de->Name = name;
		de->NameCN = cnname;
		de->MemoryWidth = atoi(propText(pp, "size"));
		de->IsPointer = false;
		if (propText(pp, "pointer").size() > 0)
			de->IsPointer = true;
		Text t = propText(pp, "color").str();
		t = t.inner("(", ")");
		de->Color = color4(t);
		s->Types += de;
		return true;
	}
	bool	ImportCPP::CPP_Enum(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent)
	{
		DexianEnum* de = new DexianEnum(s, Parent);
		Text name = text.left(":");
		Text right = text.right(":");
		Text cnname = right.left("(");
		Text::skip_symbol ss('(', ')');
		Text minner = right.inner("(", ")", &ss);
		de->Name = name;
		de->NameCN = cnname;
		Array<Text> props;
		minner.split(props, ',', &ss);
		Array<PropV> pp;
		for (int i = 0; i < props.size(); i++) {
			Text lname = props[i].left("(");
			Text value = props[i].inner("(", ")");
			Array<Text> ps;
			value.split(ps, '=');
			if (ps.size() != 2) {
				LogError("Import (%s) Load Failure(%s)\n", File, text.str());
				return false;
			}
			DexianEnum::EnumElement ee;
			ee.Name = lname;
			ee.Value = atoi(ps[0].str());
			ee.NameCN = ps[1].str();
			de->Values += ee;
		}
		//s->Enums += de;
		if (Parent)
			Parent->Enums += de;
		return true;
	}
	bool	ImportCPP::CPP_Struct(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent)
	{
		DexianStruct* de = new DexianStruct(s, Parent);
		Text name = text.left(":");
		Text right = text.right(":");
		Text cnname = right.left("(");
		Text::skip_symbol ss('(', ')');
		Text minner = right.inner("(", ")", &ss);
		de->Name = name;
		de->NameCN = cnname;
		Array<Text> props;
		minner.split(props, ',', &ss);
		Array<PropV> pp;
		for (int i = 0; i < props.size(); i++) {
			Array<Text> ps;
			props[i].split(ps, '=');
			if (ps.size() < 2) {
				LogError("Import (%s) Load Failure(%s)\n", File, text.str());
				return false;
			}
			TypeMember tm;
			tm.Type = ps[0].str();
			tm.Name = ps[1].str();
			if(ps.size()>2)
				tm.Value = ps[2].str();
			const DexianType* type = s->SearchType(tm.Type);
			if (!type) {
				LogError("Import (%s) Load Failure(%s), type is undefined\n", File, text.str(), tm.Type.str());
				return false;
			}
			tm.ID.id = type->ID;
			de->Members += tm;
		}
		//s->Structs += de;
		if (Parent)
			Parent->Structs += de;
		Parent = de;
		return true;
	}
	bool	ImportCPP::CPP_Method(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent)
	{
		DexianMethod* de = new DexianMethod(s, Parent, true);
		Text name = text.left(":");
		Text right = text.right(":");
		Text cnname = right.left("(");
		Text::skip_symbol ss('(', ')');
		Text minner = right.inner("(", ")", &ss);
		de->Name = name;
		de->NameCN = cnname;
		Array<Text> props;
		minner.split(props, ',', &ss);
		for (int i = 0; i < props.size(); i++) {
			Array<Text> ps;
			props[i].split(ps, '=');
			if (ps.size() < 2) {
				LogError("Import (%s) Load Failure(%s)\n", File, text.str());
				return false;
			}
			TypeMember tm;
			tm.Type = ps[0].str();
			tm.Name = ps[1].str();
			if (ps.size() > 2)
				tm.Value = ps[2].str();
			const DexianType* type = s->SearchType(tm.Type);
			if (!type) {
				LogError("Import (%s) Load Failure(%s), type is undefined\n", File, text.str(), tm.Type.str());
				return false;
			}
			tm.ID.id = type->ID;
			de->Members += tm;
		}
		Text::skip_symbol ssr('{', '}');
		Text rinner = right.inner("{", "}", &ssr);
		Array<Text> props_r;
		rinner.split(props_r, ',', &ssr);
		for (int i = 0; i < props_r.size(); i++) {
			Array<Text> ps;
			props_r[i].split(ps, '=');
			if (ps.size() < 2) {
				LogError("Import (%s) Load Failure(%s)\n", File, text.str());
				return false;
			}
			TypeMember tm;
			tm.Type = ps[0].str();
			tm.Name = ps[1].str();
			if (ps.size() > 2)
				tm.Value = ps[2].str();
			const DexianType* type = s->SearchType(tm.Type);
			if (!type) {
				LogError("Import (%s) Load Failure(%s), type is undefined\n", File, text.str(), tm.Type.str());
				return false;
			}
			tm.ID.id = type->ID;
			de->Results += tm;
		}
		//s->Methods += de;
		if (Parent)
			Parent->Methods += de;
		return true;
	}

	bool	ImportCPP::CPP_Class(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent)
	{
		DexianClass* de = new DexianClass(s, Parent);
		Text name = text.left(":");
		Text right = text.right(":");
		Text cnname = right.left("(");
		Text::skip_symbol ss('(', ')');
		Text minner = right.inner("(", ")", &ss);
		de->Name = name;
		de->NameCN = cnname;
		Array<Text> props;
		minner.split(props, ',', &ss);
		Array<PropV> pp;
		for (int i = 0; i < props.size(); i++) {
			Array<Text> ps;
			props[i].split(ps, '=');
			if (ps.size() < 2) {
				LogError("Import (%s) Load Failure(%s)\n", File, text.str());
				return false;
			}
			TypeMember tm;
			tm.Type = ps[0].str();
			tm.Name = ps[1].str();
			if (ps.size() > 2)
				tm.Value = ps[2].str();
			const DexianType* type = s->SearchType(tm.Type);
			if (!type) {
				LogError("Import (%s) Load Failure(%s), type is undefined\n", File, text.str(), tm.Type.str());
				return false;
			}
			tm.ID.id = type->ID;
			de->Members += tm;
		}
		//s->Classes += de;
		if (Parent)
			Parent->Classes += de;
		Parent = de;
		return true;
	}

	bool	ImportCPP::CPP_Event(const char* File, Text& text, DexianProgramLanguage* s, DexianStruct*& Parent)
	{
		DexianEvent* de = new DexianEvent(s, Parent);
		Text name = text.left(":");
		Text right = text.right(":");
		Text cnname = right.left("{");
		Text::skip_symbol ss('{', '}');
		Text minner = right.inner("{", "}", &ss);
		de->Name = name;
		de->NameCN = cnname;
		Array<Text> props;
		minner.split(props, ',', &ss);
		Array<PropV> pp;
		for (int i = 0; i < props.size(); i++) {
			Array<Text> ps;
			props[i].split(ps, '=');
			if (ps.size() < 2) {
				LogError("Import (%s) Load Failure(%s)\n", File, text.str());
				return false;
			}
			TypeMember tm;
			tm.Type = ps[0].str();
			tm.Name = ps[1].str();
			if (ps.size() > 2)
				tm.Value = ps[2].str();
			const DexianType* type = s->SearchType(tm.Type);
			if (!type) {
				LogError("Import (%s) Load Failure(%s), type is undefined\n", File, text.str(), tm.Type.str());
				return false;
			}
			tm.ID.id = type->ID;
			de->Results += tm;
		}
		//s->Methods += de;
		if (Parent)
			Parent->Events += de;
		return true;
	}

	bool	ImportCPP::ImportNext(const char* File, const Text& text, DexianProgramLanguage* system, DexianStruct* parent)
	{
		Text::skip_symbol skips('{', '}');
		Array<Text> lines, blines;
		text.split(lines, '?', &skips);
		Text::breaks(blines, lines, ';', &skips);
		//ImportSection Sec;
		//Sec.text = text;
		//Sec.SplitLine('?');
		//Sec.SplitBreak(';');
		//printf(Sec.str());
		//for (int i = 0; i < Sec.Lines.size(); i++) {
		//	printf("%s\n", Sec.Lines[i]->text.str());
		//}
		Array<TypeCB> Types;
		Types += TypeCB("type", &ImportCPP::CPP_Type);
		Types += TypeCB("enum", &ImportCPP::CPP_Enum);
		Types += TypeCB("struct", &ImportCPP::CPP_Struct);
		Types += TypeCB("method", &ImportCPP::CPP_Method);
		Types += TypeCB("class", &ImportCPP::CPP_Class);
		Types += TypeCB("event", &ImportCPP::CPP_Event);
		//Name Desc("//");
		Name CPP("C++");
		ProcImport CurrProc = 0;
		for (int i = 0; i < blines.size(); i++) {
			Text& t = blines[i];
			if (t.isFirst(CPP, CPP.size()))
			{
				system->Lan = system->CPlusPlus;
				//Result = new DexianCPPSystem();
			}
			bool bIsType = false;
			if (system) {
				for (int j = 0; j < Types.size(); j++) {
					if (t.isFirst(Types[j], Types[j].size()))
					{
						CurrProc = Types[j].cb;
						bIsType = true;
						break;
					}
				}
				if(!bIsType && CurrProc){
					DexianStruct* P = parent;
					if (!(this->*CurrProc)(File, t, system, P))
					{
						return false;
					}
					if (i < (blines.size() - 1)) {
						Text& tt = blines[i + 1];
						if (tt[0] == '{') {
							if (!ImportNext(File, tt.inner("{", "}"), system, P)) {
								return false;
							}
							i++;
						}
					}
				}
			}
		}
		return true;
	}
};
