#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * @param handshakeEnabled If false the @ref handshakePage "TCP handshake process" is disabled.
 * @note Virtual inheritance means that NetInstance constructor will never be called by this constructor.
 */
NetInstanceTCP::NetInstanceTCP(bool handshakeEnabled) : NetInstance()
{
	this->handshakeEnabled = handshakeEnabled;
}

/**
 * @brief Destructor.
 */
NetInstanceTCP::~NetInstanceTCP()
{

}

/**
 * @brief Determines whether the handshake process is enabled.
 *
 * @return true if handshake is enabled, false if not.
 * For more information see @ref handshakePage "server/client handshaking process".
 */
bool NetInstanceTCP::IsHandshakeEnabled() const
{
	return handshakeEnabled;
}

/** 
 * @brief Determines whether specified size is valid.
 * 
 * @param size Size to check.
 * @return true If @a size is larger enough (i.e. Larger than or equal to GetRecvSizeMinTCP).
 */
bool NetInstanceTCP::ValidateRecvSizeTCP(size_t size) const
{
	return(size >= GetRecvSizeMinTCP());
}
