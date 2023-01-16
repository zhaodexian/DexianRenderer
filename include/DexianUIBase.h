#pragma once
#include "PhantomBase.h"
#include "PhantomDevice.h"
#include "DexianExec.h"
#include "MyInclude.h"
#include <functional>

namespace Phantom {
	class DexianDrawEditText;
	class Texture;
	class MyFont;
	class DexianControl;
	class DexianStatic;
	class DexianButton;
	class DexianGrids;
	class DexianControl;
	class DexianViewerExec;
	class DexianViewerPlace;
	class DexianListBox;
	class DexianDialog;
	class DexianManager;
	class DexianEditBox;
};
namespace Phantom {
	typedef void(*OpenEditBoxProc)(const RectF& rc, const char* Text, DexianDrawEditText* Edit);
	typedef void(*ButtonUp)(DexianControl* b, int touchIndex, const PixelF& pos);
	typedef std::function<void(DexianButton* b)>				ButtonClickProc;
	typedef std::function<void(int selectedIndex)>				ListBoxSelectedProc;
	class  PHANTOM_API DexianFont: public Base
	{
	public:
		ImplementComValue();
		Name			fontName;
		int				nFontHeight;
		int				nWeight;
		char			bItalic;
		char			bUnderline;
		inline	bool	operator == (const DexianFont& f) const
		{
			if (nFontHeight != f.nFontHeight || nWeight != f.nWeight || bItalic != f.bItalic || bUnderline != f.bUnderline || (stricmp(fontName.t, f.fontName.t) != 0))
				return false;
			return true;
		}
		AutoPtr<MyFont>	fontPtr;
		inline	int		GetHeight() { if (nFontHeight < 0)return -nFontHeight; return nFontHeight; }
		DexianFont()
		{
			nFontHeight = 0;
			nWeight = 0;
			bItalic = 0;
			bUnderline = 0;
		}
		~DexianFont();
	};

	class  PHANTOM_API DexianTexture: public Base
	{
	public:
		ImplementComValue();
		FileName					fileName;
		char						bLoadFail;
		INT							nRef;
		int							filter;
		AutoPtr<Texture>			texturePtr;
		BaseDirectory*				Dir;
		DexianTexture() { nRef = 1; bLoadFail = false; filter = 1; Dir = 0; }
		~DexianTexture();
	};

#pragma pack(push, 1)
	struct	DexianElement
	{
		//font 和 texture 两个中只能存在一个
		PtrID		 			TextureIndex;//0xFFFF is none texture
		RectF					TextureRegion;
		PtrID					FontIndex;
		unsigned short			TextFormat;
		color4 					Color;
		color4					ShadowColor;
		void					InitTexture(const char* File, const RectF* Region, BaseDirectory* dir = 0);
		//void					InitFont
		bool					bDisplay;
		//
		enum ElementRenderMode {
			Scale = 0,
			Wrap,
			Tile,
		};
		PixelF					WrapWidth;
		ElementRenderMode		Mode;
		DexianElement();
	};


	enum AlignModeX {
		AlignLeft = 0,
		AlignRight,
		AlignLeftRight,
		AlignCenterX,
	};
	enum AlignModeY {
		AlignTop = 0,
		AlignBottom,
		AlignTopBottom,
		AlignCenterY,
	};
	class PHANTOM_API  DexianDrawCurveLine
	{
	public:
		DexianDrawCurveLine(DexianViewerPlace* Place);
		~DexianDrawCurveLine();
		bool							OnRender(float fElapsedTime, bool bIsEnabled);//如果找不到连接点返回false，可以被删除
		void							Render(const Pixel& from, const Pixel& to, bool bEnabled);//如果找不到连接点返回false，可以被删除
		PlaceNodeID						FromEventPoint, ToEventPoint;//DexianDrawBase
		void							RebuildLines();
		Pixel							FromPoint, ToPoint;
		color4							Color;
		RectF							PointRect;
		TypeArray<PixelF>				Points;
		DrawBuffer						Buffer;
		DexianViewerPlace*				Place;
	};

