#include "DlgMinMap.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMinMap::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	//RegisterControlEvent("IDC_CLOSE", (PEVENT)&CDlgMinMap::OnBtnClose);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CDlgMinMap::OnInitDialog()
{
	// ----
	return CUIDialog::OnInitDialog();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMinMap::OnFrameMove(double fTime, float fElapsedTime)
{
	CUIDialog::OnFrameMove(fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMinMap::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUIDialog::OnFrameRender(mTransform, fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------