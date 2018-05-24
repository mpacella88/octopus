// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// undefined wrapper class

class APT_system : public CWnd
{
protected:
	DECLARE_DYNCREATE(APT_system)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xB74DB4BA, 0x8C1E, 0x4570, { 0x90, 0x6E, 0xFF, 0x65, 0x69, 0x8D, 0x63, 0x2E } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// Attributes
public:
enum
{
    USB_CHOPPER = 1,
    USB_PZMOTOR = 2,
    USB_QUAD = 3,
    USB_TEC = 4,
    USB_LASER = 5,
    USB_STEPPER_DRIVE = 6,
    USB_PIEZO_DRIVE = 7,
    USB_NANOTRAK = 8
}MG17_HW_TYPES;
enum
{
    USB_BSC001 = 11,
    USB_BSC101 = 12,
    USB_BSC002 = 13,
    USB_BDC001 = 14,
    USB_BPC001 = 15,
    USB_BPC101 = 16,
    USB_BPC002 = 17,
    USB_BNT001 = 18,
    USB_BMS001 = 19,
    USB_BMS002 = 20,
    CARD_SCC101 = 21,
    CARD_DCC101 = 22,
    CARD_PCC101 = 23,
    USB_ODC001 = 24,
    USB_OST001 = 25,
    MOD_MST601 = 26,
    MOD_MPZ601 = 27,
    MOD_MNA601 = 28,
    USB_TST001 = 29,
    USB_TPZ001 = 30,
    USB_TDC001 = 31,
    USB_TSG001 = 32,
    USB_TSC001 = 33,
    USB_TNA001 = 34,
    USB_TLS001 = 35,
    USB_TTC001 = 36,
    USB_TQD001 = 38,
    USB_TLD001 = 39,
    USB_TIM001 = 40,
    USB_TCC001 = 41
}APT_HW_TYPES_EX;


// Operations
public:

// _DMG17System

// Functions
//

	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long GetNumHWUnits(long lHWType, long * plNumUnits)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_PI4 ;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lHWType, plNumUnits);		
		return result;
	}
	long GetHWType(long lSerialNum, long * plHWType)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_PI4 ;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lSerialNum, plHWType);


		CString temp;

		temp.Format( _T("HW type:%d"), result );
	
		AfxMessageBox(temp);
		
		return result;
	}
	long GetHWSerialNum(long lHWType, long lIndex, long * plSerialNum)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PI4 ;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lHWType, lIndex, plSerialNum);
		return result;
	}
	long GetHWInfo(long lSerialNum, BSTR * bstrModel, BSTR * bstrSWVer, BSTR * bstrHWNotes)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_PBSTR VTS_PBSTR VTS_PBSTR ;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lSerialNum, bstrModel, bstrSWVer, bstrHWNotes);
		return result;
	}
	long StartCtrl()
	{
		long result;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long StopCtrl()
	{
		long result;
		InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long DeleteParamSets(LPCTSTR bstrName)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_I4, (void*)&result, parms, bstrName);
		return result;
	}
	long GetNumParamSets(long * plNumSets)
	{
		long result;
		static BYTE parms[] = VTS_PI4 ;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, plNumSets);
		return result;
	}
	long GetParamSetName(long lIndex, BSTR * bstrName)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_PBSTR ;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lIndex, bstrName);
		return result;
	}
	long GetHWTypeEx(long lSerialNum, long * plHWType)
	{
		long result = 0;
		
		static BYTE parms[] = VTS_I4 VTS_PI4 ;
		
		InvokeHelper(0xa, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lSerialNum, plHWType);
		
		CString temp;

		temp.Format( _T("HW type Ex:%d"), result );
	
		AfxMessageBox(temp);
		
		return result;
	}
	long GetNumHWUnitsEx(long lHWType, long * plNumUnits)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_PI4 ;
		InvokeHelper(0xb, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lHWType, plNumUnits);
		return result;
	}
	long GetHWSerialNumEx(long lHWType, long lIndex, long * plSerialNum)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PI4 ;
		InvokeHelper(0xc, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lHWType, lIndex, plSerialNum);
		return result;
	}

// Properties
//

};