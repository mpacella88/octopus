
#include "stdafx.h"
#include "Octopus.h"
#include "APTMotor.h"
#include "OctopusPolarizer.h"
#include "OctopusCameraDlg.h"
#include "OctopusObjectivePiezo.h"

extern COctopusGlobals B;
extern COctopusPolarizer*       glob_m_pPolarizer;
extern COctopusCamera*          glob_m_pCamera;
extern COctopusObjectivePiezo*  glob_m_pObjPiezo;

COctopusPolarizer::COctopusPolarizer(CWnd* pParent)
	: CDialog(COctopusPolarizer::IDD, pParent)
{    

	B.position_degrees_T	 =   0.0;
	Initialized				 = false;
	B.POL_loaded			 = false;
	B.POL_scan_in_progress   = false;
	
	scan_number_of_steps	 =    40;
	scan_current_step		 =     0;
	scan_step_size_deg		 =   5.0;

	scan_number_of_steps_z   =     1;
	scan_current_step_z      =     1;
	scan_step_size_microns_z =   5.0;

	save_pos_deg_T			 =  51.3;
	stepsize_deg_T			 =   4.0;
	scan_start_deg_T		 =  51.3;

	PolRot_Both				 = false;
	PolRot_Only_Z			 = false;			
	PolRot_Only_Pol			 =  true;

	VERIFY(m_bmp_no.LoadBitmap(IDB_NO));
	VERIFY(m_bmp_yes.LoadBitmap(IDB_YES));

	if( Create(COctopusPolarizer::IDD, pParent) ) 
		ShowWindow( SW_SHOW );

}

BOOL COctopusPolarizer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowPos(NULL, 9, 430, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	Initialize();

	if (PolRot_Both) 
		CheckDlgButton(IDC_POL_ROT, BST_CHECKED);
	else
		CheckDlgButton(IDC_POL_ROT, BST_UNCHECKED);

	if (PolRot_Only_Pol) 
		CheckDlgButton(IDC_POL_ROT2, BST_CHECKED);
	else
		CheckDlgButton(IDC_POL_ROT2, BST_UNCHECKED);

	if (PolRot_Only_Z) 
		CheckDlgButton(IDC_POL_ROT3, BST_CHECKED);
	else
		CheckDlgButton(IDC_POL_ROT3, BST_UNCHECKED);

	return TRUE;
}

void COctopusPolarizer::OnTimer( UINT nIDEvent ) 
{
	if( nIDEvent == TIMER_POL_POS ) 
	{
		if( Initialized && !B.POL_scan_in_progress ) 
		{
			GetPosition();
			ShowPosition();
		}
	}

	if( nIDEvent == TIMER_POL_SCAN && B.POL_scan_in_progress ) 
	{
		ScanStep();
	}

	CDialog::OnTimer(nIDEvent);
}

void COctopusPolarizer::DoDataExchange(CDataExchange* pDX) 
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_MGMOTORCTRL1, Polarizer_T);

	DDX_Control( pDX, IDC_POL_REP_STEP,	    m_Step); // which step?
	DDX_Control( pDX, IDC_POL_REP_POS, 	    m_Pos);  // current position

	//size of steps
	DDX_Text(   pDX, IDC_POL_SCAN_STEP_SIZE, scan_step_size_deg);
	DDV_MinMaxDouble(pDX, scan_step_size_deg, 0.001, 90.0);

	//number of steps
	DDX_Text(   pDX, IDC_POL_SCAN_STEP_NUMBER, scan_number_of_steps);
	DDV_MinMaxUInt(pDX, scan_number_of_steps, 1, 1000);

	//size of steps
	DDX_Text(   pDX, IDC_POL_SCAN_STEP_SIZE_Z, scan_step_size_microns_z);
	DDV_MinMaxDouble(pDX, scan_step_size_microns_z, 0.001, 5.0);

	//number of steps
	DDX_Text(   pDX, IDC_POL_SCAN_STEP_NUMBER_Z, scan_number_of_steps_z);
	DDV_MinMaxUInt(pDX, scan_number_of_steps_z, 0, 200);

	DDX_Control(pDX, IDC_POL_ROT,  m_ctl_Rot_CheckBox);
	DDX_Control(pDX, IDC_POL_ROT2, m_ctl_Rot_CheckBox2);
	DDX_Control(pDX, IDC_POL_ROT3, m_ctl_Rot_CheckBox3);

	DDX_Control(pDX, IDC_POL_ROTONOFF_BMP,  m_status_rot);

}  

