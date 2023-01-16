#include "DexianUIBase.h"
#include "PhantomManager.h"

namespace Phantom {
	DexianFont::~DexianFont()
	{
		DexianManager::GetInstance().OnDelete(this);
	}
	DexianTexture::~DexianTexture()
	{
		DexianManager::GetInstance().OnDelete(this);
	}
	DexianElement::DexianElement() {
		TextureRegion.Set(0, 0, 128, 128);
		TextFormat = 0;
		Color.setRGBA(1, 1, 1, 1);
		ShadowColor.setRGBA(0, 0, 0, 1);
		WrapWidth.Set(20, 20);
		Mode = Scale;
		bDisplay = true;
	}
	void	DexianElement::InitTexture(const char* File, const RectF* Region, BaseDirectory* dir)
	{
		DexianTexture* Tex = DexianManager::GetInstance().CreateTexture(DexianManager::GetInstance().AddTextureFile(File), TextureFilter_Point, dir);
		if (Tex) {
			TextureIndex.id = Tex->GetID();
			//Tex->texturePtr->GetOriHeight();
			if (Region)
				TextureRegion = *Region;
			else {
				TextureRegion.Set(0, 0, Tex->texturePtr->GetOriWidth(), Tex->texturePtr->GetOriHeight());
			}
		}
	}
	//void	DexianElement::InitShape(DexianShape* pShape)
	//{
	//	if (pShape)
	//		Shape.id = pShape->GetID();
	//	else
	//		Shape.id = 0;
	//}
	//void	DexianRegionData::SetupRect(const Rect& rc, const color4& c, bool bLingXing)
	//{
	//	float BufferWidth = g_manager.GetBufferWidth();
	//	float BufferHeight = g_manager.GetBufferHeight();
	//	float fTan2DFov = g_manager.GetTan2DFov();
	//	rect_f screen((float)rc.left / (float)BufferWidth, (float)rc.top / (float)BufferHeight,
	//		(float)rc.right / (float)BufferWidth, (float)rc.bottom / (float)BufferHeight);
	//	PosList.resize(4);
	//	ColorList.resize(4);
	//	float3* pos = PosList.GetPtr();
	//	unsigned int* color = ColorList.GetPtr();
	//	double fZ = 0;
	//	double fY = BufferHeight;
	//	double fX = fY * (double)BufferWidth / (double)BufferHeight;
	//	double l = (screen.left - 0.5f) * fX;
	//	double t = (0.5f - screen.top) * fY;
	//	double r = (screen.right - 0.5f) * fX;
	//	double b = (0.5f - screen.bottom) * fY;
	//	//
	//	if (bLingXing) {
	//		float cy = (t + b) / 2;
	//		float cx = (l + r) / 2;
	//		pos[0].setxyz(cx, t, fZ);
	//		pos[1].setxyz(r, cy, fZ);
	//		pos[2].setxyz(cx, b, fZ);
	//		pos[3].setxyz(l, cy, fZ);
	//	}
	//	else {
	//		pos[0].setxyz(l, t, fZ);
	//		pos[1].setxyz(r, t, fZ);
	//		pos[2].setxyz(r, b, fZ);
	//		pos[3].setxyz(l, b, fZ);
	//	}
	//	unsigned int temp = c;
	//	for (int i = 0; i<4; i++)
	//		color[i] = temp;
	//	static unsigned short indexBuffer[] = { 3,1,0,3,2,1 };
	//	static unsigned short indexBufferLine[] = { 0,1,1,2,2,3,3,0 };

	//}
	//void	DexianRegionData::SetupRoundRect(const Rect& rc, float RoundRect, int smooth)
	//{

	//}
	DexianDrawBase::DexianDrawBase(DexianControl* Shape)
	{
		ModeX = AlignLeft;
		ModeY = AlignTop;
		Offset.Set(0, 0);
		Shadow = true;
		Layer = 0;
		Pickable = 0;
		Cursor = Cursor_Arrow;
		Display = true;
		ShapePtr = Shape;
	}
	DexianDrawBase::~DexianDrawBase()
	{
	}
	void			DexianDrawBase::SetLayer(int nLayer)
	{
		Layer = nLayer;
		if (ShapePtr)
			ShapePtr->InvalidateDrawSort();
	}

	void			DexianDrawBase::Invalidate()
	{
		Buffer.Valid = false;
		SBuffer.Valid = false;
	}
	void			DexianDrawBase::SetScale(float fScale)
	{
		Buffer.Scale = fScale;
		SBuffer.Scale = fScale;
	}
	void			DexianDrawBase::SetRotation(int nRotation)
	{
		if (Rotation == nRotation)return;
		Rotation = nRotation;
		float fR = (float)Rotation / 180.0f*PI;
		Buffer.Rotation = fR;
		SBuffer.Rotation = fR;
		Invalidate();
	}
	Rect			DexianDrawRect::GetRenderRect(const RectF& WindowRect)
	{
		Rect r;
		int w = rc.Width();
		int h = rc.Height();
		int centX = (WindowRect.left + WindowRect.right) / 2;
		int centY = (WindowRect.top + WindowRect.bottom) / 2;
		switch (ModeX) {
		case AlignRight:
			r.left = WindowRect.right - w - rc.left;
			r.right = r.left + w;
			break;
		case AlignLeftRight:
			r.left = rc.left + WindowRect.left;
			r.right = WindowRect.right - rc.left;
			break;
		case AlignCenterX:
			r.left = centX + rc.left;
			r.right = r.left + w;
			break;
		default:
			r.left = WindowRect.left + rc.left;
			r.right = r.left + w;
			break;
		}
		switch (ModeY) {
		case AlignBottom:
			r.top = WindowRect.bottom - h - rc.top;
			r.bottom = r.top + h;
			break;
		case AlignTopBottom:
			r.top = rc.top + WindowRect.top;
			r.bottom = WindowRect.bottom - rc.top;
			break;
		case AlignCenterY:
			r.top = centY + rc.top;
			r.bottom = r.top + h;
			break;
		default:
			r.top = WindowRect.top + rc.top;
			r.bottom = r.top + h;
			break;
		}
		r.top += Offset.y;
		r.left += Offset.x;
		r.right += Offset.x;
		r.bottom += Offset.y;
		return r;
	}
	void			DexianDrawRect::Render(const RectF& WindowRect, bool bEnabled)
	{
		Rect r = GetRenderRect(WindowRect);
		if (!(r == Last)) {
			Buffer.Valid = false;
			SBuffer.Valid = false;
			Last = r;
		}
		color4 c = color;
		if (!bEnabled)c = c.gray();
		g_manager.RenderRect2D(&Buffer, r, c);// .DrawRect2D(r, color, true, false);
	}
	void			DexianDrawRect::RenderShadow(const RectF& WindowRect, int ShadowWidth)
	{
		//Rect r = GetRenderRect(WindowRect);
		//r.inflate(ShadowWidth, ShadowWidth);
		//g_manager.RenderRect2D(&Buffer, r, DexianManager::GetInstance().ShadowColor);//g_manager.DrawRect2D(r, DexianManager::GetInstance().ShadowColor, true);
	}
	bool			DexianDrawRect::Pick(const RectF& WindowRect, const PixelF& pos)
	{
		Rect r = GetRenderRect(WindowRect);
		return r.isPtIn(pos);
	}
	DexianDrawTexture::DexianDrawTexture(DexianControl* Shape):DexianDrawRect(Shape)
	{
		FrameMode = 0;
		FrameBegin = FrameEnd = 0;
		FrameSpeed = 1.0f;
		FrameIndex = 0;
		IsCenterMode = false;
	}
	DexianDrawTexture::~DexianDrawTexture() {

	}
	void			DexianDrawTexture::SetCenterMode(bool bIsCenterMode, const float2* pCenter)
	{
		IsCenterMode = bIsCenterMode;
		if(pCenter)
			Buffer.Center = Center = *pCenter;
		else
			Buffer.Center = Center = float2(Element.TextureRegion.GetWidth() / 2.0f, Element.TextureRegion.GetHeight() / 2.0f);
	}
	void			DexianDrawTexture::SetPosition(const float2& Pos)
	{
		Position = Pos;
		Buffer.Valid = false;
		SBuffer.Valid = false;
	}
	void			DexianDrawTexture::SetRotation(int nRotation)
	{
		DexianDrawBase::SetRotation(nRotation);
		Buffer.Center = Center;// 
	}
	int				DexianDrawTexture::AddTexture(const color4& c, const char* File, const RectF* Region, BaseDirectory* dir)
	{
		DexianElement ed;
		ed.Color = c;
		ed.InitTexture(File, Region, dir);
		Frames += ed;
		return Frames.size() - 1;
	}
	void			DexianDrawTexture::PlayMultiFrame(int BeginIndex, int EndIndex, float fSpeed)
	{
		FrameMode = 2;
		FrameBegin = BeginIndex;
		FrameEnd = EndIndex;
		FrameIndex = 0.0f;
		FrameSpeed = fSpeed;
	}
	void			DexianDrawTexture::PlayOneFrame(int IdleIndex)
	{
		FrameMode = 1;
		FrameBegin = IdleIndex;
	}
	void			DexianDrawTexture::StopFrame()
	{
		FrameMode = 0;
	}
	void			DexianDrawTexture::Render(const RectF& WindowRect, bool bEnabled)
	{
		RectF r;
		if (IsCenterMode) {
			float w = (float)rc.Width();
			float h = (float)rc.Height();
			float cx = Center.x / Element.TextureRegion.Width() * w;
			float cy = Center.y / Element.TextureRegion.Height() * h;
			r = RectF(Position.x - cx, Position.y - cy, Position.x + (w-cx), Position.y + (h-cy));
		}
		else {
			r = GetRenderRect(WindowRect);
			if (!(r == Last)) {
				Buffer.Valid = false;
				SBuffer.Valid = false;
				Last = r;
			}
		}
		DexianElement* Curr = &Element;
		if (Frames.size() > 0) {
			if (FrameMode == 1) {
				Curr = &Frames[FrameBegin%Frames.size()];
			}
			else if (FrameMode == 2) {
				FrameIndex += FrameSpeed * g_manager.GetElapsedTime();
				int num = FrameEnd - FrameBegin + 1;
				int nIndex = ((int)FrameIndex % num) + FrameBegin;
				Curr = &Frames[nIndex % Frames.size()];
			}
		}
		Texture* node = DexianManager::GetInstance().GetTexture(Curr->TextureIndex);
		if (node)
			g_manager.RenderTexture(&Buffer, node, r, Curr->TextureRegion, color, bEnabled);// .DrawRect2D(r, color, true, false);
		else {
			color4 c = color;
			if (!bEnabled)c = c.gray();
			g_manager.RenderRect2D(&Buffer, r, c);
		}
	}
	void			DexianDrawTexture::RenderShadow(const RectF& WindowRect, int ShadowWidth)
	{

	}

