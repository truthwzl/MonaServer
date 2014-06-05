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
#include "Mona/Exceptions.h"
#include "Mona/Listeners.h"
#include "Mona/Peer.h"

namespace Mona {

class Publication : public virtual Object {
public:
	enum Type {
		LIVE,
		RECORD
	};

	Publication(const std::string& name,const PoolBuffers& poolBuffers);
	virtual ~Publication();

	const std::string&		name() const { return _name; }

	Client*					publisher() const { return _pPublisher; }

	const Listeners			listeners;

	const QualityOfService&	videoQOS() const { return _videoQOS; }
	const QualityOfService&	audioQOS() const { return _audioQOS; }
	const QualityOfService&	dataQOS() const { return _dataQOS; }

	void					start(Exception& ex, Peer& peer, Type type);
	void					stop(Peer& peer);

	void					pushAudio(UInt32 time,PacketReader& packet,UInt32 numberLostFragments=0);
	void					pushVideo(UInt32 time,PacketReader& packet,UInt32 numberLostFragments=0);
	void					pushData(DataReader& reader,UInt32 numberLostFragments=0);

	Listener*				addListener(Exception& ex, Peer& peer,Writer& writer);
	void					removeListener(Peer& peer);

	void					flush();

	
	const PoolBuffer&		audioCodecBuffer() const { return _audioCodecBuffer; }
	const PoolBuffer&		videoCodecBuffer() const { return _videoCodecBuffer; }

private:
	Peer*								_pPublisher;
	std::string							_name;
	std::map<Client*,Listener*>			_listeners;

	UInt32								_lastTime;
	PoolBuffer							_audioCodecBuffer;
	PoolBuffer							_videoCodecBuffer;

	QualityOfService					_videoQOS;
	QualityOfService					_audioQOS;
	QualityOfService					_dataQOS;

	bool								_new;
};


} // namespace Mona
