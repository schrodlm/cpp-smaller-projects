#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <stack>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class CDate
{
public:
    CDate(int y, int m, int d)
        : year(y), month(m), day(d)
    {
        epoch = (day + month * 31 + year * 372);
    }

    int year;
    int month;
    int day;
    long epoch;

    bool operator<(const CDate rhs) const
    {
        return this->epoch < rhs.epoch;
    }
};

class CSupermarket
{
public:
    CSupermarket &store(const string &name, const CDate &expiryDate, const int count)
    {
        // zbozi jiz existuje ve skladu
        if (skladiste.count(name))
        {
            // toto zbozi ma i stejne datum spotreby
            if (skladiste[name].count(expiryDate) == 1)
            {
                // pridam pouze count
                skladiste[name][expiryDate] += count;
            }
            // zbozi ve skladu existuje ale nema stejny datum spotreby
            else
            {
                skladiste[name].emplace(expiryDate, count);
            }
        }
        else
        {
            map<CDate, int> box;
            box.insert(make_pair(expiryDate, count));
            skladiste.insert(make_pair(name, box));
        }
        return *this;
    }

    void print_map() const
    {
        for (auto ptr = skladiste.begin(); ptr != skladiste.end(); ptr++)
        {
            cout << "name:\t" << ptr->first << endl;

            for (auto eptr = ptr->second.begin(); eptr != ptr->second.end(); eptr++)
            {
                cout << "exDate:\t" << eptr->first.day << "/" << eptr->first.month << "/" << eptr->first.year << "\ncount:\t" << eptr->second << "\n====" << endl;
            }
        }
    }

    int sold(pair<string, int> wanted_item, string real_name)
    {
        int sold_counter = 0;

        while (!skladiste[real_name].empty())
        {
            auto item = skladiste[real_name].begin();

            // nemam dost produktu
            if ((wanted_item.second - sold_counter) > item->second)
            {
                sold_counter += item->second;
                skladiste[real_name].erase(item);

                if (skladiste[real_name].empty())
                {
                    skladiste.erase(real_name);
                    return sold_counter;
                }
            }
            // itemu mam dost
            else
            {
                item->second -= (wanted_item.second - sold_counter);
                sold_counter = wanted_item.second;
                break;
            }
        }
        return sold_counter;
    }

    bool tolerantCompare(const string &a, const string &b)
    {
        if (a.size() != b.size())
            return false;

        //v kolika znacich se stringy lisi
        int numberOfDifferences = 0;

        for (int i = 0; i < (int)a.size(); i++)
        {
            // stringy se liší více než v jednom písmenu
            if (numberOfDifferences > 1)
                return false;

            if (a[i] != b[i])
                numberOfDifferences++;
        }
        return true;
    }

    bool matchSearching(string a, string &b)
    {
        if (skladiste.count(a))
        {
            b = a;
            return true;
        }
        
        int found = 0;

        for (auto it = skladiste.begin(); it != skladiste.end(); it++)
        {
            // naslo se vic veci podobnych
            if (found > 1)
            {
                return false;
            }

            if (tolerantCompare(a, it->first))
            {
                found++;
                b = it->first;
            }
        }
        if (found == 1)
            return true;
        return false;
    }map

    set<string> unsellableProducts(const list<pair<string, int>> &l)
    {
        set<string> unsellable;
        string s = ""s;
        for (auto it = l.begin(); it != l.end(); it++)
        {

            if (!matchSearching(it->first, s))
            {
                unsellable.insert(it->first);
            }
        }

        return unsellable;
    }

