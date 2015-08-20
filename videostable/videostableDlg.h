
// videostableDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "opencv2\core\core.hpp"
#include "afxcmn.h"
#include "datafromtext.h"


// CvideostableDlg 对话框
class CvideostableDlg : public CDialogEx
{
// 构造
public:
	CvideostableDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_VIDEOSTABLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	//自己添加的
	HGLRC hRC_camera1;
	HDC hDC_camera1;
	HGLRC hRC_camera1_1;
	HDC hDC_camera1_1;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenvideo();
	CButton m_openvideo;
	CStatic m_videoaddress;
	afx_msg void OnBnClickedOpenfile();
	CButton m_openfile;
	CStatic m_fileaddress;
	CSliderCtrl m_slider_video;
	afx_msg void OnNMCustomdrawSlider2Video(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_playvideo;
	afx_msg void OnBnClickedPlayvideo();
	CButton m_pausevideo;
	afx_msg void OnBnClickedPausevideo();
	CButton m_video_trans;
	afx_msg void OnBnClickedVideoTrans();
	CStatic m_image1;
	
	void ResizeImage(cv::Mat*);
	void ShowImage(cv::Mat, UINT);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	void GetDataFromText(CString);
	CStatic m_camera1;

	void ShowCamera(UINT);
	void DrawCamera();
	CStatic m_camera1_1;

	void ShowSingleCamera(UINT);
};
