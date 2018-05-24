#if !defined(AFX_H_COctopusGrating)
#define AFX_H_OctopusGrating

#include "stdafx.h"
#include "OctopusGlobals.h"
#include "Pi_gcs2_dll.h"

class COctopusGrating : public CDialog
{

public:
	
	COctopusGrating(CWnd* pParent = NULL);
	virtual ~COctopusGrating();
	enum { IDD = IDC_GRAT };

	void MoveFwd( void );
	void MoveBack( void );
	void MoveRel( double dist );
	void MoveTo( double pos );
	double GetPosition( void ) { return c_position_microns; };
	double GetMiddle( void )   { return c_middle_microns;   };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMCustomdrawExecute( NMHDR* pNMHDR, LRESULT* pResult );

	DECLARE_MESSAGE_MAP()

	CStatic m_Pos;
	int	m_Radio_S;
    bool first_tick;

	double c_position_microns;
	double c_save_microns; 
	double c_target_microns;
	double c_stepsize_microns;
	double c_middle_microns;

	char axis[17];
	int ID;
	bool connected;
	bool initialized;
	
	CString debug;
	CString result;

	void   ShowPosition( void );
	void   UpdatePosition( void );

public:

	afx_msg void OnStepSize1();
	afx_msg void OnStepSize2();
	afx_msg void OnStepSize3();
	afx_msg void OnStepSize4();
	afx_msg void OnStepSize5();

	afx_msg void OnSave();
	afx_msg void OnSaveGoTo();
	afx_msg void Center();

	virtual BOOL OnInitDialog();

	afx_msg void OnKillfocusGeneral();
	afx_msg void OnTimer(UINT nIDEvent);

	int Initialize( void );
	void Close( void );

};

#endif