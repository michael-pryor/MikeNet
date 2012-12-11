#pragma once
#include "NetSend.h"
#include "Packet.h"

/**
 * @brief Send class where packets sent have no postfix or prefix appended.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * This class is used for sending raw data with no modifications.
 */
class NetSendRaw : public NetSend
{
	/** @brief Pointer to packet containing data to be sent. */
	const Packet * packet;

	/** @brief Number of elements in NetSendRaw::buffers. */
	static const size_t NUM_BUFFERS = 1;

	/**
	 * @brief Array of buffers to be sent.
	 *
	 * - e0 is packet data.
	 */
	WSABUF buffers[NUM_BUFFERS];
public:
	NetSendRaw(const Packet * packet, bool block);
	~NetSendRaw();

	WSABUF * GetBuffer();
	size_t GetBufferAmount() const;

	static bool TestClass();
};