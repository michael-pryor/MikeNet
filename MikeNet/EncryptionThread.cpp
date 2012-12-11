#include "FullInclude.h"

DWORD WINAPI EncryptionThread( LPVOID lpParameter )
{
	ThreadSingleMessage * thread = (ThreadSingleMessage*)lpParameter;
	ThreadSingle::ThreadSetCallingThread(thread);

	// Friendly termination
	while(thread->GetTerminateRequest() == false)
	{
		ThreadMessageItem * item = thread->GetMessageItem();

		item->TakeAction();

		bool shouldCleanup = item->ShouldThreadCleanup();
		if(shouldCleanup == true)
		{
			delete item;
		}
	}

	return 0;
}