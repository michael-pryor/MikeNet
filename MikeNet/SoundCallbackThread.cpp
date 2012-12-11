#include "FullInclude.h"

/**
 * @brief	Manages sound input and output callback messages, cleaning up used buffers.
 *
 * @param	lpParameter	Pointer to the ThreadSingleMessage object that owns this thread.
 *
 * @return 0.
 */
DWORD WINAPI SoundCallbackThread(LPVOID lpParameter)
{
	const char * cCommand = "an internal function (SoundCallbackThread)";

	try
	{
		ThreadSingleMessage * thread = static_cast<ThreadSingleMessage*>(lpParameter);
		ThreadSingle::ThreadSetCallingThread(thread);

		while(thread->GetTerminateRequest() == false)
		{
			ThreadMessageItem * item = thread->GetMessageItem();
			
			// Take action.
			item->TakeAction();

			// Cleanup message.
			bool shouldCleanup = item->ShouldThreadCleanup();
			if(shouldCleanup == true)
			{
				delete item;
			}
		}
	}
	MSG_CATCH

	return 0;
}