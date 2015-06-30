#pragma once
#include "XBrowser.h"
#include "resource.h"
#include <atlmisc.h>
#include "dockingResource.h"
#include "PluginDefinition.h"
#include <iostream>
#include <sstream>
#include "markdown.h"
#include <locale>
#include <codecvt>

using namespace std;

extern CAppModule _Module;
extern NppData nppData;

#include "hoedown/html.h"

class CPreviewDlg : public CAxDialogImpl<CPreviewDlg>
{
public:
	enum { IDD = IDD__PREVIEW_DLG };

	BEGIN_MSG_MAP(CPreviewDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDC_BUTTON_PREVIEW, OnPreviewCmd)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
	//	CenterWindow(GetParent());

		RECT rc = {0};
		::GetWindowRect(GetParent(), &rc);
		MoveWindow(&rc, FALSE);
		html.Create(NULL, url_, m_hWnd, rc);
		return TRUE;
	}

	void SetBodyText(ATL::CString str)
	{
		//ATL::CString aa = html.GetCharSet();
		////MessageBox(aa);
		//html.SetCharSet(L"");
		html.SetHTMLBody(str);
	}

	void Ini(WTL::CString url)
	{
		url_ = url;
	}

	LRESULT OnPreviewCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		
		Tans();
		return 0;
	}

	void Tans()
	{
		int which = -1;
		::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
		if (which == -1)
			return ;
		HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
		int len = ::SendMessage(curScintilla, SCI_GETTEXTLENGTH, 0, 0);
		len += 1;
		char* buf = new char[len];
		::SendMessage(curScintilla, SCI_GETTEXT, len, (LPARAM)buf);
		buf[len-1] = '\0';

#if 0
		string mdString = buf;
		markdown::Document doc;
		doc.read(mdString);
		std::ostringstream stream;
		doc.write(stream);
		std::string aHtml = stream.str();
#else
		hoedown_renderer *renderer = hoedown_html_renderer_new(HOEDOWN_HTML_USE_XHTML, 0);
		hoedown_extensions extensions = (hoedown_extensions)(HOEDOWN_EXT_TABLES); // Parse PHP-Markdown style tables.
		extensions = (hoedown_extensions)(extensions|HOEDOWN_EXT_FENCED_CODE); // Parse fenced code blocks.
		extensions = (hoedown_extensions)(extensions|HOEDOWN_EXT_AUTOLINK); // Automatically turn safe URLs into links.
		extensions = (hoedown_extensions)(extensions|HOEDOWN_EXT_NO_INTRA_EMPHASIS); // Disable emphasis_between_words.
		extensions = (hoedown_extensions)(extensions|HOEDOWN_EXT_STRIKETHROUGH); // Parse ~~stikethrough~~ spans.
		hoedown_document *document = hoedown_document_new(renderer, extensions, 16);
		hoedown_buffer *html = hoedown_buffer_new(16);
		hoedown_document_render(document, html, (const uint8_t *)buf, len);
		//printf("%.*s", html->size, html->data);
		std::string aHtml = (const char *)html->data;
		hoedown_buffer_free(html);
		hoedown_document_free(document);
		hoedown_html_renderer_free(renderer);
#endif

    USES_CONVERSION;
    int codepage = (int)::SendMessage(curScintilla, SCI_GETCODEPAGE, 0, 0);

    wstring wHtml;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    switch (codepage)
    {
    case (int)SC_CHARSET_ANSI:
    case (int)SC_CHARSET_GB2312:
    case (int)936: // GBK
      wHtml = A2W(aHtml.c_str());
      break;
    case (int)SC_CP_UTF8:
      wHtml = myconv.from_bytes(aHtml);
      break;
      // ADD YOUR ENCODING HERE ...
    default:
      wHtml = _T("Not supported encoding (UTF8/GB2312/GBK/ANSI)");
      break;
    }

    SetBodyText(wHtml.c_str());

		delete[] buf;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}

	void ReSize(int x, int y)
	{
		int btnSize = 20;
		int btnwidth = x/2 - 5;
		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = x;
		rc.bottom = y;

		MoveWindow(&rc, TRUE);
		rc.bottom = y - btnSize;
		html.MoveWindow(&rc, FALSE);

		HWND hwndCheck = GetDlgItem(IDC_CHECK_LIVE_PREVIEW);

		rc.left = 0;
		rc.top = y - btnSize;
		rc.right = btnwidth;
		rc.bottom = btnSize;
		::MoveWindow(hwndCheck, rc.left, rc.top, rc.right, rc.bottom, TRUE);

		HWND hwndBtnPreview = GetDlgItem(IDC_BUTTON_PREVIEW);

		rc.left = x/2;
		rc.top = y - btnSize;
		rc.right = btnwidth;
		rc.bottom = btnSize;
		::MoveWindow(hwndBtnPreview, rc.left, rc.top, rc.right, rc.bottom, TRUE);
	}


	WTL::CString url_;
	CXHtmlView html;
};
