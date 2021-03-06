/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		UDPSOCKET.CPP
** COMPONENT:	Network & Comms Library
** DESCRIPTION:	CUDPSocket class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "UDPSocket.hpp"
#include "SocketException.hpp"
#include "WinSock.hpp"

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

CUDPSocket::CUDPSocket(Mode eMode)
	: CSocket(eMode)
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

CUDPSocket::~CUDPSocket()
{
}

/******************************************************************************
** Methods:		Type() & Protocol()
**
** Description:	Template methods to get the socket type and protocol.
**
** Parameters:	None.
**
** Returns:		SOCK_* & IPPROTO_*.
**
*******************************************************************************
*/

int CUDPSocket::Type() const
{
	return SOCK_DGRAM;
}

int CUDPSocket::Protocol() const
{
	return IPPROTO_UDP;
}

/******************************************************************************
** Method:		SendTo()
**
** Description:	Sends data to a specific host and port.
**
** Parameters:	pBuffer		The buffer to send.
**				nBufSize	The buffer size.
**				oAddr		The target host.
**				nPort		The target port number.
**
** Returns:		The number of bytes sent.
**
** Exceptions:	CSocketException.
**
*******************************************************************************
*/

size_t CUDPSocket::SendTo(const void* pBuffer, size_t nBufSize, const in_addr& oAddr, uint nPort)
{
	ASSERT(m_hSocket != INVALID_SOCKET);

	sockaddr_in	addr = { 0 };

	addr.sin_family = AF_INET;
	addr.sin_addr   = oAddr;
	addr.sin_port   = htons(static_cast<u_short>(nPort));

	int nResult = sendto(m_hSocket, static_cast<const char*>(pBuffer), static_cast<int>(nBufSize), 0,
							reinterpret_cast<sockaddr*>(&addr), sizeof(addr));

	if (nResult == SOCKET_ERROR)
		throw CSocketException(CSocketException::E_SEND_FAILED, CWinSock::LastError());

	ASSERT(static_cast<size_t>(nResult) == nBufSize);

	return nResult;
}

/******************************************************************************
** Method:		RecvFrom()
**
** Description:	Read incoming data.
**
** Parameters:	pBuffer		The buffer to write to.
**				nBufSize	The buffer size.
**				oAddr		The source host.
**				nPort		The source port number.
**
** Returns:		The number of bytes read.
**
** Exceptions:	CSocketException.
**
*******************************************************************************
*/

size_t CUDPSocket::RecvFrom(void* pBuffer, size_t nBufSize, in_addr& oAddr, uint& nPort)
{
	ASSERT(m_hSocket != INVALID_SOCKET);

	sockaddr_in	addr;
	int         nLength = sizeof(addr);

	int nResult = recvfrom(m_hSocket, static_cast<char*>(pBuffer), static_cast<int>(nBufSize), 0,
							reinterpret_cast<sockaddr*>(&addr), &nLength);

	if (nResult == SOCKET_ERROR)
		throw CSocketException(CSocketException::E_RECV_FAILED, CWinSock::LastError());

	ASSERT(addr.sin_family == AF_INET);

	oAddr = addr.sin_addr;
	nPort = ntohs(addr.sin_port);

	return nResult;
}
