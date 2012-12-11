#include "FullInclude.h"
//#include <vld.h>

void problem(bool result)
{
	if(result == false)
	{
		cout << "A problem was found during testing, please check..\n";
		int wait;
		cin >> wait;
	}
}

/**
 * @brief Tests API.
 */
void main()
{
 	/*problem(ThreadSingle::TestClass());
 	problem(CriticalSection::TestClass());
 	problem(ThreadSingleGroup::TestClass());
 	problem(CompletionPort::TestClass());
 	problem(CompletionKey::TestClass());
 	problem(ConcurrencyControl::TestClass());
 	problem(ConcurrencyControlSimple::TestClass());
 	problem(ConcurrentObject<int>::TestClass());
 	problem(ConcurrencyEvent::TestClass());
 	problem(StoreVector<int>::TestClass());
 	problem(StoreQueue<int>::TestClass());
 	problem(Counter::TestClass());
 	problem(Packet::TestClass());
 	problem(Timer::TestClass());
 	problem(Utility::TestClass());
 	problem(NetSocketSimple::TestClass());
 	problem(NetUtility::TestClass());
 	problem(NetAddress::TestClass());
 	problem(NetSend::TestClass());
 	problem(NetSendRaw::TestClass());
 	problem(NetSendPrefix::TestClass());
 	problem(NetSendPostfix::TestClass());
 	problem(NetMode::TestClass());
 	problem(NetModeTcp::TestClass());
 	problem(NetModeTcpPostfix::TestClass());
 	problem(NetModeTcpPrefixSize::TestClass());
 	problem(NetModeUdp::TestClass());
 	problem(NetModeUdpCatchAll::TestClass());
 	problem(NetModeUdpCatchAllNo::TestClass());
 	problem(NetModeUdpPerClient::TestClass());
 	problem(NetSocket::TestClass());
 	problem(NetSocketListening::TestClass());
 	problem(NetSocketTCP::TestClass());
 	problem(NetSocketUDP::TestClass());
 	problem(NetInstanceClient::TestClass());
 	problem(NetInstanceServer::TestClass());
 	problem(NetInstanceBroadcast::TestClass());
 	problem(ErrorReport::TestClass());
 	problem(ThreadSingleMessage::TestClass());
 	problem(ThreadSingleMessageKeepLast::TestClass());
 	problem(UpnpNatCommunication::TestClass());
 	problem(EncryptKey::TestClass());
 	problem(SoundDeviceInput::TestClass());
 	problem(SoundDeviceOutput::TestClass());
 	problem(SoundInstanceOutput::TestClass());*/
	//problem(mn::TestClass());
	cout << "Finished...\n";
	int wait;
	cin >> wait;
}