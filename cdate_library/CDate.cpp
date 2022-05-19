#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

//=================================================================================================
// a dummy exception class, keep this implementation
class InvalidDateException : public invalid_argument
{
public:
  InvalidDateException()
      : invalid_argument("invalid date or format")
  {
  }
};
//=================================================================================================
// date_format manipulator - a dummy implementation. Keep this code unless you implement your
// own working manipulator.
ios_base &(*date_format(const char *fmt))(ios_base &x)
{
  return [](ios_base &ios) -> ios_base &
  { return ios; };
}
//=================================================================================================
/**
 * @brief CDate is a class that holds date a perform numeric manipulations with it (overloading operators)
 * 
 */
class CDate
{

  /*


  ===========================private====================================


  */
public:
/**
 * @brief Construct a new CDate object
 * 
 */
  CDate(int year, int month, int day)
  {

    if (!isValidDate(year, month, day))
      throw InvalidDateException();

    // initializes the date in tm struct
    date.tm_year = year - 1900;
    date.tm_mon = month - 1;
    date.tm_mday = day;
    date.tm_gmtoff = 0;

    // time_epoch is the date in seconds since epoch (1970)
    time_epoch = std::mktime(&date);
  }
  //===========================================================================================================

  //========================================overloaded operators===============================================

  //==================================
  // adding days to already initialized date
  CDate &operator+(int days)
  {
    date.tm_mday += days;
    time_epoch = std::mktime(&date);
    return *this;
  }
//==================================
//substracting days by int
  CDate &operator-(int days)
  {
    date.tm_mday -= days;
    time_epoch = std::mktime(&date);
    return *this;
  }
  //==================================
  //difference in days between two CDate objects
  int operator-(CDate &rhs)
  {
    int seconds = difftime(time_epoch, rhs.time_epoch);
    int days = seconds / DAY;

    return days;
  }
  //==================================
  CDate &operator++()
  {
    date.tm_mday += 1;
    time_epoch = std::mktime(&date);

    return *this;
  }
  //==================================
  CDate operator++(int)
  {
    CDate temp = *this;
    ++(*this);

    return temp;
  }
  //==================================
  CDate &operator--()
  {
    date.tm_mday -= 1;
    time_epoch = std::mktime(&date);

    return *this;
  }
  //==================================
  CDate operator--(int)
  {
    CDate temp = *this;
    --(*this);

    return temp;
  }
  //==================================
  CDate operator=(CDate rhs)
  {
    date = rhs.date;

    return *this;
  }
  //==================================
  bool operator==(CDate &rhs)
  {
    return !difftime(time_epoch, rhs.time_epoch);
  }
//==================================
  bool operator!=(CDate &rhs)
  {
    return difftime(time_epoch, rhs.time_epoch);
  }
  //==================================
  bool operator<=(CDate &rhs)
  {
    if (time_epoch == rhs.time_epoch)
      return true;
    if (time_epoch > rhs.time_epoch)
      return false;
    return true;
  }
  //==================================
  bool operator>=(CDate &rhs)
  {
    if (time_epoch == rhs.time_epoch)
      return true;
    if (time_epoch < rhs.time_epoch)
      return false;
    return true;
  }
  //==================================
  bool operator>(CDate &rhs)
  {
    if (time_epoch > rhs.time_epoch)
      return true;
    return false;
  }
  //==================================
  bool operator<(CDate &rhs)
  {
    if (time_epoch < rhs.time_epoch)
      return true;
    return false;
  }
  //===========================================================================================================

  //=============================================friend functions==============================================
  friend std::ostream &operator<<(std::ostream &stream, const CDate &_date);
  friend std::istream &operator>>(std::istream &stream, CDate &_date);

  //===========================================================================================================
  /**
   * @brief check if date is valid
   *
   * @return true   --> date is valid
   * @return false  --> date is not valid
   */
  bool isValidDate(int year, int month, int day)
  {

    if (year > MAX_VALID_YR || year < MIN_VALID_YR)
      return false;
    if (day > 31 || day < 1)
      return false;
    if (month > 12 || month < 1)
      return false;

    // February Leap Year checking
    if (month == 2)
    {
      if (isLeapYear(year))
        return day <= 29;
      else
        return day <= 28;
    }
    // these months have 30 days
    if (month == 4 || month == 6 ||
        month == 9 || month == 11)
      return day <= 30;

    // if everything fails - date is correct
    return true;
  }
  //===========================================================================================================
  /**
   * @brief leap year check   --> return true if year is leap year
   *                          --> return false if not
   *
   */
  bool isLeapYear(int year)
  {
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
      return true;

    return false;
  }
  /*


  ===========================private====================================


  */
private:
  //===================variables declaration============================