	PixelF			DexianDrawCircle::GetCenter(const RectF& WindowRect) {
		PixelF r;
		int centX = (WindowRect.left + WindowRect.right) / 2;
		int centY = (WindowRect.top + WindowRect.bottom) / 2;
		switch (ModeX) {
		case AlignRight:
			r.x = WindowRect.right - Center.x;
			break;
		case AlignLeftRight:
		case AlignCenterX:
			r.x = centX + Center.x;
			break;
		default:
			r.x = WindowRect.left + Center.x;
			break;
		}
		switch (ModeY) {
		case AlignBottom:
			r.y = WindowRect.bottom - Center.y;
			break;
		case AlignTopBottom:
		case AlignCenterY:
			r.y = centY + Center.y;
			break;
		default:
			r.y = WindowRect.top + Center.y;
			break;
		}
		r.x += Offset.y;
		r.y += Offset.x;
		return r;
	}
	void			DexianDrawCircle::Render(const RectF& WindowRect, bool bEnabled)
	{
		PixelF C = GetCenter(WindowRect);
		Pixel P = C;
		if (P != Point) {
			Buffer.Valid = false;
			SBuffer.Valid = false;
			Point = P;
		}
		color4 c = Color;
		if (!bEnabled)c = c.gray();
		g_manager.RenderCircle2D(&Buffer, C, Radius, c, Smooth);// .DrawCircle(C, Radius, Color, Smooth, true);
	}
	void			DexianDrawCircle::RenderShadow(const RectF& WindowRect, int ShadowWidth)
	{
		PixelF C = GetCenter(WindowRect);
		g_manager.RenderCircle2D(&SBuffer, C, Radius + ShadowWidth, DexianManager::GetInstance().ShadowColor, Smooth);//g_manager.DrawCircle(C, Radius + ShadowWidth, Color, Smooth, true);
	}
	bool			DexianDrawCircle::Pick(const RectF& WindowRect, const PixelF& pos)
	{
		PixelF C = GetCenter(WindowRect);
		PixelF D = pos - C;
		float fSize = sqrtf(D.x*D.x + D.y*D.y);
		return fSize < Radius;
	}
	void			DexianDrawRoundRect::Render(const RectF& WindowRect, bool bEnabled)
	{
		Rect r = GetRenderRect(WindowRect);
		if (!(r == Last)) {
			Buffer.Valid = false;
			SBuffer.Valid = false;
			Last = r;
		}
		color4 c = color;
		if (!bEnabled)c = c.gray();
		g_manager.RenderRoundRect2D(&Buffer, r, Round, RoundMode, &c);
		//g_manager.DrawRoundRect2D(r, Round, RoundMode, &color, true, false);
	}
	void			DexianDrawRoundRect::RenderShadow(const RectF& WindowRect, int ShadowWidth)
	{
		Rect r = GetRenderRect(WindowRect);
		r.inflate(ShadowWidth, ShadowWidth);
		g_manager.RenderRoundRect2D(&SBuffer, r, Round, RoundMode, &DexianManager::GetInstance().ShadowColor);
		//g_manager.DrawRoundRect2D(r, Round, RoundMode, &DexianManager::GetInstance().ShadowColor, true, false);
	}
	bool			DexianDrawRoundRect::Pick(const RectF& WindowRect, const PixelF& pos)
	{
		Rect r = GetRenderRect(WindowRect);
		return r.isPtIn(pos);
	}
	DexianDrawEventPoint::DexianDrawEventPoint(DexianViewerPlace* _Place, DexianControl* Shape)
		:DexianDrawTexture(Shape)
	{
		Linked = false;
		ID = 1;
		Place = _Place;
		IsLeft = true;
		PickEnter = false;
		PickWidth = 10;
		CurrLink = 0;
	}
	DexianDrawEventPoint::~DexianDrawEventPoint()
	{
	}
	bool			DexianDrawEventPoint::Pick(const RectF& WindowRect, const PixelF& pos)
	{
		Rect r = GetRenderRect(WindowRect);
		Rect PR = r;
		if (this->ModeX == AlignRight)
		{
			PR.left = PR.right - PickWidth;
		}
		else {
			PR.right = PR.left + PickWidth;
		}
		return PR.isPtIn(pos);
	}
	void			DexianDrawEventPoint::Render(const RectF& WindowRect, bool bEnabled)
	{
		Rect r = GetRenderRect(WindowRect);
		Rect PR = r;
		if (this->ModeX == AlignRight)
		{
			PR.left = PR.right - PickWidth;
		}
		else {
			PR.right = PR.left + PickWidth;
		}
		if (PickEnter) {
			color4 c = color;
			c.a = 0.5f;
			if (!bEnabled)c = c.gray();
			g_manager.RenderRect2D(&SBuffer, PR, c, true);
		}
		Point.Set(r.left + r.Width() / 2, r.top + r.Height() / 2);
		if (!(r == Last)) {
			Buffer.Valid = false;
			SBuffer.Valid = false;
			Last = r;
		}
		for (int i = 0; i < Next.size(); i++) {
			DexianDrawEventPoint* DE = Place->GetEventPoint(Next[i]);
			if(DE)
				Lines[i]->Render(Point, DE->Point, bEnabled);
		}
		DexianElement* E = &Element;
		if (Linked)
			E = &FullElement;
		Texture* node = DexianManager::GetInstance().GetTexture(E->TextureIndex);
		if (node)
			g_manager.RenderTexture(&Buffer, node, r, E->TextureRegion, color, bEnabled);// .DrawRect2D(r, color, true, false);
		else {
			color4 c = color;
			if (!bEnabled)c = c.gray();
			g_manager.RenderRect2D(&Buffer, r, c);
		}
	}
	void			DexianDrawEventPoint::OnEnter()
	{
		PickEnter = true;
	}
	void			DexianDrawEventPoint::OnLeave()
	{
		PickEnter = false;
	}
	void			DexianDrawEventPoint::RenderShadow(const RectF& WindowRect, int ShadowWidth)
	{

	}
	void			DexianDrawEventPoint::OnMoving(const Pixel& pos)
	{
		if (Place->Adding) {
			if(IsLeft)
				Place->AddingPoint[0] = pos;
			else
				Place->AddingPoint[1] = pos;
			if (CurrLink)
				CurrLink->PickEnter = false;
			DexianDrawEventPoint* EP = Place->PickEventPoint(pos);
			if (EP && EP != this){// && EP->Node->Viewer != this->Node->Viewer) {
				DexianExecNodePoint* EPNode = GetDexianSystem()->GetExecPoint(EP->Node);
				DexianExecNodePoint* MyNode = GetDexianSystem()->GetExecPoint(Node);
				if (MyNode&&EPNode) {
					if (MyNode->IsLinkable(EPNode))
						EP->PickEnter = true;
				}
			}
			CurrLink = EP;
		}
	}
	void			DexianDrawEventPoint::OnButtonDown(const Pixel& pos)
	{
		//if (AddNew)
		//{
		//	Place->RemoveLine(AddNew);
		//	AddNew = 0;
		//}
		//AddNew = Place->AddLine(this, 0, color);
		//AddNew.P
		if (IsLeft) {
			Place->AddingPoint[0] = pos;
			Place->AddingPoint[1] = Point;
		}
		else {
			Place->AddingPoint[0] = Point;
			Place->AddingPoint[1] = pos;
		}
		Place->Adding = true;
		Place->AddNew->RebuildLines();
	}
	void			DexianDrawEventPoint::OnButtonUp(const Pixel& pos)
	{
		Place->Adding = false;
		DexianExecNodePoint* MyNode = GetDexianSystem()->GetExecPoint(Node);
		if (CurrLink) {
			CurrLink->PickEnter = false;
			if (CurrLink && CurrLink != this) {// && EP->Node->Viewer != this->Node->Viewer) {
				DexianExecNodePoint* EPNode = GetDexianSystem()->GetExecPoint(CurrLink->Node);
				if (MyNode&&EPNode) {
					if (MyNode->IsLinkable(EPNode)) {
						MyNode->AddLink(EPNode);
						EPNode->AddLink(MyNode);
						Place->AddLine(Node, CurrLink->Node, color4(1, 1, 1, 1));
						Linked = true;
						CurrLink->Linked = true;
						//RebuildLines();
						//CurrLink->RebuildLines();
					}
				}
			}
		}
		CurrLink = 0;
	}
	void		DexianDrawEventPoint::RebuildLines()
	{
		DexianExecNodePoint* MyNode = GetDexianSystem()->GetExecPoint(Node);
		this->Next.clear();
		if (MyNode) {
			switch (MyNode->Type)
			{
			case DexianExecNodePoint::Node_EventOuter:
			case DexianExecNodePoint::Node_TypeOuter:
			{
				for (int i = 0; i < MyNode->Next.size(); i++) {
					DexianExecNodePoint* NP = GetDexianSystem()->GetExecPoint(MyNode->Next[i]);
					this->Next += MyNode->Next[i];
				}
				break;
			}
			}
		}
		if (Next.size() > Lines.size())
			Lines.resize(Next.size());
		for (int i = 0; i < this->Next.size(); i++) {
			if (!Lines[i])
				Lines[i] = new DexianDrawCurveLine(Place);
		}

	}
	DexianDrawText::DexianDrawText(DexianControl* Shape)
		:DexianDrawRect(Shape)
	{
		TextPtr = g_manager->CreateRenderText();
		bTextBack = false;
	}
	DexianDrawText::~DexianDrawText()
	{
		safe_release(TextPtr);
	}
	void			DexianDrawText::SetPosition(const float2& Pos)
	{
		if (!TextPtr)
			return;
		float W = TextPtr->GetWidth() / 2;
		float H = TextPtr->GetHeight() / 2;
		rc.Set(Pos.x - W, Pos.y - H, Pos.x + W, Pos.y + H);
	}
	void			DexianDrawText::SetText(const char* szText)
	{
		if (!TextPtr)
			return;
		TextPtr->SetFontSize(FontSize);
		TextPtr->SetText(szText);
		//rc.Set(0, 0, TextPtr->GetWidth(), TextPtr->GetHeight());
		Buffer.Valid = false;
		InvalidText = true;
	}
	void			DexianDrawText::Render(const RectF& WindowRect, bool bEnabled)
	{
		if (TextPtr) {
			Rect r = rc;// WindowRect;// GetRenderRect(WindowRect);
			r.left += WindowRect.left;
			r.right += WindowRect.left;
			r.top += WindowRect.top;
			r.bottom += WindowRect.top;
			int x = (r.left + r.right) / 2;
			int y = (r.top + r.bottom) / 2;
			//int ofx = (rc.right - rc.left) / 2;
			//int ofy = (rc.bottom - rc.top) / 2;
			//r.Set(x - ofx, y - ofy, x + ofx, y + ofy);
			if (!(r == Last)) {
				Buffer.Valid = false;
				Last = r;
			}
			if (g_manager.m_bSizeChanged) {
				InvalidText = true;
			}
			if (InvalidText && g_manager.IsRenderVisible(r)) {
				InvalidText = false;
				Buffer.Valid = false;
				int nFontSize = (int)((float)FontSize * g_manager.GetUIScale().x);
				//if (nFontSize < 16)nFontSize = 16;
				TextPtr->SetFontSize(nFontSize);
				TextPtr->Redraw();
			}
			//if (this->ModeX == AlignCenterX) {
			//	r.left -= TextPtr->GetRealWidth() / 2;
			//	r.right = r.left + TextPtr->GetRealWidth();
			//}
			//if (this->ModeY == AlignCenterY)
			//{
			//	r.top -= TextPtr->GetRealHeight() / 2;
			//	r.bottom = r.top + TextPtr->GetRealHeight();
			//}
			int w = TextPtr->GetRealWidth() / 4;
			int h = TextPtr->GetRealHeight() / 4;
			Rect rr(x - w, y - h, x + w, y + h);
			color4 c = color;
			if (!bEnabled)c = c.gray();
			g_manager.RenderText2D(&Buffer, TextPtr, rr, c, 0, bTextBack);
		}
	}
	void			DexianDrawText::RenderShadow(const RectF& WindowRect, int ShadowWidth)
	{

	}
	DexianDrawEditText::DexianDrawEditText(DexianControl* Shape)
		:DexianDrawRect(Shape)
	{
		TextPtr = g_manager->CreateRenderEditText();
		Cursor = Cursor_Edit;
		color = color4(0, 0, 0, 1);
	}
	DexianDrawEditText::~DexianDrawEditText()
	{
		safe_release(TextPtr);
	}
	void			DexianDrawEditText::Redraw()
	{
		Buffer.Valid = false;
	}
	void			DexianDrawEditText::SetText(const char* szText)
	{
		if (!TextPtr)
			return;
		TextPtr->SetFontSize(FontSize);
		TextPtr->SetText(szText);
		//rc.Set(0, 0, TextPtr->GetWidth(), TextPtr->GetHeight());
		Buffer.Valid = false;
	}
	void			DexianDrawEditText::SetTextSize(const Size& sz)
	{
		rc.Set(0, 0, sz.cx, sz.cy);
	}
	void			DexianDrawEditText::Render(const RectF& WindowRect, bool bEnabled)
	{
		if (TextPtr) {
			Rect r = WindowRect;// GetRenderRect(WindowRect);
			//r.right += 20;
			//r.bottom += 4;
			RectF border = r;
			int x = (r.left + r.right) / 2;
			int y = (r.top + r.bottom) / 2;
			int ofx = (r.right - r.left) / 2;
			int ofy = (r.bottom - r.top) / 2;
			r.Set(x - ofx, y - ofy, x + ofx, y + ofy);
			if (!(r == Last)) {
				Buffer.Valid = false;
				RectBuffer.Valid = false;
				Last = r;
			}
			FrameRect = WindowRect;
			//border.inflate(-5, -2);
			RectF region;
			region.left = FrameRect.left*g_manager.GetUIScale().x + g_manager.GetUIOffset().x*(float)g_manager.GetBufferWidth();
			region.right = FrameRect.right*g_manager.GetUIScale().x + g_manager.GetUIOffset().x*(float)g_manager.GetBufferWidth();
			region.top = FrameRect.top*g_manager.GetUIScale().y + g_manager.GetUIOffset().y*(float)g_manager.GetBufferHeight();
			region.bottom = FrameRect.bottom*g_manager.GetUIScale().y + g_manager.GetUIOffset().y*(float)g_manager.GetBufferHeight();
			TextPtr->RealRect = region;
			if (g_manager.m_bSizeChanged) {
				InvalidText = true;
			}
			if (InvalidText && g_manager.IsRenderVisible(r)) {
				InvalidText = false;
				Buffer.Valid = false;
				int nFontSize = (int)((float)FontSize * g_manager.GetUIScale().x);
				if (nFontSize < 4)nFontSize = 4;
				TextPtr->SetFontSize(nFontSize);
				TextPtr->Redraw();
				RectBuffer.Valid = false;
			}
			color4 c = color;
			if (!bEnabled)c = c.gray();
			Rect rr = border;
			rr.inflate(5, 5);
			g_manager.RenderRect2D(&RectBuffer, rr, color4(1, 1, 1, 1), true);
			g_manager.RenderText2D(&Buffer, TextPtr, border, c);
		}
	}
	void			DexianDrawEditText::OnEnter()
	{
		g_manager.CursorMode = Cursor_Edit;
	}
	void			DexianDrawEditText::OnLeave()
	{

	}
	bool			DexianDrawEditText::Pick(const RectF& WindowRect, const PixelF& pos)
	{
		Rect r = GetRenderRect(WindowRect);
		r.right += 20;
		r.bottom += 4;
		return WindowRect.isPtIn(pos);
	}
	//void			DexianDrawEditText::OnIMEChange(const char* Text)
	//{
	//	TextPtr->SetText(Text);
	//	//CursorPos = nCursor;
	//	//SelectPos = nSelect;
	//	Buffer.Valid = false;
	//	InvalidText = true;
	//}
	void			DexianDrawEditText::OnPick()
	{
		RectF region;
		region.left = FrameRect.left*g_manager.GetUIScale().x + g_manager.GetUIOffset().x*(float)g_manager.GetBufferWidth();
		region.right = FrameRect.right*g_manager.GetUIScale().x + g_manager.GetUIOffset().x*(float)g_manager.GetBufferWidth();
		region.top = FrameRect.top*g_manager.GetUIScale().y + g_manager.GetUIOffset().y*(float)g_manager.GetBufferHeight();
		region.bottom = FrameRect.bottom*g_manager.GetUIScale().y + g_manager.GetUIOffset().y*(float)g_manager.GetBufferHeight();
		TextPtr->SetEditorMode();
		//if (DexianManager::GetInstance().OnOpenEdit)
		//	(*DexianManager::GetInstance().OnOpenEdit)(region, TextPtr->GetText(), this);
	}
	void			DexianDrawEditText::RenderShadow(const RectF& WindowRect, int ShadowWidth)
	{

	}

