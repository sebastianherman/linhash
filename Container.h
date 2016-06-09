//
// Created by sebastian on 6/9/16.
//

#ifndef HERMANSLINEARHASHTABLE_CONTAINER_H
#define HERMANSLINEARHASHTABLE_CONTAINER_H

// Container.h
//
// UE Algorithmen und Datenstrukturen - SS 2016 Universitaet Wien
// Container - Projekt
// https://cewebs.cs.univie.ac.at/ADS/ss16/

#include <iostream>
#include <functional>

enum Order { dontcare, ascending, descending };

class ContainerException : public std::exception {
public:
    virtual const char* what() const noexcept override = 0;
};

template <typename E>
class Container {
public:
    Container& operator=(const Container&) = delete;
    Container(const Container&) = delete;
    Container() = default;

    virtual ~Container() { }

    virtual void add(const E& e) { add(&e, 1); }
    virtual void add(const E e[], size_t s) = 0;

    virtual void remove(const E& e) { remove(&e, 1); }
    virtual void remove(const E e[], size_t s) = 0;

    virtual bool member(const E& e) const = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const { return size() == 0; }

    virtual size_t apply(std::function<void(const E&)> f, Order order = dontcare) const = 0;

    virtual E min() const = 0;
    virtual E max() const = 0;

    virtual std::ostream& print(std::ostream& o) const = 0;
};

template <typename E>
inline std::ostream& operator<<(std::ostream& o, const Container<E>& c) { return c.print(o); }

template <typename E> inline size_t hashValue(const E& e) { return size_t(e); }
template <typename E> inline double doubleValue(const E& e) { return double(e); }
template <typename E> inline size_t ordinalValue(const E& e) { return size_t(e); }

#endif //HERMANSLINEARHASHTABLE_CONTAINER_H