  // std::tm - struct to hold a date
  std::tm date{0};

  // mktime() - seconds since epoch to specified date
  std::time_t time_epoch = std::mktime(&date);

  const int DAY = 86400;         // number of seconds in a days
  const int MAX_VALID_YR = 2030; // used in valid date checking
  const int MIN_VALID_YR = 2000; // used in valid date checking
};

//===========================================================================================================
/**
 * @brief overloaded << operator - prints out date object in wanted format
 *
 * @param out             --> output stream
 * @param _date           --> std::tm object - can hold a date - from <cdate>
 * @return std::ostream&  --> reference to created stream
 */
std::ostream &operator<<(std::ostream &out, const CDate &_date)
{
  // adding + 1900 to the year - because time in std::time_ is measured in seconds since epoch - 1.1.1900 (month is added for the same reason)
  out << _date.date.tm_year + 1900 << "-"
      << std::setfill('0') << std::setw(2) << _date.date.tm_mon + 1 << "-"
      << std::setfill('0') << std::setw(2) << _date.date.tm_mday;

  return out;
}
//===========================================================================================================
/**
 * @brief overloaded >> - takes input from a input stream checks if it is a right format and a valid date and returns it
 *
 * @param stream          --> input stream
 * @param _date           --> std::tm object - can hold a date - from <cdate>
 * @return std::istream&  --> reference to created input stream
 */
