//
// ip/impl/address.ipp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_IP_IMPL_ADDRESS_IPP
#define BOOST_ASIO_IP_IMPL_ADDRESS_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>
#include <typeinfo>
#include <boost/throw_exception.hpp>
#include <boost/asio/detail/throw_error.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/system/system_error.hpp>

#include <boost/asio/detail/push_options.hpp>

namespace riakboost{} namespace boost = riakboost; namespace riakboost{
namespace asio {
namespace ip {

address::address()
  : type_(ipv4),
    ipv4_address_(),
    ipv6_address_()
{
}

address::address(const riakboost::asio::ip::address_v4& ipv4_address)
  : type_(ipv4),
    ipv4_address_(ipv4_address),
    ipv6_address_()
{
}

address::address(const riakboost::asio::ip::address_v6& ipv6_address)
  : type_(ipv6),
    ipv4_address_(),
    ipv6_address_(ipv6_address)
{
}

address::address(const address& other)
  : type_(other.type_),
    ipv4_address_(other.ipv4_address_),
    ipv6_address_(other.ipv6_address_)
{
}

#if defined(BOOST_ASIO_HAS_MOVE)
address::address(address&& other)
  : type_(other.type_),
    ipv4_address_(other.ipv4_address_),
    ipv6_address_(other.ipv6_address_)
{
}
#endif // defined(BOOST_ASIO_HAS_MOVE)

address& address::operator=(const address& other)
{
  type_ = other.type_;
  ipv4_address_ = other.ipv4_address_;
  ipv6_address_ = other.ipv6_address_;
  return *this;
}

#if defined(BOOST_ASIO_HAS_MOVE)
address& address::operator=(address&& other)
{
  type_ = other.type_;
  ipv4_address_ = other.ipv4_address_;
  ipv6_address_ = other.ipv6_address_;
  return *this;
}
#endif // defined(BOOST_ASIO_HAS_MOVE)

address& address::operator=(const riakboost::asio::ip::address_v4& ipv4_address)
{
  type_ = ipv4;
  ipv4_address_ = ipv4_address;
  ipv6_address_ = riakboost::asio::ip::address_v6();
  return *this;
}

address& address::operator=(const riakboost::asio::ip::address_v6& ipv6_address)
{
  type_ = ipv6;
  ipv4_address_ = riakboost::asio::ip::address_v4();
  ipv6_address_ = ipv6_address;
  return *this;
}

riakboost::asio::ip::address_v4 address::to_v4() const
{
  if (type_ != ipv4)
  {
    std::bad_cast ex;
    riakboost::throw_exception(ex);
  }
  return ipv4_address_;
}

riakboost::asio::ip::address_v6 address::to_v6() const
{
  if (type_ != ipv6)
  {
    std::bad_cast ex;
    riakboost::throw_exception(ex);
  }
  return ipv6_address_;
}

std::string address::to_string() const
{
  if (type_ == ipv6)
    return ipv6_address_.to_string();
  return ipv4_address_.to_string();
}

std::string address::to_string(riakboost::system::error_code& ec) const
{
  if (type_ == ipv6)
    return ipv6_address_.to_string(ec);
  return ipv4_address_.to_string(ec);
}

address address::from_string(const char* str)
{
  riakboost::system::error_code ec;
  address addr = from_string(str, ec);
  riakboost::asio::detail::throw_error(ec);
  return addr;
}

address address::from_string(const char* str, riakboost::system::error_code& ec)
{
  riakboost::asio::ip::address_v6 ipv6_address =
    riakboost::asio::ip::address_v6::from_string(str, ec);
  if (!ec)
  {
    address tmp;
    tmp.type_ = ipv6;
    tmp.ipv6_address_ = ipv6_address;
    return tmp;
  }

  riakboost::asio::ip::address_v4 ipv4_address =
    riakboost::asio::ip::address_v4::from_string(str, ec);
  if (!ec)
  {
    address tmp;
    tmp.type_ = ipv4;
    tmp.ipv4_address_ = ipv4_address;
    return tmp;
  }

  return address();
}

address address::from_string(const std::string& str)
{
  return from_string(str.c_str());
}

address address::from_string(const std::string& str,
    riakboost::system::error_code& ec)
{
  return from_string(str.c_str(), ec);
}

bool address::is_loopback() const
{
  return (type_ == ipv4)
    ? ipv4_address_.is_loopback()
    : ipv6_address_.is_loopback();
}

bool address::is_unspecified() const
{
  return (type_ == ipv4)
    ? ipv4_address_.is_unspecified()
    : ipv6_address_.is_unspecified();
}

bool address::is_multicast() const
{
  return (type_ == ipv4)
    ? ipv4_address_.is_multicast()
    : ipv6_address_.is_multicast();
}

bool operator==(const address& a1, const address& a2)
{
  if (a1.type_ != a2.type_)
    return false;
  if (a1.type_ == address::ipv6)
    return a1.ipv6_address_ == a2.ipv6_address_;
  return a1.ipv4_address_ == a2.ipv4_address_;
}

bool operator<(const address& a1, const address& a2)
{
  if (a1.type_ < a2.type_)
    return true;
  if (a1.type_ > a2.type_)
    return false;
  if (a1.type_ == address::ipv6)
    return a1.ipv6_address_ < a2.ipv6_address_;
  return a1.ipv4_address_ < a2.ipv4_address_;
}

} // namespace ip
} // namespace asio
} // namespace riakboost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_IP_IMPL_ADDRESS_IPP
