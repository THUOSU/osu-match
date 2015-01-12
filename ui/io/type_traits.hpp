/*!
* THUOSU Library Defination
*
* \brief type traits for user-defined struct member check
*
*/

#ifndef THUOSU_TYPE_TRAITS_HPP
#define THUOSU_TYPE_TRAITS_HPP

#include <type_traits>

//! \brief create template has_xxx<T, Type> for checking whether type T has a 'Type' field xxx
#define THUOSU_CREATE_HAS_MEMBER(_member) \
	template <typename T, typename Type, bool = std::is_class<T>::value> \
	struct has_##_member \
	{ \
	private: \
		template <Type T::*> struct helper; \
		template <typename U> \
		static std::false_type test(...); \
		template <typename U> \
		static std::true_type test(helper<&U::_member> *); \
		typedef decltype(test<T>(nullptr)) testresult; \
	public: \
		static const bool value = testresult::value; \
	}; \
	template <typename T, typename Type> \
	struct has_##_member<T, Type, false> : std::false_type{}; \

#endif // THUOSU_TYPE_TRAITS_HPP