
#if !defined(AFX_H_OctopusPolarizer)
#define AFX_H_OctopusPolarizer

#include "stdafx.h"
#include "OctopusGlobals.h"
#include "APTMotor.h"

class COctopusPolarizer : public CDialog
{

public:
	
	COctopusPolarizer(CWnd* pParent = NULL);
	virtual ~COctopusPolarizer();
	enum { IDD = IDC_STAGE_POL };

	void MoveRel( float deg );
	void MoveCW( void );
	void MoveCCW( void );
	void ScanStartStop( void );

protected:

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMCustomdrawExecute( NMHDR* pNMHDR, LRESULT* pResult );

	
	CBitmap m_bmp_yes;
	CBitmap m_bmp_no;

	DECLARE_MESSAGE_MAP()

	CStatic m_Pos;
	CStatic m_Step;
	CStatic m_status_rot;

	float save_pos_deg_T;

	UINT scan_number_of_steps;
	UINT scan_number_of_steps_z;
	u16 scan_current_step;
	u16 scan_current_step_z;
	
	float stepsize_deg_T;
	float scan_step_size_deg;
	float scan_step_size_microns_z;
	float scan_start_deg_T;

	void GetPosition( void );
	void ShowPosition( void );
	void ShowStep( void );
	void UpdatePosition( void );

	bool Initialized;
	virtual BOOL OnInitDialog();

	afx_msg void ScanSetStart( void );

	void ScanDone( void );
	void ScanStep( void );

    void MoveAE_T( float deg, bool wait );

	double FrameConvert( double deg );

	UINT TIMER_Polarizer_Pos;
	UINT TIMER_Polarizer_Scan;

	CButton m_ctl_Rot_CheckBox;
	CButton m_ctl_Rot_CheckBox2;
	CButton m_ctl_Rot_CheckBox3;

	bool PolRot_Both;
	bool PolRot_Only_Pol;
	bool PolRot_Only_Z;

public:

	afx_msg void SavePositionSet();
	afx_msg void SavePositionGoTo();

	afx_msg void OnKillfocusGeneral();
	afx_msg void OnTimer(UINT nIDEvent);

	afx_msg void OnBnClickedRot();
	afx_msg void OnBnClickedRot2();
	afx_msg void OnBnClickedRot3();

	void Initialize( void );
	void Close( void );
	afx_msg void Zero();
	CMgmotorctrl1 Polarizer_T;

};

#endif