#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stdexcept>
#include <algorithm>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class CDataType
{
public:
  
  CDataType(int _size) : size(_size){};
  virtual ~CDataType() = default;

  void operator= (const CDataType&) = delete;

  bool operator==(const CDataType& rhs) const
  {
    return (compare(*this, rhs));
  }

  bool operator!=(const CDataType& rhs) const
  {
   return (!compare(*this, rhs));
  }

  friend ostream &operator<<(ostream &out, const CDataType &a)
  {
    a.print(out);
    return out;
  };
  //virtual int type() const = 0;
  virtual std::shared_ptr<CDataType> clone() const = 0;
  virtual const size_t &getSize() const = 0;
  virtual void print(std::ostream &out) const = 0;
  virtual bool compare(const CDataType& lhs,const CDataType& rhs) const = 0;

  size_t size;
};

//============================================
//          CDataTypeInt : CDataType
//============================================

/**
 * @brief this data type holds int value, it has all the operations of class CDataType
 *
 */

class CDataTypeInt : public CDataType
{
public:
  CDataTypeInt()
      : CDataType(4) {}

  const size_t &getSize()const override { return size; }
  
  shared_ptr<CDataType> clone() const override{return std::shared_ptr<CDataType>(new CDataTypeInt(*this));}
  
  bool compare(const CDataType& this_object, const CDataType& rhs) const override
  {
    if (typeid(this_object) == typeid(rhs) )
    {
      //cout << "CDataTypeInt - MATCH in type" << endl;
      return true;
    }
    //cout << "CDataTypeInt - DIFFERS in type" << endl;
    return false;
  }

protected:
  void print(std::ostream &out) const override
  {
    out << "int";
  }
};

//============================================
//          CDataTypeDouble : CDataType
//============================================
/**
 * @brief this data type holds double value, it has all the operations of class CDataType
 *
 */
class CDataTypeDouble : public CDataType
{
public:
  CDataTypeDouble()
      : CDataType(8){}

  const size_t &getSize() const override { return size; }
  shared_ptr<CDataType> clone() const override{return shared_ptr<CDataType>(new CDataTypeDouble(*this));}
  /**
   * @brief operator == checks if data type given is CDataTypeInt
   *
   * @tparam T_     --> template -> can be replaced by any datatype
   * @param rhs     --> right hand side
   */
  bool compare(const CDataType& this_object, const CDataType& rhs) const override
  {
    if (typeid(this_object) == typeid(rhs) )
    {
      //cout << "CDataTypeDouble - MATCH in type" << endl;
      return true;
    }
    //cout << "CDataTypeDouble - DIFFERS in type" << endl;
    return false;
  }

protected:
  void print(std::ostream &out) const override
  {
    out << "double";
  }
};

//============================================
//          CDataTypeEnum : CDataType
//============================================

/**
 * @brief This data type holds
 *
 */
class CDataTypeEnum : public CDataType
{
public:
  CDataTypeEnum()
      : CDataType(4) {}

  const size_t &getSize() const override { return size; }
  std::shared_ptr<CDataType> clone() const override {return std::shared_ptr<CDataType>(new CDataTypeEnum(*this));}

  /**
   * @brief operator == checks if data type given is CDataTypeEnum and both have the same enums;
   *
   * @tparam T_     --> template -> can be replaced by any datatype
   * @param rhs     --> right hand side
   */
  bool compare(const CDataType& this_object, const CDataType& rhs) const override
  {
     if (typeid(this_object) != typeid(rhs) )
     {
        //cout << "CDataTypeEnum - DIFFERS in type" << endl;
        return false;
     }
     const CDataTypeEnum& enum_rhs = dynamic_cast<const CDataTypeEnum&>(rhs);
     auto it2 = enum_rhs.table.begin();
    for (auto it = table.begin(); it != table.end(); it++)
    {
      if (it->first != it2->first)
      {
        //cout << "CDataTypeEnum - DIFFERS in string name" << endl;
        return false;
      }
      it2++;
    }
    //cout << "CDataTypeEnum - MATCH in type and content" << endl;
    return true;
  }


  /**
   * @brief adds another enum, if the enum already exists - exception is thrown
   *
   * @return CDataTypeDouble&
   */
  CDataTypeEnum& add(const string &str)
  {
    auto it = table.find(str);
    if (it != table.end())
    {
      throw std::invalid_argument("Duplicate enum value: " + str );
    }
    this->table.emplace(str, index);
    this->v_table.push_back(str);
    index++;
    return *this;
  }

