/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// extended_type_info_no_rtti.cpp: specific implementation of type info
// that is NOT based on typeid

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstring>
#include <cstddef> // NULL
#include <Pothos/serialization/impl/assert.hpp>

#include <Pothos/serialization/impl/config.hpp>
#if defined(POTHOS_NO_STDC_NAMESPACE)
namespace std{ using ::strcmp; }
#endif

#define POTHOS_SERIALIZATION_SOURCE
#include <Pothos/serialization/extended_type_info_no_rtti.hpp>

#define EXTENDED_TYPE_INFO_NO_RTTI_KEY 2

namespace Pothos { 
namespace serialization { 
namespace no_rtti_system { 

POTHOS_SERIALIZATION_DECL(POTHOS_PP_EMPTY())  
extended_type_info_no_rtti_0::extended_type_info_no_rtti_0(
    const char * key
) :
    extended_type_info(EXTENDED_TYPE_INFO_NO_RTTI_KEY, key)
{}

POTHOS_SERIALIZATION_DECL(bool)
extended_type_info_no_rtti_0::is_less_than(
    const Pothos::serialization::extended_type_info &rhs) const 
{
    // shortcut for common case
    if(this == & rhs)
        return false;
    const char * l = get_key();
    const char * r = rhs.get_key();
    // if this assertion is triggered, it could mean one of the following
    // a) This class was never exported - make sure all calls which use
    // this method of type id are in fact exported.
    // b) This class was used (e.g. serialized through a pointer) before
    // it was exported.  Make sure that classes which use this method
    // of type id are NOT "automatically" registered by serializating 
    // through a pointer to the to most derived class.  OR make sure
    // that the BOOST_CLASS_EXPORT is included in every file
    // which does this.
    POTHOS_ASSERT(NULL != l);
    POTHOS_ASSERT(NULL != r);
    return std::strcmp(l, r) < 0;
}

POTHOS_SERIALIZATION_DECL(bool)
extended_type_info_no_rtti_0::is_equal(
    const Pothos::serialization::extended_type_info &rhs) const 
{
    // shortcut for common case
    if(this == & rhs)
        return true;
    // null keys don't match with anything
    const char * l = get_key();
    POTHOS_ASSERT(NULL != l);
    if(NULL == l)
        return false;
    const char * r = rhs.get_key();
    POTHOS_ASSERT(NULL != r);
    if(NULL == r)
        return false;
    return 0 == std::strcmp(l, r);
}

POTHOS_SERIALIZATION_DECL(POTHOS_PP_EMPTY())  
extended_type_info_no_rtti_0::~extended_type_info_no_rtti_0()
{}

} // namespece detail
} // namespace serialization
} // namespace boost
