//this file is part of notepad++
//Copyright (C)2003 Don HO ( donho@altern.org )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef GOTILINE_DLG_H
#define GOTILINE_DLG_H

#include "DockingDlgInterface.h"
#include "resource.h"
#include "PreViewDlg.h"
#include <atlfile.h>

static WTL::CString GetTempPath()
{
	TCHAR szPath[MAX_PATH]; 
	GetTempPath(MAX_PATH, szPath);
	WTL::CString sTempPath = szPath;
	sTempPath += TEXT("nppmarkdown");
	return sTempPath;
}



static WTL::CString IniTempHtmlFile()
{
	WTL::CString sTempPath = GetTempPath();
  WTL::CString sTemplate;
	CreateDirectory(sTempPath, NULL);
	sTempPath += L"\\html.html";
  const char* html = "<html><body></body></html>";

  sTemplate = GetTempPath() + L"\\template.html";
  if (!PathFileExistsW(sTemplate))
  {
    CAtlFile file;
    file.Create(sTempPath, GENERIC_READ | GENERIC_WRITE, NULL, CREATE_ALWAYS);
    DWORD dwWrite = 0;
    file.Write(html, strlen(html), &dwWrite);
    sTemplate = sTempPath;
  }

  return sTemplate;
}


class DemoDlg : public DockingDlgInterface
{
public :
	DemoDlg() : DockingDlgInterface(IDD_PLUGINGOLINE_DEMO){};

    virtual void display(bool toShow = true) {
        DockingDlgInterface::display(toShow);
    };

	void setParent(HWND parent2set){
		_hParent = parent2set;
	};

	void create(tTbData * data, bool isRTL = false)
	{
		DockingDlgInterface::create(data, isRTL);
		dlg = new CPreviewDlg;
		WTL::CString sHtmlPath;
		//sHtmlPath = IniTempHtmlFile();
		WCHAR npp_path[MAX_PATH];
		GetModuleFileName(NULL, npp_path, MAX_PATH);
		WCHAR *p = wcsrchr(npp_path, L'\\');
		*p = 0;
		sHtmlPath.Format(L"%s\\plugins\\NppMarkdown.html", npp_path);
		dlg->Ini(sHtmlPath);
		dlg->Create(_hSelf, NULL);
	};

CPreviewDlg* dlg;
protected :
	virtual BOOL CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);


	

};

#endif //GOTILINE_DLG_H
