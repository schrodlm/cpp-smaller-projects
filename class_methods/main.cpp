#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

//==========================CVATRegister================================
class CVATRegister
{

  //========================private=====================================
private:
  struct Company
  {
    pair<string, string> m_NameAddr;
    string m_taxID;
    size_t m_income;
    pair<string, string> m_realNameAddr;

    // overloading operator < so I can use it in lower_bound() - binary search
    bool operator<(pair<string, string> const rhs) const
    {
      return m_NameAddr < rhs;
    }
    bool operator<(string const rhs) const
    {
      return m_taxID < rhs;
    }
    bool operator==(string const rhs) const
    {
      return m_taxID == rhs;
    }
  };

  vector<Company> Companies;
  vector<Company> id_Companies;
  mutable vector<int> factures;

  // strings to save case insensitive version of name and address in
  mutable string non_constant_name;
  mutable string non_constant_address;

  //==========================public=======================================
public:
  CVATRegister(void)
  {
  }

  ~CVATRegister(void)
  {
  }

  vector<Company>::iterator nameAddrSetup(const string &name, const string &addr, pair<string, string> &full_pair)
  {
    non_constant_name = name;
    non_constant_address = addr;

    // changing name and address we are looking for to upper characters
    transform(non_constant_name.begin(), non_constant_name.end(), non_constant_name.begin(), ::toupper);
    transform(non_constant_address.begin(), non_constant_address.end(), non_constant_address.begin(), ::toupper);

    full_pair = make_pair(non_constant_name, non_constant_address);
    auto pos = lower_bound(Companies.begin(), Companies.end(), full_pair);

    return pos;
  }

  /**
   * @brief creates a new node in list of companies if conditions are met (pair name and adress are unique && id is unique)
   *        - O(n*log n) --> but it is already sorting the list so later on sorting is not needed
   *
   * @param name    --> name of the company
   * @param addr    --> address of the company
   * @param taxID   --> id of the company
   * @return true   --> company was added
   * @return false  --> company didn't met conditions
   */
  bool newCompany(const string &name, const string &addr, const string &taxID)
  {
    non_constant_name = name;
    non_constant_address = addr;

    // changing name and address to uppercase - one of the tasks was to search to be case insensitive (only for name and address)

    transform(non_constant_name.begin(), non_constant_name.end(), non_constant_name.begin(), ::toupper);
    transform(non_constant_address.begin(), non_constant_address.end(), non_constant_address.begin(), ::toupper);
    // create a pair of name and adress
    const pair<string, string> full_pair = make_pair(name, addr);

    // for case insestive
    const pair<string, string> non_constant_pair = make_pair(non_constant_name, non_constant_address);

    // iterate through existing companies using iterator and binary search through them with lower_bound()
    auto pos = lower_bound(Companies.begin(), Companies.end(), non_constant_pair);
    // if it isnt a duplicate of existing company add it a return true
    Company new_company;
    if (pos == Companies.end() || (*pos).m_NameAddr != non_constant_pair)
    {

      // in function firstCompany() and nextCompany() -> I need to return real name and address
      new_company.m_realNameAddr = full_pair;
      // in rest of my functions I need to work with them as case insestive
      new_company.m_NameAddr = non_constant_pair;

      new_company.m_taxID = taxID;
      new_company.m_income = 0;

      Companies.insert(pos, new_company);
    }
    else
      return false;

    auto pos_id = lower_bound(id_Companies.begin(), id_Companies.end(), taxID);
    if (pos_id == id_Companies.end() || (*pos_id).m_taxID != taxID)
    {
      id_Companies.insert(pos_id, new_company);
      return true;
    }
    return false;
  }
  /**
   * @brief removes company identified by its unique pair of name and address
   *
   * @param name    --> name of the company
   * @param addr    --> address of the company
   * @return true   --> company was removed succesfully
   * @return false  --> company doesn't exist
   */
  bool cancelCompany(const string &name, const string &addr)
  {
    pair<string, string> full_pair;
    auto pos = nameAddrSetup(name, addr, full_pair);

    if (pos != Companies.end() && pos->m_NameAddr == full_pair)
    {
      string taxID = pos->m_taxID;
      auto pos_id = lower_bound(id_Companies.begin(), id_Companies.end(), taxID);
      id_Companies.erase(pos_id);
      Companies.erase(pos);
      return true;
    }
    return false;
  }
  /**
   * @brief overloaded cancelCmpany function - company is now identified with its unique id
   *
   */
  bool cancelCompany(const string &taxID)
  {

    auto pos_id = lower_bound(id_Companies.begin(), id_Companies.end(), taxID);
    if (pos_id != id_Companies.end() && pos_id->m_taxID == taxID)
    {
      pair<string, string> nameAddr = pos_id->m_NameAddr;
      auto pos = lower_bound(Companies.begin(), Companies.end(), nameAddr);
      Companies.erase(pos);
      id_Companies.erase(pos_id);
      return true;
    }
    return false;
  }
  /**
   * @brief adding a facture to company specified by its unique id
   *
   * @param taxID   --> id of the company
   * @param amount  --> facture value
   * @return true   --> facture was invoiced sucessfully
   * @return false  --> company doesn't exist
   */
  bool invoice(const string &taxID, unsigned int amount)
  {
    auto pos_id = lower_bound(id_Companies.begin(), id_Companies.end(), taxID);
    if (pos_id->m_taxID == taxID)
    {
      pair<string, string> nameAddr = pos_id->m_NameAddr;
      auto pos = lower_bound(Companies.begin(), Companies.end(), nameAddr);
      pos->m_income += amount;
      pos_id->m_income += amount;
      factures.push_back(amount);

      return true;
    }
    return false;
  }
  /**
   * @brief overloaded invoice function - company is now identified by its unique pair of name and address
   *
   */
  bool invoice(const string &name, const string &addr, unsigned int amount)
  {
    pair<string, string> full_pair;
    auto pos = nameAddrSetup(name, addr, full_pair);

    if (pos->m_NameAddr == full_pair)
    {
      string taxID = pos->m_taxID;

      auto pos_id = lower_bound(id_Companies.begin(), id_Companies.end(), taxID);
      pos_id->m_income += amount;
      pos->m_income += amount;

      // for medianInvoice function -> finding median of all invoiced factures
      factures.push_back(amount);
      return true;
    }
    return false;
  }
  /**
   * @brief sum of all factures added to company specified by its name and address
   *
   * @param name        --> name of the company
   * @param addr        --> address of the company
   * @param sumIncome   --> reference to sum of factures
   * @return true       --> company and sum was found
   * @return false      --> company doesn't exist
   */
  bool audit(const string &name, const string &addr, unsigned int &sumIncome) const
  {
    pair<string, string> full_pair;
    auto pos = (const_cast<CVATRegister *>(this))->nameAddrSetup(name, addr, full_pair);

    if (pos->m_NameAddr == full_pair)
    {
      sumIncome = pos->m_income;
      return true;
    }
    return false;
  }

