#include "FullInclude.h"

/**
 * @brief Converts integer into enum for ProtocolMode.
 *
 * This is useful for compatibility with languages other than C++.
 *
 * @param	mode	Integer to convert.
 *
 * @return	enum equivalent of @a mode.
 */
NetMode::ProtocolMode NetMode::ConvertToProtocolModeTCP(int mode)
{
	switch(mode)
	{
	case(TCP_PREFIX_SIZE):
	case(TCP_POSTFIX):
	case(TCP_RAW):
		return static_cast<NetMode::ProtocolMode>(mode);
		break;

	default:
		_ErrorException(true,"converting from integer to TCP protocol mode, invalid mode received",0,__LINE__,__FILE__);
		return NetMode::TCP_PREFIX_SIZE;
		break;
	}
}

/**
 * @brief Converts integer into enum for ProtocolMode.
 *
 * This is useful for compatibility with languages other than C++.
 *
 * @param	mode	Integer to convert.
 *
 * @return	enum equivalent of @a mode.
 */
NetMode::ProtocolMode NetMode::ConvertToProtocolModeUDP(int mode)
{
	switch(mode)
	{
	case(UDP_PER_CLIENT):
	case(UDP_PER_CLIENT_PER_OPERATION):
	case(UDP_CATCH_ALL):
	case(UDP_CATCH_ALL_NO):
		return static_cast<NetMode::ProtocolMode>(mode);
		break;

	default:
		_ErrorException(true,"converting from integer to UDP protocol mode, invalid mode received",0,__LINE__,__FILE__);
		return NetMode::UDP_CATCH_ALL_NO;
		break;
	}
}

/**
 * @brief Determines whether the specified protocol mode is TCP based.
 *
 * @param	mode	Protocol mode to validate. 
 * @return	true if protocol mode is a valid TCP mode, false if not.
 */
bool NetMode::ValidateProtocolModeTCP( ProtocolMode mode )
{
	switch(mode)
	{
	case(TCP_PREFIX_SIZE):
	case(TCP_POSTFIX):
	case(TCP_RAW):
		return true;
		break;

	default:
		return false;
		break;
	}
}

/**
 * @brief Determines whether the specified protocol mode is UDP based.
 *
 * @param	mode	Protocol mode to validate. 
 * @return	true if protocol mode is a valid UDP mode, false if not.
 */
bool NetMode::ValidateProtocolModeUDP( ProtocolMode mode )
{
	switch(mode)
	{
	case(UDP_PER_CLIENT):
	case(UDP_PER_CLIENT_PER_OPERATION):
	case(UDP_CATCH_ALL):
	case(UDP_CATCH_ALL_NO):
		return true;
		break;

	default:
		return false;
		break;
	}
}


/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetMode::TestClass()
{
	cout << "Testing NetMode class...\n";
	bool problem = false;

	if(ConvertToProtocolModeUDP(2) != NetMode::UDP_PER_CLIENT_PER_OPERATION)
	{
		cout << "ConvertToProtocolMode is bad\n";
		problem = true;
	}
	else
	{
		cout << "ConvertToProtocolMode is good\n";
	}
	
	if(ConvertToProtocolModeUDP(3) != NetMode::UDP_CATCH_ALL)
	{
		cout << "ConvertToProtocolModeUDP is bad\n";
		problem = true;
	}
	else
	{
		cout << "ConvertToProtocolModeUDP is good\n";
	}

	if(ConvertToProtocolModeTCP(1) != NetMode::TCP_PREFIX_SIZE)
	{
		cout << "ConvertToProtocolModeTCP is bad\n";
		problem = true;
	}
	else
	{
		cout << "ConvertToProtocolModeTCP is good\n";
	}

	if(ValidateProtocolModeTCP(NetMode::TCP_POSTFIX) == false)
	{
		cout << "ValidateProtocolModeTCP is bad\n";
		problem = true;
	}
	else
	{
		cout << "ValidateProtocolModeTCP is good\n";
	}

	if(ValidateProtocolModeTCP(NetMode::UDP_CATCH_ALL_NO) == true)
	{
		cout << "ValidateProtocolModeTCP is bad\n";
		problem = true;
	}
	else
	{
		cout << "ValidateProtocolModeTCP is good\n";
	}


	if(ValidateProtocolModeUDP(NetMode::UDP_PER_CLIENT) == false)
	{
		cout << "ValidateProtocolModeUDP is bad\n";
		problem = true;
	}
	else
	{
		cout << "ValidateProtocolModeUDP is good\n";
	}

	cout << "\n\n";
	return !problem;
}
