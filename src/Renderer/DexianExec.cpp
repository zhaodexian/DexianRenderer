// DexianLanguage.cpp : 定义 DLL 应用程序的导出函数。
//

#include "DexianExec.h"
#include "ImportCPP.h"

namespace Phantom {

	DexianType::DexianType(DexianProgramLanguage* s, DexianStruct* parent) {
		//Edit = Editable_None;
		Color.setRGBA(1, 1, 1, 1);
		Limit = Edit_Text;
		ID = GetDexianSystem()->NewIdentity();
		IsPointer = false;//是否指针类型
		MemoryWidth = sizeof(int);//内存块大小
		System = s;
		Parent = parent;
	}
	DexianType::~DexianType() {

	}
	void	DexianType::OnPrint(const char* pre)
	{
		printf("%s %s(%s)Editable(%d)Memory(%d)Ispointer(%d)\n", pre, Name.t, NameCN.t, Limit, MemoryWidth, IsPointer);
	}
	void	DexianType::EditableFromText(const char* text)
	{
		if (stricmp(text, "int") == 0)
		{
			//Edit = Editable_Int;
			Limit = Edit_Int;
		}
		else if (stricmp(text, "bool") == 0) {
			Limit = Edit_Boolean;
		}
		else if (stricmp(text, "Float") == 0)
		{
			//Edit = Editable_Float;
			Limit = Edit_Float;
		}
		else if (stricmp(text, "Int64") == 0) {
			//Edit = Editable_Int64;
			Limit = Edit_Int;
		}
		else if (stricmp(text, "Double") == 0) {
			//Edit = Editable_Double;
			Limit = Edit_Float;
		}
		else if (stricmp(text, "Text") == 0) {
			//Edit = Editable_Text;
			Limit = Edit_Text;
		}
		else if (stricmp(text, "Memory") == 0) {
			//Edit = Editable_Memory;
			Limit = Edit_None;
		}
		else {
			Limit = Edit_None;
		}
		//Edit = Editable_None;
	}
	DexianEnum::DexianEnum(DexianProgramLanguage* s, DexianStruct* parent)
		:DexianType(s, parent)
	{
	}
	void	DexianEnum::OnPrint(const char* pre)
	{
		char mypre[512];
		sprintf(mypre, " %s %s", pre, Name.t);
		printf("%s{ %s(%s)Memory(%d)\n", mypre, Name.t, NameCN.t, MemoryWidth);
		for (int i = 0; i < Values.size(); i++) {
			printf("%s members {%s(%s)=%d}\n", mypre, Values[i].Name.t, Values[i].NameCN.t, Values[i].Value);
		}
		printf("%s };\n", mypre);
	}
	DexianStruct::DexianStruct(DexianProgramLanguage* s, DexianStruct* p)
		:DexianType(s, p)
	{
		Parent = p;
	}
	void		DexianStruct::OnPrint(const char* pre)
	{
		char mypre[512];
		sprintf(mypre, " %s %s", pre, Name.t);
		printf("%s{ %s(%s)Memory(%d)\n", mypre, Name.t, NameCN.t, MemoryWidth);
		if (Members.size() > 0) {
			for (int i = 0; i < Members.size(); i++) {
				printf("%s member {%s(%s)=%s}\n", mypre, Members[i].Name.t, Members[i].Type.t, Members[i].Value.t);
			}
		}
		if (Enums.size() > 0) {
			printf("%s enums{\n", mypre);
			for (int i = 0; i < Enums.size(); i++) {
				Enums[i]->OnPrint(mypre);
			}
			printf("%s };\n", pre);
		}
		if (Structs.size() > 0) {
			printf("%s structs{\n", mypre);
			for (int i = 0; i < Structs.size(); i++) {
				Structs[i]->OnPrint(mypre);
			}
			printf("%s };\n", pre);
		}
		if (Methods.size() > 0) {

			printf("%s methods{\n", mypre);
			for (int i = 0; i < Methods.size(); i++) {
				Methods[i]->OnPrint(mypre);
			}
			printf("%s };\n", pre);
		}
		if (Classes.size() > 0) {

			printf("%s classes{\n", mypre);
			for (int i = 0; i < Classes.size(); i++) {
				Classes[i]->OnPrint(mypre);
			}
			printf("%s };\n", pre);
		}
		printf("%s };\n", mypre);
	}
	DexianMethod::DexianMethod(DexianProgramLanguage* s, DexianStruct* parent, bool bCallable)
		: DexianEvent(s, parent)
	{
		IsCallable = bCallable;
	}
	DexianMethod::~DexianMethod()
	{
	}
	void	DexianMethod::OnPrint(const char* pre)
	{
		char mypre[512];
		sprintf(mypre, " %s %s", pre, Name.t);
		printf("%s{ %s(%s)Memory(%d)\n", mypre, Name.t, NameCN.t, MemoryWidth);
		for (int i = 0; i < Members.size(); i++) {
			printf("%s member {%s(%s)=%s}\n", mypre, Members[i].Name.t, Members[i].Type.t, Members[i].Value.t);
		}
		printf("%s };\n", mypre);
	}
	DexianEvent::DexianEvent(DexianProgramLanguage* s, DexianStruct* parent)
		: DexianType(s, parent)
	{
	}
	DexianEvent::~DexianEvent()
	{
	}
	void	DexianEvent::OnPrint(const char* pre)
	{
		char mypre[512];
		sprintf(mypre, " %s %s", pre, Name.t);
		printf("%s{ %s(%s)Event\n", mypre, Name.t, NameCN.t);
		for (int i = 0; i < Results.size(); i++) {
			printf("%s results {%s(%s)=%s}\n", mypre, Results[i].Name.t, Results[i].Type.t, Results[i].Value.t);
		}
		printf("%s };\n", mypre);
	}
	DexianClass::DexianClass(DexianProgramLanguage* s, DexianStruct* parent)
		:DexianStruct(s, parent)
	{

	}
	DexianClass::~DexianClass()
	{

	}
	void					DexianProgramLanguage::OnPrint(const char* pre)
	{
		char mypre[512];
		sprintf(mypre, "%s %s", pre, Name.t);
		printf("%s{ %s(%s)Memory(%d)\n", mypre, Name.t, NameCN.t, MemoryWidth);
		printf("%s types{\n", mypre);
		for (int i = 0; i < Types.size(); i++) {
			Types[i]->OnPrint(mypre);
		}
		printf("%s };\n", mypre);
		DexianClass::OnPrint(pre);
		printf("%s };\n", mypre);
	}
	const DexianType*		DexianProgramLanguage::SearchType(const char* type) const
	{
		for (int i = 0; i < Types.size(); i++) {
			if (Types[i]->Name == type || Types[i]->NameCN == type) {
				return Types[i];
			}
		}
		for (int i = 0; i < Enums.size(); i++) {
			if (Enums[i]->Name == type || Enums[i]->NameCN == type) {
				return Enums[i];
			}
		}
		for (int i = 0; i < Structs.size(); i++) {
			if (Structs[i]->Name == type || Structs[i]->NameCN == type) {
				return Structs[i];
			}
		}
		for (int i = 0; i < Classes.size(); i++) {
			if (Classes[i]->Name == type || Classes[i]->NameCN == type) {
				return Classes[i];
			}
		}
		return 0;
	}
	DexianProgramLanguage::DexianProgramLanguage()
		:DexianClass(0, 0)
	{
		Lan = None;
	}
	DexianProgramLanguage::~DexianProgramLanguage()
	{
	}
	void				DexianProgramLanguage::Initialize()
	{
		AllTypes.clear();
		for (int i = 0; i < Types.size(); i++) {
			AllTypes += Types[i];
			DexianMethod* m = new DexianMethod(this, this, false);
			m->Name = TypeName("Make ") + Types[i]->Name;
			m->NameCN = TypeName("Make ") + Types[i]->NameCN;
			TypeMember tm;
			tm.Type = Types[i]->Name;
			//tm.Name = Types[i]->Name;
			tm.ID.id = Types[i]->ID;
			m->Members += tm;
			//tm.Name = "";
			m->Results += tm;
			Methods += m;
		}
		for (int i = 0; i < Enums.size(); i++) {
			AllTypes += Enums[i];
		}
		for (int i = 0; i < Structs.size(); i++) {
			AllTypes += Structs[i];
		}
		for (int i = 0; i < Classes.size(); i++) {
			AllTypes += Classes[i];
		}
	}
	const DexianType*	DexianProgramLanguage::SearchType(PtrID& id) const
	{
		if (id.index < AllTypes.size() && AllTypes[id.index]->ID == id.id)
			return AllTypes[id.index];
		for (int i = 0; i < AllTypes.size(); i++) {
			if (AllTypes[i]->ID == id.id)
			{
				id.index = i;
				return AllTypes[i];
			}
		}
		id.id = 0;
		return 0;
	}
	DexianPlace::DexianPlace()
	{
		ID = GetDexianSystem()->NewIdentity();
	}
	DexianPlace::~DexianPlace()
	{
		destroy(Viewers);
	}
	DexianPlaceExec::DexianPlaceExec(DexianProgramLanguage* _Sys, DexianPlace* _Place)
	{
		System = _Sys;
		Place = _Place;
		ID = GetDexianSystem()->NewIdentity();
	}
	DexianPlaceExec::~DexianPlaceExec()
	{
		destroy(Nodes);
	}
	void	DexianPlaceExec::RebuildNodes()
	{
		Nodes.clear();
		for (int i = 0; i < LeftEvents.size(); i++) {
			Nodes += LeftEvents[i];
		}
		for (int i = 0; i < RightEvents.size(); i++) {
			Nodes += RightEvents[i];
		}
		for (int i = 0; i < LeftParams.size(); i++) {
			Nodes += LeftParams[i];
		}
		for (int i = 0; i < RightParams.size(); i++) {
			Nodes += RightParams[i];
		}
	}
	DexianExecNodePoint::DexianExecNodePoint() {
		Viewer = 0;
		Type = Node_UnKnown;
		ID = GetDexianSystem()->NewIdentity();
	}
	DexianExecNodePoint::DexianExecNodePoint(DexianPlaceExec* v, const char* _Name, Node_Type type)
	{
		Name = _Name;
		Viewer = v;
		Type = type;
		ID = GetDexianSystem()->NewIdentity();
	}
	void		DexianExecNodePoint::AddLink(const DexianExecNodePoint* nP)
	{
		for (int i = 0; i < Next.size(); i++) {
			if (Next[i].Place.id == nP->Viewer->Place->GetID() && Next[i].Exec.id == nP->Viewer->GetID() && Next[i].Node.id == nP->GetID())
				return;
		}
		Next += PlaceNodeID(nP->Viewer->Place->GetID(), nP->Viewer->GetID(), nP->GetID());
	}
	bool	DexianExecNodePoint::IsLinkable(const DexianExecNodePoint* Other) const
	{
		if (Type == Node_EventInner)
		{
			if (Other->Type == Node_EventOuter)
				return true;
		}
		else if (Type == Node_EventOuter)
		{
			if (Other->Type == Node_EventInner)
				return true;
		}
		else if (Type == Node_TypeInner)
		{
			if (Other->Type == Node_TypeOuter && TypeID.id == Other->TypeID.id)
				return true;
		}
		else if (Type == Node_TypeOuter)
		{
			if (Other->Type == Node_TypeInner && TypeID.id == Other->TypeID.id)
				return true;
		}
		return false;
	}
	DexianPlaceExec*	DexianPlace::AddEvent(DexianEvent* e)
	{
		DexianPlaceExec* v = new DexianPlaceExec(e->System, this);
		v->Name = e->Name;
		v->Place = this;
		v->System = e->System;
		v->RightEvents += new DexianExecNodePoint(v, "", DexianExecNodePoint::Node_EventOuter);
		for (int i = 0; i < e->Results.size(); i++) {
			DexianExecNodePoint* n = new DexianExecNodePoint(v, e->Results[i].Name, DexianExecNodePoint::Node_TypeOuter);
			n->TypeID = e->Results[i].ID;
			v->RightParams += n;
		}
		v->RebuildNodes();
		Viewers += v;
		return v;
	}
	DexianPlaceExec*	DexianPlace::AddViewer(DexianMethod* m)
	{
		DexianPlaceExec* v = new DexianPlaceExec(m->System, this);
		v->Name = m->Name;
		v->Place = this;
		v->System = m->System;
		if (m->IsCallable) {
			v->LeftEvents += new DexianExecNodePoint(v, "", DexianExecNodePoint::Node_EventInner);
			v->RightEvents += new DexianExecNodePoint(v, "", DexianExecNodePoint::Node_EventOuter);
		}
		for (int i = 0; i < m->Members.size(); i++) {
			DexianExecNodePoint* n = new DexianExecNodePoint(v, m->Members[i].Name, DexianExecNodePoint::Node_TypeInner);
			n->TypeID = m->Members[i].ID;
			v->LeftParams += n;
		}
		for (int i = 0; i < m->Results.size(); i++) {
			DexianExecNodePoint* n = new DexianExecNodePoint(v, m->Results[i].Name, DexianExecNodePoint::Node_TypeOuter);
			n->TypeID = m->Results[i].ID;
			v->RightParams += n;
		}
		v->RebuildNodes();
		Viewers += v;
		return v;
	}
	DexianSystem*		GetDexianSystem()
	{
		static DexianSystem s;
		return &s;
	}
	DexianSystem::DexianSystem()
	{
		Identity = 1;
	}
	DexianSystem::~DexianSystem()
	{
		destroy(Programs);
		destroy(Places);
	}
	DexianExecNodePoint*	DexianSystem::GetExecPoint(PlaceNodeID& id)
	{
		DexianPlace* Place = search(Places, id.Place);
		if (!Place)return 0;
		DexianPlaceExec* P = search(Place->Viewers, id.Exec);
		if (!P)return 0;
		DexianExecNodePoint* NP = search(P->Nodes, id.Node);
		return NP;
	}
	bool			DexianSystem::Import(const char* file)
	{
		ImportCPP c;
		DexianProgramLanguage* l = c.Import(file);
		if (!l)
			return false;
		Programs += l;
		l->Initialize();
		return true;
	}
	DexianPlace*	DexianSystem::AddPlace()
	{
		DexianPlace* p = new DexianPlace();
		Places += p;
		return p;
	}
};