	DexianDrawCurveLine::DexianDrawCurveLine(DexianViewerPlace* _Place)
	{
		FromPoint = ToPoint = Pixel(0, 0);
		Color.setRGBA(1, 1, 1, 1);
		Place = _Place;
	}
	DexianDrawCurveLine::~DexianDrawCurveLine()
	{
	}
	PixelF   HCurve(const PixelF& p1, const PixelF& p2, const PixelF& t1, const PixelF& t2, float l)
	{
		float i2 = l * l;
		float i3 = i2 * l;
		float express = 3.0 * i2 - 2.0 * i3;
		return p1*(1.0 - express) + p2*express + t1*(l - 2.0*i2 + i3) + t2*(i3 - i2);
	}
	PixelF   Bezier(const PixelF& p1, const PixelF& center, const PixelF& p2, float t)
	{
		float td = 1.0f - t;
		return p1*(td*td) + center*(2 * t*td) + p2*(t*t);
	}
	void			DexianDrawCurveLine::RebuildLines()
	{
		PixelF Dir = ToPoint - FromPoint;
		float Size = Dir.Length();
		int nLength = (int)(Size / 10.0f * g_manager.GetUIScale().x);
		if (nLength < 0)nLength = 0;
		Points.resize((nLength + 1), false);
		PixelF Center = (FromPoint + ToPoint)*0.5f;
		PointRect.Set(10000000.0f, 10000000.0f, -10000000.0f, -10000000.0f);
		for (int i = 0; i <= nLength; i++) {
			PixelF v = HCurve(FromPoint, ToPoint, PixelF(Size, 0), PixelF(Size, 0), (double)i / (double)nLength);
			Points[i].Set(v.x, v.y);
			PointRect.left = getmin(v.x, PointRect.left);
			PointRect.right = getmax(v.x, PointRect.right);
			PointRect.top = getmin(v.y, PointRect.top);
			PointRect.bottom = getmax(v.y, PointRect.bottom);
			//Points[(nLength+1)*2-i-1].Set(v.x, v.y + 0.1f);
		}
		Buffer.Valid = false;
	}
	void			DexianDrawCurveLine::Render(const Pixel& from, const Pixel& to, bool bEnabled)
	{
		if (from != FromPoint || to != ToPoint)
		{
			FromPoint = from;
			ToPoint = to;
			RebuildLines();
		}
		if (Points.size() > 0) {
			color4 c = Color;
			if (!bEnabled)c = c.gray();
			g_manager.RenderLines2D(&Buffer, PointRect, Points.GetPtr(), Points.size(), c, 1.0f + 1.0f / g_manager.GetUIScale().x);
		}
	}
	bool			DexianDrawCurveLine::OnRender(float fElapsedTime, bool bIsEnabled)
	{
		DexianDrawEventPoint* From = Place->GetEventPoint(FromEventPoint);
		DexianDrawEventPoint* To = Place->GetEventPoint(ToEventPoint);
		if (!From || !To)
			return false;
		if (From->Point != FromPoint || To->Point != ToPoint)
		{
			FromPoint = From->Point;
			ToPoint = To->Point;
			RebuildLines();
		}
		if (Points.size() > 0) {
			color4 c = Color;
			if (!bIsEnabled)c = c.gray();
			g_manager.RenderLines2D(&Buffer, PointRect, Points.GetPtr(), Points.size(), Color, 1.0f + 1.0f / g_manager.GetUIScale().x);
		}
		//g_manager.DrawLines2D(Points.GetPtr(), Points.size(), Color, true, 1.0f + 1.0f / g_manager.GetUIScale().x);
		return true;
	}
	DexianViewerPlace::DexianViewerPlace(DexianControl* Parent)
		:DexianControl(Parent)
	{
		Adding = false;
		AddNew = new DexianDrawCurveLine(this);
	}
	DexianViewerPlace::~DexianViewerPlace()
	{
		safe_delete(AddNew);
		Deletes(Lines);
	}
	DexianDrawCurveLine*	DexianViewerPlace::AddLine(const PlaceNodeID& from, const PlaceNodeID& to, const color4& c)
	{
		DexianDrawCurveLine* D = new DexianDrawCurveLine(this);
		D->FromEventPoint = from;
		D->ToEventPoint = to;
		D->Color = c;
		Lines.push_back(D);
		return D;
	}
	void	DexianViewerPlace::RemoveLine(DexianDrawCurveLine* c)
	{
		Erase(Lines, c);
		if (c)
			delete c;
	}
	char 	DexianViewerPlace::OnMouseMessage(unsigned int uMsg, PixelF pt, unsigned int touchIndex)
	{
		return false;
	}
	void	DexianViewerPlace::OnRender(float fElapsedTime, bool bIsEnabled)
	{
		if (Adding) {
			AddNew->Render(AddingPoint[0], AddingPoint[1], bEnabled);
		}
		for (int i = 0; i < Lines.size(); i++) {
			if (!Lines[i]->OnRender(fElapsedTime, bIsEnabled && this->bEnabled)) {
				delete Lines[i];
				i--;
			}
		}
	}

	DexianControlData::DexianControlData()
	{
		bEnabled = true;
		Display = true;
		bRotation = false;
		fRotation = 0.0f;
		WindowRect.Set(0, 0, 128, 128);
		ClientRect.Set(0, 0, 128, 128);
		bIsPickPixel = false;
		Layer = 0;
		Mat.identity();
		bMoveDown = false;
		MoveOffset.Set(0, 0);
		bIsInvalidate = true;
		bIsCaption = false;
		bClipInWindow = true;
		bMoveEnable = false;
		bAlign = false;
		Scale = 1.0f;
	}
	DexianControl::DexianControl(DexianControl *dialogPtr)
	{
		ParentPtr = dialogPtr;
		Elements.push_back(&Back);
		ID = GetDexianSystem()->NewIdentity();
		RequestSort = false;
		CallbackButtonUp = 0;
		Offset.Set(0, 0);
		if (ParentPtr)
			ParentPtr->RequestSort = true;
		else
			DexianManager::GetInstance().RequestSort = true;
		Proc = 0;
		RequestDrawSort = false;
		EnterDraw = -1;
		DownDraw = -1;
		bIsBodyDown = false;
		this->SetWindowRect(RectF(0, 0, g_manager.GetBufferWidth(), g_manager.GetBufferHeight()));
		offsetRect.Set(0, 0, 0, 0);
	}

	DexianControl::~DexianControl()
	{
		if (DexianManager::GetInstance().MouseDown == this)DexianManager::GetInstance().MouseDown = 0;
		if (DexianManager::GetInstance().MouseEnter == this)DexianManager::GetInstance().MouseEnter = 0;
		for (int i = 0; i < Draws.size(); i++) {
			Draws[i]->ShapePtr = 0;
			delete Draws[i];
		}
		Draws.clear();
		for (int i = 1; i < Elements.size(); i++) {
			delete Elements[i];
		}
		for (int i = 0; i < Childs.size(); i++)
		{
			delete Childs[i];
		}
		//if (!ParentPtr)
		//	DexianManager::GetInstance().OnDelete(this);
	}

	BOOL 	DexianControl::IsPtIn(PixelF pt)
	{
		CPUTime(DexianControl);
		if (!bIsPickPixel)
		{
			//DexianControl* S = DexianManager::GetInstance().GetShape(Back.Shape);
			//if (S)
			//{
			//	int nType = S->Pick(WindowRect, pt);
			//	return nType;
			//}
			return WindowRect.isPtIn(pt.x, pt.y);
		}
		Texture* node = DexianManager::GetInstance().GetTexture(Back.TextureIndex);
		if (!node)
			return WindowRect.isPtIn(pt.x, pt.y);
		RectF rc = WindowRect;//m_elementList[element]->rcTexture;
		PixelF center((WindowRect.left + WindowRect.right) / 2, (WindowRect.top + WindowRect.bottom) / 2);
		float2 scale((float)WindowRect.Width() / (float)Back.TextureRegion.Width(), (float)WindowRect.Height() / (float)Back.TextureRegion.Height());
		rc.left = (int)((float)(rc.left - center.x) * scale.x) + center.x;
		rc.right = (int)((float)(rc.right - center.x) * scale.x) + center.x;
		rc.top = (int)((float)(rc.top - center.y) * scale.y) + center.y;
		rc.bottom = (int)((float)(rc.bottom - center.y) * scale.y) + center.y;
		if (rc.isPtIn(pt.x, pt.y))
		{
			//
			if (!node->GetImageBuffer().IsAlphaImage())
				return true;
			if (!node->GetImageBuffer().alphas)
				node->GenericAlphas();//构造透明
			float fx = (float)(pt.x - rc.left) + Back.TextureRegion.left;
			float fy = (float)(pt.y - rc.top) + Back.TextureRegion.top;
			//if(ele->m_bIsInvWTo){
			//	float fx	=	(float)(pt.x - rc.right) + ele->rcTexture.left - ele->m_imgOffset.x;
			//}
			const Rect& tex = Back.TextureRegion;
			if (!tex.isPtIn(Pixel(fx, fy)))
				return false;
			BOOL bPick = node->GetImageBuffer().IsAlpha((int)fx, (int)fy);
			return bPick;
		}
		return false;
	}
	VOID				DexianControl::SetClientRect(const RectF& rc)//by dialo
	{
		ClientRect = rc;
		WindowRect = ClientRect;
		if (ParentPtr) {
			WindowRect.Offset(ParentPtr->WindowRect.left, ParentPtr->WindowRect.top);
		}
		this->bIsInvalidate = true;
	}
	VOID				DexianControl::SetWindowRect(const RectF& rc)
	{
		CPUTime(DexianControl);
		WindowRect = rc;
		ClientRect = WindowRect;
		if (ParentPtr)
			ClientRect.SetBeginPoint(WindowRect.left - ParentPtr->WindowRect.left, WindowRect.top - ParentPtr->WindowRect.top);
		for (int i = 0; i < Childs.size(); i++) {
			RectF W(PixelF(WindowRect.left + Childs[i]->ClientRect.left, WindowRect.top + Childs[i]->ClientRect.top), Childs[i]->WindowRect.Width(), Childs[i]->WindowRect.Height());
			Childs[i]->SetWindowRect(W);
		}
		this->bIsInvalidate = true;
		for (int i = 0; i < Draws.size(); i++) {
			Draws[i]->Invalidate();
		}
	}
	DexianControl*		DexianControl::Pick(int x, int y)
	{
		CPUTime(DexianControl);
		for (int i = 0; i < Childs.size(); i++) {
			if (Childs[i]->Pick(x, y))
				return Childs[i];
		}
		if (IsPtIn(Pixel(x, y)))
			return this;
		return 0;
	}
	int		SortDexianControl(const void* pp1, const void* pp2)
	{
		DexianControl* c1 = *(DexianControl**)pp1;
		DexianControl* c2 = *(DexianControl**)pp2;
		if (c1->Layer < c2->Layer)
			return 1;
		else if (c1->Layer > c2->Layer)
			return -1;
		return 0;
	}
	VOID				DexianControl::FrameMove(double fTime, float fElapsedTime)
	{
		CPUTime(DexianControl);
		if (RequestDrawSort) {
			RequestDrawSort = false;
			qsort(&Draws.at(0), Draws.size(), sizeof(DexianDrawBase*), SortDexianControl);
		}
		if (RequestSort && Childs.size() > 0) {
			RequestSort = false;
			qsort(&Childs.at(0), Childs.size(), sizeof(DexianControl*), SortDexianControl);
		}
		//InWindowOffset = Offset + offset;
		for (int i = 0; i < Childs.size(); i++) {
			//if (bIsInvalidate)
			//	Childs[i]->bIsInvalidate = true;
			Childs[i]->FrameMove(fTime, fElapsedTime);
		}
		//if (bIsInvalidate) {
		//	bIsInvalidate = false;
		if (ParentPtr) {
			WindowRect.Set(ClientRect.left + ParentPtr->WindowRect.left, ClientRect.top + ParentPtr->WindowRect.top, ClientRect.right + ParentPtr->WindowRect.left,
				ClientRect.bottom + ParentPtr->WindowRect.top);
		}
		//}
	}
	void				DexianControl::Setup2DSpace()
	{
		g_manager.Setup2DSpace(Offset.x, Offset.y, Scale, Scale);
	}
	void				DexianControl::SetRotation(char bEnable, float rot)
	{
		CPUTime(DexianControl);
		bRotation = bEnable;
		fRotation = rot;
	}

