
// videostableDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "videostable.h"
#include "videostableDlg.h"
#include "afxdialogex.h"
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <gl/glut.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef PI
#define PI  3.141592653
#endif // !PI


using namespace cv;

//添加的全局变量 by TPS
CString global_video_path = (_T("")); //视频地址
CString global_text_path = (_T("")); //特征文件地址
boolean global_transformed = false; //视频是否已经经过处理
int global_video_position = 0; //视频播放到的位置
boolean global_pauseORnot = false; //视频是否暂停
vector<CameraData> global_CD;//来自文档的相机数据
vector<OtherData> global_OD;  //来自文档的其他数据
vector<PointData> global_PD; //来自文档的特征点数据

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CvideostableDlg 对话框



CvideostableDlg::CvideostableDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CvideostableDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CvideostableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPENVIDEO, m_openvideo);
	DDX_Control(pDX, IDC_VIDEOAD2, m_videoaddress);
	DDX_Control(pDX, IDC_OPENFILE, m_openfile);
	DDX_Control(pDX, IDC_FILEAD2, m_fileaddress);
	DDX_Control(pDX, IDC_SLIDER2_VIDEO, m_slider_video);
	DDX_Control(pDX, IDC_PLAYVIDEO, m_playvideo);
	DDX_Control(pDX, IDC_PAUSEVIDEO, m_pausevideo);
	DDX_Control(pDX, IDC_VIDEO_TRANS, m_video_trans);
	DDX_Control(pDX, IDC_IMAGE1, m_image1);
	DDX_Control(pDX, IDC_CAMERA1, m_camera1);
	DDX_Control(pDX, IDC_CAMERA1_1, m_camera1_1);
}

BEGIN_MESSAGE_MAP(CvideostableDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENVIDEO, &CvideostableDlg::OnBnClickedOpenvideo)
	ON_BN_CLICKED(IDC_OPENFILE, &CvideostableDlg::OnBnClickedOpenfile)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2_VIDEO, &CvideostableDlg::OnNMCustomdrawSlider2Video)
	ON_BN_CLICKED(IDC_PLAYVIDEO, &CvideostableDlg::OnBnClickedPlayvideo)
	ON_BN_CLICKED(IDC_PAUSEVIDEO, &CvideostableDlg::OnBnClickedPausevideo)
	ON_BN_CLICKED(IDC_VIDEO_TRANS, &CvideostableDlg::OnBnClickedVideoTrans)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CvideostableDlg 消息处理程序