std::istream &operator>>(std::istream &stream, CDate &_date)
{
  int year, month, day;
  char c1, c2;

  // input format check
  if ((stream >> year >> c1 >> month >> c2 >> day) && c1 == '-' && c2 == '-')
  {
    // valid date check
    if (!_date.isValidDate(year, month, day))
    {
      // if format is wrong - set the failbit
      stream.setstate(std::ios::failbit);
      return stream;
    }
    _date.date.tm_year = year - 1900;
    _date.date.tm_mon = month - 1;
    _date.date.tm_mday = day;

    return stream;
  }

  // wrong format was provided
  else
  {
    stream.setstate(std::ios::failbit);
    return stream;
  }
}
#ifndef __PROGTEST__
int main(void)
{
  ostringstream oss;
  istringstream iss;

  CDate a(2000, 1, 2);
  CDate b(2010, 2, 3);
  CDate c(2004, 2, 10);
  oss.str("");
  oss << a;
  assert(oss.str() == "2000-01-02");
  oss.str("");
  oss << b;
  assert(oss.str() == "2010-02-03");
  oss.str("");
  oss << c;
  assert(oss.str() == "2004-02-10");
  a = a + 1500;
  oss.str("");
  oss << a;
  assert(oss.str() == "2004-02-10");
  b = b - 2000;
  oss.str("");
  oss << b;
  assert(oss.str() == "2004-08-13");
  assert(b - a == 185);
  assert((b == a) == false);
  assert((b != a) == true);
  assert((b <= a) == false);
  assert((b < a) == false);
  assert((b >= a) == true);
  assert((b > a) == true);
  assert((c == a) == true);
  assert((c != a) == false);
  assert((c <= a) == true);
  assert((c < a) == false);
  assert((c >= a) == true);
  assert((c > a) == false);
  a = ++c;
  oss.str("");
  oss << a << " " << c;
  assert(oss.str() == "2004-02-11 2004-02-11");
  a = --c;
  oss.str("");
  oss << a << " " << c;
  assert(oss.str() == "2004-02-10 2004-02-10");
  a = c++;
  oss.str("");
  oss << a << " " << c;
  assert(oss.str() == "2004-02-10 2004-02-11");
  a = c--;
  oss.str("");
  oss << a << " " << c;
  assert(oss.str() == "2004-02-11 2004-02-10");
  iss.clear();
  iss.str("2015-09-03");
  assert((iss >> a));
  oss.str("");
  oss << a;
  assert(oss.str() == "2015-09-03");
  a = a + 70;
  oss.str("");
  oss << a;
  assert(oss.str() == "2015-11-12");

  CDate d(2000, 1, 1);
  try
  {
    CDate e(2000, 32, 1);
    assert("No exception thrown!" == nullptr);
  }
  catch (...)
  {
  }
  iss.clear();
  iss.str("2000-12-33");
  assert(!(iss >> d));
  oss.str("");
  oss << d;
  assert(oss.str() == "2000-01-01");
  iss.clear();
  iss.str("2000-11-31");
  assert(!(iss >> d));
  oss.str("");
  oss << d;
  assert(oss.str() == "2000-01-01");
  iss.clear();
  iss.str("2000-02-29");
  assert((iss >> d));
  oss.str("");
  oss << d;
  assert(oss.str() == "2000-02-29");
  iss.clear();
  iss.str("2001-02-29");
  assert(!(iss >> d));
  oss.str("");
  oss << d;
  assert(oss.str() == "2000-02-29");

  //-----------------------------------------------------------------------------
  // bonus test examples
  //-----------------------------------------------------------------------------
  // CDate f ( 2000, 5, 12 );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2000-05-12" );
  // oss . str ("");
  // oss << date_format ( "%Y/%m/%d" ) << f;
  // assert ( oss . str () == "2000/05/12" );
  // oss . str ("");
  // oss << date_format ( "%d.%m.%Y" ) << f;
  // assert ( oss . str () == "12.05.2000" );
  // oss . str ("");
  // oss << date_format ( "%m/%d/%Y" ) << f;
  // assert ( oss . str () == "05/12/2000" );
  // oss . str ("");
  // oss << date_format ( "%Y%m%d" ) << f;
  // assert ( oss . str () == "20000512" );
  // oss . str ("");
  // oss << date_format ( "hello kitty" ) << f;
  // assert ( oss . str () == "hello kitty" );
  // oss . str ("");
  // oss << date_format ( "%d%d%d%d%d%d%m%m%m%Y%Y%Y%%%%%%%%%%" ) << f;
  // assert ( oss . str () == "121212121212050505200020002000%%%%%" );
  // oss . str ("");
  // oss << date_format ( "%Y-%m-%d" ) << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-01-1" );
  // assert ( ! ( iss >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-1-01" );
  // assert ( ! ( iss >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-001-01" );
  // assert ( ! ( iss >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2000-05-12" );
  // iss . clear ();
  // iss . str ( "2001-01-02" );
  // assert ( ( iss >> date_format ( "%Y-%m-%d" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2001-01-02" );
  // iss . clear ();
  // iss . str ( "05.06.2003" );
  // assert ( ( iss >> date_format ( "%d.%m.%Y" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2003-06-05" );
  // iss . clear ();
  // iss . str ( "07/08/2004" );
  // assert ( ( iss >> date_format ( "%m/%d/%Y" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2004-07-08" );
  // iss . clear ();
  // iss . str ( "2002*03*04" );
  // assert ( ( iss >> date_format ( "%Y*%m*%d" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2002-03-04" );
  // iss . clear ();
  // iss . str ( "C++09format10PA22006rulez" );
  // assert ( ( iss >> date_format ( "C++%mformat%dPA2%Yrulez" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2006-09-10" );
  // iss . clear ();
  // iss . str ( "%12%13%2010%" );
  // assert ( ( iss >> date_format ( "%%%m%%%d%%%Y%%" ) >> f ) );
  // oss . str ("");
  // oss << f;
  // assert ( oss . str () == "2010-12-13" );

  // CDate g ( 2000, 6, 8 );
  // iss . clear ();
  // iss . str ( "2001-11-33" );
  // assert ( ! ( iss >> date_format ( "%Y-%m-%d" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "29.02.2003" );
  // assert ( ! ( iss >> date_format ( "%d.%m.%Y" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "14/02/2004" );
  // assert ( ! ( iss >> date_format ( "%m/%d/%Y" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "2002-03" );
  // assert ( ! ( iss >> date_format ( "%Y-%m" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "hello kitty" );
  // assert ( ! ( iss >> date_format ( "hello kitty" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "2005-07-12-07" );
  // assert ( ! ( iss >> date_format ( "%Y-%m-%d-%m" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-06-08" );
  // iss . clear ();
  // iss . str ( "20000101" );
  // assert ( ( iss >> date_format ( "%Y%m%d" ) >> g ) );
  // oss . str ("");
  // oss << g;
  // assert ( oss . str () == "2000-01-01" );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
