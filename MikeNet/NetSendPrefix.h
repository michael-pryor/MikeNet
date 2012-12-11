#pragma once
#include "NetSend.h"
#include "Packet.h"

/**
 * @brief Send class where packets sent have a prefix.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * This class makes use of scatter/gather I/O to maximize efficiency.
 */
class NetSendPrefix : public NetSend
{
	/** @brief Stores prefix. */
	Packet prefix;

	/** @brief Pointer to packet containing data to be sent. */
	const Packet * packet;

	/** @brief Number of elements in NetSendPrefix::buffers. */
	static const size_t NUM_BUFFERS = 2;

	/**
	 * @brief Array of buffers to be sent.
	 *
	 * - e0 is packet prefix
	 * - e1 is packet data.
	 */
	WSABUF buffers[NUM_BUFFERS];

public:
	NetSendPrefix(const Packet * packet, bool block, const Packet & prefix);
	~NetSendPrefix();

	WSABUF * GetBuffer();
	size_t GetBufferAmount() const;
	
	static bool TestClass();
};