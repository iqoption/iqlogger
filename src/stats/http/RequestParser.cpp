//
// RequestParser.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "RequestParser.h"
#include "Request.h"

using namespace iqlogger::stats::http;

RequestParser::RequestParser() : m_state(RequestParser::state::method_start) {}

void RequestParser::reset() {
  m_state = RequestParser::state::method_start;
}

RequestParser::result_type RequestParser::consume(Request& req, char input) {
  switch (m_state) {
  case state::method_start:

    if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return result_type::bad;
    } else {
      m_state = state::method;
      req.method.push_back(input);
      return result_type::indeterminate;
    }

  case state::method:

    if (input == ' ') {
      m_state = state::uri;
      return result_type::indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return result_type::bad;
    } else {
      req.method.push_back(input);
      return result_type::indeterminate;
    }

  case state::uri:

    if (input == ' ') {
      m_state = state::http_version_h;
      return result_type::indeterminate;
    } else if (is_ctl(input)) {
      return result_type::bad;
    } else {
      req.uri.push_back(input);
      return result_type::indeterminate;
    }

  case state::http_version_h:

    if (input == 'H') {
      m_state = state::http_version_t_1;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::http_version_t_1:

    if (input == 'T') {
      m_state = state::http_version_t_2;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::http_version_t_2:

    if (input == 'T') {
      m_state = state::http_version_p;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::http_version_p:

    if (input == 'P') {
      m_state = state::http_version_slash;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::http_version_slash:

    if (input == '/') {
      req.http_version_major = 0;
      req.http_version_minor = 0;
      m_state = state::http_version_major_start;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::http_version_major_start:

    if (is_digit(input)) {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      m_state = state::http_version_major;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::http_version_major:

    if (input == '.') {
      m_state = state::http_version_minor_start;
      return result_type::indeterminate;
    } else if (is_digit(input)) {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::http_version_minor_start:

    if (is_digit(input)) {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      m_state = state::http_version_minor;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::http_version_minor:

    if (input == '\r') {
      m_state = state::expecting_newline_1;
      return result_type::indeterminate;
    } else if (is_digit(input)) {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::expecting_newline_1:

    if (input == '\n') {
      m_state = state::header_line_start;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::header_line_start:

    if (input == '\r') {
      m_state = state::expecting_newline_3;
      return result_type::indeterminate;
    } else if (!req.headers.empty() && (input == ' ' || input == '\t')) {
      m_state = state::header_lws;
      return result_type::indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return result_type::bad;
    } else {
      req.headers.push_back(Header());
      req.headers.back().name.push_back(input);
      m_state = state::header_name;
      return result_type::indeterminate;
    }

  case state::header_lws:

    if (input == '\r') {
      m_state = state::expecting_newline_2;
      return result_type::indeterminate;
    } else if (input == ' ' || input == '\t') {
      return result_type::indeterminate;
    } else if (is_ctl(input)) {
      return result_type::bad;
    } else {
      m_state = state::header_value;
      req.headers.back().value.push_back(input);
      return result_type::indeterminate;
    }

  case state::header_name:

    if (input == ':') {
      m_state = state::space_before_header_value;
      return result_type::indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return result_type::bad;
    } else {
      req.headers.back().name.push_back(input);
      return result_type::indeterminate;
    }

  case state::space_before_header_value:

    if (input == ' ') {
      m_state = state::header_value;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::header_value:

    if (input == '\r') {
      m_state = state::expecting_newline_2;
      return result_type::indeterminate;
    } else if (is_ctl(input)) {
      return result_type::bad;
    } else {
      req.headers.back().value.push_back(input);
      return result_type::indeterminate;
    }

  case state::expecting_newline_2:

    if (input == '\n') {
      m_state = state::header_line_start;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }

  case state::expecting_newline_3:

    return (input == '\n') ? result_type::good : result_type::bad;

  default:

    return result_type::bad;
  }
}

bool RequestParser::is_char(int c) {
  return c >= 0 && c <= 127;
}

bool RequestParser::is_ctl(int c) {
  return (c >= 0 && c <= 31) || (c == 127);
}

bool RequestParser::is_tspecial(int c) {
  switch (c) {
  case '(':
  case ')':
  case '<':
  case '>':
  case '@':
  case ',':
  case ';':
  case ':':
  case '\\':
  case '"':
  case '/':
  case '[':
  case ']':
  case '?':
  case '=':
  case '{':
  case '}':
  case ' ':
  case '\t':
    return true;
  default:
    return false;
  }
}

bool RequestParser::is_digit(int c) {
  return c >= '0' && c <= '9';
}
