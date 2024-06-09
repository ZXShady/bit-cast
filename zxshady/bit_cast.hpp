
#ifndef ZXSHADY_BIT_CAST_HPP
#define ZXSHADY_BIT_CAST_HPP

#include <array>
#include <type_traits>



#if defined(_MSC_VER) && !defined(__clang__)
#define ZXSHADY_CPP_VER _MSVC_LANG
#else
#define ZXSHADY_CPP_VER __cplusplus
#endif

#if ZXSHADY_CPP_VER >= 201703L
#define ZXSHADY_NODISCARD [[nodiscard]]
#else
#define ZXSHADY_NODISCARD
#endif

#ifdef ZXSHADY_CPP_VER >= 202002L
#include <bit>
#else
#include <cstring>
#endif

namespace zxshady {
	
template<typename T>
struct has_operator_addressof {
private:
    template<typename T2 = T>
    static auto free_check(T2&& t) -> decltype(operator&(t), void(), std::true_type{});
    static auto free_check(...) -> std::false_type;
    template<typename T2 = T>
    static auto member_check(T2&& t) -> decltype(t.operator&(), void(), std::true_type{});
    static auto member_check(...) -> std::false_type;
    template<typename T2 = T>
    static auto check(T2&& t) -> decltype(&t, void(), std::true_type{});
    static auto check(...) -> std::false_type;
public:
    constexpr static bool free_function = 
        decltype(free_check(std::declval<T>()))::value;

    constexpr static bool member_function = 
        decltype(member_check(std::declval<T>()))::value;

    constexpr static bool value = 
        decltype(check(std::declval<T>()))::value;

    constexpr static bool overloaded = member_function || free_function;
};


template<typename T,typename std::enable_if<has_operator_addressof<T>::overloaded,int>::type = 0>
T* addressof(T& t) noexcept {
	return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(t)));	
}


template<typename T,typename std::enable_if<!has_operator_addressof<T>::overloaded,int>::type = 0>
constexpr T* addressof(T& t) noexcept {
	return &t;
}

template<typename T>
void addressof(const T&&) = delete;

template<typename>
struct is_bounded_array : public std::false_type {};

template<typename T,std::size_t N>
struct is_bounded_array<T[N]> : public std::true_type {};

template<typename>
struct is_unbounded_array : public std::false_type {};

template<typename T>
struct is_unbounded_array<T[]> : public std::true_type {};

template<typename>
struct to_std_array{};

template<typename T,std::size_t N>
struct to_std_array<T[N]> {using type = std::array<T,N>;};

template<typename To,typename From,typename std::enable_if<!std::is_array<To>::value,int>::type = 0>
#ifdef __cpp_lib_bit_cast
constexpr
#endif
ZXSHADY_NODISCARD
To bit_cast(const From& from) noexcept {
	static_assert(sizeof(To) == sizeof(From),"Size mismatch.");
	static_assert(std::is_trivially_copyable<To>::value,"To must be trivially copyable.");
	static_assert(std::is_trivially_copyable<From>::value,"From must be trivially copyable.");
#ifdef __cpp_lib_bit_cast
    return std::bit_cast<To>(from);
#else
	To to;
	std::memcpy(zxshady::addressof(to),zxshady::addressof(from),sizeof(From));
	return to;
#endif
}

template<typename To,typename From,typename std::enable_if<is_bounded_array<To>::value,int>::type = 0>
#ifdef __cpp_lib_bit_cast
constexpr
#endif
ZXSHADY_NODISCARD
typename to_std_array<To>::type bit_cast(const From& from) noexcept {
	static_assert(sizeof(To) == sizeof(From),"Size mismatch.");
	static_assert(std::is_trivially_copyable<To>::value,"To must be trivially copyable.");
	static_assert(std::is_trivially_copyable<From>::value,"From must be trivially copyable.");

#ifdef __cpp_lib_bit_cast
    return std::bit_cast<typename to_std_array<To>::type>(from);
#else
	typename to_std_array<To>::type to;
	std::memcpy(zxshady::addressof(to[0]),zxshady::addressof(from),sizeof(From));
	return to;
#endif
}

template<typename To,typename From,typename std::enable_if<is_unbounded_array<To>::value,int>::type = 0>
#ifdef __cpp_lib_bit_cast
constexpr
#endif
ZXSHADY_NODISCARD
std::array<typename std::remove_extent<To>::type,
	(sizeof(From) / sizeof(typename std::remove_extent<To>::type))> bit_cast(const From& from) noexcept {
		
	using noextent = typename std::remove_extent<To>::type;

	static_assert(sizeof(From) % sizeof(noextent) == 0, "zxshady::bit_cast<To[]>(From) an array of suitable size of type To must be equal to sizeof(From)");
	static_assert(std::is_trivially_copyable<To>::value, "zxshady::bit_cast<To>(From) To must be trivially copyable");
	static_assert(std::is_trivially_copyable<From>::value, "zxshady::bit_cast<To>(From) From must be trivially copyable");
#ifdef __cpp_lib_bit_cast
	return std::bit_cast<std::array<noextent,(sizeof(From) / sizeof(noextent))>>(from);
#else
	std::array<noextent,(sizeof(From) / sizeof(noextent))> to;
	std::memcpy(zxshady::addressof(to[0]), zxshady::addressof(from), sizeof(From));
	return to;
#endif
}

}

#endif // !defined(ZXSHADY_BIT_CAST_HPP)