	class	 PHANTOM_API DexianDrawBase
	{
	public:
		DexianDrawBase(DexianControl* Shape);
		virtual ~DexianDrawBase();
		virtual	void			Render(const RectF& WindowRect, bool bEnabled) = 0;
		virtual	void			RenderShadow(const RectF& WindowRect, int ShadowWidth) = 0;
		virtual	bool			Pick(const RectF& WindowRect, const PixelF& pos) = 0;
		virtual void			OnPick() {}
		virtual void			OnEnter() {}
		virtual void			OnLeave() {}
		virtual void			OnButtonDown(const Pixel& pos) {}
		virtual void			OnButtonUp(const Pixel& pos) {}
		virtual void			OnMoving(const Pixel& pos) {}
		virtual void			SetScale(float fScale);
		virtual void			SetRotation(int nRotation);
		inline	int				GetLayer() { return Layer; }
		inline	DexianControl*	GetShapePtr() { return ShapePtr; }
		void					SetLayer(int nLayer);
		void					Invalidate();
		CursorModes				GetCursorMode() { return Cursor; }
		AlignModeX				ModeX;
		AlignModeY				ModeY;
		enum {
			Type_Unknown = 0,
			Type_Rect,
			Type_Texture,
			Type_RoundRect,
			Type_Circle,
			Type_EventPoint,
			Type_Text,
			Type_EditText,
		};
		virtual int				GetType() = 0;
		bool					Display;
		DrawBuffer				Buffer;
		DrawBuffer				SBuffer;
		Pixel					Offset;
		char					Shadow;
		char					Pickable;
		CursorModes				Cursor;
		int						Rotation;
	protected:
		DexianControl*			ShapePtr;
		int						Layer;
		friend class DexianControl;
	};
	class	 PHANTOM_API DexianDrawRect : public DexianDrawBase
	{
	public:
		DexianDrawRect(DexianControl* Shape):DexianDrawBase(Shape){}
		Rect					GetRenderRect(const RectF& WindowRect);
		void					Render(const RectF& WindowRect, bool bEnabled);
		void					RenderShadow(const RectF& WindowRect, int ShadowWidth);
		bool					Pick(const RectF& WindowRect, const PixelF& pos);
		virtual int				GetType() { return Type_Rect; }
		RectF					rc;
		color4					color;
		Rect					Last;
	};
	class	 PHANTOM_API DexianDrawTexture : public DexianDrawRect
	{
	public:
		DexianDrawTexture(DexianControl* Shape);
		virtual ~DexianDrawTexture();
		void					Render(const RectF& WindowRect, bool bEnabled);
		void					RenderShadow(const RectF& WindowRect, int ShadowWidth);
		virtual int				GetType() { return Type_Texture; }
		//
		void					SetCenterMode(bool bIsCenterMode, const float2* Center);
		void					SetPosition(const float2& Pos);
		//按照角度进行设置
		virtual void			SetRotation(int nRotation);
		int						AddTexture(const color4& c, const char* File, const RectF* Region = 0, BaseDirectory* dir = 0);
		void					PlayMultiFrame(int BeginIndex, int EndIndex, float fSpeed);
		void					PlayOneFrame(int IdleIndex);
		void					StopFrame();
		DexianElement			Element;
	protected:
		Array<DexianElement>	Frames;
		int						FrameMode;
		int						FrameBegin, FrameEnd;
		float					FrameSpeed;
		float					FrameIndex;
		float2					Center;
		float2					Position;
		bool					IsCenterMode;
	};

