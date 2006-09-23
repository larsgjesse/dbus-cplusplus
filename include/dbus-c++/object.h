/*
 *
 *  D-Bus++ - C++ bindings for DBus
 *
 *  Copyright (C) 2005-2006  Paolo Durante <shackan@gmail.com>
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#ifndef __DBUSXX_OBJECT_H
#define __DBUSXX_OBJECT_H

#include <string>
#include <list>

#include "interface.h"
#include "connection.h"
#include "message.h"
#include "types.h"

namespace DBus {

class Object
{
protected:

	Object( Connection& conn, const Path& path, const char* service );
	
public:

	virtual ~Object();

	inline const DBus::Path& path() const;

	inline const std::string& service() const;
	
 	inline Connection& conn();

private:

	virtual bool handle_message( const Message& ) = 0;
	virtual void register_obj() = 0;
	virtual void unregister_obj() = 0;

private:

	Connection	_conn;
	DBus::Path	_path;
	std::string	_service;
};

/*
*/

Connection& Object::conn()
{
	return _conn;
}

const DBus::Path& Object::path() const
{
	return _path;
}

const std::string& Object::service() const
{
	return _service;
}

/*
*/

class ObjectAdaptor;

typedef std::list<ObjectAdaptor*> ObjectAdaptorPList;

class ObjectAdaptor : public Object, public virtual AdaptorBase
{
public:

	static ObjectAdaptor* from_path( const Path& path );

	static ObjectAdaptorPList from_path_prefix( const std::string& prefix );

	struct Private;

	ObjectAdaptor( Connection& conn, const Path& path );

	~ObjectAdaptor();

	inline const ObjectAdaptor* object() const;

protected:

	class Continuation
	{
	public:

		inline MessageIter& writer();

		inline void* tag();

	private:

		Continuation( Connection& conn, const CallMessage& call, const void* tag );

		Connection _conn;
		CallMessage _call;
		MessageIter _writer;
		ReturnMessage _return;
		const void* _tag;

	friend class ObjectAdaptor;
	};

	void return_later( const void* tag );

	void return_now( Continuation* ret );

	void return_error( Continuation* ret, const Error error );

	Continuation* find_continuation( const void* tag );

private:

	void _emit_signal( SignalMessage& );

	bool handle_message( const Message& );

	void register_obj();
	void unregister_obj();

	typedef std::map<const void*, Continuation*> ContinuationMap;
	ContinuationMap _continuations;

friend struct Private;
};

const ObjectAdaptor* ObjectAdaptor::object() const
{
	return this;
}

void* ObjectAdaptor::Continuation::tag()
{
	return const_cast<void*>(_tag);
}

MessageIter& ObjectAdaptor::Continuation::writer()
{
	return _writer;
}

/*
*/

class ObjectProxy;

typedef std::list<ObjectProxy*> ObjectProxyPList;

class ObjectProxy : public Object, public virtual ProxyBase
{
public:

	ObjectProxy( Connection& conn, const Path& path, const char* service = "" );

	~ObjectProxy();

	inline const ObjectProxy* object() const;

private:

	Message _invoke_method( CallMessage& );

	bool handle_message( const Message& );

	void register_obj();
	void unregister_obj();

private:

	MessageSlot _filtered;
};

const ObjectProxy* ObjectProxy::object() const
{
	return this;
}

} /* namespace DBus */

#endif//__DBUSXX_OBJECT_H