BEGIN_MESSAGE_MAP(COctopusPolarizer, CDialog)
	
	ON_BN_CLICKED(IDC_POL_CW,			      MoveCW)
	ON_BN_CLICKED(IDC_POL_CCW,			      MoveCCW)
	ON_BN_CLICKED(IDC_POL_SAVE,				  SavePositionSet)
	ON_BN_CLICKED(IDC_POL_SAVE_GOTO,		  SavePositionGoTo)

	ON_BN_CLICKED(IDC_POL_SCAN,		          ScanStartStop)
	ON_BN_CLICKED(IDC_POL_SCAN_SET_START,     ScanSetStart)
	ON_BN_CLICKED(IDC_POL_ZERO,			      Zero)

	ON_EN_KILLFOCUS(IDC_POL_SCAN,			    OnKillfocusGeneral)
	ON_EN_KILLFOCUS(IDC_POL_SCAN_STEP_SIZE,     OnKillfocusGeneral)
	ON_EN_KILLFOCUS(IDC_POL_SCAN_STEP_NUMBER,   OnKillfocusGeneral)
	ON_EN_KILLFOCUS(IDC_POL_SCAN_STEP_SIZE_Z,   OnKillfocusGeneral)
	ON_EN_KILLFOCUS(IDC_POL_SCAN_STEP_NUMBER_Z, OnKillfocusGeneral)
	
	ON_BN_CLICKED(IDC_POL_ROT,                   OnBnClickedRot)
	ON_BN_CLICKED(IDC_POL_ROT2,                  OnBnClickedRot2)
	ON_BN_CLICKED(IDC_POL_ROT3,                  OnBnClickedRot3)
	ON_WM_TIMER()

END_MESSAGE_MAP()

void COctopusPolarizer::OnKillfocusGeneral() { UpdateData( true ); }
COctopusPolarizer::~COctopusPolarizer() {  }

void COctopusPolarizer::MoveCW(  void ) { MoveRel( +1 * stepsize_deg_T ); }
void COctopusPolarizer::MoveCCW( void ) { MoveRel( -1 * stepsize_deg_T ); }

void COctopusPolarizer::MoveRel( float deg ) 
{ 
	Polarizer_T.MoveRelativeEnc( 0, deg, 0.0, 0, true ); 
}

void COctopusPolarizer::MoveAE_T( float deg, bool wait ) 
{ 
	Polarizer_T.MoveAbsoluteEnc( 0, FrameConvert(deg), 0.0, 0, wait ); 
}

void COctopusPolarizer::Initialize( void ) 
{
	Polarizer_T.SetHWSerialNum(83819652);
	Polarizer_T.StartCtrl();
	Polarizer_T.SetRotStageModes( 0, 3, 1 );
	Polarizer_T.MoveHome( 0, false );
	Polarizer_T.SetVelParams( 0, 5.0, 20.0, 20.0 );
	Polarizer_T.SetBLashDist( 0, 0.0 );

	MoveAE_T( scan_start_deg_T, true );

	Initialized  = true;
	B.POL_loaded = true;

	SetTimer( TIMER_POL_POS, 500, NULL ); //this is for position display
}

void COctopusPolarizer::OnBnClickedRot()
{
    int l_ChkBox = m_ctl_Rot_CheckBox.GetCheck();
	
	if (l_ChkBox == 0) 
		PolRot_Both = false;
	else if (l_ChkBox == 1) 
	{
		PolRot_Both     =  true;
		PolRot_Only_Pol = false; CheckDlgButton(IDC_POL_ROT2, BST_UNCHECKED);
		PolRot_Only_Z   = false; CheckDlgButton(IDC_POL_ROT3, BST_UNCHECKED);
	}
	UpdateData( false );
}

