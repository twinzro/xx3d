#include "ClientNetWork.h"
#include "MainRoot.h"
#include "RenderSystem.h"
#include "TextRender.h"
#include "IniFile.h"
#include "RegData.h"
#include "GlobalFunction.h"
#include "Audio.h"
#include "protocol.h"
#include "FileSystem.h"
#include "RenderNodeMgr.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// # (leo123) i think files patchs and names of files must be in defines

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define GAME_TITLE		"MU2"
#define GAME_TITLE_W	L"MU2"

//#include <windows.h>
#include <stdio.h>
#include <string.h>
#define LEN 1024
// 深度优先递归遍历目录中所有的文件
BOOL  DirectoryList(LPCSTR Path, int nRenderType)
{
	WIN32_FIND_DATA FindData;
	HANDLE hError;
	int FileCount = 0;
	char FilePathName[LEN];
	// 构造路径
	char FullPathName[LEN];
	strcpy(FilePathName, Path);
	strcat(FilePathName, "\\*.*");
	hError = FindFirstFile(FilePathName, &FindData);
	if (hError == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	while(::FindNextFile(hError, &FindData))
	{
		// 过虑.和..
		if (strcmp(FindData.cFileName, ".") == 0 
			|| strcmp(FindData.cFileName, "..") == 0 )
		{
			continue;
		}

		// 构造完整路径
		wsprintf(FullPathName, "%s\\%s", Path,FindData.cFileName);
		FileCount++;
		// 输出本级的文件
		//printf("\n%d  %s  ", FileCount, FullPathName);
		char szName[255];
		strcpy(szName,FindData.cFileName);
		int nLen = strlen(szName);
		if (nLen>3 && strcmp(szName+nLen-3,".fx")==0)
		{
			szName[nLen-3]=0;
			CShader* pShader = CRenderSystem::getSingleton().registerShader(szName,FullPathName);
			if (pShader)
			{
				pShader->setRenderType(nRenderType);
			}
		}
// 		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
// 		{
// 			printf("<Dir>");
// 			DirectoryList(FullPathName);
// 		}



	}
	return 0;
}

CMainRoot::CMainRoot()
{
	#ifdef _DEBUG
	if(LoadRenderSystemLibrary("Plugins\\RenderSystemDX9d.dll") == true)
	#else
	if(LoadRenderSystemLibrary("Plugins\\RenderSystemDX9.dll") == true)
	#endif
	{
		// # InitLua
		//InitLua(CUICombo::m_pLuaState);
		// ----
		// UI Theme
		std::string strThemeFilename	= "Data\\UI\\UIStyle.xml";
		// ----
		GetStyleMgr().Create(strThemeFilename.c_str());
		// ----
		// # UI Font
		std::string strLanguage			= IniGetStr("GameUI.cfg", "UIConfig", "language");
		std::string strFont1			= IniGetStr("Font.cfg",strLanguage.c_str(), "font1");
		// ----
		UIGraph::getInstance().initFont(strFont1.c_str(),13);
		// ----
		// # Create UI
		std::string strUIFilename		= IniGetStr("GameUI.cfg", "UIConfig", "file");
		// ----
		CDlgMain::getInstance().create(strUIFilename.c_str(), "IDD_MAIN");
		// ----
		// # Load UI Language
		setUILanguage(strLanguage);
		// ----
		// # (leo123) : i think next variables must be in register
		// # Create Render System Window
		int nWidth						= IniGetInt("Game.cfg", "display", "width", 800);
		int nHeight						= IniGetInt("Game.cfg", "display", "height", 500);
		// ----
		CreateRenderWindow(GAME_TITLE_W, nWidth, nHeight);
		// # Common Materials
		//CRenderNodeMgr::getInstance().loadRenderNode("Data\\Common.mat.csv",NULL);
		//CRenderNodeMgr::getInstance().loadRenderNode("EngineRes\\Common.mat.csv",NULL);
		//DirectoryList("EngineRes\\shader",MATERIAL_GEOMETRY);
		DirectoryList("EngineRes\\shader\\geometry",RF_GEOMETRY);
		DirectoryList("EngineRes\\shader\\glow",RF_GLOW);
		DirectoryList("EngineRes\\shader\\bump",RF_BUMP);
		DirectoryList("EngineRes\\shader\\post-processing",RF_GEOMETRY);
		DirectoryList("EngineRes\\shader\\ui",RF_GEOMETRY);
		//DirectoryList("Data\\shader\\");
		// ----
		// # NetWork
		NETWORK.SetHWND	(m_hWnd);
		NETWORK.SetWinMsgNum(WM_GM_JOIN_CLIENT_MSG_PROC);
		NETWORK.SetProtocolCore(ProtocolCore);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CMainRoot::~CMainRoot()
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMainRoot::setUILanguage(const std::string & strLanguage)
{
	std::string strUIFilename			= IniGetStr("GameUI.cfg", "UIConfig", "file");
	std::string strStringFilename		= ChangeExtension(strUIFilename, "String" + strLanguage + ".ini");
	// ----
	CDlgMain::getInstance().loadString(strStringFilename.c_str());
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMainRoot::OnFrameMove(double fTime, float fElapsedTime)
{
	static double s_LiveTime = 0.0f;
	// ----
	s_LiveTime				+= fElapsedTime;
	// ----
	if(s_LiveTime > 10.0f)
	{
		s_LiveTime = 0.0f;
		// ----
		CSLiveClient();
	}
	// ----
	CRoot::OnFrameMove		(fTime, fElapsedTime);
	CDlgMain::getInstance().OnFrameMove	(fTime, fElapsedTime);
	// ----
	GetAudio().FrameMove	(fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMainRoot::OnFrameRender(double fTime, float fElapsedTime)
{
	if(m_pRenderSystem->BeginFrame() == true)
	{
		m_pRenderSystem->ClearBuffer(true, true, 0x0);
		// ----
		CDlgMain::getInstance().OnFrameRender(Matrix::UNIT, fTime, fElapsedTime);
		// ----
		m_pRenderSystem->EndFrame();
	}
	// ----
	CRoot::OnFrameRender(fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CMainRoot::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bNoFurtherProcessing = false;
	// -----
	if (uMsg == WM_GM_JOIN_CLIENT_MSG_PROC)
	{
		switch( lParam & 0xFFFF & 0xFFFF)
		{
			case 1:
			{
				NETWORK.DataRecv();
			}
			break;

			case 2:
			{
				NETWORK.FDWRITE_MsgDataSend();
			}
			break;
		}
	}
	// ----
	bNoFurtherProcessing = CDlgMain::getInstance().MsgProc(hWnd, uMsg, wParam, lParam);
	// ----
	return bNoFurtherProcessing;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------