  /**
   * @brief overloaded audit function - company is now identified by its unique id
   *
   */
  bool audit(const string &taxID, unsigned int &sumIncome) const
  {
    auto pos_id = lower_bound(id_Companies.begin(), id_Companies.end(), taxID);
    {
      if (pos_id->m_taxID == taxID)
      {
        sumIncome = pos_id->m_income;
        return true;
      }
    }
    return false;
  }

  /**
   * @brief seraching for first company in alpabetically sorted list of companies
   *
   * @param name   --> name of the first company
   * @param addr   --> address of the first company
   * @return true  --> first company found
   * @return false --> list of companies is empty
   */
  bool firstCompany(string &name, string &addr) const
  {
    if (Companies.empty())
      return false;
    vector<Company>::const_iterator it = Companies.begin();
    name = it->m_realNameAddr.first;
    addr = it->m_realNameAddr.second;
    return true;
  }

  /**
   * @brief identify company with name and address contained in given strings and return company after that one
   *         -> companies should be sorted alphebetically
   *
   * @param name   --> name of company next to one we search for
   * @param addr   --> address of company next to one we search for
   * @return true  --> company identified
   * @return false --> we reached end of company list or name and address werent previously given
   */
  bool nextCompany(string &name, string &addr) const
  {
    string non_constant_name = name;
    string non_constant_address = addr;

    // changing name and address we are looking for to upper characters
    transform(non_constant_name.begin(), non_constant_name.end(), non_constant_name.begin(), ::toupper);
    transform(non_constant_address.begin(), non_constant_address.end(), non_constant_address.begin(), ::toupper);

    const pair<string, string> full_pair = make_pair(non_constant_name, non_constant_address);
    auto pos = lower_bound(Companies.begin(), Companies.end(), full_pair);
    if ((*pos).m_NameAddr == full_pair)
    {
      // the targeted company was last in the list so we can't find the next one
      if (++pos == Companies.end())
        return false;
      name = pos->m_realNameAddr.first;
      addr = pos->m_realNameAddr.second;
      return true;
    }
    return false;
  }

  /**
   * @brief takes all factures that has been invoiced by invoice() function and returns median
   *
   * @return unsigned int --> median
   */
  unsigned int medianInvoice(void) const
  {
    if (factures.empty())
      return 0;

    int size = factures.size();

    /* nth_element() = algorithm which rearranges the list in such a way such that the element at the nth position
    is the one which should be at that position if we sort the list.
    */
    // --> basically sort only necessery part of the array
    nth_element(factures.begin(), factures.begin() + size / 2, factures.end());
    return factures[size / 2];
  }

  //==========================debug function============================

  // friend std::ostream &operator<<(std::ostream &stream, CVATRegister::Company comp);
  // void print() const
  // {
  //   for (auto const &i : Companies)
  //   {
  //     cout << i << endl;
  //   }

  //   cout << "========ID==========" << endl;
  //   for (auto const &i : id_Companies)
  //   {
  //     cout << i << endl;
  //   }
  // }