	VOID				DexianControl::CopyFrom(DexianControl* c)
	{
		CPUTime(DexianControl);
		//
		DexianControlData*	from = static_cast<DexianControlData*>(c);
		DexianControlData*	to = static_cast<DexianControlData*>(this);
		*to = *from;
		for (int i = 1; i < Elements.size(); i++) {
			*Elements[i] = *c->Elements[i];
		}
	}
	DexianControl*			DexianControl::CloneNew()
	{
		CPUTime(DexianControl);
		DexianControl*	ctrl = 0;
		ctrl = (DexianControl*)DexianManager::CreateControl(GetType(), this);
		ctrl->CopyFrom(this);
		return ctrl;
	}
	const matrix4x4&		DexianControl::GetControlMat()
	{
		CPUTime(DexianControl);
		if (bIsInvalidate)
		{
			PixelF pos(WindowRect.left + ParentPtr->WindowRect.left, WindowRect.top + ParentPtr->WindowRect.top);
			//
			matrix4x4 matRotation;
			matrix4x4* pMatRot = 0;
			float3* pCenter;
			float3 center;
			Mat.identity();
			if (bRotation)
			{
				pCenter = &center;
				center.setxyz(RotationCenter.x + ParentPtr->WindowRect.left, RotationCenter.y + ParentPtr->WindowRect.top, 0);
				matrixRotationZ(&matRotation, Radians(fRotation));
				pMatRot = &matRotation;
			}
			if (pMatRot)
			{
				Mat._41 = -(float)pCenter->x;
				Mat._42 = -(float)pCenter->y;
				matrixMultiply(&Mat, &Mat, pMatRot);
				Mat._41 = (float)pos.x + pCenter->x;
				Mat._42 = (float)pos.y + pCenter->y;
			}
			else
			{
				Mat._41 = (float)pos.x;
				Mat._42 = (float)pos.y;
			}
		}
		bIsInvalidate = false;
		return Mat;
	}
	PixelF		DexianControl::ClientToScreen(const PixelF& pixel)
	{
		CPUTime(DexianControl);
		const matrix4x4& mat = GetControlMat();
		float3 pos(pixel.x, pixel.y, 0);
		pos.x -= mat._41;
		pos.y -= mat._42;
		pos.transform(pos, mat);
		return PixelF(pos.x, pos.y);
	}

	PixelF		DexianControl::ScreenToClient(const PixelF& pixel)
	{
		CPUTime(DexianControl);
		const matrix4x4& mat = GetControlMat();
		matrix4x4 matInv;
		matrixInverse(&matInv, 0, &mat);
		float3 pos(pixel.x, pixel.y, 0);
		pos.transform(pos, matInv);
		pos.x += mat._41;
		pos.y += mat._42;
		return Pixel(pos.x, pos.y);
	}
	void					DexianControl::ShowControl(const char* name, bool bDisplay)
	{
		for (int i = 0; i < Childs.size(); i++) {
			if (Childs[i]->Name == name) {
				Childs[i]->Display = bDisplay;
				return;
			}
		}
	}
	DexianEditBox*			DexianControl::AddEditBox(const RectF& clientRect, const char* text) {
		DexianEditBox* ds = new DexianEditBox(this);
		ds->SetClientRect(clientRect);
		Childs.push_back(ds);
		return ds;
	}
	DexianButton*			DexianControl::AddButton(const RectF& clientRect, ButtonClickProc CB, const char* text)
	{
		DexianButton* ds = new DexianButton(this);
		ds->SetClientRect(clientRect);
		ds->Callback = CB;
		if(text)
			ds->text = ds->AddText(text, color4(1, 1, 1, 1), 24);
		Childs.push_back(ds);
		return ds;
	}
	//DexianStatic*			DexianControl::AddText(const char* text)
	//{
	//	DexianStatic* ds = new DexianStatic(this);
	//	ds->SetClientRect(this->ClientRect);
	//	ds->Name = text;
	//	Childs.push_back(ds);
	//	return ds;
	//}
	DexianStatic*			DexianControl::AddStatic(const RectF& clientRect, bool bBack)
	{
		DexianStatic* ds = new DexianStatic(this);
		ds->SetClientRect(clientRect);
		ds->Back.bDisplay = bBack;
		Childs.push_back(ds);
		return ds;
	}
	DexianGrids*			DexianControl::AddGrids()
	{
		DexianGrids* dg = new DexianGrids(this);
		Childs.push_back(dg);
		return dg;
	}
	DexianListBox*			DexianControl::AddListBox(const RectF& clientRect, ListBoxSelectedProc Callback)
	{
		DexianListBox* dg = new DexianListBox(this);
		dg->SetClientRect(clientRect);
		dg->Callback = Callback;
		Childs.push_back(dg);
		return dg;
	}
	DexianDrawEventPoint*			DexianViewerExec::PickEventPoint(const Pixel& pos)
	{
		for (int i = 0; i < Draws.size(); i++) {
			if (Draws[i]->Pickable&&Draws[i]->GetType()== DexianDrawBase::Type_EventPoint){
				if (Draws[i]->Pick(WindowRect, pos))
					return static_cast<DexianDrawEventPoint*>(Draws[i]);
			}
		}
		return 0;
	}
	DexianDrawEventPoint*			DexianViewerPlace::PickEventPoint(const Pixel& pos)
	{
		CPUTime(DexianViewerPlace);
		for (int i = 0; i < Execs.size(); i++) {
			DexianDrawEventPoint* ep = Execs[i]->PickEventPoint(pos);
			if (ep)
				return ep;
		}
		return 0;
	}
	DexianViewerExec*				DexianViewerPlace::AddExec(const RectF& clientRect, const DexianPlaceExec& Viewer, BaseDirectory* TextureDir)
	{
		DexianViewerExec* dg = new DexianViewerExec(this);
		dg->Place = this;
		dg->SetClientRect(clientRect);
		dg->Setup(Viewer, TextureDir);
		Childs.push_back(dg);
		Execs.push_back(dg);
		return dg;
	}
	DexianDrawEventPoint*			DexianViewerPlace::GetEventPoint(PlaceNodeID& id)
	{
		DexianViewerExec* E = search(Execs, id.Exec);
		if (!E)
			return 0;
		return search(E->EventPoints, id.Node);
	}
	DexianViewerPlace*		DexianControl::AddPlace()
	{
		DexianViewerPlace* dg = new DexianViewerPlace(this);
		Childs.push_back(dg);
		return dg;
	}
	void		DexianControl::Render(float fElapsedTime, bool bParentEnabled)
	{
		int ShadowWidth = DexianManager::GetInstance().ShadowWidth;
		if (ShadowWidth > 0) {
			for (int i = 0; i < Draws.size(); i++) {
				if (Draws[i]->Shadow && Draws[i]->Display)
					Draws[i]->RenderShadow(WindowRect, ShadowWidth);
			}
		}
		if (Proc)
			Proc();
		OnRender(fElapsedTime, bParentEnabled&&this->bEnabled);
		OnRenderDraws(bParentEnabled&&this->bEnabled);
		for (int i = (int)Childs.size() - 1; i >= 0; i--) {
			if (Childs[i]->Display)
				Childs[i]->Render(fElapsedTime, bParentEnabled&&this->bEnabled);
		}
	}
	void		DexianControl::OnRenderDraws(bool bIsEnabled) {
		for (int i = 0; i < Draws.size(); i++) {
			if (Draws[i]->Display)
				Draws[i]->Render(WindowRect + offsetRect, bIsEnabled);
		}
	}
	char 		DexianControl::OnMouseMessage(unsigned int uMsg, PixelF pos, unsigned int touchIndex)
	{
		if (!ParentPtr)return 0;
		switch (uMsg)
		{
		case InputEventID_MouseMove:
		{
			if (bMoveDown == (touchIndex + 1))
			{
				int x, y;
				x = pos.x - MoveOffset.x;
				y = pos.y - MoveOffset.y;
				int sw = g_manager.GetUIBufferWidth();
				int sh = g_manager.GetUIBufferHeight();
				if (bMoveEnable) {
					int w = WindowRect.Width();
					int h = WindowRect.Height();
					if (bAlign) {
						int nAlign = DexianManager::GetInstance().AlignPixel;
						x = (int)round((float)x / (float)nAlign)*nAlign;
						y = (int)round((float)y / (float)nAlign)*nAlign;
					}
					SetWindowRect(RectF(x, y, x + w, y + h));
				}
				else if (bIsCaption) {
					int w = ParentPtr->WindowRect.Width();
					int h = ParentPtr->WindowRect.Height();
					if (bClipInWindow) {
						if (x > (sw - w))
							x = sw - w;
						else if (x < 0)
							x = 0;
						if (y > (sh - h))
							y = sh - h;
						else if (y < 0)
							y = 0;
					}
					ParentPtr->SetWindowRect(RectF(x, y, x + w, y + h));
				}
				return (DexianManager::GetInstance().MouseDown == this);
			}
			else if (IsPtIn(pos)) {
				DexianManager::GetInstance().MouseEnter = this;
				g_manager.CursorMode = Cursor_Move;
			}
			else {
				DexianManager::GetInstance().MouseEnter = 0;
				DexianManager::GetInstance().MouseDown = 0;
				g_manager.CursorMode = Cursor_Arrow;
			}
			return false;
		}
		break;
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
		{
			if (IsPtIn(pos) && touchIndex == 0)
			{
				int x = 0, y = 0;
				if (bMoveEnable) {
					x = WindowRect.left;
					y = WindowRect.top;
				}
				else if (bIsCaption) {
					x = ParentPtr->WindowRect.left;
					y = ParentPtr->WindowRect.top;
				}
				else
					return false;
				PixelF pos;
				g_manager.GetCursorPos2D(pos, touchIndex);
				this->MoveOffset.Set(pos.x - x, pos.y - y);
				this->bMoveDown = (touchIndex + 1);
				DexianManager::GetInstance().MouseDown = this;
				return true;
			}
			break;
		}
		case InputEventID_ButtonUp:
		{
			if (CallbackButtonUp&&IsPtIn(pos))
				(*CallbackButtonUp)(this, touchIndex, pos);
			g_manager.CursorMode = Cursor_Arrow;
			if (bMoveDown)
			{
				bMoveDown = false;
				if (DexianManager::GetInstance().MouseDown == this)
					DexianManager::GetInstance().MouseDown = 0;
				return true;
			}
			break;
		}
		}
		return false;
	}
	char		DexianControl::OnInputEvent(unsigned int uMsg, PixelF pt, unsigned int touchIndex)
	{
		CPUTime(DexianControl);
		if (!Display)
			return false;
		PixelF pos = pt;
		if (uMsg == 5)
			touchIndex = 3;
		g_manager.GetCursorPos2D(pos, touchIndex);
		if (!bEnabled)
			return IsPtIn(pt);
		for (int i = Childs.size() - 1; i >= 0; i--) {
			if (Childs[i]->IsPtIn(pos)) {
				char bEvent = Childs[i]->OnInputEvent(uMsg, pt, touchIndex);
				if (bEvent)
					return true;
			}
		}
		return OnMouseMessage(uMsg, pt, touchIndex);
	}
	void					DexianControl::RenderElement(DexianElement* Element, bool bIsEnabled)
	{
		if (!Element || !Element->bDisplay)
			return;
		g_manager->SetBlendMode((BlendMode)BlendMode::BlendMode_SRCALPHA, (BlendMode)BlendMode::BlendMode_INVSRCALPHA);
		//int BeginX = 0;
		//int BeginY = 0;
		//if (ParentPtr)
		//{
		//	BeginX = ParentPtr->WindowRect.left;
		//	BeginY = ParentPtr->WindowRect.top;
		//}
		color4 c = Element->Color;
		if (!bIsEnabled)
			c = c.gray();
		//WindowRect.Set(BeginX + ClientRect.left, BeginY + ClientRect.top, BeginX + ClientRect.right, BeginY + ClientRect.bottom);
		Texture* Tex = DexianManager::GetInstance().GetTexture(Element->TextureIndex);
		if (Tex)
		{
			//int nHighlight = 0;
			//if (pElement->m_nColorOP == Phantom::TextureBlend_MODULATE2X)
			//	nHighlight = 1;
			//else if (pElement->m_nColorOP == Phantom::TextureBlend_MODULATE4X)
			//	nHighlight = 2;
			//
			switch (Element->Mode) {
			case DexianElement::Scale:
			{
				g_manager.Draw2D(Tex, WindowRect, Element->TextureRegion, &c, &Mat);// .DrawSprite2D(Tex, Element->TextureRegion, bEnabled, c, &Mat, 0, false);
				break;
			}
			case DexianElement::Wrap:
			{
				break;
			}
			case DexianElement::Tile:
			{
				//float tWidth = (int)Element->TextureRegion.GetWidth();
				//float tHeight = (int)Element->TextureRegion.GetHeight();
				//int maxWidth = WindowRect.GetWidth();
				//int maxHeight = WindowRect.GetHeight();
				//int countW = (int)(WindowRect.GetWidth() / tWidth);
				//int countH = (int)(WindowRect.GetHeight() / tHeight);
				//int sx = tWidth;
				//int sy = tHeight / 2;
				//for (int x = 0; x <= countW; x++)
				//{
				//	for (int y = 0; y <= countH; y++)
				//	{
				//		Rect rcTex(x * tWidth, y * tHeight, 0, 0);// (x + 1) * textureWidth, (y + 1) * textureHeight);
				//		rcTex.right = rcTex.left + tWidth;
				//		rcTex.bottom = rcTex.top + tHeight;
				//		if (rcTex.right > maxWidth)
				//			rcTex.right = maxWidth;
				//		if (rcTex.bottom > maxHeight)
				//			rcTex.bottom = maxHeight;
				//		rcTexture.right = rcTexture.left + rcTex.GetWidth();
				//		rcTexture.bottom = rcTexture.top + rcTex.GetHeight();
				//		if (rcTexture.left == rcTexture.right || rcTexture.top == rcTexture.bottom)
				//			continue;
				//		//
				//		rcTex.Offset(rcScreen.left, rcScreen.top);
				//		if (pElement->m_nTileWidth>0)
				//			rcTex.Offset(pElement->m_nTileOffX*x, pElement->m_nTileOffY*x);
				//		if (pElement->m_nTileHeight>0)
				//			rcTex.Offset(pElement->m_nTileOffX*y, pElement->m_nTileOffY*y);
				//		if (rcTex.right<0 || rcTex.bottom<0 || rcTex.left>g_manager.GetBufferWidth() || rcTex.top>g_manager.GetBufferHeight())
				//			continue;
				//		rect_f sr(fScaleX*pElement->m_nTileScaleX, fScaleY*pElement->m_nTileScaleY, rcTex.left, rcTex.top);
				//		g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, 0, &sr, nHighlight, bEnabled, pElement->m_bIsInvWTo, pElement->m_bIsInvHTo, m_bEnableDlgMatrix);
				//	}

				break;
			}
			}
			//g_manager.Draw2D(Tex, WindowRect, Element->TextureRegion, Element->Color, 
		}
		else {
			g_manager.DrawRect2D(WindowRect, c, true);
		}
	}
	void		DexianControl::SetLayer(int nLayer)
	{
		this->Layer = nLayer;
		if (ParentPtr)
			ParentPtr->RequestSort = true;
	}
	void	DexianControl::Remove(DexianDrawBase* b)
	{
		for (int i = 0; i < Draws.size(); i++) {
				break;
			if (Draws[i] == b) {
				delete Draws[i];
				Draws.erase(i, 1);
			}
		}
	}
	int		SortDexianDraws(const void* pp1, const void* pp2)
	{
		DexianDrawBase* c1 = *(DexianDrawBase**)pp1;
		DexianDrawBase* c2 = *(DexianDrawBase**)pp2;
		if (c1->GetLayer() < c2->GetLayer())
			return 1;
		else if (c1->GetLayer() > c2->GetLayer())
			return -1;
		return 0;
	}
	DexianEditBox::DexianEditBox(DexianControl* dlg)
		:DexianControl(dlg)
	{
		Text = AddEditableText(RectF(0, 0, 0, 0), "", color4(0, 0, 0, 1), 16);
		Text->Pickable = true;
	}
	DexianEditBox::~DexianEditBox()
	{

	}
	BOOL 	DexianEditBox::IsPtIn(PixelF pt)
	{
		CPUTime(DexianControl);
		for (int i = 0; i < Draws.size(); i++) {
			if (Draws[i]->Pickable && Draws[i]->Display && Draws[i]->Pick(WindowRect, pt)) {
				return Draws[i]->GetType();
			}
		}
		for (int i = 0; i < Childs.size(); i++) {
			if (Childs[i]->IsPtIn(pt))
				return true;
		}
		if (bMoveEnable)
			return WindowRect.isPtIn(pt);
		return false;
	}
	const char*		DexianEditBox::GetText()
	{
		return Text->TextPtr->GetText();
	}

