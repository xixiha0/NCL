////////////////////////////////////////////////////////////////////////////////
//! \file   IDDEClient.hpp
//! \brief  The IDDEClient interface declaration.
//! \author Chris Oldwood

// Check for previous inclusion
#ifndef NCL_IDDECLIENT_HPP
#define NCL_IDDECLIENT_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <Core/SharedPtr.hpp>
#include "IDDECltConv.hpp"

namespace DDE
{

////////////////////////////////////////////////////////////////////////////////
//! A DDE client.

class XDDEClient
{
public:
	//! Destructor.
	virtual ~XDDEClient() {};

	//! Open a conversation for the service and topic.
	virtual IDDECltConv* CreateConversation(const tchar* service, const tchar* topic) = 0;

	//! Close the conversation.
	virtual void DestroyConversation(IDDECltConv* conv) = 0;

	//! Query for all running servers and topics.
	virtual void QueryAll(CStrArray& astrServers, CStrArray& astrTopics) const = 0;
};

//! The default IDDEClient smart pointer type.
typedef Core::SharedPtr<IDDEClient> IDDEClientPtr;

//namespace DDE
}

#endif // NCL_IDDECLIENT_HPP
