/*
Copyright 2014 Mona
mathieu.poux[a]gmail.com
jammetthomas[a]gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License received along this program for more
details (or else see http://www.gnu.org/licenses/).

This file is a part of Mona.
*/

#pragma once

#include "Mona/Mona.h"
#include "Mona/Protocol.h"
#include <functional>

namespace Mona {

class UDProtocol;
class Protocols : public virtual Object {
public:
	Protocols(Invoker& invoker) : _invoker(invoker) {}

	void load(Sessions& sessions);
	void unload() { _protocols.clear(); }
	void manage() { for (std::unique_ptr<Protocol>& pProtocol : _protocols) pProtocol->manage(); }

private:
	template<class ProtocolType, typename ...Args >
	void loadProtocol(const char* name, UInt16 port, Sessions& sessions, Args&&... args) {
		std::string buffer;
		if (!_invoker.getNumber(String::Format(buffer, name, ".port"), port))
			_invoker.setNumber(buffer, port);
		if(port==0)
			return;
		std::unique_ptr<Protocol> pProtocol(new ProtocolType(name, _invoker, sessions, args ...));

		std::string host("0.0.0.0");
		_invoker.getString("host", host);
	
		// configs to protocol params!
		Parameters::ForEach forEach;
		forEach = [&pProtocol](const std::string& key, const std::string& value) -> void {
			pProtocol->setString(key, value);
		};
		_invoker.iterate(String::Format(buffer,name,"."),forEach);

		Exception ex;
		bool success = false;
		pProtocol->getString("host",host);
		EXCEPTION_TO_LOG(success = ((ProtocolType*)pProtocol.get())->load(ex,host,port), name, " server")
		if (!success)
			return;

		// copy protocol params to invoker params
		for (auto& it : *pProtocol)
			_invoker.setString(String::Format(buffer,name,".",it.first), it.second);

		NOTE(name, " server started on ",host,":",port, dynamic_cast<UDProtocol*>(pProtocol.get()) ? " (UDP)" : " (TCP)");
		_protocols.emplace_back(pProtocol.release());
	}


	std::vector<std::unique_ptr<Protocol>>	_protocols;
	Invoker&								_invoker;
};


} // namespace Mona