	char 	DexianEditBox::OnMouseMessage(unsigned int uMsg, PixelF pos, unsigned int touchIndex)
	{
		switch (uMsg)
		{
		case InputEventID_MouseMove:
		{
			int nPick = -1;
			for (int i = 0; i < Draws.size(); i++) {
				if (Draws[i]->Pickable && Draws[i]->Display && Draws[i]->Pick(WindowRect, pos)) {
					nPick = i;
					break;
				}
			}
			if (DownDraw >= 0) {
				Draws[DownDraw]->OnMoving(pos);
				if (EnterDraw >= 0) {
					Draws[EnterDraw]->OnLeave();
				}
				EnterDraw = -1;
			}
			if (bMoveDown == (touchIndex + 1))
			{
				if (bIsBodyDown) {
					int x, y;
					x = pos.x - MoveOffset.x;
					y = pos.y - MoveOffset.y;
					int sw = g_manager.GetUIBufferWidth();
					int sh = g_manager.GetUIBufferHeight();
					if (bMoveEnable) {
						int w = WindowRect.Width();
						int h = WindowRect.Height();
						if (bAlign) {
							int nAlign = DexianManager::GetInstance().AlignPixel;
							x = (int)round((float)x / (float)nAlign)*nAlign;
							y = (int)round((float)y / (float)nAlign)*nAlign;
						}
						SetWindowRect(RectF(x, y, x + w, y + h));
					}
					else if (bIsCaption) {
						int w = ParentPtr->WindowRect.Width();
						int h = ParentPtr->WindowRect.Height();
						if (bClipInWindow) {
							if (x > (sw - w))
								x = sw - w;
							else if (x < 0)
								x = 0;
							if (y > (sh - h))
								y = sh - h;
							else if (y < 0)
								y = 0;
						}
						ParentPtr->SetWindowRect(RectF(x, y, x + w, y + h));
					}
				}
				return (DexianManager::GetInstance().MouseDown == this);
			}
			else if (IsPtIn(pos)) {
				DexianManager::GetInstance().MouseEnter = this;
				g_manager.CursorMode = Cursor_Grip;
			}
			else {
				DexianManager::GetInstance().MouseEnter = 0;
				DexianManager::GetInstance().MouseDown = 0;
				g_manager.CursorMode = Cursor_Arrow;
			}
			if (EnterDraw >= Draws.size())
				EnterDraw = -1;
			if (nPick != EnterDraw) {
				if (EnterDraw >= 0) {
					Draws[EnterDraw]->OnLeave();
				}
				if (nPick >= 0) {
					Draws[nPick]->OnEnter();
				}
				EnterDraw = nPick;
				return true;
			}
			if (EnterDraw >= 0) {
				//if(Draws[EnterDraw]->GetType()==DexianDrawBase::Type_EditText)
				g_manager.CursorMode = Draws[EnterDraw]->GetCursorMode();
			}
			else {
				g_manager.CursorMode = Cursor_Arrow;
			}
			return false;
		}
		break;
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
		{
			if (touchIndex == 0)
			{
				int nPick = -1;
				for (int i = 0; i < Draws.size(); i++) {
					if (Draws[i]->Pickable && Draws[i]->Display && Draws[i]->Pick(WindowRect, pos)) {
						nPick = i;
						break;
					}
				}
				if (nPick >= 0) {
					bMoveDown = (touchIndex + 1);
					DexianManager::GetInstance().MouseDown = this;
					Draws[nPick]->OnPick();
					DownDraw = nPick;
					Draws[nPick]->OnButtonDown(pos);
				}
				else {
					int x = 0, y = 0;
					if (bMoveEnable) {
						x = WindowRect.left;
						y = WindowRect.top;
					}
					else if (bIsCaption) {
						x = ParentPtr->WindowRect.left;
						y = ParentPtr->WindowRect.top;
					}
					else
						return false;
					PixelF pos;
					g_manager.GetCursorPos2D(pos, touchIndex);
					this->MoveOffset.Set(pos.x - x, pos.y - y);
					this->bMoveDown = (touchIndex + 1);
					DexianManager::GetInstance().MouseDown = this;
					bIsBodyDown = true;
					return true;
				}
				return true;
			}
			break;
		}
		case InputEventID_ButtonUp:
		{
			if (CallbackButtonUp&&IsPtIn(pos))
				(*CallbackButtonUp)(this, touchIndex, pos);
			g_manager.CursorMode = Cursor_Arrow;
			bIsBodyDown = false;
			if (bMoveDown)
			{
				bMoveDown = false;
				if (DexianManager::GetInstance().MouseDown == this)
					DexianManager::GetInstance().MouseDown = 0;
				if (DownDraw >= 0) {
					Draws[DownDraw]->OnButtonUp(pos);
					g_manager.CursorMode = Cursor_Arrow;// Draws[DownDraw]->GetCursorMode();
				}
				DownDraw = -1;
				//if (Callback)
				//	(*Callback)();
				return true;
			}
			break;
		}
		}
		return false;
	}
	DexianDrawRect*			DexianControl::AddRect(const RectF& rc, const color4& c)
	{
		DexianDrawRect* D = new DexianDrawRect(this);
		D->ModeX = AlignLeft;
		D->ModeY = AlignTop;
		D->rc = rc;
		D->color = c;
		Draws.push_back(D);
		return D;
	}
	DexianDrawRect*			DexianControl::AddRect(const color4& c)
	{
		DexianDrawRect* D = new DexianDrawRect(this);
		D->ModeX = AlignLeft;
		D->ModeY = AlignTop;
		D->rc = this->ClientRect;
		D->color = c;
		Draws.push_back(D);
		return D;
	}
	DexianDrawTexture*		DexianControl::AddTexture(const RectF& rc, const color4& c, const char* File, const RectF* Region, BaseDirectory* dir)
	{
		DexianDrawTexture* D = new DexianDrawTexture(this);
		D->ModeX = AlignLeft;
		D->ModeY = AlignTop;
		D->rc = rc;
		D->color = c;
		D->Element.InitTexture(File, Region, dir);
		Draws.push_back(D);
		return D;
	}
	DexianDrawRoundRect*	DexianControl::AddRoundRect(const RectF& rc, const color4& c, float fRoundRadius, int nRoundMode)
	{
		DexianDrawRoundRect* D = new DexianDrawRoundRect(this);
		D->ModeX = AlignLeft;
		D->ModeY = AlignTop;
		D->rc = rc;
		D->color = c;
		D->Round = fRoundRadius;
		D->RoundMode = nRoundMode;
		Draws.push_back(D);
		return D;
	}
	DexianDrawText*			DexianControl::AddText(const char* Text, const color4& c, int FontSize, const char* szFontName, int FontStyle)
	{
		DexianDrawText* D = new DexianDrawText(this);
		D->ModeX = AlignCenterX;
		D->ModeY = AlignCenterY;
		D->TextPtr->SetFontName(szFontName);
		D->TextPtr->SetFontSize(FontSize);
		D->TextPtr->SetFontStyle(FontStyle);
		D->FontSize = FontSize;
		D->SetText(Text);
		D->color = c;
		D->rc = RectF(0, 0, WindowRect.GetWidth(), WindowRect.Height());
		Draws.push_back(D);
		return D;
	}
	DexianDrawEditText*		DexianControl::AddEditableText(const RectF& rc, const char* Text, const color4& c, int FontSize, const char* szFontName, int FontStyle)
	{
		DexianDrawEditText* D = new DexianDrawEditText(this);
		D->ModeX = AlignLeft;
		D->ModeY = AlignTop;
		D->TextPtr->SetFontName(szFontName);
		D->TextPtr->SetFontSize(FontSize);
		D->TextPtr->SetFontStyle(FontStyle);
		D->FontSize = FontSize;
		D->SetText(Text);
		D->rc = rc;
		D->TextPtr->SetText(Text);
		D->color = c;
		Draws.push_back(D);
		return D;
	}
	DexianDrawCircle*		DexianControl::AddCircle(const PixelF& center, float Radius, const color4& c, int Smooth)
	{
		DexianDrawCircle* D = new DexianDrawCircle(this);
		D->ModeX = AlignLeft;
		D->ModeY = AlignTop;
		D->Center = center;
		D->Color = c;
		D->Smooth = Smooth;
		D->Radius = Radius;
		Draws.push_back(D);
		return D;
	}
	DexianDrawEventPoint*			DexianControl::AddEventPoint(DexianViewerPlace* Place, const RectF& rc, const color4& c, const char* File, const RectF* Region, const char* FullFile, const RectF* FullRegion, BaseDirectory* dir)
	{
		if (!Place)
			return 0;
		DexianDrawEventPoint* D = new DexianDrawEventPoint(Place, this);
		D->ModeX = AlignLeft;
		D->ModeY = AlignTop;
		D->rc = rc;
		D->color = c;
		D->Element.InitTexture(File, Region, dir);
		D->FullElement.InitTexture(FullFile, FullRegion, dir);
		Draws.push_back(D);
		return D;
	}
	DexianViewerExec::DexianViewerExec(DexianControl* dlg)
		:DexianControl(dlg)
	{
		FontSize = 24;
		TitleSize = 40;
		IconSize = 32;
		FrameReserved = 16;
		Title = 0;
		RectTitle = 0;
		RectBody = 0;
	}
	DexianViewerExec::~DexianViewerExec()
	{

	}
	enum EFontType {
		Font_Title = 0,
		Font_Member,
	};

