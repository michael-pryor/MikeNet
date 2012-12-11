#pragma once

namespace PointerConverter
{
	/**
	 * @return a reference from @a intPtr of the generic type.
	 */
	template<typename T1>
	T1 & GetRefFromInt(__int64 intPtr)
	{
		return *(T1*)intPtr;
	}

	/**
	 * @return a pointer from @a intPtr of the generic type.
	 */
	template<typename T2>
	T2 * GetPtrFromInt(__int64 intPtr)
	{
		return (T2*)intPtr;
	}

	/**
	 * @return a reference from @a voidPtr of the generic type.
	 */
	template<typename T3>
	T3 & GetRef(const void * voidPtr)
	{
		return *(T3*)voidPtr;
	}

	/**
	 * @return a pointer from @a voidPtr of the generic type.
	 */
	template<typename T4>
	T4 * GetPtr(const void * voidPtr)
	{
		return (T4*)voidPtr;
	}
}