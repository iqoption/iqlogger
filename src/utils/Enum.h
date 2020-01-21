//
// Enum.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <boost/integer.hpp>
#include <boost/preprocessor.hpp>

#define CREATE_ENUM(EnumName, EnumSize, EnumUnderlyingType, enumValSeq)                 \
  using EnumUnderlyingType = boost::uint_value_t<BOOST_PP_SEQ_SIZE(enumValSeq)>::least; \
  enum class EnumName : EnumUnderlyingType                                              \
  {                                                                                     \
    BOOST_PP_SEQ_ENUM(enumValSeq)                                                       \
  };                                                                                    \
  constexpr static EnumUnderlyingType EnumSize = BOOST_PP_SEQ_SIZE(enumValSeq);
