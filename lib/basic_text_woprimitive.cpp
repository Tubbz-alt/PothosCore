/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_text_woprimitive.cpp:

// (C) Copyright 2004 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <Pothos/serialization/impl/config.hpp>

#ifdef POTHOS_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else

#if (defined _MSC_VER) && (_MSC_VER == 1200)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

#include <ostream>

#define POTHOS_WARCHIVE_SOURCE
#include <Pothos/archive/detail/auto_link_warchive.hpp>
#include <Pothos/archive/impl/basic_text_oprimitive.ipp>

namespace Pothos {
namespace archive {

template class basic_text_oprimitive<std::wostream> ;

} // namespace archive
} // namespace boost

#endif // BOOST_NO_STD_WSTREAMBUF
