#ifndef __DEXIAN_EXEC_H__
#define __DEXIAN_EXEC_H__
#include <vector>
#include <string>
#include "PhantomBase.h"
namespace Phantom {

	typedef NameT<128>						TypeName;
	struct	NameID: public TypeName
	{
		int		id, index;
	};
	class	DexianStruct;
	class	DexianEvent;
	class	DexianMethod;
	class	DexianClass;
	class	DexianProgramLanguage;

	class	DexianType {
	public:
		int									GetID() { return ID; }
		const char*							GetName() { return Name; }
		//enum Editable {//操作符只有int64和double，小一些的数字也会被转换成这两个值进行计算，
		//				   //在csv中生命的时候也是包含可操作符号(I,D,T)三种（整数，浮点，文本）
		//	Editable_None = 0,
		//	Editable_Int,
		//	Editable_Float,
		//	Editable_Int64,
		//	Editable_Double,
		//	Editable_Text,
		//	Editable_Memory,
		//	Editable_Custom,//用户自定义其他
		//};
		void								EditableFromText(const char* text);
		virtual	void						OnPrint(const char* pre = "");
		DexianType(DexianProgramLanguage* s, DexianStruct* parent);
		~DexianType();
		//Editable							Edit;
		TypeName							Name, NameCN;
		color4								Color;
		EditLimit							Limit;
		char								IsPointer;//是否指针类型
		int									MemoryWidth;//内存块大小
		DexianStruct*						Parent;
		DexianProgramLanguage*						System;
		//
		int									ID;
		friend class DexianExecSystem;
	};
	class	DexianEnum: public DexianType {
	public:
		DexianEnum(DexianProgramLanguage* s, DexianStruct* parent);
		~DexianEnum() {}
		virtual	void						OnPrint(const char* pre);
		struct EnumElement {
			TypeName						Name;
			TypeName						NameCN;
			int								Value;
		};
		Array<EnumElement>					Values;
	};
	struct TypeMember {
		TypeName							Type;
		TypeName							Name;
		TypeName							Value;
		PtrID								ID;
	};
	class	DexianStruct: public DexianType {
	public:
		DexianStruct(DexianProgramLanguage* s, DexianStruct* parent);
		~DexianStruct() {}
		virtual	void						OnPrint(const char* pre);
		Array<TypeMember>					Members;//成员列表
		Array<DexianEnum*>					Enums;
		Array<DexianStruct*>				Structs;
		Array<DexianMethod*>				Methods;
		Array<DexianClass*>					Classes;
		Array<DexianEvent*>					Events;
	};
	class	DexianEvent: public DexianType
	{
	public:
		DexianEvent(DexianProgramLanguage* s, DexianStruct* parent);
		~DexianEvent();
		virtual	void						OnPrint(const char* pre);
		Array<TypeMember>					Results;//返回成员列表
	};
	class	DexianMethod : public DexianEvent
	{
	public:
		DexianMethod(DexianProgramLanguage* s, DexianStruct* parent, bool bCallable);
		~DexianMethod();
		virtual	void						OnPrint(const char* pre);
		bool								IsCallable;
		Array<TypeMember>					Members;//成员列表
	};
	class	DexianClass: public DexianStruct
	{
	public:
		DexianClass(DexianProgramLanguage* s, DexianStruct* parent);
		~DexianClass();
	public:
	};
	class	DexianProgramLanguage: public DexianClass
	{
	public:
		DexianProgramLanguage();
		~DexianProgramLanguage();
		virtual	void						OnPrint(const char* pre);
		virtual void						Initialize();
		//bool								LoadBaseType(const char* csvFile);
		////添加一个类型
		//bool								AddStruct(const char* csvFile);
		//DexianType*						
		const DexianType*					SearchType(PtrID& id) const;
		const DexianType*					SearchType(const char* type) const;
		enum Language {
			None = 0,
			CPlusPlus = 1,
		};
		Language							Lan;
	public:
		Array<DexianType*>					Types;
		//Array<DexianEnum*>					Enums;
		//Array<DexianStruct*>				Structs;
		//Array<DexianMethod*>				Methods;
		//Array<DexianClass*>					Classes;
		Array<DexianType*>					AllTypes;
	};