void COctopusPolarizer::OnBnClickedRot2()
{
    int l_ChkBox = m_ctl_Rot_CheckBox2.GetCheck();
	
	if (l_ChkBox == 0) 
		PolRot_Only_Pol = false;
	else if (l_ChkBox == 1) 
	{
		PolRot_Only_Pol =  true;
		PolRot_Both     = false; CheckDlgButton(IDC_POL_ROT , BST_UNCHECKED);
		PolRot_Only_Z   = false; CheckDlgButton(IDC_POL_ROT3, BST_UNCHECKED);
	}
	UpdateData( false );
}

void COctopusPolarizer::OnBnClickedRot3()
{
	//Z only
    int l_ChkBox = m_ctl_Rot_CheckBox3.GetCheck();
	
	if (l_ChkBox == 0) 
		PolRot_Only_Z = false;
	else if (l_ChkBox == 1) 
	{
		PolRot_Only_Pol  = false; CheckDlgButton(IDC_POL_ROT2, BST_UNCHECKED);
		PolRot_Both      = false; CheckDlgButton(IDC_POL_ROT , BST_UNCHECKED);
		PolRot_Only_Z    =  true;
	}
	UpdateData( false );
}
void COctopusPolarizer::GetPosition( void ) 
{ 
	float pos;
	Polarizer_T.GetPosition( 0, (float *)&pos ); 
	B.position_degrees_T = FrameConvert( pos );
}

double  COctopusPolarizer::FrameConvert( double deg )
{
	if ( deg > 359.999 )
		return (deg - 360.0);
	else if ( deg < 0.0 )
		return (360.0 + deg);
	else 
		return deg;
}

void COctopusPolarizer::Close( void ) 
{ 
	Polarizer_T.StopCtrl();
	Initialized = false; 
}

void COctopusPolarizer::Zero( void ) 
{	
	MoveAE_T( 0.0, false ); 
}

void COctopusPolarizer::SavePositionGoTo( void ) 
{	
	MoveAE_T( save_pos_deg_T, false );
}

void COctopusPolarizer::SavePositionSet( void ) 
{	
	GetPosition(); 
	save_pos_deg_T = B.position_degrees_T;
}

void COctopusPolarizer::ShowPosition( void )
{
	CString str;

	if( IsWindowVisible() ) 
	{
		str.Format(_T("Current: %.1f\nSave: %.1f\nScanStart: %.1f\n"), \
			B.position_degrees_T, save_pos_deg_T, scan_start_deg_T);
		m_Pos.SetWindowText( str );
	}
}

void COctopusPolarizer::ShowStep( void )
{
	CString str;

	if( IsWindowVisible() ) 
	{
		str.Format(_T("Pol step: %d of %d\nZ-plane: %d of %d"), \
			scan_current_step, scan_number_of_steps, scan_current_step_z, scan_number_of_steps_z);
		m_Step.SetWindowText( str );
	}
}

BOOL COctopusPolarizer::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int id = LOWORD(wParam);     // Notification code
	if( id == 2 ) return FALSE;  // Trap ESC key
	if( id == 1 ) return FALSE;  // Trap RTN key
    return CDialog::OnCommand(wParam, lParam);
}

void COctopusPolarizer::ScanSetStart( void )
{
	GetPosition(); 
	scan_start_deg_T = B.position_degrees_T;
}

void COctopusPolarizer::ScanStartStop( void )
{
	if ( B.POL_scan_in_progress )
	{
		//stop scan
		ScanDone();
		m_status_rot.SetBitmap( m_bmp_no );
	}
	else
	{   
		//go to beginning, if we are going to rotate
		if ( PolRot_Both || PolRot_Only_Pol )
		{
			MoveAE_T( scan_start_deg_T, true );
		} 
		
		//see if camera is on
		if ( B.Camera_Thread_running ) 
		{
			AfxMessageBox( _T("Please stop the camera, and then try again.") );
			return;
		}

		scan_current_step       = 1;
		scan_current_step_z     = 1;
		B.POL_scan_in_progress  = true;

		GetPosition();
		ShowPosition();
		ShowStep();

		m_status_rot.SetBitmap( m_bmp_yes );

		TIMER_Polarizer_Scan = SetTimer( TIMER_POL_SCAN, 100, NULL );
	}
}