	const char*	GetTypeFont(EFontType t)
	{
		switch (t) {
		case Font_Title:
			return "Century Scrollbook";
		case Font_Member:
			return "Century Scrollbook";
		}
		return "Arial";
	}
	const int EditableSpace = 10;
	void		DexianViewerExec::SetupEvents(Array<EventLine>& Results, const Array<DexianExecNodePoint*>& Events, int Line, bool isLeft,
		const char* IconFile, const char* FullIconFile, BaseDirectory* TextureDir)
	{
		int nHeight = 0;
		int nMaxWidth = 0;
		Results.clear();
		for (int i = 0; i < Events.size(); i++) {
			EventLine EL;
			const DexianExecNodePoint& Event = *Events[i];
			int TextWidth = 0;
			if (Event.Name.t[0]) {
				DexianDrawText* Text = 0;
				Text = this->AddText(Event.Name, color4(1, 1, 1, 1),
					FontSize, GetTypeFont(Font_Member));
				Text->Offset.y += TitleSize + (i + Line)*IconSize + (IconSize - FontSize) / 2;
				Text->Offset.x = (FrameReserved + IconSize)*(isLeft ? 1 : -1);
				Text->TextPtr->SetFontSize(Text->FontSize);
				Text->TextPtr->Redraw();
				Text->SetLayer(10);
				TextWidth = Text->TextPtr->GetWidth();
				EL.Text = Text;
			}
			int EditableWidth = 0;
			color4 color(1, 1, 1, 1);
			PtrID tid = Event.TypeID;
			const DexianType* ST = Event.Viewer->System->SearchType(tid);
			if (ST) {
				color = ST->Color;
				if (isLeft && ST->Limit != Edit_None) {
					DexianDrawEditText* Text = this->AddEditableText(Rect(0, 0, 60, 26), Event.Name, color4(1, 1, 1, 1),
						FontSize-2, GetTypeFont(Font_Member), RenderEditText::Bold);
					Text->Offset.y += TitleSize + (i + Line)*IconSize;// +(IconSize - FontSize) / 2;
					Text->Offset.x = (FrameReserved + IconSize + TextWidth + EditableSpace)*(isLeft ? 1 : -1);
					Text->TextPtr->SetFontSize(Text->FontSize);
					Text->TextPtr->Redraw();
					Text->TextPtr->Align = RenderEditText::AlignText_Center | RenderEditText::AlignText_VCenter;
					Text->SetLayer(10);
					Text->Pickable = true;
					Text->TextPtr->Callback = this;
					Text->TextPtr->CallbackParam = Text;
					EditableWidth = Text->TextPtr->GetWidth() + EditableSpace;
					EL.Editable = Text;
				}
			}
			DexianDrawEventPoint* EventArrow = AddEventPoint(Place, RectF(0, 0, IconSize, IconSize), 
				color, IconFile, 0, FullIconFile, 0, TextureDir);
			EL.EventArrow = EventArrow;
			EventArrow->Node = PlaceNodeID(Events[i]->Viewer->Place->GetID(), Events[i]->Viewer->GetID(), Events[i]->GetID());
			EventArrow->Offset.y += TitleSize + (i + Line)*IconSize;
			EventArrow->Offset.x = FrameReserved*(isLeft ? 1 : -1);
			EventArrow->IsLeft = isLeft;
			EventArrow->PickWidth = TextWidth + IconSize;
			int nT = TextWidth + EditableWidth + IconSize + FrameReserved;
			EventArrow->Pickable = true;
			EventArrow->Cursor = Cursor_Grip;
			if (nT > nMaxWidth)
				nMaxWidth = nT;
			nHeight = (TitleSize + (i + Line)*IconSize) + IconSize;
			EL.Height = IconSize;
			EventArrow->ID = Events[i]->GetID();
			EventPoints += EventArrow;
			Results += EL;
			//Text->BindPoint = EventArrow;
		}
		//return Size(nMaxWidth, nHeight);
	}
	Size	DexianViewerExec::GetSize(Array<EventLine>& Ins, int Line)
	{
		int nHeight = 0;
		int nMaxWidth = 0;
		for (int i = 0; i < Ins.size(); i++) {
			EventLine& EL = Ins[i];
			int TextWidth = 0;
			if(EL.Text)
				TextWidth = EL.Text->TextPtr->GetWidth();
			int EditableWidth = 0;
			if (EL.Editable && EL.Editable->Display) {
				EditableWidth = EL.Editable->rc.Width() + EditableSpace;
				if (EditableWidth < 30)
					EditableWidth = 30;
			}
			int nT = TextWidth + EditableWidth + IconSize + FrameReserved;
			if (nT > nMaxWidth)
				nMaxWidth = nT;
			nHeight = (TitleSize + (i + Line)*IconSize) + IconSize;
			EL.Height = IconSize;
		}
		return Size(nMaxWidth, nHeight);
	}
	void	DexianViewerExec::RecalcSize()
	{
		int nWidth = 128;
		int nMaxLeft = 0;
		int nMaxRight = 0;
		int nMaxTitle = 0;
		int nHeight = 0;
		if(Title)
			nMaxTitle = Title->TextPtr->GetWidth();
		Size nMaxEvent = GetSize(LeftEvents, 0);
		if (nMaxEvent.cx > nMaxLeft)nMaxLeft = nMaxEvent.cx;
		if (nMaxEvent.cy > nHeight)nHeight = nMaxEvent.cy;
		nMaxEvent = GetSize(LeftParams, LeftEvents.size());
		if (nMaxEvent.cx > nMaxLeft)nMaxLeft = nMaxEvent.cx;
		if (nMaxEvent.cy > nHeight)nHeight = nMaxEvent.cy;
		nMaxEvent = GetSize(RightEvents, 0);
		if (nMaxEvent.cx > nMaxRight)nMaxRight = nMaxEvent.cx;
		if (nMaxEvent.cy > nHeight)nHeight = nMaxEvent.cy;
		nMaxEvent = GetSize(RightParams, RightEvents.size());
		if (nMaxEvent.cx > nMaxLeft)nMaxLeft = nMaxEvent.cx;
		if (nMaxEvent.cy > nHeight)nHeight = nMaxEvent.cy;
		int nLR = nMaxLeft + nMaxRight + 20;
		if (nLR > nMaxTitle)
			nMaxTitle = nLR;
		nWidth = nMaxTitle + 120;
		nHeight += 10;
		RectTitle->rc.Width(nWidth);
		RectTitle->rc.Height(nHeight);
		RectBody->rc.Width(nWidth);
		RectBody->rc.Height(TitleSize);
		this->WindowRect.Width(nWidth);
		this->WindowRect.Height(nHeight);
		this->ClientRect.Width(nWidth);
		this->ClientRect.Height(nHeight);
	}
	void	DexianViewerExec::OnChanged(RenderEditText* t, void* Param)
	{
		//DexianDrawEditText* Text = (DexianDrawEditText*)Param;
		//int nWidth = t->GetTextWidth();
		//if (nWidth < 50)nWidth = 50;
		//if (nWidth > 300)nWidth = 300;
		//Text->rc.Width(nWidth);
		////Text->Buffer.Valid = false;
		////Text->RectBuffer.Valid = false;
		//Text->InvalidText = true;
		//RecalcSize();
	}
	void	DexianViewerExec::Setup(const DexianPlaceExec& Viewer, BaseDirectory* TextureDir)
	{
		if (Viewer.Name.t[0]) {
			Title = this->AddText(Viewer.Name, color4(1, 1, 1, 1),
				FontSize, GetTypeFont(Font_Title));
			Title->Offset.y += 5;
			Title->TextPtr->SetFontSize(Title->FontSize);
			Title->TextPtr->Redraw();
			Title->SetLayer(10);
		}
		//this->bIsCaption = true;
		SetupEvents(LeftEvents, Viewer.LeftEvents, 0, true, "EventArrow.png", "EventArrowFull.png", TextureDir);
		SetupEvents(LeftParams, Viewer.LeftParams, Viewer.LeftEvents.size(), true, "ParamIcon.png", "ParamIconFull.png", TextureDir);
		SetupEvents(RightEvents, Viewer.RightEvents, 0, false, "EventArrow.png", "EventArrowFull.png", TextureDir);
		SetupEvents(RightParams, Viewer.RightParams, Viewer.RightEvents.size(), false, "ParamIcon.png", "ParamIconFull.png", TextureDir);
		RectTitle = this->AddRoundRect(RectF(0, 0, 15, 15), color4(0.01f, 0.01f, 0.01f, 0.2), 20.0f, 0xff);
		RectTitle->SetLayer(-2);
		RectBody = this->AddRoundRect(RectF(0, 0, 15, 15), color4(1, 0, 0, 0.5), 20.0f, 1 | 2);
		RectBody->Shadow = false;
		RectBody->SetLayer(-1);
		AddTexture(RectF(5, -5, 48, 48), color4(1, 1, 1, 1), "EventIcon.png", 0, TextureDir);
		this->RequestSort = true;
		bClipInWindow = false;
		bMoveEnable = true;
		bAlign = true;
		this->ID = Viewer.GetID();
		RecalcSize();
	}
	DexianListLine::DexianListLine(DexianListBox* list, const char* szText, const char* IconFile, BaseDirectory* TextureDir)
	{
		ListBox = list;
		Icon = 0;
		Text = 0;
		if (IconFile)
			Icon = list->AddTexture(RectF(0, 0, 32, 32), color4(1, 1, 1, 1), IconFile, 0, TextureDir);
		Text = list->AddText(szText, color4(1, 1, 1, 1), 24, GetTypeFont(Font_Title));
		Text->Offset.x = 32;
		Text->InvalidText = true;
		if(Icon)
			Icon->Display = false;
		Text->Display = false;
		//Text->TextPtr->SetFontSize(18);
		//Text->TextPtr->Redraw();
		Text->SetLayer(1);
		Param = 0;
	}
	DexianListLine::~DexianListLine()
	{
		if (ListBox)
		{
			ListBox->Remove(Text);
			ListBox->Remove(Icon);
		}
	}
	DexianListBox::DexianListBox(DexianControl* dlg)
		:DexianControl(dlg)
	{
		Title = 0;
		Selected = 0;
		Back = 0;
		SelectedIndex = -1;
		nTitleHeight = 0;
		nLineHeight = 24;
		Callback = 0;
		Title = this->AddText("", color4(1, 1, 1, 1), 24, GetTypeFont(Font_Title));
		Title->TextPtr->SetFontSize(24);
		Title->TextPtr->Redraw();
		Title->SetLayer(1);
		Selected = this->AddRect(RectF(0, 0, 0, nLineHeight), color4(0.01f, 0.8f, 0.5f, 0.8));
		Selected->SetLayer(-1);
		Selected->Display = false;
		HotRect = this->AddRect(RectF(0, 0, 0, nLineHeight), color4(0.01f, 0.2f, 0.5f, 1));
		HotRect->SetLayer(-2);
		HotRect->Display = false;
		Back = this->AddRect(RectF(0, 0, 0, 0), color4(0.01f, 0.01f, 0.01f, 0.5));
		Back->SetLayer(-3);
		BackColor = color4(0.1f, 0.1f, 0.1f, 1.0f);
		PaddingW = PaddingH = 5;
		nFirstIndex = 0;
		nHotIndex = -1;
		nSelectIndex = -1;
	}
	DexianListBox::~DexianListBox()
	{
		ClearAllItems();
	}
	DexianListLine*		DexianListBox::GetItemAt(int Index)
	{
		if (Index < 0 || Index >= Lines.size())
			return 0;
		return Lines[Index];
	}
	void				DexianListBox::SetClientRect(const RectF& rc)
	{
		DexianControl::SetClientRect(rc);
		Selected->rc.right = rc.GetWidth();
		HotRect->rc.right = rc.GetWidth();
		Back->rc.right = rc.GetWidth();
	}
	void				DexianListBox::SetSelected(int index)
	{
		SelectedIndex = index;
		if (SelectedIndex < 0 || SelectedIndex >= Lines.size())
			Selected->Display = false;
		else {
			Selected->Display = true;
			Selected->Offset.y = nTitleHeight + nLineHeight*index + 3;
		}
	}
	void				DexianListBox::SetTitleText(const char* Text)
	{
		Title->SetText(Text);
	}
	BOOL 				DexianListBox::IsPtIn(PixelF pt)
	{
		return WindowRect.isPtIn(pt);
	}
	void				DexianListBox::OnRender(float fElapsedTime, bool bIsEnabled)
	{
		color4 c = BackColor;
		if (!bIsEnabled)
			c = c.gray();
		g_manager.DrawRect2D(WindowRect, c, true);
	}
	void				DexianListBox::ClearAllItems() {
		for (int i = 0; i < Lines.size(); i++) {
			delete Lines[i];
		}
		Lines.clear();
		this->nHotIndex = -1;
		this->SelectedIndex = -1;
		this->nFirstIndex = 0;
	}
	void				DexianListBox::UpdateItems()
	{
		for (int i = 0; i < Lines.size(); i++) {
			int Top = (i - nFirstIndex) * nLineHeight + nTitleHeight + 3;
			Lines[i]->rc = RectF(PaddingW, Top + PaddingH, this->WindowRect.GetWidth() - PaddingW * 2, Top + nLineHeight - PaddingH * 2);
			Lines[i]->Text->rc = Lines[i]->rc;
			if (Lines[i]->Icon)
				Lines[i]->Icon->rc = Lines[i]->rc;
			Lines[i]->Text->Invalidate();
			if (Lines[i]->rc.top >= 0 && Lines[i]->rc.top < WindowRect.bottom) {
				Lines[i]->Text->Display = true;
			}
			else {
				Lines[i]->Text->Display = false;
			}
			//if(i>=this->nFirstIndex)
		}
	}
	void				DexianListBox::OnRenderDraws(bool bIsEnabled) {
		UpdateItems();
		HotRect->Offset.y = nTitleHeight + nLineHeight*(nHotIndex-nFirstIndex) + 3;
		HotRect->Invalidate();
		Selected->Offset.y = nTitleHeight + nLineHeight*(SelectedIndex - nFirstIndex) + 3;
		Selected->Invalidate();
		if (HotRect->Display && HotRect->Offset.y >= 0 && HotRect->Offset.y < WindowRect.GetHeight())
			HotRect->Render(WindowRect + offsetRect, bIsEnabled);
		if (Selected->Display && Selected->Offset.y >= 0 && Selected->Offset.y < WindowRect.GetHeight())
			Selected->Render(WindowRect + offsetRect, bIsEnabled);
		for (int i = 0; i < Lines.size(); i++) {
			if (Lines[i]->rc.top >= 0 && Lines[i]->rc.top < WindowRect.GetHeight()) {
				Lines[i]->Text->Render(WindowRect, bIsEnabled);
			}
		}
	}
	DexianListLine*		DexianListBox::AddItem(const char* Text, const char* IconFile, void* Context, BaseDirectory* TextureDir)
	{
		DexianListLine* L = new DexianListLine(this, Text, IconFile, TextureDir);
		L->Context = Context;
		Lines.push_back(L);
		//int nHeight = nTitleHeight + nLineHeight*Lines.size() + nTitleHeight;
		//RectF rc = ClientRect;
		//rc.bottom = rc.top + nHeight;
		////Back->Buffer.Valid = false;
		//SetClientRect(rc);
		return L;
	}
	int			DexianListBox::GetIndexAtPoint(const PixelF& pos)
	{
		int index = (pos.y - WindowRect.top - nTitleHeight) / nLineHeight + nFirstIndex;
		if (!IsPtIn(pos))
			return -1;
		if (index < 0 || index >= this->Lines.size())
			return -1;
		return index;
	}
	char 		DexianListBox::OnMouseMessage(unsigned int uMsg, PixelF pos, unsigned int touchIndex)
	{
		if (!ParentPtr)return 0;
		switch (uMsg)
		{
		case InputEventID_Wheel:
		{
			if (pos.x < 0) {
				if(nFirstIndex<(Lines.size()-5))
					this->nFirstIndex++;
			}
			else {
				if (nFirstIndex > 0)
					nFirstIndex--;
			}
			return true;
		}
		case InputEventID_MouseMove:
		{
			if (bMoveDown == (touchIndex + 1))
			{
				return (DexianManager::GetInstance().MouseDown == this);
			}
			else if (IsPtIn(pos)) {
				DexianManager::GetInstance().MouseEnter = this;
				nHotIndex = GetIndexAtPoint(pos);
				if (nHotIndex < 0 || nHotIndex >= Lines.size())
					HotRect->Display = false;
				else {
					HotRect->Display = true;
				}
			}
			else {
				HotRect->Display = false;
				DexianManager::GetInstance().MouseEnter = 0;
				DexianManager::GetInstance().MouseDown = 0;
			}
			return false;
		}
		break;
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
		{
			if (IsPtIn(pos) && touchIndex == 0)
			{
				bMoveDown = true;
				DexianManager::GetInstance().MouseDown = this;
				return true;
			}
			break;
		}
		case InputEventID_ButtonUp:
		{
			if (CallbackButtonUp&&IsPtIn(pos))
				(*CallbackButtonUp)(this, touchIndex, pos);
			if (bMoveDown)
			{
				int hotIndex = GetIndexAtPoint(pos);
				SelectedIndex = hotIndex;
				if (hotIndex >= 0 && hotIndex < Lines.size())
				{
					Selected->Display = true;
				}
				bMoveDown = false;
				if (DexianManager::GetInstance().MouseDown == this)
					DexianManager::GetInstance().MouseDown = 0;
				if (Callback)
					Callback(SelectedIndex);
				return true;
			}
			break;
		}
		}
		return false;
	}

