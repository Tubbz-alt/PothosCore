//  (C) Copyright Noel Belcourt 2007.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  PGI C++ compiler setup:

#define POTHOS_COMPILER_VERSION __PGIC__##__PGIC_MINOR__
#define POTHOS_COMPILER "PGI compiler version " POTHOS_STRINGIZE(POTHOS_COMPILER_VERSION)

//
// Threading support:
// Turn this on unconditionally here, it will get turned off again later
// if no threading API is detected.
//

#if __PGIC__ >= 11

// options requested by configure --enable-test
#define POTHOS_HAS_PTHREADS
#define POTHOS_HAS_THREADS
#define POTHOS_HAS_PTHREAD_YIELD
#define POTHOS_HAS_NRVO
#define POTHOS_HAS_LONG_LONG

// options --enable-test wants undefined
#undef POTHOS_NO_STDC_NAMESPACE
#undef POTHOS_NO_EXCEPTION_STD_NAMESPACE
#undef POTHOS_DEDUCED_TYPENAME

#define POTHOS_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL
#define POTHOS_NO_TWO_PHASE_NAME_LOOKUP
#define POTHOS_NO_CXX11_AUTO_MULTIDECLARATIONS
#define POTHOS_NO_CXX11_AUTO_DECLARATIONS

#elif __PGIC__ >= 10

// options requested by configure --enable-test
#define POTHOS_HAS_THREADS
#define POTHOS_HAS_NRVO
#define POTHOS_HAS_LONG_LONG
#if defined(linux) || defined(__linux) || defined(__linux__)
#  define POTHOS_HAS_STDINT_H
#endif

// options --enable-test wants undefined
#undef POTHOS_NO_STDC_NAMESPACE
#undef POTHOS_NO_EXCEPTION_STD_NAMESPACE
#undef POTHOS_DEDUCED_TYPENAME

#elif __PGIC__ >= 7

#define POTHOS_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL
#define POTHOS_NO_TWO_PHASE_NAME_LOOKUP
#define POTHOS_NO_SWPRINTF
#define POTHOS_NO_CXX11_AUTO_MULTIDECLARATIONS
#define POTHOS_NO_CXX11_AUTO_DECLARATIONS

#else

#  error "Pgi compiler not configured - please reconfigure"

#endif
//
// C++0x features
//
//   See boost\config\suffix.hpp for BOOST_NO_LONG_LONG
//
#define POTHOS_NO_CXX11_CHAR16_T
#define POTHOS_NO_CXX11_CHAR32_T
#define POTHOS_NO_CXX11_CONSTEXPR
#define POTHOS_NO_CXX11_DECLTYPE
#define POTHOS_NO_CXX11_DECLTYPE_N3276
#define POTHOS_NO_CXX11_DEFAULTED_FUNCTIONS
#define POTHOS_NO_CXX11_DELETED_FUNCTIONS
#define POTHOS_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#define POTHOS_NO_CXX11_EXTERN_TEMPLATE
#define POTHOS_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define POTHOS_NO_CXX11_LAMBDAS
#define POTHOS_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#define POTHOS_NO_CXX11_NOEXCEPT
#define POTHOS_NO_CXX11_NULLPTR
#define POTHOS_NO_CXX11_NUMERIC_LIMITS
#define POTHOS_NO_CXX11_RANGE_BASED_FOR
#define POTHOS_NO_CXX11_RAW_LITERALS
#define POTHOS_NO_CXX11_RVALUE_REFERENCES
#define POTHOS_NO_CXX11_SCOPED_ENUMS
#define POTHOS_NO_SFINAE_EXPR
#define POTHOS_NO_CXX11_STATIC_ASSERT
#define POTHOS_NO_SWPRINTF
#define POTHOS_NO_CXX11_TEMPLATE_ALIASES
#define POTHOS_NO_CXX11_UNICODE_LITERALS
#define POTHOS_NO_CXX11_VARIADIC_TEMPLATES
#define POTHOS_NO_CXX11_VARIADIC_MACROS
#define POTHOS_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX

#define POTHOS_NO_CXX11_HDR_UNORDERED_SET
#define POTHOS_NO_CXX11_HDR_UNORDERED_MAP
#define POTHOS_NO_CXX11_HDR_TYPEINDEX
#define POTHOS_NO_CXX11_HDR_TYPE_TRAITS
#define POTHOS_NO_CXX11_HDR_TUPLE
#define POTHOS_NO_CXX11_HDR_THREAD
#define POTHOS_NO_CXX11_HDR_SYSTEM_ERROR
#define POTHOS_NO_CXX11_HDR_REGEX
#define POTHOS_NO_CXX11_HDR_RATIO
#define POTHOS_NO_CXX11_HDR_RANDOM
#define POTHOS_NO_CXX11_HDR_MUTEX
#define POTHOS_NO_CXX11_HDR_INITIALIZER_LIST
#define POTHOS_NO_CXX11_HDR_FUTURE
#define POTHOS_NO_CXX11_HDR_FORWARD_LIST
#define POTHOS_NO_CXX11_HDR_CONDITION_VARIABLE
#define POTHOS_NO_CXX11_HDR_CODECVT
#define POTHOS_NO_CXX11_HDR_CHRONO
#define POTHOS_NO_CXX11_HDR_ARRAY
#define POTHOS_NO_CXX11_USER_DEFINED_LITERALS
#define POTHOS_NO_CXX11_ALIGNAS
#define POTHOS_NO_CXX11_TRAILING_RESULT_TYPES
#define POTHOS_NO_CXX11_INLINE_NAMESPACES

//
// version check:
// probably nothing to do here?

