#if !defined(AFX_H_OctopusScope)
#define AFX_H_OctopusScope

#include "stdafx.h"
#include "afxwin.h"
#include <process.h>
#include "cport.h"

class COctopusScope : public CDialog
{

public:
	
	COctopusScope(CWnd* pParent = NULL);
	virtual ~COctopusScope();
	enum { IDD = IDC_SCOPE };
	
	void Z_StepUp( void );
	void Z_StepDown( void );
	void BrightField( int volt );
	void ChangePath( void );
	void Close( void );
	void EpiFilterWheel( int cube );
	void BrightFieldFilterWheel( int filter );

protected:

	bool   Scope_initialized;
	bool   Path_camera;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnNMCustomdrawExecute( NMHDR* pNMHDR, LRESULT* pResult );

	DECLARE_MESSAGE_MAP()

	CPort* pPortScope;

	bool WriteScope( CString str );
	CString ReadScope( u16 CharsToRead );

	bool Init( void );

	CStatic m_Pos;

	int	m_Radio_S;
	int m_Radio_OBJ;
	int m_Radio_FW;
	int m_Radio_BFW;

	bool working;
	double position_now;
  	u16 stepsize_10nm;
    u32 old_position;

	void DisplayBFIntensity( int volt );
	void DisplayPosition( void );

	int  GetObj( void );
	int  GetFW( void );
	int  GetBFW( void );

	
	CStatic     m_Slider_Setting;
	CSliderCtrl m_Slider;
	CString     m_Slider_Setting_String;

	bool first_tick;

public:

	afx_msg void OnObjectiveStepSize1();
	afx_msg void OnObjectiveStepSize2();
	afx_msg void OnObjectiveStepSize3();
	afx_msg void OnObjectiveStepSize4();

	afx_msg void OnObj_1();
	afx_msg void OnObj_2();
	afx_msg void OnObj_3();
	afx_msg void OnObj_4();
	afx_msg void OnObj_5();
	afx_msg void OnObj_6();

	afx_msg void OnFW_1();
	afx_msg void OnFW_2();
	afx_msg void OnFW_3();
	afx_msg void OnFW_4();
	afx_msg void OnFW_5();
	afx_msg void OnFW_6();

	afx_msg void OnBFW_1();
	afx_msg void OnBFW_2();
	afx_msg void OnBFW_3();
	afx_msg void OnBFW_4();
	afx_msg void OnBFW_5();
	afx_msg void OnBFW_6();

	void Objective( int obj );

	afx_msg void OnTimer(UINT nIDEvent);

	afx_msg void ObjESC();
	afx_msg void ObjRTN();

	void GetPosition( void );

};

#endif