#pragma once

/**
 * @brief Manages a signal (known as an event) which can be true or false.
 */
class ConcurrencyEvent
{
	HANDLE eventObject;
	bool manualReset;

public:
	void Initialize(bool initialState, bool manualReset);
	ConcurrencyEvent(bool initialState, bool manualReset = true);
	ConcurrencyEvent(const ConcurrencyEvent & obj);
	ConcurrencyEvent & operator=(const ConcurrencyEvent & obj);
	bool operator==(const ConcurrencyEvent & obj) const;
	bool operator!=(const ConcurrencyEvent & obj) const;

	virtual ~ConcurrencyEvent(void);

	bool IsManualReset() const;

	bool Get() const;
	void Set(bool value);

	void WaitUntilSignaled() const;
	bool WaitUntilSignaled(DWORD timeout) const;

	HANDLE GetEventHandle() const;

	static bool TestClass();

};

