/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		IDDECLIENTLISTENER.HPP
** COMPONENT:	Network & Comms Library
** DESCRIPTION:	The IDDEClientListener interface declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef IDDECLIENTLISTENER_HPP
#define IDDECLIENTLISTENER_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "DDEFwd.hpp"

/******************************************************************************
**
** The callback interface for DDE Client events.
**
*******************************************************************************
*/

class IDDEClientListener
{
public:
	//
	// Methods.
	//
	virtual void OnRegister(const tchar* pszBaseName, const tchar* pszInstName) = 0;
	virtual void OnUnregister(const tchar* pszBaseName, const tchar* pszInstName) = 0;
	virtual void OnDisconnect(DDE::IDDECltConv* pConv) = 0;
	virtual void OnAdvise(CDDELink* pLink, const CDDEData* pData) = 0;

protected:
	// Make interface.
	virtual ~IDDEClientListener() {};
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // IDDECLIENTLISTENER_HPP