	class	 PHANTOM_API DexianDrawRoundRect : public DexianDrawRect
	{
	public:
		DexianDrawRoundRect(DexianControl* Shape):DexianDrawRect(Shape){}
		void					Render(const RectF& WindowRect, bool bEnabled);
		void					RenderShadow(const RectF& WindowRect, int ShadowWidth);
		bool					Pick(const RectF& WindowRect, const PixelF& pos);
		virtual int				GetType() { return Type_RoundRect; }
		float					Round;
		int						RoundMode;
	};
	class	 PHANTOM_API DexianDrawCircle : public DexianDrawBase
	{
	public:
		DexianDrawCircle(DexianControl* Shape):DexianDrawBase(Shape){}
		PixelF					GetCenter(const RectF& WindowRect);
		void					Render(const RectF& WindowRect, bool bEnabled);
		void					RenderShadow(const RectF& WindowRect, int ShadowWidth);
		bool					Pick(const RectF& WindowRect, const PixelF& pos);
		virtual int				GetType() { return Type_Circle; }
		Pixel					Point;
		PixelF					Center;
		float					Radius;
		color4					Color;
		int						Smooth;
	};
	class  PHANTOM_API DexianDrawEventPoint: public DexianDrawTexture
	{
	public:
		DexianDrawEventPoint(DexianViewerPlace* Place, DexianControl* Shape);
		~DexianDrawEventPoint();
		virtual	void			Render(const RectF& WindowRect, bool bEnabled);
		virtual	void			RenderShadow(const RectF& WindowRect, int ShadowWidth);
		inline	unsigned int	GetID() { return ID; }
		virtual int				GetType() { return Type_EventPoint; }
		bool					Pick(const RectF& WindowRect, const PixelF& pos);
		virtual void			OnButtonDown(const Pixel& pos);
		virtual void			OnButtonUp(const Pixel& pos);
		virtual void			OnMoving(const Pixel& pos);
		virtual void			OnEnter();
		virtual void			OnLeave();
		void					RebuildLines();
		DexianElement			FullElement;
		Pixel					Point;
		char					PickEnter;
		int						PickWidth;
		char					Linked;
		char					IsLeft;
		unsigned int			ID;
		DexianDrawEventPoint*	CurrLink;
		TypeArray<DexianDrawCurveLine*>	Lines;
		DexianViewerPlace*		Place;
		PlaceNodeID				Node;
		Array<PlaceNodeID>		Next;
	};
	class  PHANTOM_API DexianDrawText: public DexianDrawRect
	{
	public:
		DexianDrawText(DexianControl* Shape);
		~DexianDrawText();
		virtual	void			Render(const RectF& WindowRect, bool bEnabled);
		virtual	void			RenderShadow(const RectF& WindowRect, int ShadowWidth);
		void					SetText(const char* szText);
		void					SetPosition(const float2& Pos);
		virtual int				GetType() { return Type_Text; }
		RenderText*				TextPtr;
		int						FontSize;
		char					InvalidText;
		bool					bTextBack;
	};
	class PHANTOM_API  DexianDrawEditText : public DexianDrawRect
	{
	public:
		DexianDrawEditText(DexianControl* Shape);
		~DexianDrawEditText();
		virtual	void			Render(const RectF& WindowRect, bool bEnabled);
		virtual	void			RenderShadow(const RectF& WindowRect, int ShadowWidth);
		void					Redraw();
		void					SetText(const char* szText);
		void					SetTextSize(const Size& sz);
		bool					Pick(const RectF& WindowRect, const PixelF& pos);
		virtual int				GetType() { return Type_EditText; }
		virtual void			OnEnter();
		virtual void			OnLeave();
		virtual void			OnPick();
		DrawBuffer				RectBuffer;
		RenderEditText*			TextPtr;
		int						FontSize;
		char					InvalidText;
		RectF					FrameRect;
		RectF					TextureRect;
		//unsigned short			CursorPos, SelectPos;
	};
	
