#include "FullInclude.h"

DWORD WINAPI _ManageSoundOutput(LPVOID lpParameter)
{
	const char * cCommand = "an internal function (_ManageSoundOutput)";

	ThreadSingle * thread = (ThreadSingle*)lpParameter;
	ThreadSingle::ThreadSetCallingThread(thread);

	try
	{
		while(thread->GetTerminateRequest() == false)
		{
			Sleep(1);

			// Deal with all output devices
			SoundInstanceOutput::EnterInstanceOutputList();

			try
			{
				for(size_t n = 0;n<SoundInstanceOutput::GetNumInstanceOutput();n++)
				{
					SoundInstanceOutput * outputInstance = SoundInstanceOutput::GetInstanceOutput(n);
					outputInstance->DecideWhetherToOutput();			
				}
			}
			// Release control of all objects before throwing final exception
			catch(ErrorReport & error){SoundInstanceOutput::LeaveInstanceOutputList(); throw(error);}
			catch(...){SoundInstanceOutput::LeaveInstanceOutputList(); throw(-1);}

			SoundInstanceOutput::LeaveInstanceOutputList();
		}
	}
	MSG_CATCH

	return 0;
}