  // void printMedian() const
  // {
  //   for (vector<int>::const_iterator it = factures.begin(); it != factures.end(); it++)
  //   {
  //     cout << (*it) << endl;
  //   }
  //   cout << factures.size() << endl;
  // }
};

// std::ostream &operator<<(std::ostream &stream, CVATRegister::Company comp)
// {

//   stream << "=====\n"
//          << "Jmeno firmy: " << comp.m_NameAddr.first << "\nAdresa: " << comp.m_NameAddr.second << "\ntaxID: " << comp.m_taxID << "\namount: " << comp.m_income << endl;
//   stream << "=====\n";
//   return stream;
// }

#ifndef __PROGTEST__
int main(void)
{
  string name, addr;
  unsigned int sumIncome;

  CVATRegister b1;
  assert(b1.newCompany("ACME", "Thakurova", "666/666"));
  assert(b1.newCompany("ACME", "Kolejni", "666/666/666"));
  assert(b1.newCompany("Dummy", "Thakurova", "123456"));
  assert(b1.invoice("666/666", 2000));
  assert(b1.medianInvoice() == 2000);
  assert(b1.invoice("666/666/666", 3000));
  assert(b1.medianInvoice() == 3000);
  assert(b1.invoice("123456", 4000));
  assert(b1.medianInvoice() == 3000);
  assert(b1.invoice("aCmE", "Kolejni", 5000));
  assert(b1.medianInvoice() == 4000);
  assert(b1.audit("ACME", "Kolejni", sumIncome) && sumIncome == 8000);
  assert(b1.audit("123456", sumIncome) && sumIncome == 4000);
  assert(b1.firstCompany(name, addr) && name == "ACME" && addr == "Kolejni");
  assert(b1.nextCompany(name, addr) && name == "ACME" && addr == "Thakurova");
  assert(b1.nextCompany(name, addr) && name == "Dummy" && addr == "Thakurova");
  assert(!b1.nextCompany(name, addr));
  assert(b1.cancelCompany("ACME", "KoLeJnI"));
  assert(b1.medianInvoice() == 4000);
  assert(b1.cancelCompany("666/666"));
  assert(b1.medianInvoice() == 4000);
  assert(b1.invoice("123456", 100));
  assert(b1.medianInvoice() == 3000);
  assert(b1.invoice("123456", 300));
  assert(b1.medianInvoice() == 3000);
  assert(b1.invoice("123456", 200));
  assert(b1.medianInvoice() == 2000);
  assert(b1.invoice("123456", 230));
  assert(b1.medianInvoice() == 2000);
  assert(b1.invoice("123456", 830));
  assert(b1.medianInvoice() == 830);
  assert(b1.invoice("123456", 1830));
  assert(b1.medianInvoice() == 1830);
  assert(b1.invoice("123456", 2830));
  assert(b1.medianInvoice() == 1830);
  assert(b1.invoice("123456", 2830));
  assert(b1.medianInvoice() == 2000);
  assert(b1.invoice("123456", 3200));
  assert(b1.medianInvoice() == 2000);
  assert(b1.firstCompany(name, addr) && name == "Dummy" && addr == "Thakurova");
  assert(!b1.nextCompany(name, addr));
  assert(b1.cancelCompany("123456"));
  assert(!b1.firstCompany(name, addr));

  CVATRegister b2;
  assert(b2.newCompany("ACME", "Kolejni", "abcdef"));
  assert(b2.newCompany("Dummy", "Kolejni", "123456"));
  assert(!b2.newCompany("AcMe", "kOlEjNi", "1234"));
  assert(b2.newCompany("Dummy", "Thakurova", "ABCDEF"));
  assert(b2.medianInvoice() == 0);
  assert(b2.invoice("ABCDEF", 1000));
  assert(b2.medianInvoice() == 1000);
  assert(b2.invoice("abcdef", 2000));
  assert(b2.medianInvoice() == 2000);
  assert(b2.invoice("aCMe", "kOlEjNi", 3000));
  assert(b2.medianInvoice() == 2000);
  assert(!b2.invoice("1234567", 100));
  assert(!b2.invoice("ACE", "Kolejni", 100));
  assert(!b2.invoice("ACME", "Thakurova", 100));
  assert(!b2.audit("1234567", sumIncome));
  assert(!b2.audit("ACE", "Kolejni", sumIncome));
  assert(!b2.audit("ACME", "Thakurova", sumIncome));
  assert(!b2.cancelCompany("1234567"));
  assert(!b2.cancelCompany("ACE", "Kolejni"));
  assert(!b2.cancelCompany("ACME", "Thakurova"));
  assert(b2.cancelCompany("abcdef"));
  //b2.print();
  assert(b2.medianInvoice() == 2000);
  assert(!b2.cancelCompany("abcdef"));
  assert(b2.newCompany("ACME", "Kolejni", "abcdef"));
  assert(b2.cancelCompany("ACME", "Kolejni"));
  assert(!b2.cancelCompany("ACME", "Kolejni"));

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