	DexianStatic::DexianStatic(DexianControl* dlg)
		:DexianControl(dlg)
	{

	}
	DexianStatic::~DexianStatic()
	{

	}
	void					DexianStatic::OnRender(float fElapsedTime, bool bIsEnabled)
	{
		RenderElement(&Back, bIsEnabled);
	}
	char 					DexianStatic::OnMouseMessage(unsigned int uMsg, PixelF pos, unsigned int touchIndex)
	{
		if (!ParentPtr)return 0;
		switch (uMsg)
		{
		case InputEventID_MouseMove:
		{
			if (bMoveDown == (touchIndex + 1))
			{
				return (DexianManager::GetInstance().MouseDown == this);
			}
			else if (IsPtIn(pos)) {
				DexianManager::GetInstance().MouseEnter = this;
				g_manager.CursorMode = Cursor_Grip;
			}
			else {
				DexianManager::GetInstance().MouseEnter = 0;
				DexianManager::GetInstance().MouseDown = 0;
				g_manager.CursorMode = Cursor_Arrow;
			}
			return false;
		}
		break;
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
		{
			if (IsPtIn(pos) && touchIndex == 0)
			{
				bMoveDown = true;
				DexianManager::GetInstance().MouseDown = this;
				return true;
			}
			break;
		}
		case InputEventID_ButtonUp:
		{
			if (CallbackButtonUp&&IsPtIn(pos))
				(*CallbackButtonUp)(this, touchIndex, pos);
			g_manager.CursorMode = Cursor_Arrow;
			if (bMoveDown)
			{
				bMoveDown = false;
				if (DexianManager::GetInstance().MouseDown == this)
					DexianManager::GetInstance().MouseDown = 0;
				return true;
			}
			break;
		}
		}
		return false;
	}

	DexianButton::DexianButton(DexianControl* dlg)
		:DexianControl(dlg)
	{
		text = 0;
	}
	DexianButton::~DexianButton()
	{

	}
	void		DexianButton::OnRender(float fElapsedTime, bool bIsEnabled)
	{
		color4 c = Back.Color;
		if (bMoveDown)
			Back.Color *= 0.5f;
		RenderElement(&Back, bIsEnabled);
		Back.Color = c;
	}

	char 		DexianButton::OnMouseMessage(unsigned int uMsg, PixelF pos, unsigned int touchIndex)
	{
		if (!ParentPtr)return 0;
		switch (uMsg)
		{
		case InputEventID_MouseMove:
		{
			if (bMoveDown == (touchIndex + 1))
			{
				return (DexianManager::GetInstance().MouseDown == this);
			}
			else if (IsPtIn(pos)) {
				DexianManager::GetInstance().MouseEnter = this;
				g_manager.CursorMode = Cursor_Grip;
			}
			else {
				DexianManager::GetInstance().MouseEnter = 0;
				DexianManager::GetInstance().MouseDown = 0;
				g_manager.CursorMode = Cursor_Arrow;
			}
			return false;
		}
		break;
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
		{
			if (IsPtIn(pos) && touchIndex == 0)
			{
				bMoveDown = true;
				DexianManager::GetInstance().MouseDown = this;
				offsetRect.Set(1, 1, 1, 1);
				return true;
			}
			break;
		}
		case InputEventID_ButtonUp:
		{
			if (CallbackButtonUp&&IsPtIn(pos))
				(*CallbackButtonUp)(this, touchIndex, pos);
			g_manager.CursorMode = Cursor_Arrow;
			if (bMoveDown)
			{
				bMoveDown = false;
				if (DexianManager::GetInstance().MouseDown == this)
					DexianManager::GetInstance().MouseDown = 0;
				offsetRect.Set(0, 0, 0, 0);
				if (Callback)
					Callback(this);
				return true;
			}
			break;
		}
		}
		return false;
	}
	DexianGrids::DexianGrids(DexianControl* dlg)
		:DexianControl(dlg)
	{
		AlignPixel = 16;
		GroupScale = 8;//组格子倍数
		LineColor.setRGBA(0.6f, 0.6f, 0.6f, 1.0f);
		GroupColor.setRGBA(0.2f, 0.2f, 0.2f, 1.0f);
		Popup = 0;
	}
	DexianGrids::~DexianGrids()
	{

	}
	char 		DexianGrids::OnMouseMessage(unsigned int uMsg, PixelF pos, unsigned int touchIndex)
	{
		if (!ParentPtr)
			return 0;
		switch (uMsg)
		{
		case InputEventID_Wheel:
		{
			float fMaxScale = 5.0f;
			float fMinScale = 0.1f;
			float fScale = 1.0f;
			if (pos.x > 0) {
				fScale = 1.1f;
				if (ParentPtr->Scale >= fMaxScale)
					break;
			}
			else {
				fScale = 0.9f;
				if (ParentPtr->Scale <= fMinScale)
					break;
			}
			ParentPtr->Scale = ParentPtr->Scale * fScale;// , fMinScale, fMaxScale);
			PixelF ret = g_manager.GetCursorPos(3);
			PixelF of = ParentPtr->Offset - ret;
			ParentPtr->Offset = of * fScale + ret;
			g_manager.m_bSizeChanged = true;
			//ParentPtr->bIsInvalidate = true;
			return true;
		}
		case InputEventID_MouseMove:
		{
			if (bMoveDown == (touchIndex + 1))
			{
				pos = g_manager.GetCursorPos(touchIndex);
				PixelF P = (pos - DownPos) + DownPos;
				int x, y;
				x = P.x - MoveOffset.x;
				y = P.y - MoveOffset.y;
				int sw = g_manager.GetUIBufferWidth();
				int sh = g_manager.GetUIBufferHeight();
				{
					//ParentPtr->Offset.x = x;
					//ParentPtr->Offset.y = y;
					int w = ParentPtr->WindowRect.Width();
					int h = ParentPtr->WindowRect.Height();
					//if (x > (sw - w))
					//	x = sw - w;
					//else if (x < 0)
					//	x = 0;
					//if (y >(sh - h))
					//	y = sh - h;
					//else if (y < 0)
					//	y = 0;
					ParentPtr->SetWindowRect(RectF(x, y, x + w, y + h));
				}
				return true;
			}
			return (DexianManager::GetInstance().MouseDown == this);
		}
		break;
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
		{
			if (touchIndex == 1 && IsPtIn(pos))
			{
				g_manager.CursorMode = Cursor_Grip;
				int x = 0, y = 0;
				{
					x = ParentPtr->WindowRect.left;
					y = ParentPtr->WindowRect.top;
					//x = ParentPtr->Offset.x;
					//y = ParentPtr->Offset.y;
				}
				pos = g_manager.GetCursorPos(touchIndex);
				DownPos = pos;
				this->MoveOffset.Set(pos.x - x, pos.y - y);
				this->bMoveDown = (touchIndex + 1);
				DexianManager::GetInstance().MouseDown = this;
				return true;
			}
			break;
		}
		case InputEventID_ButtonUp:
		{
			if (touchIndex == 1 && IsPtIn(pos))
			{
				Pixel V = DownPos - pos;
				if (V.x == 0 && V.y == 0) {
					if (CallbackButtonUp)//&&IsPtIn(pos))
						(*CallbackButtonUp)(this, touchIndex, pos);
				}
			}
			//if (Popup)
			//	Popup->Display = false;
			//if (touchIndex == 1) {
			//	if (Popup)
			//	{
			//		PixelF P = g_manager.GetCursorPos(touchIndex);
			//		Popup->Display = true;
			//		RectF W = Popup->WindowRect;
			//		W.SetBeginPoint(P.x, P.y);
			//		Popup->SetWindowRect(W);
			//	}
			//}
			if (bMoveDown)
			{
				g_manager.CursorMode = Cursor_Arrow;
				bMoveDown = false;
				if (DexianManager::GetInstance().MouseDown == this)
					DexianManager::GetInstance().MouseDown = 0;
				return true;
			}
			break;
		}
		}
		return false;
	}
	void					DexianGrids::OnRender(float fElapsedTime, bool bIsEnabled)
	{
		int nAlign = AlignPixel;
		int nScale = (int)(1.0f / ParentPtr->Scale);// 5 - (int)(ParentPtr->Scale*5.0f);
		int nM = 1;
		while (true) {
			if (nScale <= nM)
			{
				nScale = nM / 2;
				if (nScale < 1)nScale = 1;
				break;
			}
			nM *= 4;
		}
		nAlign = nScale * AlignPixel;
		//else
		//	nAlign = AlignPixel / (1 - nScale);
		int x = ParentPtr->WindowRect.left;
		int y = ParentPtr->WindowRect.top;
		int xBegin = (x % nAlign);
		int yBegin = (y % nAlign);
		int Width = (float)g_manager.GetBufferWidth() / ParentPtr->Scale;
		int Height = (float)g_manager.GetBufferHeight() / ParentPtr->Scale;
		int xGroup = (x % (nAlign*GroupScale)) / nAlign;
		int yGroup = (y % (nAlign*GroupScale)) / nAlign;
		int OffsetX = -((int)((float)ParentPtr->Offset.x / ParentPtr->Scale) / nAlign + 1) * nAlign;
		int OffsetY = -((int)((float)ParentPtr->Offset.y / ParentPtr->Scale) / nAlign + 1) * nAlign;
		int numX = Width / nAlign + 5;
		int numY = Height / nAlign + 5;
		Pixel xys[2];
		color4 cl = LineColor;
		if (!bIsEnabled)cl = cl.gray();
		color4 cg = GroupColor;
		if (!bIsEnabled)cg = cg.gray();
		for (int nX = 0; nX < numX; nX++) {
			xys[0].Set(xBegin + nX*nAlign + OffsetX, OffsetY);
			xys[1].Set(xBegin + nX*nAlign + OffsetX, OffsetY + Height);
			int gIndex = (nX - xGroup + (OffsetX / nAlign)) % GroupScale;
			if (gIndex != 0)
				g_manager.DrawLines2D(xys, 2, cl, false);
		}
		for (int nY = 0; nY < numY; nY++) {
			xys[0].Set(OffsetX, yBegin + nY*nAlign + OffsetY);
			xys[1].Set(OffsetX + Width, yBegin + nY*nAlign + OffsetY);
			int gIndex = (nY - yGroup + (OffsetY / nAlign)) % GroupScale;
			if (gIndex != 0)
				g_manager.DrawLines2D(xys, 2, cl, false);
		}
		//Rect groupRect;
		for (int nX = 0; nX < numX; nX++) {
			xys[0].Set(xBegin + nX*nAlign + OffsetX, OffsetY);
			xys[1].Set(xBegin + nX*nAlign + OffsetX, OffsetY + Height);
			//groupRect.Set(xBegin + nX*nAlign - 1, 0, xBegin + nX*nAlign + 1, Height);
			int gIndex = (nX - xGroup + (OffsetX / nAlign)) % GroupScale;
			if (gIndex == 0)
				g_manager.DrawLines2D(xys, 2, cg, false); //g_manager.DrawRect2D(groupRect, GroupColor, true);
		}
		for (int nY = 0; nY < numY; nY++) {
			xys[0].Set(OffsetX, yBegin + nY*nAlign + OffsetY);
			xys[1].Set(OffsetX + Width, yBegin + nY*nAlign + OffsetY);
			//groupRect.Set(0, yBegin + nY*nAlign - 1, Width, yBegin + nY*nAlign + 1);
			int gIndex = (nY - yGroup + (OffsetY / nAlign)) % GroupScale;
			if (gIndex == 0)
				g_manager.DrawLines2D(xys, 2, cg, false); //g_manager.DrawRect2D(groupRect, GroupColor, true);
		}
	}
	DexianComboBox::DexianComboBox(DexianControl* dlg)
		:DexianListBox(dlg)
	{

	}
	DexianComboBox::~DexianComboBox()
	{

	}