  std::unordered_map<string, int> table;
  vector<string> v_table;
  int index = 0;

protected:
  void print(std::ostream &out) const override
  {
    out << "enum\n" << "{\n";
    for (auto pair = v_table.begin(); pair != v_table.end(); pair++)
    {
      out << *pair;
      if(pair == v_table.end() - 1) break;
      out << ",\n";
    }
    out << "\n}";
  }
};

//============================================
//          CDataTypeStruct : CDataType
//============================================

class CDataTypeStruct : public CDataType
{
public:
  CDataTypeStruct()
      : CDataType(0) {}

  const size_t &getSize() const override { return size; }
  shared_ptr<CDataType> clone() const override {return shared_ptr<CDataType>(new CDataTypeStruct(*this));}

  CDataTypeStruct &addField(const string &str, const CDataType &x)
  {
    auto it = std::find_if(content.begin(), content.end(), [&str](const std::pair<std::string,std::shared_ptr<CDataType>> element){ return element.first == str;} );
    if(it != content.end()) throw std::invalid_argument("Duplicate field: " + str );
    std::shared_ptr<CDataType> name = x.clone();
    content.emplace_back(str, name);
    size = size + name->size;

    return *this;
  }
  const CDataType& field(const string& str) const
  {
    auto it = std::find_if(content.begin(), content.end(), [&str](const std::pair<std::string,std::shared_ptr<CDataType>> element){ return element.first == str;} );
    if(it == content.end()) throw std::invalid_argument("Unknown field: " + str );

    return *it->second;


  }
  bool compare(const CDataType& this_object, const CDataType& rhs) const override 
  {
    try{
    dynamic_cast<const CDataTypeStruct&>(rhs);
    }
    catch(const std::bad_cast& e)
    {
      return false;
    }
    const CDataTypeStruct& struct_rhs = dynamic_cast<const CDataTypeStruct&>(rhs);
    auto it_rhs = struct_rhs.content.begin();

    for (auto i = content.begin(); i != content.end();)
    {
      if( *i->second != *it_rhs->second) return false;
      it_rhs++;
      i++;
      if(it_rhs ==struct_rhs.content.end() && i != content.end()) return false;
    }
    return true;
  }

  std::vector<pair<string, std::shared_ptr<CDataType>>> content;

protected:
  void print(std::ostream &out) const
  {
    out << "struct\n{\n";
    for (auto &i : content)
    {
      out << *i.second << " " << i.first << ";" << endl;
    }
    out << "}" << endl;
  }
};

 #ifndef __PROGTEST__

static bool whitespaceMatch(const string & a,
                            const string & b) {
    return true;
}