BOOL CvideostableDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CRect rect_init;
	GetDlgItem(IDC_IMAGE1)->GetWindowRect(&rect_init);
	ScreenToClient(&rect_init);
	GetDlgItem(IDC_IMAGE1)->MoveWindow(rect_init.left, rect_init.top,400,400,true);

	GetDlgItem(IDC_CAMERA1)->GetWindowRect(&rect_init);
	ScreenToClient(&rect_init);
	GetDlgItem(IDC_CAMERA1)->MoveWindow(rect_init.left, rect_init.top,400,400,true);

	GetDlgItem(IDC_CAMERA1_1)->GetWindowRect(&rect_init);
	ScreenToClient(&rect_init);
	GetDlgItem(IDC_CAMERA1_1)->MoveWindow(rect_init.left, rect_init.top,400,400,true);

	//设置pixel格式
	static PIXELFORMATDESCRIPTOR pfd1 = {
		sizeof(LPPIXELFORMATDESCRIPTOR),1,
		PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,
		0,
		0,
		0,0,0,0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};
	static PIXELFORMATDESCRIPTOR pfd2 = pfd1;

	int pixelformat;
	hDC_camera1 = GetDlgItem(IDC_CAMERA1)->GetDC()->GetSafeHdc();
	if(!(pixelformat = ChoosePixelFormat(hDC_camera1 , &pfd1))){
		MessageBox(_T("1"));
		return false;
	}
	if(!SetPixelFormat(hDC_camera1, pixelformat, &pfd1)){
		MessageBox(_T("2"));
		return false;
	}

	hDC_camera1_1 = GetDlgItem(IDC_CAMERA1_1)->GetDC()->GetSafeHdc();
	if(!(pixelformat = ChoosePixelFormat(hDC_camera1_1 , &pfd2))){
		MessageBox(_T("1.1"));
		return false;
	}
	if(!SetPixelFormat(hDC_camera1_1, pixelformat, &pfd2)){
		MessageBox(_T("2.1"));
		return false;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CvideostableDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CvideostableDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
		CDialog::UpdateWindow();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CvideostableDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CvideostableDlg::OnBnClickedOpenvideo()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilter[] = _T("所有文件(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("avi"), NULL, 0, szFilter, this);
	CString strFilePath;

	if(IDOK == fileDlg.DoModal()){
		strFilePath = fileDlg.GetPathName();
		m_videoaddress.SetWindowTextW(strFilePath);
		global_video_path = strFilePath;
	}
}


void CvideostableDlg::OnBnClickedOpenfile()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilter[] = _T("所有文件(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("avi"), NULL, 0, szFilter, this);
	CString strFilePath;

	if(IDOK == fileDlg.DoModal()){
		strFilePath = fileDlg.GetPathName();
		m_fileaddress.SetWindowTextW(strFilePath);
		global_text_path = strFilePath;
	}
}


void CvideostableDlg::OnNMCustomdrawSlider2Video(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//int nPos = m_slider_video.GetPos();
	//global_video_position = nPos;
	//global_pauseORnot = true;
	*pResult = 0;
}


void CvideostableDlg::ResizeImage(Mat* img){
	//TODO 添加用于缩放图片的函数 by TPS
	int w = (*img).cols;
	int h = (*img).rows;
	int max = w>h?w:h;
	float s = (float)max/256.0f;
	int nw = (int)(w/s);
	int nh = (int)(h/s);
	Size si = Size(nw, nh);
	Mat img2 = Mat(si, CV_32S);
	resize(*img, img2, si);
	*img = img2;
	//int tlx = nw>nh?0:((int)((256-nw)/2));
	//int tly = nw>nh?((int)((256-nh)/2)):0;
	
}

void CvideostableDlg::ShowImage(Mat img, UINT ID){
	//TODO 添加用于显示图片到控件中的函数 by TPS
	CDC* pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect r;
	GetDlgItem(ID)->GetClientRect(&r);
	int rw = r.right - r.left;
	int rh = r.bottom - r.top;
	int iw = img.cols;
	int ih = img.rows;
	int tx = (int)(rw -iw)/2;
	int ty = (int)(rh -ih)/2;
	SetRect(r, tx, ty, tx+iw, ty+ih);

	//接下来将Mat转为CImage
	CImage cimg;
	int width    = img.cols;
	int height   = img.rows;
	int channels = img.channels();
	cimg.Destroy(); //clear
	cimg.Create(width, height,8*channels ); //默认图像像素单通道占用1个字节
	//copy values
	uchar* ps;
	uchar* pimg = (uchar*)cimg.GetBits(); //A pointer to the bitmap buffer
	int step = cimg.GetPitch();
	for (int i = 0; i < height; ++i)
	{
		ps = (img.ptr<uchar>(i));
		for ( int j = 0; j < width; ++j )
		{
			if ( channels == 1 ) //gray
			{
				*(pimg + i*step + j) = ps[j];
			}
			else if ( channels == 3 ) //color
			{
				for (int k = 0 ; k < 3; ++k )
				{
					*(pimg + i*step + j*3 + k ) = ps[j*3 + k];
				}			
			}
		}	
	}

	//利用CImage装入Picture控件中
	cimg.Draw(hDC, r);
	ReleaseDC(pDC);
}

void CvideostableDlg::OnBnClickedPlayvideo()
{
	// TODO: 在此添加控件通知处理程序代码
	if(global_text_path == ""){
		MessageBox(_T("尚未选择特征文件"));
		return;
	}else if(global_video_path == ""){
		MessageBox(_T("尚未选择视频文件"));
		return;
	}else if(global_transformed == false){
		MessageBox(_T("尚未对源视频进行处理"));
		return;
	};
	string tmp = string(CT2A(global_video_path));
	VideoCapture capture(tmp);
	if(!capture.isOpened()){   //这一段好像有错......
		MessageBox(_T("未能打开视频文件"));
		return;
	}
	global_pauseORnot = false;
	double rate = capture.get(CV_CAP_PROP_FPS);
	int frames = (int)capture.get(CV_CAP_PROP_FRAME_COUNT);
	int wait = 1000/rate;
	m_slider_video.SetRange(0,frames-1);
	m_slider_video.SetTicFreq(1);
	cvNamedWindow("video1");
	cvResizeWindow("video1",1,1);
	HWND hwnd1 = (HWND)cvGetWindowHandle("video1");
	HWND hParent1 = ::GetParent(hwnd1);
	HWND hwnd1_1 = ::FindWindow(_T("CvideostableDlg"),_T("videostable"));
	::SetParent(hwnd1,hwnd1_1);
	::ShowWindow(hParent1, SW_HIDE);
	while((global_pauseORnot == false) && (global_video_position < frames)){
		capture.set(CV_CAP_PROP_POS_FRAMES, global_video_position);
		Mat nimage;
		capture.read(nimage);
		ResizeImage(&nimage);
		ShowImage(nimage,IDC_IMAGE1);

		ShowSingleCamera(IDC_CAMERA1_1);

		m_slider_video.SetPos(global_video_position);
		waitKey(wait);
		global_video_position++;

	}
}


void CvideostableDlg::OnBnClickedPausevideo()
{
	// TODO: 在此添加控件通知处理程序代码
	global_pauseORnot = true;
}


void CvideostableDlg::OnBnClickedVideoTrans()
{
	// TODO: 在此添加控件通知处理程序代码
	if(global_text_path == ""){
		MessageBox(_T("尚未选择特征文件"));
		return;
	}else if(global_video_path == ""){
		MessageBox(_T("尚未选择视频文件"));
		return;
	}
	GetDataFromText(global_text_path);

	/*int size1 = global_CD.size();		//用于检查读入的数据的size
	int size2 = global_OD.size();
	CString s, s2;
	s.Format(_T("%d"), size1);
	s2.Format(_T("%d"), size2);
	s = s + s2;
	MessageBox(s);*/  

	ShowCamera(IDC_CAMERA1);

	global_transformed = true;
}


void CvideostableDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
	global_pauseORnot = true;
	global_video_position = pSlider->GetPos();

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CvideostableDlg::GetDataFromText(CString str){
	//用于读取CString路径里的文件内容
	using namespace std;
	ifstream thefile(str);
	if(!thefile){
		MessageBox(_T("读取特征文件失败"));
		exit(1);
	}
	char buffer[500];
	do{
		thefile.getline(buffer,500);
	}while(buffer[0]!='#'||buffer[1]!='t'); //这个匹配机制不太好....
	do{
		thefile.getline(buffer,500);
		if(buffer[0]=='#'){
			break;
		}

		CameraData cdtemp;
		OtherData odtemp;
		sscanf_s(buffer,
				"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
				&cdtemp.CameraPosition[0], &cdtemp.CameraPosition[1], &cdtemp.CameraPosition[2],
				&cdtemp.RotationAxisA[0], &cdtemp.RotationAxisA[1], &cdtemp.RotationAxisA[2], 
				&cdtemp.RotationAxisH[0], &cdtemp.RotationAxisH[1], &cdtemp.RotationAxisH[2], 
				&cdtemp.RotationAxisV[0], &cdtemp.RotationAxisV[1], &cdtemp.RotationAxisV[2], 
				&cdtemp.Radialdistortion[0], &cdtemp.Radialdistortion[1], 
				&cdtemp.PixelSize[0],&cdtemp.PixelSize[1],
				&cdtemp.ImageSize[0],&cdtemp.ImageSize[1],
				&odtemp.PPO[0],&odtemp.PPO[1],
				&odtemp.FL,
				&odtemp.HFofV,
				&odtemp.RotationAxisH0[0], &odtemp.RotationAxisH0[1], &odtemp.RotationAxisH0[2], 
				&odtemp.RotationAxisV0[0], &odtemp.RotationAxisV0[1], &odtemp.RotationAxisV0[2]
			);
		cdtemp.CameraPosition[0] = -cdtemp.CameraPosition[0];
		cdtemp.CameraPosition[1] = -cdtemp.CameraPosition[1];
		cdtemp.CameraPosition[2] = -cdtemp.CameraPosition[2];
		global_CD.push_back(cdtemp);
		global_OD.push_back(odtemp);

		thefile.getline(buffer,500);
	}while(buffer[0]=='#'&&buffer[1]=='t');

	//PointData pdtemp;  //对于3D特征点坐标的处理还没有写！
	thefile.close();
}

void CvideostableDlg::ShowCamera(UINT ID){
	//取得ID对应的绘画区域并在其间显示相机轨迹
	if(!(hRC_camera1 = wglCreateContext(hDC_camera1))){
		MessageBox(_T("3"));
		exit(3);
	}
	if(!wglMakeCurrent(hDC_camera1, hRC_camera1)){
		MessageBox(_T("4"));
		exit(4);
	}

	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	glViewport(0, 0, rect.Width(), rect.Height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, rect.Width()/rect.Height(), 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	DrawCamera();

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC_camera1);
}

void CvideostableDlg::DrawCamera(){
	//在指定区域内画出相机轨迹
	//int cdsize = global_CD.size();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(global_CD[0].CameraPosition[0]+(5*global_OD[0].FL),     global_CD[0].CameraPosition[1]+(5*global_OD[0].FL),    global_CD[0].CameraPosition[2]+(5*global_OD[0].FL),
		      global_CD[global_CD.size()-1].CameraPosition[0],       global_CD[global_CD.size()-1].CameraPosition[1],      global_CD[global_CD.size()-1].CameraPosition[2],
			  0, 1, 0);

	for (int i = 0; i < global_CD.size();)
	{
		glPushMatrix();
		i += (int)(global_CD.size()/30+1);
	}

	for (int i = 0; i < global_CD.size();)
	{

		double HorizIncre = global_CD[i].ImageSize[0] * global_CD[i].PixelSize[0];  //表示投影面一半的宽度mm
		double VertIncre = global_CD[i].ImageSize[1] * global_CD[i].PixelSize[1];  //投影面一半的高度mm

		glLoadIdentity();
		glPopMatrix();
		float m[16] = { global_OD[i].RotationAxisH0[0],global_OD[i].RotationAxisV0[0],global_CD[i].RotationAxisA[0],0,
						global_OD[i].RotationAxisH0[1],global_OD[i].RotationAxisV0[1],global_CD[i].RotationAxisA[1],0,
						global_OD[i].RotationAxisH0[2],global_OD[i].RotationAxisV0[2],global_CD[i].RotationAxisA[2],0,
						0 ,0, 0, 1
						};
		glMultMatrixf(m);

		//前面三条line为x y z轴
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(global_CD[i].CameraPosition[0], global_CD[i].CameraPosition[1], global_CD[i].CameraPosition[2]);
		glVertex3f(global_CD[i].CameraPosition[0]+10, global_CD[i].CameraPosition[1], global_CD[i].CameraPosition[2]);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(global_CD[i].CameraPosition[0], global_CD[i].CameraPosition[1], global_CD[i].CameraPosition[2]);
		glVertex3f(global_CD[i].CameraPosition[0], global_CD[i].CameraPosition[1]+10, global_CD[i].CameraPosition[2]);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(global_CD[i].CameraPosition[0], global_CD[i].CameraPosition[1], global_CD[i].CameraPosition[2]);
		glVertex3f(global_CD[i].CameraPosition[0], global_CD[i].CameraPosition[1], global_CD[i].CameraPosition[2]+10);
		glEnd();

		//后面5个平面用于构造四棱锥相机模型
		double point0[] ={global_CD[i].CameraPosition[0],                global_CD[i].CameraPosition[1],                  global_CD[i].CameraPosition[2]};
		double point1[] ={-1*HorizIncre+global_CD[i].CameraPosition[0],  VertIncre+global_CD[i].CameraPosition[1],        global_CD[i].CameraPosition[2]-global_OD[i].FL};
		double point2[] ={HorizIncre+global_CD[i].CameraPosition[0],     VertIncre+global_CD[i].CameraPosition[1],        global_CD[i].CameraPosition[2]-global_OD[i].FL};
		double point3[] ={-1*HorizIncre+global_CD[i].CameraPosition[0],  -1*VertIncre+global_CD[i].CameraPosition[1],     global_CD[i].CameraPosition[2]-global_OD[i].FL};
		double point4[] ={HorizIncre+global_CD[i].CameraPosition[0],     -1*VertIncre+global_CD[i].CameraPosition[1],     global_CD[i].CameraPosition[2]-global_OD[i].FL};

		glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.9f, 0.9f);
		glVertex3f(point0[0],point0[1],point0[2]);
		glVertex3f(point1[0],point1[1],point1[2]);
		glVertex3f(point2[0],point2[1],point2[2]);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(point0[0],point0[1],point0[2]);
		glVertex3f(point2[0],point2[1],point2[2]);
		glVertex3f(point4[0],point4[1],point4[2]);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(point0[0],point0[1],point0[2]);
		glVertex3f(point1[0],point1[1],point1[2]);
		glVertex3f(point3[0],point3[1],point3[2]);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.3f, 0.3f, 0.3f);
		glVertex3f(point0[0],point0[1],point0[2]);
		glVertex3f(point3[0],point3[1],point3[2]);
		glVertex3f(point4[0],point4[1],point4[2]);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.3f, 0.3f, 0.3f);
		glVertex3f(point1[0],point1[1],point1[2]);
		glVertex3f(point2[0],point2[1],point2[2]);
		glVertex3f(point4[0],point4[1],point4[2]);
		glVertex3f(point3[0],point3[1],point3[2]);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(point1[0],point1[1],point1[2]);
		glVertex3f(point2[0],point2[1],point2[2]);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(point2[0],point2[1],point2[2]);
		glVertex3f(point4[0],point4[1],point4[2]);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(point4[0],point4[1],point4[2]);
		glVertex3f(point3[0],point3[1],point3[2]);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(point1[0],point1[1],point1[2]);
		glVertex3f(point3[0],point3[1],point3[2]);
		glEnd();

		i += (int)(global_CD.size()/30+1);
	}

	SwapBuffers(hDC_camera1); 

	
}

