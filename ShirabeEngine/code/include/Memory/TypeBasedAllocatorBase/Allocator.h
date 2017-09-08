#ifndef _SHIRABE_CORE__TYPE_BASED_MEMORY_ALLOCATOR_H_
#define _SHIRABE_CORE__TYPE_BASED_MEMORY_ALLOCATOR_H_

#include <stddef.h>
#include <xmemory0>

#include "Memory/STLTypeBase.h"
#include "Memory/TypeBasedAllocatorBase/AllocatorPolicy.h"
#include "Memory/TypeBasedAllocatorBase/AllocatorTraits.h"

namespace Engine {
	namespace Memory {

		template<typename T>
		class Allocator
			: public std::allocator<T> {};

		/// <Summary>
		/// Manages allocation and deallocation of MeasurementChunks.
		/// </Summary>
		template <typename T,
			typename PolicyT = AllocatorPolicyBase<T>,
			typename TraitsT = AllocatorTraitsBase<T>>
			class AllocatorBase
			: public STLTypeBase<T>,
			public PolicyT,
			public TraitsT {
			typedef PolicyT policy_type; // Policy = Strategie, Methode
			typedef TraitsT traits_type; // Traits = Eigenschaften

			public:
				// Allocator typedefs mandatory due to STL-Std.
				// Base definitions in Policy, override for convenience.
				typedef typename PolicyT::size_type       size_type;
				typedef typename PolicyT::difference_type difference_type;
				typedef typename PolicyT::pointer         pointer;
				typedef typename PolicyT::const_pointer   const_pointer;
				typedef typename PolicyT::reference       reference;
				typedef typename PolicyT::const_reference const_reference;
				typedef typename PolicyT::value_type      value_type;

				/* Allocator rebind to convert an allocator<T> to allocator<U> */
				template<typename U>
				struct rebind {
					/* WTF?! Adding template before rebind resolves errors! */
					typedef AllocatorBase<U,
						typename PolicyT::template rebind<U>::other,
						typename TraitsT::template rebind<U>::other> other;
				};

				explicit AllocatorBase() {}
				AllocatorBase(AllocatorBase const& instToCpy) : PolicyT(instToCpy), TraitsT(instToCpy) {}

				template <typename U>
				AllocatorBase(AllocatorBase<U> const&) {}

				template<typename U, typename PolicyU, typename TraitsU>
				AllocatorBase(AllocatorBase<U, PolicyU, TraitsU> const&) {}

				// Allow overloading, since maybe a more specialized allocator
				// should be able to implement this basic behaviour!
				virtual ~AllocatorBase() {}
		};


		// determines if memory from another
		// allocator can be deallocated from this one
		template<typename T, typename PolicyT, typename TraitsT>
		inline bool operator==(AllocatorBase<T, PolicyT, TraitsT> const& lhs,
							   AllocatorBase<T, PolicyT, TraitsT> const& rhs)
		{
			return operator==(static_cast<PolicyT&>(lhs), static_cast<PolicyT&>(rhs));
		};

		template<typename T, typename PolicyT, typename TraitsT,
			typename U, typename PolicyU, typename TraitsU>
			inline bool operator==(AllocatorBase<T, PolicyT, TraitsT> const& lhs,
								   AllocatorBase<U, PolicyU, TraitsU> const& rhs)
		{
			return operator==(static_cast<PolicyT&>(lhs), static_cast<PolicyU&>(rhs));
		};

		template<typename T, typename PolicyT, typename TraitsT, typename _OtherAllocator>
		inline bool operator==(AllocatorBase<T, PolicyT, TraitsT> const& lhs,
							   _OtherAllocator                    const& rhs)
		{
			return operator==(static_cast<PolicyT&>(lhs), rhs);
		};

		template<typename T, typename PolicyT, typename TraitsT>
		inline bool operator!=(AllocatorBase<T, PolicyT, TraitsT> const& lhs,
							   AllocatorBase<T, PolicyT, TraitsT> const& rhs)
		{
			return !operator==(lhs, rhs);
		};

		template<typename T, typename PolicyT, typename TraitsT,
			typename U, typename PolicyU, typename TraitsU>
			inline bool operator!=(AllocatorBase<T, PolicyT, TraitsT> const& lhs,
								   AllocatorBase<U, PolicyU, TraitsU> const& rhs)
		{
			return !operator==(lhs, rhs);
		};

		template<typename T, typename PolicyT, typename TraitsT, typename _OtherAllocator>
		inline bool operator!=(AllocatorBase<T, PolicyT, TraitsT> const& lhs,
							   _OtherAllocator                    const& rhs)
		{
			return !operator==(lhs, rhs);
		};

	}
}
#endif