	class	DexianCPPSystem: public DexianProgramLanguage
	{
	public:
	};
	class DexianPlaceExec;
	class DexianPlace;
	typedef NameT<128>						PointName;
	struct	PlaceNodeID
	{
		PtrID		Place;
		PtrID		Exec;
		PtrID		Node;
		PlaceNodeID() {};
		PlaceNodeID(int PlaceID, int ExecID, int NodeID) {
			Place.id = PlaceID;
			Exec.id = ExecID;
			Node.id = NodeID;
		}
	};
	class	DexianExecNodePoint
	{
	public:
		enum Node_Type {
			Node_UnKnown = 0,
			Node_EventInner,
			Node_EventOuter,
			Node_TypeInner,
			Node_TypeOuter,
		};
		DexianExecNodePoint();
		DexianExecNodePoint(DexianPlaceExec* v, const char* _Name, Node_Type type);
		bool								IsLinkable(const DexianExecNodePoint* Other) const;
		void								AddLink(const DexianExecNodePoint* nP);
		inline	int							GetID() const { return ID; }
	public:
		PointName							Name;
		Node_Type							Type;
		PtrID								TypeID;
		DexianPlaceExec*					Viewer;
		Array<PlaceNodeID>					Next;
	private:
		int									ID;
	};
	class	DexianPlaceExec
	{
	public:
		DexianPlaceExec(DexianProgramLanguage* _Sys, DexianPlace* _Place);
		~DexianPlaceExec();
		void								RebuildNodes();
	public:
		Array<DexianExecNodePoint*>				LeftEvents;
		Array<DexianExecNodePoint*>				RightEvents;
		Array<DexianExecNodePoint*>				LeftParams;
		Array<DexianExecNodePoint*>				RightParams;
		TypeName							Name;
		DexianProgramLanguage*				System;
		DexianPlace*						Place;
		Array<DexianExecNodePoint*>				Nodes;
		inline	int							GetID() const { return ID; }
	private:
		int									ID;
	};
	class	DexianPlace
	{
	public:
		DexianPlace();
		virtual ~DexianPlace();
		DexianPlaceExec*					AddEvent(DexianEvent* e);
		DexianPlaceExec*					AddViewer(DexianMethod* m);
		Array<DexianPlaceExec*>				Viewers;
		inline	int							GetID() const { return ID; }
	private:
		int									ID;
	};

	class	DexianSystem {
	public:
		DexianSystem();
		~DexianSystem();
		bool								Import(const char* file);//
		DexianPlace*						AddPlace();
		DexianExecNodePoint*				GetExecPoint(PlaceNodeID& id);
	public:
		Array<DexianProgramLanguage*>		Programs;
		Array<DexianPlace*>					Places;
		unsigned int						NewIdentity() { return (Identity++); }
	protected:
		unsigned int						Identity;
	};
	DexianSystem*		GetDexianSystem();
	//enum DexianOperation
	//{
	//	DAdd = 0,
	//	DSubtract,
	//	DMultiple,
	//	DDivide,
	//	DMod,
	//};
	//enum EnumBaseType {//操作符只有int64和double，小一些的数字也会被转换成这两个值进行计算，
	//	//在csv中生命的时候也是包含可操作符号(I,D,T)三种（整数，浮点，文本）
	//	Dint = 0,
	//	Dfloat,
	//	Dint64,
	//	Ddouble,
	//	//Dchar = 0,
	//	//Duchar,
	//	//Dshort,
	//	//Dushort,
	//	//Dint,
	//	//Duint,
	//	//Dint64,
	//	//Duint64,
	//	//Dfloat,
	//	//Ddouble,
	//	Dtext,
	//	Dcustom,//用户自定义其他
	//};


};

#endif