    void sell(list<pair<string, int>> &l)
    {
        set<string> unsellable = unsellableProducts(l);
        
        for (auto it = l.begin(); it != l.end();)
        {
            string s = ""s;
            if (unsellable.find(it->first) != unsellable.end())
            {
                it++;
                continue;
            }

            if (!matchSearching(it->first, s))
            {
                it++;
                continue;
            }

            int pocet_prodanych = sold(*it, s);

            // prodalo se pozadovane mnozstvi
            if (pocet_prodanych == it->second)
            {
                it = l.erase(it);
                continue;
            }
            //
            else
            {
                it->second -= pocet_prodanych;
            }
            it++;
        }
    }
//========================EXPIRED===========================================
    list<pair<string, int>> expired(CDate date) const
    {
        // possbile to store descending based on key
        multimap<int, string, greater<int>> expire_products;

        for (auto ptr = skladiste.begin(); ptr != skladiste.end(); ptr++)
        {
            int count = 0;
            for (auto eptr = ptr->second.begin(); eptr != ptr->second.end(); eptr++)
            {
                if (eptr->first.epoch < date.epoch)
                    count += eptr->second;
            }
            if (count > 0)
                expire_products.emplace(count, ptr->first);
        }
        list<pair<string, int>> l;
        for (const auto &it : expire_products)
            l.emplace_back(it.second, it.first);

        return l;
    }

private:
    unordered_map<string, map<CDate, int>> skladiste;
};
#ifndef __PROGTEST__
int main(void)
{
    CSupermarket s;
    s.store("bread", CDate(2016, 4, 30), 100)
        .store("butter", CDate(2016, 5, 10), 10)
        .store("beer", CDate(2016, 8, 10), 50)
        .store("bread", CDate(2016, 4, 25), 100)
        .store("okey", CDate(2016, 7, 18), 5);

    s.print_map();
    list<pair<string, int>> l0 = s.expired(CDate(2018, 4, 30));
    assert(l0.size() == 4);
    assert((l0 == list<pair<string, int>>{{"bread", 200}, {"beer", 50}, {"butter", 10}, {"okey", 5}}));

    list<pair<string, int>> l1{{"bread", 2}, {"Coke", 5}, {"butter", 20}};
    s.sell(l1);
    assert(l1.size() == 2);
    assert((l1 == list<pair<string, int>>{{"Coke", 5}, {"butter", 10}}));

    list<pair<string, int>> l2 = s.expired(CDate(2016, 4, 30));
    assert(l2.size() == 1);
    assert((l2 == list<pair<string, int>>{{"bread", 98}}));

    list<pair<string, int>> l3 = s.expired(CDate(2016, 5, 20));
    assert(l3.size() == 1);
    assert((l3 == list<pair<string, int>>{{"bread", 198}}));

    list<pair<string, int>> l4{{"bread", 105}};
    s.sell(l4);
    assert(l4.size() == 0);
    assert((l4 == list<pair<string, int>>{}));

    list<pair<string, int>> l5 = s.expired(CDate(2017, 1, 1));
    s.print_map();
    assert(l5.size() == 3);
    assert((l5 == list<pair<string, int>>{{"bread", 93}, {"beer", 50}, {"okey", 5}}));

    s.store("Coke", CDate(2016, 12, 31), 10);

    list<pair<string, int>> l6{{"Cake", 1}, {"Coke", 1}, {"cake", 1}, {"coke", 1}, {"cuke", 1}, {"Cokes", 1}};
    s.sell(l6);
    assert(l6.size() == 3);
    assert((l6 == list<pair<string, int>>{{"cake", 1}, {"cuke", 1}, {"Cokes", 1}}));

    list<pair<string, int>> l7 = s.expired(CDate(2017, 1, 1));
    s.print_map();
    assert(l7.size() == 4);
    assert((l7 == list<pair<string, int>>{{"bread", 93}, {"beer", 50}, {"Coke", 7}, {"okey", 5}}));

    s.store("cake", CDate(2016, 11, 1), 5);

    list<pair<string, int>> l8{{"Cake", 1}, {"Coke", 1}, {"cake", 1}, {"coke", 1}, {"cuke", 1}};
    s.sell(l8);
    assert(l8.size() == 2);
    assert((l8 == list<pair<string, int>>{{"Cake", 1}, {"coke", 1}}));

    list<pair<string, int>> l9 = s.expired(CDate(2017, 1, 1));
    assert(l9.size() == 5);
    assert((l9 == list<pair<string, int>>{{"bread", 93}, {"beer", 50}, {"Coke", 6}, {"okey", 5}, {"cake", 3}}));

    list<pair<string, int>> l10{{"cake", 15}, {"Cake", 2}};
    s.sell(l10);
    assert(l10.size() == 2);
    assert((l10 == list<pair<string, int>>{{"cake", 12}, {"Cake", 2}}));

    list<pair<string, int>> l11 = s.expired(CDate(2017, 1, 1));
    assert(l11.size() == 4);
    assert((l11 == list<pair<string, int>>{{"bread", 93}, {"beer", 50}, {"Coke", 6}, {"okey", 5}}));

    s.print_map();
    list<pair<string, int>> l12{{"Cake", 4}};
    s.sell(l12);
    assert(l12.size() == 0);
    assert((l12 == list<pair<string, int>>{}));

    list<pair<string, int>> l13 = s.expired(CDate(2017, 1, 1));
    assert(l13.size() == 4);
    assert((l13 == list<pair<string, int>>{{"bread", 93}, {"beer", 50}, {"okey", 5}, {"Coke", 2}}));

    list<pair<string, int>> l14{{"Beer", 20}, {"Coke", 1}, {"bear", 25}, {"beer", 10}};
    s.sell(l14);
    assert(l14.size() == 1);
    assert((l14 == list<pair<string, int>>{{"beer", 5}}));

    s.store("ccccb", CDate(2019, 3, 11), 100)
        .store("ccccd", CDate(2019, 6, 9), 100)
        .store("dcccc", CDate(2019, 2, 14), 100);

    list<pair<string, int>> l15{{"ccccc", 10}};
    s.sell(l15);
    assert(l15.size() == 1);
    assert((l15 == list<pair<string, int>>{{"ccccc", 10}}));

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