	DexianManager&			DexianManager::GetInstance()
	{
		return *g_manager.m_dxManager;
	}
	DexianManager::DexianManager()
	{
		m_fToolTipTime = 10.0f;
		m_bEnableScaling = false;
		MouseEnter = 0;
		MouseDown = 0;
		AlignPixel = 16;
		ShadowColor = color4(0.1f, 0.1f, 0.1f, 0.9f);
		ShadowWidth = 1;
		OnOpenEdit = 0;
		m_toolTip = 0;
		m_toolTipText = 0;
	}
	DexianManager::~DexianManager()
	{
		for (int i = 0; i < STable.size(); i++) {
			delete STable[i];
		}
		Destroy();
	}
	void	DexianManager::CreateTooltip()
	{
		if (m_toolTip)
			delete m_toolTip;
		m_toolTip = new DexianControl(0);
		m_toolTip->SetClientRect(RectF(0, 0, 128, 24));
		m_toolTip->AddRect( RectF(0, 0, 128, 24), color4(0, 0, 0, 0.1f));
		m_toolTipText = m_toolTip->AddText( "TEST ToolTip", color4(1, 1, 0.1, 1.0f), 24);
	}
	//void							DexianManager::OnIMEChange(void* Context, const short* Text, unsigned short nCursor, unsigned short nSelect)
	//{
	//	DexianDrawEditText* DE = (DexianDrawEditText*)Context;
	//	if (DE)
	//		DE->OnIMEChange(Text, nCursor, nSelect);
	//}
	Texture*						DexianManager::GetTexture(PtrID& id)
	{
		DexianTexture* Tex = Search(m_textureMap, id);
		if (Tex)
			return Tex->texturePtr;
		return 0;
	}
	MyFont*							DexianManager::GetFont(PtrID& id)
	{
		DexianFont* F = Search(m_fontMap, id);
		if (F)
			return F->fontPtr;
		return 0;
	}
	//DexianShape*					DexianManager::GetShape(PtrID& id)
	//{
	//	DexianShape* F = Search(m_shapeMap, id);
	//	if (F)
	//		return F;
	//	return 0;
	//}
	//DexianShape*					DexianManager::NewShape(const char* Name)
	//{
	//	DexianShape* Shape = SearchShape(Name);
	//	if (Shape)
	//		return Shape;
	//	Shape = new DexianShape();
	//	Shape->Name = Name;
	//	m_shapeMap.push_back(Shape);
	//	return Shape;
	//}
	//DexianShape*					DexianManager::SearchShape(const char* Name)
	//{
	//	return Search(m_shapeMap, Name);
	//}
	void							DexianManager::OnDelete(DexianTexture* tex) {
		Erase(m_textureMap, tex);
	}
	void							DexianManager::OnDelete(DexianFont* tex) {
		Erase(m_fontMap, tex);
	}
	void							DexianManager::OnDelete(DexianControl* tex) {
		Erase(m_controlMap, tex);
	}
	DexianFont*						DexianManager::CreateFont(const char* szFontName, int height, int weight, char italic, char underline)
	{
		CPUTime(UIManager);
		if (!szFontName)
			szFontName = "Arial";
		for (int i = 0; i < (int)m_fontMap.size(); i++)
		{
			DexianFont* pFontNode = m_fontMap[i];
			if (pFontNode->nFontHeight == height && pFontNode->nWeight == weight && pFontNode->bItalic == italic && pFontNode->bUnderline == underline &&
				0 == stricmp(pFontNode->fontName, szFontName))
			{
				pFontNode->AddRef();
				return pFontNode;
			}
		}
		DexianFont* pNewFontNode = new DexianFont();
		if (pNewFontNode == NULL)
			return 0;
		pNewFontNode->fontName = szFontName;
		pNewFontNode->nFontHeight = height;
		pNewFontNode->nWeight = weight;
		pNewFontNode->bItalic = italic;
		pNewFontNode->bUnderline = underline;
		m_fontMap.push_back(pNewFontNode);
		return pNewFontNode;
	}
	int								DexianManager::AddTextureFile(const char* szFile)
	{
		for (int i = 0; i < m_textureFiles.size(); i++) {
			if (m_textureFiles[i] == szFile)
				return i;
		}
		m_textureFiles.push_back(FileName(szFile));
		return (int)m_textureFiles.size() - 1;
	}
	DexianTexture*					DexianManager::CreateTexture(int TextureIndex, int filter, BaseDirectory* dir)
	{
		CPUTime(UIManager);
		if (TextureIndex < 0 || TextureIndex >= (int)m_textureFiles.size())
			return 0;
		FileName f = m_textureFiles[TextureIndex];
		f.setIsFile();
		for (int i = 0; i < (int)m_textureMap.size(); i++)
		{
			DexianTexture* pTextureNode = m_textureMap[i];
			if (pTextureNode->fileName == f)
			{
				pTextureNode->filter = filter;
				pTextureNode->bLoadFail = false;
				pTextureNode->Dir = dir;
				pTextureNode->AddRef();
				return pTextureNode;
			}
		}
		DexianTexture* pTextureNode = new DexianTexture();
		if (pTextureNode == NULL)
			return 0;
		pTextureNode->fileName = f;
		pTextureNode->nRef = 1;
		pTextureNode->Dir = dir;
		pTextureNode->filter = filter;
		m_textureMap.push_back(pTextureNode);
		Texture* Tex = 0;
		if (g_manager.LoadTexture(&Tex, pTextureNode->fileName, false, filter, true, pTextureNode->Dir))
		{
			pTextureNode->texturePtr = Tex;
			Tex->Release();
		}
		return pTextureNode;
	}
	DexianControl*					DexianManager::CreateControl(DexianControlData::ControlType Type, DexianControl* Parent)
	{
		DexianControl* cc = 0;
		switch (Type) {
		case DexianControlData::Type_Base:
			cc = new DexianControl(Parent);
			break;
		}
		return cc;
	}
	DexianControl*					DexianManager::CreateControl()
	{
		DexianControl* cc = CreateControl(DexianControlData::ControlType::Type_Base, 0);
		if (cc)
			m_controlMap.push_back(cc);
		return cc;
	}
	//
	void							DexianManager::Render(float fElapsedTime)
	{
		for (int i = (int)m_controlMap.size() - 1; i >= 0; i--) {
			m_controlMap[i]->Setup2DSpace();
			if(m_controlMap[i]->Display)
				m_controlMap[i]->Render(fElapsedTime, true);
		}
	}
	DexianControl*					DexianManager::Pick(int x, int y)
	{
		DexianControl* c = 0;
		for (int i = 0; i < m_controlMap.size(); i++) {
			m_controlMap[i]->Setup2DSpace();
			if (m_controlMap[i]->Display) {
				if (c = m_controlMap[i]->Pick(x, y))
					return c;
			}
		}
		return 0;
	}
	//
	BOOL							DexianManager::OnInputEvent(unsigned int uMsg, const Pixel& pos, unsigned int touchIndex)
	{
		DexianControl* c = 0;
		if (MouseEnter) {
			if (uMsg == InputEventID_MouseMove)
			{
				DexianControl* cc = MouseEnter->ParentPtr;
				while (cc) {
					cc->Setup2DSpace();
					cc = cc->ParentPtr;
				}
				return MouseEnter->OnInputEvent(uMsg, pos, touchIndex);
			}
			MouseEnter = 0;
		}
		if (MouseDown)
		{
			DexianControl* cc = MouseDown->ParentPtr;
			while (cc) {
				cc->Setup2DSpace();
				cc = cc->ParentPtr;
			}
			return MouseDown->OnInputEvent(uMsg, pos, touchIndex);
		}
		for (int i = 0; i < m_controlMap.size(); i++) {
			m_controlMap[i]->Setup2DSpace();
			if (m_controlMap[i]->OnInputEvent(uMsg, pos, touchIndex))
				return true;
		}
		return false;
	}
	//VOID							DexianManager::SetToolTip(ToolTipData* t, const short_t* text, const Rect& rcInWindow, UIPopupType type)
	//{

	//}
	void							DexianManager::OnFrameMove(double fTime, float fElapsedTime)
	{
		if (RequestSort && m_controlMap.size() > 0) {
			RequestSort = false;
			qsort(&m_controlMap.at(0), m_controlMap.size(), sizeof(DexianControl*), SortDexianControl);
		}
		DexianControl* c = 0;
		for (int i = 0; i < m_controlMap.size(); i++) {
			m_controlMap[i]->FrameMove(fTime, fElapsedTime);
		}
	}
	VOID							DexianManager::Destroy()
	{
		for (int i = 0; i < m_controlMap.size(); i++) {
			delete m_controlMap[i];
		}
		m_controlMap.clear();
	}
	const char*						DexianManager::GetText(const char* ID)
	{
		for (int i = 0; i < STable.size(); i++) {
			if (STable[i]->ID == ID)
				return STable[i]->Value;
		}
		return ID;
	}
	void							DexianManager::LoadStringTable(const char* File)
	{
		Phantom::TableTxt* tt = Phantom::loadTable_s(File, ',');
		if (!tt)
			return;
		for (int i = 0; i < tt->GetNumofLine(); i++) {
			StringTable* st = new StringTable();
			st->ID = tt->getLineItemI(i, 0);
			st->Value = tt->getLineItemI(i, 1);
			STable.push_back(st);
		}
	}

};
