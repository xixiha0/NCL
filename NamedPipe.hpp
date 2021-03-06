/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NAMEDPIPE.HPP
** COMPONENT:	Network & Comms Library
** DESCRIPTION:	The CNamedPipe class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NAMEDPIPE_HPP
#define NAMEDPIPE_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/Buffer.hpp>
#include <WCL/Event.hpp>

/******************************************************************************
**
** The base class for Named Pipes.
**
*******************************************************************************
*/

class CNamedPipe /*: private NotCopyable*/
{
public:
	//
	// Properties.
	//
	HANDLE Handle() const;
	bool   IsOpen() const;

	DWORD  TimeOut() const;
	void   SetTimeOut(DWORD dwTimeOut);

	//
	// Methods.
	//
	size_t Available();
	size_t Peek(void* pBuffer, size_t nBufSize);
	size_t Peek(CBuffer& oBuffer, size_t nBufSize);
	void Read(void* pBuffer, size_t nBufSize);
	void Read(CBuffer& oBuffer);
	void Write(const void* pBuffer, size_t nBufSize);
	void Write(const CBuffer& oBuffer);

	virtual void Close();

protected:
	//
	// Members.
	//
	HANDLE		m_hPipe;		// The pipe handle.
	CString		m_strName;		// The pipe name.
	CEvent		m_oReadEvent;	// Read Overlapped I/O event.
	OVERLAPPED	m_oReadIO;		// Read Overlapped I/O data.
	CEvent		m_oWriteEvent;	// Write Overlapped I/O event.
	OVERLAPPED	m_oWriteIO;		// Write Overlapped I/O data.
	bool		m_bPrevWrite;	// Write outstanding?
	size_t		m_nPrevBytes;	// Previous write size.
	DWORD		m_dwTimeOut;	// Connect/Read/Write time-out.

	//
	// Constants.
	//
	static const DWORD DEF_TIMEOUT;

	//
	// Constructors/Destructor.
	// NB: Abstract class.
	//
	CNamedPipe();
	CNamedPipe(const CNamedPipe&);
	CNamedPipe& operator=(const CNamedPipe&);
	virtual ~CNamedPipe();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline HANDLE CNamedPipe::Handle() const
{
	return m_hPipe;
}

inline bool CNamedPipe::IsOpen() const
{
	return (m_hPipe != INVALID_HANDLE_VALUE);
}

inline DWORD CNamedPipe::TimeOut() const
{
	return m_dwTimeOut;
}

inline void CNamedPipe::SetTimeOut(DWORD dwTimeOut)
{
	m_dwTimeOut = dwTimeOut;
}

inline size_t CNamedPipe::Peek(CBuffer& oBuffer, size_t nBufSize)
{
	return Peek(oBuffer.Buffer(), nBufSize);
}

inline void CNamedPipe::Read(CBuffer& oBuffer)
{
	Read(oBuffer.Buffer(), oBuffer.Size());
}

inline void CNamedPipe::Write(const CBuffer& oBuffer)
{
	Write(oBuffer.Buffer(), oBuffer.Size());
}

#endif // NAMEDPIPE_HPP
