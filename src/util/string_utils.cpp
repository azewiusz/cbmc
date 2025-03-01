/*******************************************************************\

Module:

Author: Daniel Poetzl

\*******************************************************************/

#include "string_utils.h"
#include "exception_utils.h"
#include "invariant.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <iomanip>

/// Remove all whitespace characters from either end of a string. Whitespace
/// in the middle of the string is left unchanged
/// \param s: the string to strip
/// \return The stripped string
std::string strip_string(const std::string &s)
{
  auto pred=[](char c){ return std::isspace(c); };

  std::string::const_iterator left
    =std::find_if_not(s.begin(), s.end(), pred);
  if(left==s.end())
    return "";

  std::string::size_type i=std::distance(s.begin(), left);

  std::string::const_reverse_iterator right
    =std::find_if_not(s.rbegin(), s.rend(), pred);
  std::string::size_type j=std::distance(right, s.rend())-1;

  return s.substr(i, (j-i+1));
}

void split_string(
  const std::string &s,
  char delim,
  std::vector<std::string> &result,
  bool strip,
  bool remove_empty)
{
  PRECONDITION(result.empty());
  // delim can't be a space character if using strip
  PRECONDITION(!std::isspace(delim) || !strip);

  if(s.empty())
  {
    result.push_back("");
    return;
  }

  std::string::size_type n=s.length();
  INVARIANT(n > 0, "Empty string case should already be handled");

  std::string::size_type start=0;
  std::string::size_type i;

  for(i=0; i<n; i++)
  {
    if(s[i]==delim)
    {
      std::string new_s=s.substr(start, i-start);

      if(strip)
        new_s=strip_string(new_s);

      if(!remove_empty || !new_s.empty())
        result.push_back(new_s);

      start=i+1;
    }
  }

  std::string new_s=s.substr(start, n-start);

  if(strip)
    new_s=strip_string(new_s);

  if(!remove_empty || !new_s.empty())
    result.push_back(new_s);

  if(result.empty())
    result.push_back("");
}

void split_string(
  const std::string &s,
  char delim,
  std::string &left,
  std::string &right,
  bool strip)
{
  // delim can't be a space character if using strip
  PRECONDITION(!std::isspace(delim) || !strip);

  std::vector<std::string> result;

  split_string(s, delim, result, strip);
  if(result.size() != 2)
  {
    throw deserialization_exceptiont{"expected string '" + s +
                                     "' to contain two substrings "
                                     "delimited by " +
                                     delim + " but has " +
                                     std::to_string(result.size())};
  }

  left=result[0];
  right=result[1];
}

std::vector<std::string> split_string(
  const std::string &s,
  char delim,
  bool strip,
  bool remove_empty)
{
  std::vector<std::string> result;
  split_string(s, delim, result, strip, remove_empty);
  return result;
}

std::string trim_from_last_delimiter(
  const std::string &s,
  const char delim)
{
  std::string result;
  const size_t index=s.find_last_of(delim);
  if(index!=std::string::npos)
    result=s.substr(0, index);
  return result;
}

std::string escape(const std::string &s)
{
  std::string result;

  for(std::size_t i=0; i<s.size(); i++)
  {
    if(s[i]=='\\' || s[i]=='"')
      result+='\\';

    result+=s[i];
  }

  return result;
}

std::string escape_non_alnum(const std::string &to_escape)
{
  std::ostringstream escaped;
  for(auto &ch : to_escape)
  {
    if(ch == '_')
      escaped << "__";
    else if(isalnum(ch))
      escaped << ch;
    else
      escaped << '_' << std::hex << std::setfill('0') << std::setw(2)
              << (unsigned int)ch;
  }
  return escaped.str();
}
