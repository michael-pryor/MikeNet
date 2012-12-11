#pragma once
#include "FullInclude.h"
#include "StdComparator.h"
#include "NetServerClient.h"
#include "NetAddress.h"
#include "ComparatorNetAddress.h"

/**
 * @brief	Sorts NetServerClient objects by their UDP address.
 *
 * Can also be used to search for a specific UDP address within a StoreVector of ServerClient objects.
 *
 * @author	Michael Pryor
 * @date	12/04/2011.
 */

// TODO: Does not get called because inheritance doesn't work with templates...
class ComparatorServerClientFindByAddressUDP : public ComparatorNetAddress
{
	bool sorting;

public:
	/**
	 * Constructor, used when searching for a client with the specified UDP address.
	 *
	 * @param sorting If true then the comparator will compare. 
	 */
	ComparatorServerClientFindByAddressUDP(bool sorting)
	{
		this->sorting = sorting;
	}

private:

	/**
	 * @brief	Determine whether this object is being used to search for a specific address.
	 *
	 * @return	true if searching for specific address, false if not. 
	 */
	bool IsSearchingForSpecificAddress() const
	{
		return !sorting;
	}
public:
	/**
	 * @brief	Compares two non NULL objects.
	 *
	 * @param p_object1 Object to be compared with addressToSearchFor or @a object2. Must not be NULL and must be of ServerClient type.
	 * @param p_object2 Object to be compared with @a object1. When sorting this parameter is used, when
	 * performing a binary search for a specific address this parameter is ignored. Must be of ServerClient type.
	 *
	 * @return	true if the weight of object1's address is less than that of object2. False otherwise.
	 */
	bool Compare(const void * p_object1, const void * p_object2) const
	{
		const NetServerClient * object1 = static_cast<const NetServerClient*>(p_object1);
		const NetAddress & address1 = object1->GetConnectedAddressUDP();

		const NetAddress * addressPtr2;
		if(IsSearchingForSpecificAddress())
		{
			addressPtr2 = static_cast<const NetAddress*>(p_object2);
		}
		else
		{
			const NetServerClient * object2 = static_cast<const NetServerClient*>(p_object2);
			addressPtr2 = &object2->GetConnectedAddressUDP();
		}
		const NetAddress & address2 = *addressPtr2;
		
		__int64 weightAddress1 = GetWeight(&address1);
		__int64 weightAddress2 = GetWeight(&address2);

		// The weights only account for the IP address,
		// now we compare the ports.
		if(weightAddress1 == weightAddress2)
		{
			return address1.GetPort() < address2.GetPort();
		}
		else
		{
			// IP addresses are different.
			return weightAddress1 < weightAddress2;
		}
	}

};