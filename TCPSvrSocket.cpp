/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		TCPSVRSOCKET.CPP
** COMPONENT:	Network & Comms Library
** DESCRIPTION:	CTCPSvrSocket class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "TCPSvrSocket.hpp"
#include "TCPCltSocket.hpp"
#include "IServerSocketListener.hpp"
#include "SocketException.hpp"
#include "WinSock.hpp"
#include <limits.h>
#include <algorithm>

#ifdef _MSC_VER
// Conditional expression is constant.
// Caused by FD_SET().
#pragma warning ( disable : 4127 )
#endif

#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 2)) // GCC 4.2+
// missing initializer for member 'X'
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
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

CTCPSvrSocket::CTCPSvrSocket(Mode eMode)
	: CTCPSocket(eMode)
	, m_aoSvrListeners()
{
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

CTCPSvrSocket::~CTCPSvrSocket()
{
}

/******************************************************************************
** Method:		Listen()
**
** Description:	Open the socket for listening on the given port.
**
** Parameters:	nPort		The port number.
**				nBackLog	The connection queue size.
**
** Returns:		Nothing.
**
** Exceptions:	CSocketException.
**
*******************************************************************************
*/

void CTCPSvrSocket::Listen(uint nPort, uint nBackLog)
{
	ASSERT(m_hSocket == INVALID_SOCKET);
	ASSERT(nPort     <= USHRT_MAX);

	// Save parameters.
	m_nPort = nPort;

	sockaddr_in	addr = { 0 };

	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port        = htons(static_cast<u_short>(nPort));

	// Create the socket.
	Create(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Bind socket to port.
	if (bind(m_hSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
		throw CSocketException(CSocketException::E_BIND_FAILED, CWinSock::LastError());

	// Start accepting client connections.
	if (listen(m_hSocket, nBackLog)  == SOCKET_ERROR)
		throw CSocketException(CSocketException::E_LISTEN_FAILED, CWinSock::LastError());

	// If async mode, do select.
	if (m_eMode == ASYNC)
		CWinSock::BeginAsyncSelect(this, (FD_ACCEPT | FD_CLOSE));
}

/******************************************************************************
** Method:		CanAccept()
**
** Description:	Queries if there is a client connection waiting.
**
** Parameters:	None.
**
** Returns:		true or false.
**
** Exceptions:	CSocketException.
**
*******************************************************************************
*/

bool CTCPSvrSocket::CanAccept() const
{
	fd_set  aoSockets;
	TIMEVAL oWaitTime = { 0 };

	FD_ZERO(&aoSockets);
	FD_SET(m_hSocket, &aoSockets);

	// Check for readability on the socket.
	if (select(1, &aoSockets, nullptr, nullptr, &oWaitTime) == SOCKET_ERROR)
		throw CSocketException(CSocketException::E_SELECT_FAILED, CWinSock::LastError());

	return FD_ISSET(m_hSocket, &aoSockets);
}

/******************************************************************************
** Method:		Accept()
**
** Description:	Accepts a client connection.
**
** Parameters:	None.
**
** Returns:		A socket to the client.
**
** Exceptions:	CSocketException.
**
*******************************************************************************
*/

CTCPCltSocket* CTCPSvrSocket::Accept()
{
	CTCPCltSocket* pSocket = AllocCltSocket();

	Accept(pSocket);

	return pSocket;
}

/******************************************************************************
** Method:		Accept()
**
** Description:	Accepts a client connection.
**
** Parameters:	pCltSocket		The client socket to accept on.
**
** Returns:		Nothing.
**
** Exceptions:	CSocketException.
**
*******************************************************************************
*/

void CTCPSvrSocket::Accept(CTCPCltSocket* pCltSocket)
{
	ASSERT(pCltSocket != nullptr);

	SOCKET       hSocket;
	sockaddr_in  addr      = { 0 };;
	int          nAddrSize = sizeof(addr);

	// Accept the next client connection.
	if ((hSocket = accept(m_hSocket, reinterpret_cast<sockaddr*>(&addr), &nAddrSize)) == INVALID_SOCKET)
		throw CSocketException(CSocketException::E_ACCEPT_FAILED, CWinSock::LastError());

	pCltSocket->Attach(hSocket, m_eMode);
}

/******************************************************************************
** Method:		AddServerListener()
**
** Description:	Add a socket event listener.
**
** Parameters:	pListener	The listener.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CTCPSvrSocket::AddServerListener(IServerSocketListener* pListener)
{
	ASSERT(pListener != nullptr);

	m_aoSvrListeners.push_back(pListener);
}

/******************************************************************************
** Method:		RemoveServerListener()
**
** Description:	Remove a socket event listener.
**
** Parameters:	pListener	The listener.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CTCPSvrSocket::RemoveServerListener(IServerSocketListener* pListener)
{
	ASSERT(pListener != nullptr);

	CSvrListeners::iterator it = std::find(m_aoSvrListeners.begin(), m_aoSvrListeners.end(), pListener);

	if (it != m_aoSvrListeners.end())
		m_aoSvrListeners.erase(it);
}

/******************************************************************************
** Method:		OnAsyncSelect()
**
** Description:	Async socket event handler.
**
** Parameters:	nEvent		The event.
**				nError		The error.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CTCPSvrSocket::OnAsyncSelect(int nEvent, int nError)
{
	// Connection waiting?
	if ( (nEvent & FD_ACCEPT) && (nError == 0) )
		OnAcceptReady();

	// Forward to base class.
	CSocket::OnAsyncSelect(nEvent, nError);
}

/******************************************************************************
** Method:		OnAcceptReady()
**
** Description:	The socket has a connection waiting.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CTCPSvrSocket::OnAcceptReady()
{
	typedef CSvrListeners::const_iterator iter;

	// Notify listeners.
	for (iter it = m_aoSvrListeners.begin(); it != m_aoSvrListeners.end(); ++it)
		(*it)->OnAcceptReady(this);
}

/******************************************************************************
** Method:		OnClosed()
**
** Description:	The socket was closed.
**
** Parameters:	nReason		The error code.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CTCPSvrSocket::OnClosed(int nReason)
{
	typedef CSvrListeners::const_iterator iter;

	// Clean-up.
	Close();

	// Notify listeners.
	for (iter it = m_aoSvrListeners.begin(); it != m_aoSvrListeners.end(); ++it)
		(*it)->OnClosed(this, nReason);
}

/******************************************************************************
** Method:		OnError()
**
** Description:	An async error occurred for an event.
**
** Parameters:	nEvent		The event.
**				nError		The error.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CTCPSvrSocket::OnError(int nEvent, int nError)
{
	typedef CSvrListeners::const_iterator iter;

	// Notify listeners.
	for (iter it = m_aoSvrListeners.begin(); it != m_aoSvrListeners.end(); ++it)
		(*it)->OnError(this, nEvent, nError);
}

/******************************************************************************
** Method:		AllocCltSocket()
**
** Description:	Allocate a client TCP socket.
**
** Parameters:	nEvent		The event.
**				nError		The error.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CTCPCltSocket* CTCPSvrSocket::AllocCltSocket()
{
	return new CTCPCltSocket(m_eMode);
}
