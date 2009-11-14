#pragma once

//#include <memory>
#include <algorithm>
#include <string>
#include <cassert>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/noncopyable.hpp>
#include "tree.hh"

namespace runpac { namespace fcgixx {

template<typename Key = std::string, typename Value = std::string>
struct cache : boost::noncopyable
{
    struct cache_node;

    typedef Key key_type;
    typedef Value value_type;

    typedef boost::shared_ptr<cache_node> shared_node;
    typedef boost::weak_ptr<cache_node> weak_node;

    typedef tree<shared_node> tree_type;
    typedef boost::unordered_map<key_type, weak_node> table_type;

    typedef typename tree_type::iterator iterator;

    struct cache_node : boost::noncopyable
    {
        cache_node(const key_type& key, const value_type& value, table_type& lookup)
            : key(key), value(value), lookup(lookup)
        { }

        ~cache_node()
        {
            typename table_type::iterator it = lookup.find(key);
            assert(it != lookup.end());
            lookup.erase(it);
        }

        key_type key;
        value_type value;
        table_type& lookup;
    };


    void add(const key_type& key, const value_type& value)
    {
        typename table_type::const_iterator it = table.find(key);
        if (it != table.end()) {
            quick_del(it);
        }
        typename tree_type::iterator top = nodes.begin();
        shared_node p(new cache_node(key, value, table));
        nodes.insert(top, p);
        table[key] = weak_node(p);
    }

    void add(iterator& parent, const key_type& key, const value_type& value)
    {
        typename table_type::const_iterator it = table.find(key);
        if (it != table.end()) {
            quick_del(it);
        }
        shared_node p(new cache_node(key, value, table));
        nodes.append_child(parent, p);
        table[key] = weak_node(p);
    }

    void add(const key_type& parent, const key_type& key, const value_type& value)
    {
        iterator itp = location(parent);
        if (valid(itp)) {
            add(itp, key, value);
        }
    }

    iterator location(const key_type& key) const
    {
        iterator itr = nodes.end();
        typename table_type::const_iterator it = table.find(key);
        if (it != table.end()) {
            shared_node s = it->second.lock();
            itr = find(nodes.begin(), nodes.end(), s);
        }
        return itr;
    }

    bool valid(const iterator& it) const
    {
        return it != nodes.end();
    }


    void del(const key_type& key)
    {
        typename table_type::const_iterator it = table.find(key);
        if (it != table.end()) {
            quick_del(it);
        }
    }

    value_type get(const key_type& key) const
    {
        typename table_type::const_iterator it = table.find(key);
        if (it != table.end()) {
            shared_node s = it->second.lock();
            return s->value;
        }
        return value_type();
    }

    bool has(const key_type& key) const
    {
        return table.find(key) != table.end();
    }

    void clear()
    {
        nodes.clear();
    }

    //////
    std::string check()
    {
        std::string s("CHECK: ");
        s += "table.size: " + boost::lexical_cast<std::string>(table.size());
        s += " nodes.size: " + boost::lexical_cast<std::string>(nodes.size());
        s += " <br/>\n";
        return s;
    }

    std::string check_list()
    {
        std::string s("CHECK_LIST: ");
        for (typename table_type::const_iterator it(table.begin()), ite(table.end()); it != ite; ++it) {
            s += it->first + " - ";
        }
        return s + " <br/>\n";
    }
    /////

private:
    void quick_del(const typename table_type::const_iterator& it)
    {
        typename tree_type::iterator itr;
        {
            shared_node s = it->second.lock();
            itr = find(nodes.begin(), nodes.end(), s);
        }
        assert(itr != nodes.end());
        nodes.erase(itr);
    }

private:
    table_type table;
    tree_type nodes;


};


} } // ns