void CvideostableDlg::ShowSingleCamera(UINT ID){
	if(!(hRC_camera1_1 = wglCreateContext(hDC_camera1_1))){
		MessageBox(_T("3.1"));
		exit(3.1);
	}
	if(!wglMakeCurrent(hDC_camera1_1, hRC_camera1_1)){
		MessageBox(_T("4.1"));
		exit(4.1);
	}

	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	glViewport(0, 0, rect.Width(), rect.Height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, rect.Width()/rect.Height(), 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(global_CD[0].CameraPosition[0]+(5*global_OD[0].FL),     global_CD[0].CameraPosition[1]+(5*global_OD[0].FL),    global_CD[0].CameraPosition[2]+(5*global_OD[0].FL),
		      global_CD[global_CD.size()-1].CameraPosition[0],       global_CD[global_CD.size()-1].CameraPosition[1],      global_CD[global_CD.size()-1].CameraPosition[2],
			  0, 1, 0);

	double HorizIncre = global_CD[global_video_position].ImageSize[0]*global_CD[global_video_position].PixelSize[0];  //表示投影面一半的宽度mm
	double VertIncre = global_CD[global_video_position].ImageSize[1]*global_CD[global_video_position].PixelSize[1];  //投影面一半的高度mm

		float m[16] = { global_OD[global_video_position].RotationAxisH0[0],global_OD[global_video_position].RotationAxisV0[0],global_CD[global_video_position].RotationAxisA[0],0,
						global_OD[global_video_position].RotationAxisH0[1],global_OD[global_video_position].RotationAxisV0[1],global_CD[global_video_position].RotationAxisA[1],0,
						global_OD[global_video_position].RotationAxisH0[2],global_OD[global_video_position].RotationAxisV0[2],global_CD[global_video_position].RotationAxisA[2],0,
						0 ,0, 0, 1
						};
		glMultMatrixf(m);

		//前面三条line为x y z轴
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(global_CD[global_video_position].CameraPosition[0], global_CD[global_video_position].CameraPosition[1], global_CD[global_video_position].CameraPosition[2]);
		glVertex3f(global_CD[global_video_position].CameraPosition[0]+10, global_CD[global_video_position].CameraPosition[1], global_CD[global_video_position].CameraPosition[2]);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(global_CD[global_video_position].CameraPosition[0], global_CD[global_video_position].CameraPosition[1], global_CD[global_video_position].CameraPosition[2]);
		glVertex3f(global_CD[global_video_position].CameraPosition[0], global_CD[global_video_position].CameraPosition[1]+10, global_CD[global_video_position].CameraPosition[2]);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(global_CD[global_video_position].CameraPosition[0], global_CD[global_video_position].CameraPosition[1], global_CD[global_video_position].CameraPosition[2]);
		glVertex3f(global_CD[global_video_position].CameraPosition[0], global_CD[global_video_position].CameraPosition[1], global_CD[global_video_position].CameraPosition[2]+10);
		glEnd();

		//后面5个平面用于构造四棱锥相机模型
		double point0[] ={global_CD[global_video_position].CameraPosition[0],                global_CD[global_video_position].CameraPosition[1],                  global_CD[global_video_position].CameraPosition[2]};
		double point1[] ={-1*HorizIncre+global_CD[global_video_position].CameraPosition[0],  VertIncre+global_CD[global_video_position].CameraPosition[1],        global_CD[global_video_position].CameraPosition[2]-global_OD[global_video_position].FL};
		double point2[] ={HorizIncre+global_CD[global_video_position].CameraPosition[0],     VertIncre+global_CD[global_video_position].CameraPosition[1],        global_CD[global_video_position].CameraPosition[2]-global_OD[global_video_position].FL};
		double point3[] ={-1*HorizIncre+global_CD[global_video_position].CameraPosition[0],  -1*VertIncre+global_CD[global_video_position].CameraPosition[1],     global_CD[global_video_position].CameraPosition[2]-global_OD[global_video_position].FL};
		double point4[] ={HorizIncre+global_CD[global_video_position].CameraPosition[0],     -1*VertIncre+global_CD[global_video_position].CameraPosition[1],     global_CD[global_video_position].CameraPosition[2]-global_OD[global_video_position].FL};

		glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.9f, 0.9f);
		glVertex3f(point0[0],point0[1],point0[2]);
		glVertex3f(point1[0],point1[1],point1[2]);
		glVertex3f(point2[0],point2[1],point2[2]);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(point0[0],point0[1],point0[2]);
		glVertex3f(point2[0],point2[1],point2[2]);
		glVertex3f(point4[0],point4[1],point4[2]);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(point0[0],point0[1],point0[2]);
		glVertex3f(point1[0],point1[1],point1[2]);
		glVertex3f(point3[0],point3[1],point3[2]);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.3f, 0.3f, 0.3f);
		glVertex3f(point0[0],point0[1],point0[2]);
		glVertex3f(point3[0],point3[1],point3[2]);
		glVertex3f(point4[0],point4[1],point4[2]);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.3f, 0.3f, 0.3f);
		glVertex3f(point1[0],point1[1],point1[2]);
		glVertex3f(point2[0],point2[1],point2[2]);
		glVertex3f(point4[0],point4[1],point4[2]);
		glVertex3f(point3[0],point3[1],point3[2]);
		glEnd();
	
	SwapBuffers(hDC_camera1_1); 

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC_camera1_1);
}