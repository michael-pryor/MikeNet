#include "FullInclude.h"

/**
 * @brief	Thread manages UPnP NAT, performing actions as instructed by UpnpNatCommunication, using controls of UpnpNatAction.
 *
 * @param	lpParameter	Pointer to ThreadSingleMessageKeepLast object which has ownership of thread. The parameter of this object
 * must be a pointer to a pointer to store the UpnpNatAction object which is constructed by this thread (as a requirement of COM).
 *
 * @return	0.
 */
DWORD WINAPI UpnpNatActionThread(LPVOID lpParameter)
{
	const char * cCommand = "an internal function (UpnpNatActionThread)";

	ThreadSingleMessageKeepLast * thread = static_cast<ThreadSingleMessageKeepLast*>(lpParameter);
	ThreadSingle::ThreadSetCallingThread(thread);
	UpnpNatAction ** control = static_cast<UpnpNatAction**>(thread->GetParameter());

	ComUtility::Initialize();

	try
	{
		// Setup NAT.
		// This must be done in this thread.
		*control = new (nothrow) UpnpNatAction();
		Utility::DynamicAllocCheck(*control,__LINE__,__FILE__);

		while(thread->GetTerminateRequest() == false)
		{
			ThreadMessageItem * message = thread->GetMessageItem();

			try
			{
				message->TakeAction();
			}
			catch (ErrorReport & error)
			{
				ErrorReport * aux = new (nothrow) ErrorReport(error);
				Utility::DynamicAllocCheck(aux,__LINE__,__FILE__);
				aux->SetCommand(cCommand);
				thread->SetError(aux);
			}

			bool shouldCleanup = message->ShouldThreadCleanup();
			if(shouldCleanup == true)
			{
				delete message;
			}
		}

		// Cleanup NAT.
		delete *control;
		*control = NULL;
	}
	MSG_CATCH

	ComUtility::Cleanup();

	return 0;
}