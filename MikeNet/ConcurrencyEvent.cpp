#include "FullInclude.h"

/**
 * @brief	Initializes object.
 *
 * @param	initialState	True if the event object should start signaled, false if it should start unsignaled.
 * @param	manualReset		When the state of a manual-reset event object is signaled, it remains signaled until
 * it is explicitly reset to nonsignaled by the ConcurrencyEvent::Set function.
 * 
 * When the state of an auto-reset (non manual-reset) event object is signaled, it remains signaled until a single
 * waiting thread is released (e.g. ConcurrencyEvent::WaitUntilSignaled returns); the system then automatically
 * resets the state to nonsignaled. If no threads are waiting, the event object's state remains signaled.
 */
void ConcurrencyEvent::Initialize(bool initialState, bool manualReset)
{
	this->manualReset = manualReset;

	eventObject = CreateEvent(NULL,manualReset,initialState,NULL);
	_ErrorException((eventObject == NULL),"creating an event object",WSAGetLastError(),__LINE__,__FILE__);
}

/**
 * @brief	Constructor. 
 *
 * @param	initialState	True if the event object should start signaled, false if it should start unsignaled.
 * @param	manualReset		See ConcurrencyEvent::Initialize for more information.
 */
ConcurrencyEvent::ConcurrencyEvent(bool initialState, bool manualReset)
{
	Initialize(initialState,manualReset);
}

/**
 * @brief Deep copy constructor.
 *
 * Only copies the state of the signal, rather than the event object itself.
 *
 * @param	obj	Object to copy.
 */
ConcurrencyEvent::ConcurrencyEvent( const ConcurrencyEvent & obj )
{
	Initialize(obj.Get(),obj.IsManualReset());
}

/**
 * @brief Deep assignment operator.
 *
 * Only copies the state of the signal, rather than the event object itself.
 *
 * @param	copyMe	Object to copy. 
 * @return	reference to this object.
 */
ConcurrencyEvent & ConcurrencyEvent::operator= (const ConcurrencyEvent & copyMe)
{
	Set(copyMe.Get());
	return *this;
}


/**
 * @brief	Equality operator. 
 *
 * @param	compare	Object to compare with this object.
 *
 * @return	true if @a compare's signal is the same as this object's signal. 
 */
bool ConcurrencyEvent::operator==(const ConcurrencyEvent & compare) const
{
	return this->Get() == compare.Get();
}

/**
 * @brief	Inequality operator. 
 *
 * @param	compare	Object to compare with this object.
 *
 * @return	false if @a compare's signal is the same as this object's signal.  
 */
bool ConcurrencyEvent::operator!=(const ConcurrencyEvent & compare) const
{
	return !(*this == compare);
}

/**
 * @brief	Determine if manual reset is enabled for the event object.
 *
 * @return	true if manual reset is enabled, false if not.
 */
bool ConcurrencyEvent::IsManualReset() const
{
	return manualReset;
}

/**
 * @brief	Destructor. 
 */
ConcurrencyEvent::~ConcurrencyEvent(void)
{
	const char * cCommand = "An internal function (~ConcurrencyEvent)";
	try
	{
		BOOL result = CloseHandle(eventObject);
		_ErrorException((result == FALSE),"closing an event object's handle",WSAGetLastError(),__LINE__,__FILE__);
	}
	MSG_CATCH
}

/**
 * @brief Queries an event object.
 *
 * @return  true if @a eventObject is signaled, false if not.
 */
bool ConcurrencyEvent::Get() const
{
	DWORD result = WaitForSingleObject(eventObject,0);
	_ErrorException((result == WAIT_FAILED),"retrieving the status of an event object",GetLastError(),__LINE__,__FILE__);

	if(result == WAIT_OBJECT_0)
	{
		return true;
	}
	else
	{
		if(result == WAIT_TIMEOUT)
		{
			return false;
		}
		else
		{
			_ErrorException(true,"retrieving the status of an event object, invalid return value received (see error code)",result,__LINE__,__FILE__);
			return false;
		}
	}
}

/**
 * @brief Changes the value of an event object.
 *
 * @param value True if the event object should be signaled, false if it should be unsignaled.
 */
void ConcurrencyEvent::Set(bool value)
{
	if(value == true)
	{
		BOOL result = SetEvent(eventObject);
		_ErrorException((result == FALSE),"setting an event object to true (signaled)",WSAGetLastError(),__LINE__,__FILE__);
	}
	else
	{
		BOOL result = ResetEvent(eventObject);
		_ErrorException((result == FALSE),"setting an event object to false (unsignaled)",WSAGetLastError(),__LINE__,__FILE__);
	}
}

/**
 * @brief	Waits until the event object is signaled i.e. ConcurrencyEvent::Get() returns true.
 *
 * @param	timeout	Length of time to wait in milliseconds before aborting and returning true.
 *
 * @return	true if the operation timed out and the event is probably not signaled.
 * @return	false if the event is now signaled.
 */
bool ConcurrencyEvent::WaitUntilSignaled( DWORD timeout ) const
{
	DWORD result = WaitForSingleObject(eventObject, timeout);
	_ErrorException((result == WAIT_FAILED),"waiting for an event object to be signaled",WSAGetLastError(),__LINE__,__FILE__);

	return !(result == WAIT_OBJECT_0);
}

/**
 * @brief	Waits until the event object is signaled i.e. ConcurrencyEvent::Get() returns true.
 */
void ConcurrencyEvent::WaitUntilSignaled() const
{
	bool result = WaitUntilSignaled(INFINITE);
	_ErrorException((result == true),"waiting an infinite amount of time for an event object to be signaled",WSAGetLastError(),__LINE__,__FILE__);
}

/**
 * @brief	Retrieves a handle to the event object that this object wraps around.
 *
 * @return	event handle.
 */
HANDLE ConcurrencyEvent::GetEventHandle() const
{
	return eventObject;
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool ConcurrencyEvent::TestClass()
{
	bool problem = false;
	cout << "Testing ConcurrencyEvent class...\n";

	ConcurrencyEvent eventObject(true);

	cout << "Event object status: " << eventObject.Get() << '\n';
	if(eventObject.Get() == false)
	{
		cout << "Get is bad\n";
		problem = true;
	}
	else
	{
		cout << "Get is good\n";
	}

	cout << "Waiting for signaled state..\n";
	eventObject.WaitUntilSignaled();
	cout << "Finished waiting\n";

	cout << "Setting event object to false, then true, then false..\n";
	eventObject.Set(false);
	eventObject.Set(true);
	eventObject.Set(true);
	eventObject.Set(false);
	eventObject.Set(false);

	if(eventObject.Get() == true)
	{
		cout << "Set is bad\n";
		problem = true;
	}
	else
	{
		cout << "Set is good\n";
	}

	cout << "Waiting for event object to be signaled, this will never happen..\n";
	bool timedOut = eventObject.WaitUntilSignaled(1000);
	cout << "Finished waiting, timed out: " << timedOut << '\n';

	if(timedOut == false)
	{
		cout << "WaitUntilSignaled is bad\n";
		problem = true;
	}
	else
	{
		cout << "WaitUntilSignaled is good\n";
	}

	cout << "\n\n";
	return !problem;
}