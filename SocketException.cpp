/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SOCKETEXCEPTION.CPP
** COMPONENT:	Network & Comms Library
** DESCRIPTION:	CSocketException class definition.
**
*******************************************************************************
*/

#include "ncl.hpp"

#ifdef _DEBUG
// For memory leak detection.
#define new DBGCRT_NEW
#endif

/******************************************************************************
** Method:		Constructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CSocketException::CSocketException(int eErrCode, int nWSACode)
	: m_nWSACode(nWSACode)
{
	m_nErrorCode = eErrCode;

	// Get error code symbol.
	CString strSymbol = CWinSock::ErrorToSymbol(nWSACode);

	// Convert error to string.
	switch(eErrCode)
	{
		case E_CREATE_FAILED:
			m_strErrorText.Format("Failed to create socket: %s", strSymbol);
			break;

		case E_RESOLVE_FAILED:
			m_strErrorText.Format("Failed to resolve hostname: %s", strSymbol);
			break;

		case E_CONNECT_FAILED:
			m_strErrorText.Format("Failed to connect to host: %s", strSymbol);
			break;

		case E_BIND_FAILED:
			m_strErrorText.Format("Failed to bind socket: %s", strSymbol);
			break;

		case E_LISTEN_FAILED:
			m_strErrorText.Format("Failed to listen on socket: %s", strSymbol);
			break;

		case E_ACCEPT_FAILED:
			m_strErrorText.Format("Failed to accept client connection: %s", strSymbol);
			break;

		case E_SEND_FAILED:
			m_strErrorText.Format("Failed to send data: %s", strSymbol);
			break;

		case E_RECV_FAILED:
			m_strErrorText.Format("Failed to receive data: %s", strSymbol);
			break;

		case E_PEEK_FAILED:
			m_strErrorText.Format("Failed to peek incoming data: %s", strSymbol);
			break;

		case E_SELECT_FAILED:
			m_strErrorText.Format("Failed to query socket state: %s", strSymbol);
			break;

		case E_QUERY_FAILED:
			m_strErrorText.Format("Failed to query socket info: %s", strSymbol);
			break;

		case E_DISCONNECTED:
			m_strErrorText.Format("Connection closed: %s", strSymbol);
			break;

		// Shouldn't happen!
		default:
			ASSERT_FALSE();
			break;
	}
}

/******************************************************************************
** Method:		Destructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CSocketException::~CSocketException()
{
}