/*! @file   static_initializer.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   27-06-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

#ifndef PULSAR_STATIC_INITIALIZER_HPP
#define PULSAR_STATIC_INITIALIZER_HPP 1

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: C++
#include <type_traits>

// Pulsar
namespace pul
{
	/*! @brief
	 *
	 *  @tparam _Ty
	 */
	template <typename _Ty>
	class static_initializer
	{
	public:
		/// Constructor
		static_initializer()
		{
			_Ty::__init();
		}
		static_initializer(static_initializer<_Ty> const &) = delete;

		/// Destructor
		~static_initializer()
		{
			_Ty::__terminate();
		}
	};

	/// STATIC-INITIALIZER: Macro -> Initializer
#ifndef PF_DONT_STATIC_INITIALIZE
#	define pf_static_initializer(_Ty, name) pf_decl_static pf_decl_inline static_initializer<_Ty> initializer_;
#else // ^^^ !PF_DONT_STATIC_INITIALIZE ^^^ / vvv PF_DONT_STATIC_INITIALIZE vvv
#	define pf_static_initializer(_Ty)
#endif // PF_DONT_STATIC_INITIALIZE

/// STATIC-INITIALIZER: Macro -> Friendship
#define pf_static_initializer_allow(_Ty) pf_decl_friend static_initializer<_Ty>
}

#endif // !PULSAR_STATIC_INITIALIZER_HPP