template <typename T_>
static bool whitespaceMatch(const T_ &x, const string &ref)
{
  ostringstream oss;
  oss << x;
  return whitespaceMatch(oss.str(), ref);
}
int main(void)
{

  // CDataTypeInt
  //======================================
  cout << "=====CDataTypeInt=====\n"
       << endl;

  // konstruktor
  CDataTypeInt int1;
  CDataTypeInt int2;

  // <<, getSize()
  cout << int1 << endl;
  assert(int1.getSize() == 4);

  // ==, !=
  assert(int1 == int2);
  assert((int1 != int2) == false);


  // CDataTypeDouble
  //======================================
  cout << "\n\n";
  cout << "=====CDataTypeDouble=====\n"
       << endl;
  // konstruktor
  CDataTypeDouble d1;
  CDataTypeDouble d2;

  // <<, getSize()
  cout << d1 << endl;
  assert(d1.getSize() == 8);

  // ==, !=
  assert(d1 == d2);
  assert((d1 != d2) == false);

  // CDataTypeEnum
  //======================================
  cout << "\n\n";
  cout << "=====CDataTypeEnum=====\n"
       << endl;

  // konstruktor
  CDataTypeEnum enum1;
  CDataTypeEnum enum2;

  enum1.add("Test");
  enum2.add("Test");

  // <<, getSize()
  cout << enum1 << endl;
  assert(enum1.getSize() == 4);

  // ==, !=
  assert(enum1 == enum2);
  assert((enum1 != enum2) == false);

  enum1.add("Test2").add("Test3");
  enum2.add("Test2").add("Test4");
  cout << enum1 << endl;

  assert((enum1 == enum2) == false);
  assert(enum1 != enum2);




  // CDataTypeStruct
  //======================================


  CDataTypeStruct  a = CDataTypeStruct () .
                        addField ( "m_Length", CDataTypeInt () ) .
                        addField ( "m_Status", CDataTypeEnum () .
                          add ( "NEW" ) .
                          add ( "FIXED" ) .
                          add ( "BROKEN" ) .
                          add ( "DEAD" ) ).
                        addField ( "m_Ratio", CDataTypeDouble () );

  CDataTypeStruct b = CDataTypeStruct () .
                        addField ( "m_Length", CDataTypeInt () ) .
                        addField ( "m_Status", CDataTypeEnum () .
                          add ( "NEW" ) .
                          add ( "FIXED" ) .
                          add ( "BROKEN" ) .
                          add ( "READY" ) ).
                        addField ( "m_Ratio", CDataTypeDouble () );

  CDataTypeStruct c =  CDataTypeStruct () .
                        addField ( "m_First", CDataTypeInt () ) .
                        addField ( "m_Second", CDataTypeEnum () .
                          add ( "NEW" ) .
                          add ( "FIXED" ) .
                          add ( "BROKEN" ) .
                          add ( "DEAD" ) ).
                        addField ( "m_Third", CDataTypeDouble () );

  CDataTypeStruct  d = CDataTypeStruct () .
                        addField ( "m_Length", CDataTypeInt () ) .
                        addField ( "m_Status", CDataTypeEnum () .
                          add ( "NEW" ) .
                          add ( "FIXED" ) .
                          add ( "BROKEN" ) .
                          add ( "DEAD" ) ).
                        addField ( "m_Ratio", CDataTypeInt () );
// cout << "buffer" << endl;
//   std::stringstream buffer;
// buffer << a << std::endl;
// cout << buffer.str() << endl;


  assert ( whitespaceMatch ( a, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "}") );


  assert ( whitespaceMatch ( c, "struct\n"
    "{\n"
    "  int m_First;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Second;\n"
    "  double m_Third;\n"
    "}") );

  assert ( whitespaceMatch ( d, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  int m_Ratio;\n"
    "}") );

  assert ( a != b );
  assert ( a == c );
  assert ( a != d );
  assert ( a . field ( "m_Status" ) == CDataTypeEnum () . add ( "NEW" ) . add ( "FIXED" ) . add ( "BROKEN" ) . add ( "DEAD" ) );
  assert ( a . field ( "m_Status" ) != CDataTypeEnum () . add ( "NEW" ) . add ( "BROKEN" ) . add ( "FIXED" ) . add ( "DEAD" ) );
  assert ( a != CDataTypeInt() );
  assert ( whitespaceMatch ( a . field ( "m_Status" ), "enum\n"
    "{\n"
    "  NEW,\n"
    "  FIXED,\n"
    "  BROKEN,\n"
    "  DEAD\n"
    "}") );

  CDataTypeStruct aOld = a;
  b . addField ( "m_Other", CDataTypeDouble ());

  a . addField ( "m_Sum", CDataTypeInt ());

  assert ( a != aOld );
  assert ( a != c );
  assert ( aOld == c );
  assert ( whitespaceMatch ( a, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "  int m_Sum;\n"
    "}") );

  assert ( whitespaceMatch ( b, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    READY\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "  double m_Other;\n"
    "}") );

  c . addField ( "m_Another", a . field ( "m_Status" ));

  assert ( whitespaceMatch ( c, "struct\n"
    "{\n"
    "  int m_First;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Second;\n"
    "  double m_Third;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Another;\n"
    "}") );

  d . addField ( "m_Another", a . field ( "m_Ratio" ));

  assert ( whitespaceMatch ( d, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  int m_Ratio;\n"
    "  double m_Another;\n"
    "}") );

  assert ( a . getSize () == 20 );
  assert ( b . getSize () == 24 );
  try
  {
    a . addField ( "m_Status", CDataTypeInt () );
    assert ( "addField: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Duplicate field: m_Status"sv );
  }

  try
  {
    cout << a . field ( "m_Fail" ) << endl;
    assert ( "field: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Unknown field: m_Fail"sv );
  }

  try
  {
    CDataTypeEnum en;
    en . add ( "FIRST" ) .
         add ( "SECOND" ) .
         add ( "FIRST" );
    assert ( "add: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Duplicate enum value: FIRST"sv );
  }

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