void COctopusPolarizer::ScanStep( void )
{
	
	if ( glob_m_pCamera == NULL ) return;
	
	KillTimer( TIMER_Polarizer_Scan );

	if( PolRot_Only_Pol )
	{
		if( scan_current_step <= scan_number_of_steps )
		{
			//move the polarizers
			//where are we?
			GetPosition();
			ShowPosition();
			ShowStep();

			if ( glob_m_pCamera != NULL )
			{
				//take a triple of pictures	
				glob_m_pCamera->TakePicture(); 
				glob_m_pCamera->TakePicture(); 
				glob_m_pCamera->TakePicture(); 
			}

			float target = scan_start_deg_T + (scan_current_step * scan_step_size_deg);
			
			if ( target > 360.00 ) target = target - 360.0;
			
			MoveAE_T( target, true ); 
		
			scan_current_step++;
			
			TIMER_Polarizer_Scan = SetTimer( TIMER_POL_SCAN, 200, NULL );
		}
		else
		{
			ScanDone();
		}
	}
	else if( PolRot_Both )
	{
		if( scan_current_step <= scan_number_of_steps )
		{
			//move the polarizers
			//where are we?
			GetPosition();
			ShowPosition();
			ShowStep();

			if ( glob_m_pCamera != NULL )
			{
				//take a triple of pictures	
				glob_m_pCamera->TakePicture(); 
				glob_m_pCamera->TakePicture(); 
				glob_m_pCamera->TakePicture(); 
			}

			float target = scan_start_deg_T + (scan_current_step * scan_step_size_deg);
			
			if ( target > 360.00 ) target = target - 360.0;
			
			MoveAE_T( target, true ); 
		
			scan_current_step++;

			TIMER_Polarizer_Scan = SetTimer( TIMER_POL_SCAN, 200, NULL );
		}
		else if ( scan_current_step_z < scan_number_of_steps_z )
		{
			// move in Z
			// so that each scan starts with a fresh file
			// when rotating the polarizer
			glob_m_pCamera->FileClose();
			
			//go to beginning
			MoveAE_T( scan_start_deg_T, true );
			
			scan_current_step = 1;
			
			if ( glob_m_pObjPiezo != NULL )
				glob_m_pObjPiezo->MoveRelZ( scan_step_size_microns_z );

			scan_current_step_z++;
			
			TIMER_Polarizer_Scan = SetTimer( TIMER_POL_SCAN, 200, NULL );	
		}
		else
		{
			ScanDone();
		}
	}
	else if( PolRot_Only_Z )
	{
		if ( scan_current_step_z <= scan_number_of_steps_z )
		{
			if ( glob_m_pCamera != NULL )
			{
				//take a triple of pictures	
				glob_m_pCamera->TakePicture(); 
				glob_m_pCamera->TakePicture(); 
				glob_m_pCamera->TakePicture(); 
			}

			//and move, UNLESS, we are already at the "top"
			if ( scan_current_step_z < scan_number_of_steps_z )
			{
				if( glob_m_pObjPiezo != NULL )
					glob_m_pObjPiezo->MoveRelZ( scan_step_size_microns_z );
				
				scan_current_step_z++;

				TIMER_Polarizer_Scan = SetTimer( TIMER_POL_SCAN, 200, NULL );	
			}
			else
			{
				ScanDone();
			}
		}
		else
		{   //this should never really happen, right?
			ScanDone();
		}
	}

}

void COctopusPolarizer::ScanDone( void )
{	
	//need to go back to beginning
	KillTimer( TIMER_Polarizer_Scan );
	
	if ( PolRot_Both || PolRot_Only_Z )
	{
		if ( glob_m_pObjPiezo != NULL && (scan_number_of_steps_z > 1) )
			glob_m_pObjPiezo->MoveRelZ( -1 * float(scan_number_of_steps_z - 1) * scan_step_size_microns_z );
	}

	B.POL_scan_in_progress = false;

	if ( PolRot_Both || PolRot_Only_Pol )
	{
		MoveAE_T( scan_start_deg_T, true );
	}
	
	m_status_rot.SetBitmap( m_bmp_no );

	GetPosition();
	ShowPosition();
}
