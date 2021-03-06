#include "RPGSkyUIGraph.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "Audio.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static Node3DUIGraph g_uiGraph;
// ----
UIGraph * UIGraph::m_pInstace = & g_uiGraph;
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Node3DUIGraph::frameUpdate()
{
	s_RPGSkyTextRender.OnFrameMove();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void* Node3DUIGraph::createTexture(const char * szTexture)
{
	unsigned long uTex = CRenderSystem::getSingleton().GetTextureMgr().RegisterTexture(szTexture,1);
	if (uTex<=0)
	{
		std::string strTexture = GetStyleMgr().getDir()+szTexture;
		uTex = CRenderSystem::getSingleton().GetTextureMgr().RegisterTexture(strTexture.c_str(),1);
	}
	return (void*)uTex;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Node3DUIGraph::releaseTexture(void * pTexture)
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Node3DUIGraph::setShader(const char* szShader)
{
	if (szShader==NULL||strlen(szShader)==0)
	{
		CRenderSystem::getSingleton().SetShader("ui");
	}
	else
	{
		CRenderSystem::getSingleton().SetShader(szShader);
	}
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Node3DUIGraph::drawSprite(const RECT& rcDest, void* pTexture, Color32 color, const RECT* rcSrc, const RECT* rcCenterSrc)
{
	GetGraphics().drawTex(rcDest, (int)pTexture, color, rcSrc, rcCenterSrc);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Node3DUIGraph::CalcTextRect(const wchar_t* wcsText, CRect<float> & rcDest)
{
	// ---
	// # (leo123) Why rcDest = rect and not directly to rcDest ? unless memory use.
	// ----
	RECT rect = rcDest.getRECT();
	// ----
	s_RPGSkyTextRender.calcUBBRect(wcsText, rect);
	// ----
	rcDest	= rect;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Node3DUIGraph::initDrawText()
{
	CRenderSystem & R = CRenderSystem::getSingleton();
	CRect<int>	rcViewport;
	Matrix		mView=R.getViewMatrix();
	Matrix		mProjection=R.getProjectionMatrix();
	// ----
	R.getViewport(rcViewport);
	// ----
	mView.Invert();
	// ----
	{
		float fZ = 1;
		// ---
		Matrix mNewWorld= mView * 
			Matrix::newTranslation(Vec3D(- fZ / mProjection._11 , fZ / mProjection._22,fZ)) *
			Matrix::newScale(Vec3D(fZ / mProjection._11 / rcViewport.right * 2.0f,- fZ / mProjection._22 / rcViewport.bottom * 2.0f,
			- fZ / mProjection._33 / rcViewport.bottom* 2.0f));
		// ----
		R.setWorldMatrix(mNewWorld);
	}
}

void Node3DUIGraph::DrawText(const wchar_t* wcsText, CUIStyle & style, int nIndex,const CRect<float> & rcDest, int nCount)
{
	Color32 color			= 0xFFFFFFFF;
	// ----
	if(color.a != 0)
	{
		ULONG uFormat			= 0;
		// ----
		// No need to draw fully transparent layers
		// ----
		s_RPGSkyTextRender.drawText(wcsText, rcDest.getRECT(), nCount, uFormat, color.c);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Node3DUIGraph::drawText(const wchar_t* wcsText, int cchText, const RECT & rc, UINT format, unsigned long color, RECT * prcRet)
{
	CRenderSystem & R = CRenderSystem::getSingleton();
	// ----
	//R.SetTextureFactor(color);
	// ----
	//R.SetTextureColorOP(1, TBOP_MODULATE, TBS_CURRENT,TBS_TFACTOR);
	//R.SetTextureAlphaOP(1 ,TBOP_MODULATE, TBS_CURRENT,TBS_TFACTOR);
	// ----
	s_RPGSkyTextRender.drawText(wcsText,rc, cchText, format, color);

}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Node3DUIGraph::initFont(const char* szFontFilename, size_t size)
{
	s_RPGSkyTextRender.load(szFontFilename);
	s_RPGSkyTextRender.setShadowBorder(1);
	// ----
	s_RPGSkyTextRender.setFontSize(size);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int Node3DUIGraph::GetFontSize()
{
	return s_RPGSkyTextRender.GetCharHeight();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Matrix Node3DUIGraph::setUIMatrix(const Matrix & mTransform, const CRect<float> & rc, const Vec3D & vTranslation, const Vec3D & vRotate)
{
	CRect<int>	rcViewport;
	// ----
	Matrix		mProjection;
	Matrix		mRotate;
	Matrix		mTrans;
	Matrix		mView;
	// ----
	float fZ	= 50;
	// ----
	CRenderSystem & R = CRenderSystem::getSingleton();
	// ----
	R.getViewport(rcViewport);
	// ----
	mView.unit();
	// ----
	R.setViewMatrix(mView);
	// ----
	mProjection.MatrixPerspectiveFovLH(PI/4,(float)rcViewport.right/(float)rcViewport.bottom,0.1f,100);
	// ----
	R.setProjectionMatrix(mProjection);
	// ----
	mRotate.rotate(vRotate);
	// ----
	mTrans.translation(vTranslation);
	// ----
	Matrix mWorld = mTransform * Matrix::newTranslation(Vec3D(rc.left + 0.5f * rc.getWidth(), rc.top + 0.5f * rc.getHeight(), 0))
		* mTrans *mRotate *
		Matrix::newTranslation(Vec3D( -0.5f * rc.getWidth(), -0.5f * rc.getHeight(),0));
	{
		Matrix mNewWorld = Matrix::newTranslation(Vec3D(- fZ / mProjection._11, fZ / mProjection._22 , fZ))
			* Matrix::newScale(Vec3D(fZ / mProjection._11 / rcViewport.right * 2.0f,- fZ / mProjection._22 / rcViewport.bottom
			* 2.0f, - fZ / mProjection._33 / rcViewport.bottom * 2.0f)) * mWorld;
		// ----
		R.setWorldMatrix(mNewWorld);
	}
	// ----
	return mWorld;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool Node3DUIGraph::scriptStringAnalyse(CScriptStringAnalysis & analysis, const wchar_t *wcsText)
{
	return s_RPGSkyTextRender.scriptStringAnalyse(analysis,wcsText);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Node3DUIGraph::playSound(const char * szFilename)
{
	GetAudio().playSound(szFilename);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CTextRender & Node3DUIGraph::getTextRender()
{
	// ----
	return s_RPGSkyTextRender;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------