#pragma once
#include "NetSend.h"
#include "Packet.h"

/**
 * @brief Send class where packets sent have a postfix automatically appended.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * This class makes use of scatter/gather I/O to maximize efficiency.
 */
class NetSendPostfix : public NetSend
{
	/** @brief Pointer to packet containing postfix. */
	const Packet * postfix;

	/** @brief Pointer to packet containing data to send. */
	const Packet * packet;

	/** @brief Number of elements in NetSendPostfix::buffers. */
	static const size_t NUM_BUFFERS = 2;

	/**
	 * @brief Array of buffers to be sent.
	 *
	 * - e0 is packet data.
	 * - e1 is postfix indicating end of packet. 
	 */
	WSABUF buffers[NUM_BUFFERS];
public:
	NetSendPostfix(const Packet * packet, bool block, const Packet * postfix);
	~NetSendPostfix();

	WSABUF * GetBuffer();
	size_t GetBufferAmount() const;

	static bool TestClass();
};