	struct	DexianControlData
	{
		char							bEnabled;
		PtrID							Group;
		DexianElement					Back;
		enum ControlType {
			Type_Base,
			Type_Static,
			Type_Button,
			Type_Text,
		};
		//
		char 							Display;
		NameT<64>						Name;
		char							bRotation;
		float							fRotation;
		PixelF							RotationCenter;
		RectF							ClientRect;
		RectF							WindowRect;
		RectF							offsetRect;
		char							bIsPickPixel;
		short							Layer;
		matrix4x4						Mat;
		char							bMoveDown;
		PixelF							MoveOffset;
		PixelF							DownPos;
		char							bIsInvalidate;
		char							bIsCaption;//是否标题栏
		char							bClipInWindow;
		char							bMoveEnable;
		char							bAlign;
		float							Scale;
		DexianControlData();
	};
#pragma pack(pop)
	typedef std::function<void()>		RenderProc;
	typedef std::function<bool()>		PickProc;
	//[this]() {DexianControl::OnCustomRender(); }
	class PHANTOM_API  DexianControl : public Base, public DexianControlData
	{
	public:
		ImplementComValue();
		DexianControl(DexianControl* dlg);
		virtual ~DexianControl();
		//DexianUIControlBase				m_this;
		virtual	DexianControl*			CloneNew();
		virtual	DexianStatic*			AddStatic(const RectF& clientRect, bool bBack = true);
		//virtual	DexianStatic*			AddText(const char* text);
		virtual	DexianButton*			AddButton(const RectF& clientRect, ButtonClickProc Callback, const char* text = 0);
		virtual	DexianEditBox*			AddEditBox(const RectF& clientRect, const char* text = 0);
		virtual DexianGrids*			AddGrids();
		//virtual DexianControl*			AddShape(const RectF& clientRect);
		virtual DexianViewerPlace*		AddPlace();
		virtual DexianListBox*			AddListBox(const RectF& clientRect, ListBoxSelectedProc Callback);
		virtual	VOID					CopyFrom(DexianControl* e);
		inline	BOOL	operator	==	(const char* szName) { return (this->Name == szName); }
		virtual char 					OnInputEvent(unsigned int uMsg, PixelF pt, unsigned int touchIndex);
		virtual	void					Render(float fElapsedTime, bool bEnabled = true);
		virtual void					RenderElement(DexianElement* Element, bool bIsEnabled);
		virtual void					SetCustomRender(RenderProc rc) { Proc = rc; }
	public:
		virtual char 					OnMouseMessage(unsigned int uMsg, PixelF pt, unsigned int touchIndex);
		virtual void					OnRender(float fElapsedTime, bool bIsEnabled) {}
		virtual void					OnRenderDraws(bool bIsEnabled);
		//virtual void					OnScale(float fScale, PixelF Center);
		DexianControl*					Parent() { return ParentPtr; }
	public:
		virtual BOOL 					IsPtIn(PixelF pt);
		//
		DexianDrawRect*					AddRect(const color4& c);
		DexianDrawRect*					AddRect(const RectF& rc, const color4& c);
		DexianDrawTexture*				AddTexture(const RectF& rc, const color4& c, const char* File, const RectF* Region, BaseDirectory* dir = 0);
		DexianDrawCircle*				AddCircle(const PixelF& center, float Radius, const color4& c, int Smooth);
		DexianDrawRoundRect*			AddRoundRect(const RectF& rc, const color4& c, float fRoundRadius, int nRoundMode/*1=lefttop,2=righttop,4=rightbottom,8=leftbottom*/);
		DexianDrawEventPoint*			AddEventPoint(DexianViewerPlace* Place, const RectF& rc, const color4& c, const char* File, const RectF* Region, const char* FullFile, const RectF* FullRegion, BaseDirectory* dir = 0);
		DexianDrawText*					AddText(const char* Text, const color4& c, int FontSize, const char* szFontName = "Arial", int FontStyle = RenderText::Default);
		DexianDrawEditText*				AddEditableText(const RectF& rc, const char* Text, const color4& c, int FontSize, const char* szFontName = "Arial", int FontStyle = RenderText::Default);
		//virtual char 					OnMouseMessage(unsigned int uMsg, PixelF pt, unsigned int touchIndex);
		void							Remove(DexianDrawBase* b);
		//void							AddRoundRect(DexianDrawBase::AlignMode Mode, const Rect& rc, const color4& c, float RoundSize, bool bLingXing = false);
		Array<DexianDrawBase*>			Draws;
		inline	void					InvalidateDrawSort() { RequestDrawSort = true; }
		void							ShowControl(const char* name, bool bDisplay);
	protected:
		int								EnterDraw;
		int								DownDraw;
		char							RequestDrawSort;
		bool							bIsBodyDown;
	public:
		virtual void					Setup2DSpace();
		virtual void 					OnMouseEnter() {}
		virtual void 					OnMouseLeave() {}
		//virtual BOOL 					IsPtIn(PixelF pt);
		virtual	VOID					SetClientRect(const RectF& rc);//by dialog
		virtual	VOID					SetWindowRect(const RectF& rc);//by dialog
		virtual	PixelF					ScreenToClient(const PixelF& pixel);
		virtual	PixelF					ClientToScreen(const PixelF& pixel);
		//virtual	char					LoadControl(CSafeFileHelperR& r);
		//virtual	char					SaveControl(CSafeFileHelperW& w);
		virtual	void					SetRotation(char bEnable, float rot);
		virtual	char					IsRotation() { return bRotation; }
		DexianControl*					Pick(int x, int y);
		virtual	VOID					FrameMove(double fTime, float fElapsedTime);
		inline	INT						GetHeight() { return WindowRect.bottom - WindowRect.top; }
		inline	INT						GetWidth() { return WindowRect.right - WindowRect.left; }
		virtual	DexianControlData::ControlType			GetType() { return DexianControlData::Type_Base; }
		inline	BOOL					IsEnabled() { return bEnabled; }
		const matrix4x4&				GetControlMat();
		void							SetLayer(int nLayer);
		PixelF							Offset;
		inline int						GetID() const { return ID; }
		ButtonUp						CallbackButtonUp;
	protected:
		friend class DexianManager;
		int								ID;
		DexianControl*					ParentPtr;
		Array<DexianControl*>			Childs;
		Array<DexianElement*>			Elements;
		char							RequestSort;
		RenderProc						Proc;
	};
	class PHANTOM_API  DexianStatic : public DexianControl
	{
	public:
		DexianStatic(DexianControl* dlg);
		~DexianStatic();
		virtual void					OnRender(float fElapsedTime, bool bIsEnabled);
		virtual char 					OnMouseMessage(unsigned int uMsg, PixelF pt, unsigned int touchIndex);
	};
	class  PHANTOM_API DexianButton: public DexianControl
	{
	public:
		DexianButton(DexianControl* dlg);
		~DexianButton();
		virtual void					OnRender(float fElapsedTime, bool bIsEnabled);
		virtual char 					OnMouseMessage(unsigned int uMsg, PixelF pt, unsigned int touchIndex);
		ButtonClickProc					Callback;
		DexianDrawText*					text;
	};
	template <typename T>
	class Pointer{
	public:
		Pointer() { P = 0; }
		Pointer(T* p) { P = p; }
		~Pointer(){
			if(P)
				delete P;
		}
		void	operator = (T* t) { P = t; }
		T*		operator->() { return P; }
		T*		P;
	};
	//
	class	 PHANTOM_API DexianViewerExec : public DexianControl, public RenderEditTextChanged
	{
	public:
		DexianViewerExec(DexianControl* dlg);
		~DexianViewerExec();
		void									Setup(const DexianPlaceExec& Viewer, BaseDirectory* TextureDir);
		DexianDrawEventPoint*					PickEventPoint(const Pixel& pos);
		void									RecalcSize();
	protected:
		struct EventLine {
			DexianDrawText*			Text;
			DexianDrawEditText*		Editable;
			DexianDrawEventPoint*	EventArrow;
			int						Height;
			EventLine() { Text = 0; Editable = 0; EventArrow = 0; Height = 0; }
		};
		void									SetupEvents(Array<EventLine>& Results, const Array<DexianExecNodePoint*>& Events, int Line, bool isLeft, const char* IconFile, const char* FullIconFile, BaseDirectory* TextureDir);
		Size									GetSize(Array<EventLine>& Ins, int Line);
		virtual void							OnChanged(RenderEditText* t, void* Param);
		DexianDrawText*							Title;
		DexianDrawRoundRect*					RectTitle;
		DexianDrawRoundRect*					RectBody;
		Array<EventLine>						LeftEvents, RightEvents, LeftParams, RightParams;
	public:
		DexianViewerPlace*						Place;
		Array<DexianDrawEventPoint*>			EventPoints;
		int										FontSize, TitleSize, IconSize, FrameReserved;
	};
	class	 PHANTOM_API DexianViewerPlace : public DexianControl
	{
	public:
		DexianViewerPlace(DexianControl* dlg);
		~DexianViewerPlace();
		virtual BOOL 							IsPtIn(PixelF pt) { return true; }
		virtual char 							OnMouseMessage(unsigned int uMsg, PixelF pt, unsigned int touchIndex);
		virtual void							OnRender(float fElapsedTime, bool bIsEnabled);
		DexianDrawCurveLine*					AddLine(const PlaceNodeID& from, const PlaceNodeID& to, const color4& c);
		virtual DexianViewerExec*				AddExec(const RectF& clientRect, const DexianPlaceExec& Viewer, BaseDirectory* TextureDir);
		DexianDrawEventPoint*					PickEventPoint(const Pixel& pos);
		void									RemoveLine(DexianDrawCurveLine* c);
		DexianDrawEventPoint*					GetEventPoint(PlaceNodeID& id);
	public:
		TypeArray<DexianDrawCurveLine*>			Lines;
		TypeArray<DexianViewerExec*>			Execs;
	protected:
		DexianDrawCurveLine*					AddNew;
		bool									Adding;
		Pixel									AddingPoint[2];
		friend class							DexianDrawEventPoint;
	};
	class DexianListBox;
	struct PHANTOM_API DexianListLine
	{
		DexianListLine(DexianListBox* list, const char* Text, const char* IconFile, BaseDirectory* TextureDir);
		~DexianListLine();
		DexianDrawTexture*						Icon;
		DexianDrawText*							Text;
		void*									Context;
		DexianListBox*							ListBox;
		Rect									rc;
		int										Param;
	};
	class	 PHANTOM_API DexianListBox : public DexianControl
	{
	public:
		DexianListBox(DexianControl* dlg);
		~DexianListBox();
		DexianListLine*							AddItem(const char* Text, const char* IconFile = 0, void* Context = 0, BaseDirectory* TextureDir = 0);
		DexianListLine*							GetItemAt(int Index);
		virtual BOOL 							IsPtIn(PixelF pt);
		void									SetTitleText(const char* Text);
		void									SetSelected(int index);
		virtual char 							OnMouseMessage(unsigned int uMsg, PixelF pt, unsigned int touchIndex);
		ListBoxSelectedProc						Callback;
		int										GetIndexAtPoint(const PixelF& pos);
		virtual void							OnRender(float fElapsedTime, bool bIsEnabled);
		VOID									SetClientRect(const RectF& rc);//by dialo
		virtual void							OnRenderDraws(bool bIsEnabled);
		void									UpdateItems();
		void									ClearAllItems();
	public:
		DexianDrawText*							Title;
		DexianDrawRect*							Selected;
		DexianDrawRect*							HotRect;
		DexianDrawRect*							Back;
		int										SelectedIndex;
		int										nTitleHeight,nLineHeight;
		Array<DexianListLine*>					Lines;
		color4									BackColor;
		int										PaddingW, PaddingH;
		int										nFirstIndex;
		int										nHotIndex;
		int										nSelectIndex;
	};
	class	 PHANTOM_API DexianComboBox : public DexianListBox
	{
	public:
		DexianComboBox(DexianControl* dlg);
		~DexianComboBox();
		DexianDrawText*							Text;
		DexianDrawRoundRect*					Back;
		//DexianDrawShape*						Down;
	};
	class	 PHANTOM_API DexianEditBox : public DexianControl
	{
	public:
		DexianEditBox(DexianControl* dlg);
		~DexianEditBox();
		DexianDrawEditText*						Text;
		BOOL 									IsPtIn(PixelF pt);
		virtual char 							OnMouseMessage(unsigned int uMsg, PixelF pt, unsigned int touchIndex);
		void									SetText(const char* t);
		const char*								GetText();
		//DexianDrawShape*						Down;
	};
	class	 PHANTOM_API DexianGrids : public DexianControl
	{
	public:
		DexianGrids(DexianControl* dlg);
		~DexianGrids();
		virtual void					OnRender(float fElapsedTime, bool bIsEnabled);
		virtual char 					OnMouseMessage(unsigned int uMsg, PixelF pt, unsigned int touchIndex);
		virtual BOOL 					IsPtIn(PixelF pt) { return true; }
		int								AlignPixel;
		int								GroupScale;//组格子倍数
		color4							LineColor, GroupColor;
		void							SetPopup(DexianControl* c) { Popup = c; }
		DexianControl*					Popup;
	};
	struct	StringTable
	{
		Name		ID;
		Text		Value;
	};
	class PHANTOM_API DexianManager
	{
	public:
		DexianManager();
		~DexianManager();
		DexianFont*						CreateFont(const char* szFontName, int height, int weight, char italic = false, char underline = false);
		DexianTexture*					CreateTexture(int TextureIndex, int filter = TextureFilter_Point, BaseDirectory* dir = 0);
		//DexianControl*					NewShape(const char* Name);
		//DexianControl*					SearchShape(const char* Name);
		int								AddTextureFile(const char* szFile);
		DexianControl*					CreateControl();
		//
		DexianControl*					Pick(int x, int y);
		//
		BOOL							OnInputEvent(unsigned int uMsg, const Pixel& pos, unsigned int touchIndex);
		//VOID							SetToolTip(ToolTipData* t, const short_t* text, const Rect& rcInWindow, UIPopupType type);
		void							OnResize() {};
		VOID							Destroy();
		static DexianManager&			GetInstance();
		//
		Texture*						GetTexture(PtrID& id);
		MyFont*							GetFont(PtrID& id);
		//
		//void							OnIMEChange(void* Context, const short* Text, unsigned short nCursor, unsigned short nSelect);
		//DexianControl*					GetShape(PtrID& id);
		DexianControl*					MouseEnter, *MouseDown;
		unsigned int					AlignPixel;
		color4							ShadowColor;
		int								ShadowWidth;
		OpenEditBoxProc					OnOpenEdit;
		//
		const char*						GetText(const char* ID);
		void							LoadStringTable(const char* File);
		TypeArray<StringTable*>		STable;
	public:
		VOID							EnableScaling(BOOL b) { m_bEnableScaling = b; }	//是否将界面缩放
		BOOL							IsEnableScaling() { return m_bEnableScaling; }		//
		VOID							SetToolTipTime(float fTime) { m_fToolTipTime = fTime; }
		float							GetToolTipTime() { return m_fToolTipTime; }	//返回提示框显示时间
		//
		DexianControl*					m_toolTip;
		DexianDrawText*					m_toolTipText;
		void							CreateTooltip();
		void							ShowToolTip(const char* Text);
	protected:
		void							OnFrameMove(double fTime, float fElapsedTime);	//
		void							Render(float fElapsedTime);
		static DexianControl*			CreateControl(DexianControlData::ControlType Type, DexianControl* Parent);
		void							OnDelete(DexianTexture* tex);
		void							OnDelete(DexianFont* tex);
		void							OnDelete(DexianControl* tex);
	protected:
		TypeArray< DexianTexture* >		m_textureMap;
		TypeArray< DexianFont* >		m_fontMap;
		TypeArray< DexianControl* >		m_controlMap;
		//TypeArray< DexianControl* >		m_shapeMap;
		TypeArray<FileName>				m_textureFiles;
		std::string						m_imageFilePath;
		float							m_fToolTipTime;
		BOOL							m_bEnableScaling;
		bool							RequestSort;
		friend class DexianFont;
		friend class DexianTexture;
		friend class DexianControl;
		friend class Manager;
	};
	const char*		T(const char* ID);

};
