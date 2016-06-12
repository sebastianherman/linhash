//
// Created by sebastian on 6/9/16.
//

#ifndef HERMANSLINEARHASHTABLE_HERMANSLINEARHASHTABLE_H
#define HERMANSLINEARHASHTABLE_HERMANSLINEARHASHTABLE_H

#include <iostream>
#include <ostream>
#include "Container.h"

class HermansLinearHashtableEmptyException : public ContainerException {
public:
    virtual const char * what() const noexcept override { return "HermansLinearHashtable: empty"; }
};

template <typename E, size_t N=40>
class HermansLinearHashtable : public Container<E> {
    class Bucket {
        size_t bucketSize;
        size_t elementsInBucket;
        E v[N];
        bool hasOverflow;
        bool lastBucket;
        Bucket * nextBucket = nullptr;

        /* Function to decide if the current bucket is full */
        bool isFull() const {
            if (bucketSize == elementsInBucket) {
                return true;
            }
            return false;
        }

        size_t findElementPosition(const E& e) {
            for (size_t i = 0; i < elementsInBucket; i++) {
                if (v[i] == e) {
                    return i;
                }
            }
            return false;
        }

        /* Function to check if the given paramenter is a member of the current bucket */
        bool memberOfBucket_(const E& e) const {
            for (size_t i = 0; i < elementsInBucket; i++) {
                if (v[i] == e) {
                    return true;
                }
            }
            return false;
        }

    public:
        /* Bucket constructor */
        Bucket() : bucketSize(N), elementsInBucket(0), hasOverflow(false), lastBucket(true) {}

        ~Bucket() {
            if (this->hasOverflow) {
                delete this->nextBucket;
            }
        }

        /* Function to add element to current bucket or corresponding overflow bucket */
        void addElementInBucket(const E& e) {

            /* If the passed element is in the current bucket, break */
            if (memberOfBucket_(e)) {
                return;
            }
            else {
                /* If the current bucket is full, but has no overflow bucket, create one and insert the passed argument into that one */
                if (isFull() && nextBucket==nullptr && this->hasOverflow==false) {
                    nextBucket = new Bucket;
                    this->hasOverflow = true;
                    this->lastBucket = false;
                    nextBucket->addElementInBucket(e);
                    nextBucket->lastBucket = true;
                }
                    /* If the current bucket is full and already has an overflow bucket insert into the overflow bucket */
                else if (isFull() && this->hasOverflow==true && nextBucket!=nullptr) {
                    nextBucket->addElementInBucket(e);
                }
                    /* If the current bucket is NOT full insert into the current one */
                else if (!isFull() && this->hasOverflow==false) {
                    this->v[elementsInBucket] = e;
                    this->elementsInBucket++;
                }
            }
        }

        void removeWithFlag(const E& e) {

            if (memberOfBucket(e)) {

                Bucket * currentBucket = this;

                while (currentBucket != nullptr) {

                    if (currentBucket->memberOfBucket_(e)) {
                        size_t postionOfELement = currentBucket->findElementPosition(e);

                        Bucket * lastBucket = this;
                        while (lastBucket != nullptr) {
                            if (lastBucket->nextBucket == nullptr) {
                                break;
                            }
                            lastBucket = lastBucket->nextBucket;
                        }

                        currentBucket->v[postionOfELement] = lastBucket->v[lastBucket->elementsInBucket-1];
                        lastBucket->elementsInBucket--;

                        Bucket * beforeLastBucketPointer = this;
                        while (!beforeLastBucketPointer->lastBucket) {
                            if (beforeLastBucketPointer->nextBucket != nullptr && beforeLastBucketPointer->nextBucket->nextBucket ==
                                                                                  nullptr) {
                                break;
                            }
                            beforeLastBucketPointer = beforeLastBucketPointer->nextBucket;
                        }


                        if (lastBucket->elementsInBucket == 0) {
                            delete beforeLastBucketPointer->nextBucket;
                            beforeLastBucketPointer->nextBucket = nullptr;
                            beforeLastBucketPointer->lastBucket = true;
                            beforeLastBucketPointer->hasOverflow = false;
                        }
                    }

                    currentBucket = currentBucket->nextBucket;
                }
            }
            else {
                return;
            }
        }

        /* Function to check whether the passed argument is a member of the bucket or its corresponding overflow buckets */
        bool memberOfBucket(const E& e) const {

            /* If the passed argument is a member of the current bucket, return true */
            if (memberOfBucket_(e)) {
                return true;
            }
                /* If the passed argument is not a member of the current bucket, but the current bucket has overflow buckets, check the overflow buckets */
            else if (!memberOfBucket_(e) && this->hasOverflow && nextBucket != nullptr) {
                return nextBucket->memberOfBucket(e);
            }
            return false;
        }

        /* Function to print the bucket with its overflow buckets */
        std::ostream& printBucket(std::ostream& o) const {
            if (this->hasOverflow) {
                for (size_t i = 0; i < bucketSize; i++) {
                    if (i<=elementsInBucket-1) {
                        o <<"[" << i << "]:" << v[i] <<" ";
                    }
                    else {
                        o << "[" <<i << "]: empty\n";
                    }
                }
                o << " ->: ";
                nextBucket->printBucket(o);
            }
            else {
                for (size_t i = 0; i < bucketSize; i++) {
                    if (elementsInBucket != 0) {
                        if (i <= elementsInBucket-1) {
                            o << "["<< i << "]:" << v[i] << " ";
                        }
                        else {
                            o <<"["<< i << "]: empty ";
                        }
                    }
                    else {
                        o <<"["<< i << "]: empty ";
                    }
                }
            }
            o << "\n";

            return o;
        }

        /* Method to get the element of the bucket at the desired index */
        E getElement(size_t index) {
            return v[index];
        }

        /* Method which returns if the bucket has overflow */
        bool is_overflow() {
            return this->hasOverflow;
        }

        /* Method which returns whether the bucket is full or not */
        bool is_full() {
            return isFull();
        }

        /* Method which returns the pointer to the next overflow bucket, in case one exists */
        Bucket * getNextBucketPointer() {

            /* If has overflow is set to true and nextbucket is not set as nullpointer */
            if (this->hasOverflow && this->nextBucket != nullptr) {
                Bucket * addressOfNextBucket = nextBucket;
                return addressOfNextBucket;
            }
            else {
                return nullptr;
            }
        }

        /* Returns the size of the current bucket including overflows */
        size_t getSizeOfWholeBucket() {
            if (elementsInBucket > 0 && !this->hasOverflow) {
                return elementsInBucket;
            }
            else if (elementsInBucket > 0 && this->hasOverflow){
                return elementsInBucket + nextBucket->getSizeOfWholeBucket();
            }
            else {
                return 0;
            }
        }

        /* Returns the size of the current bucket */
        size_t getNumberOfElementsInBucket() {
            return elementsInBucket;
        }

        void setLastBucket(bool value) {
            this->lastBucket = value;
        }
    };

    /* Variables which hold data related to the hashtable */
    size_t d;
    size_t tableSize;
    size_t nextToSplit;
    Bucket ** hashTable = nullptr;

    /* Function to calculate the power of a number; returns size_t */
    size_t power(size_t _base, size_t _exponent) {
        size_t base = _base;
        size_t exponent = _exponent;
        size_t power = 1;

        while (exponent != 0) {
            power *= base;
            --exponent;
        }

        return power;
    }

    /* Returns the hashvalue of the given parameter based on the desired number of last bits */
    size_t getIndex(const E& e, size_t numberOfLastBits) const {

        size_t hashValueToBeUsed = hashValue(e);

        size_t indexToReturn = hashValueToBeUsed & ((1 << numberOfLastBits) - 1);

        /* If the last bits are less than nextToSplit look at one more bit */
        if (indexToReturn < this->nextToSplit) {
            indexToReturn = hashValueToBeUsed & ((1 << (numberOfLastBits+1)) - 1);
        }

        return indexToReturn;
    }

    /* Add method */
    void add_(const E& e) {
        if (!member_(e)) {
            /* Get the index of the element to be added*/
            size_t index = getIndex(e, this->d);

            /* Pointer to the bucket at the index where the element will be inserted*/
            Bucket * targetBucket = hashTable[index];

            /* While the target bucket has overflow and the pointer to the next bucket is not null; targetbucket gets the value of the nextbucket */
            while (targetBucket->is_overflow() && targetBucket->getNextBucketPointer() != nullptr) {
                targetBucket = targetBucket->getNextBucketPointer();
            }

            /* If the target bucket is not full; an overflow bucket will not be added; split does not occur; insert into the bucket at the desired index */
            if (!targetBucket->is_full()) {
                hashTable[index]->addElementInBucket(e);
            }

                /* If the target bucket is full; a split will occur */
            else if (targetBucket->is_full()) {

                /* Insert the element into the original table at the index */
                hashTable[index]->addElementInBucket(e);

                /* Create a new table; copy the old table into the new one */
                Bucket ** newHashTable = new Bucket * [this->tableSize+1];
                for (size_t i = 0; i < tableSize; i++) {
                    newHashTable[i] = hashTable[i];
                }

                /* Last position of the new table gets a bucket */
                newHashTable[this->tableSize] = new Bucket;

                /* Delete the old table */
                delete[] hashTable;
                hashTable = nullptr;

                /* The hashtable gets the address of the new one */
                hashTable = newHashTable;

                size_t oldNextToSplit = this->nextToSplit;

                tableSize++;
                nextToSplit++;

                /* Check if the round is over */
                if (nextToSplit == power(2, this->d)) {
                    d++;
                    nextToSplit = 0;
                }

                /* Pointer to the bucket at the index where the nextToSplit was before splitting */
                Bucket * splitBucket = hashTable[oldNextToSplit];
                Bucket * removeBucket = hashTable[oldNextToSplit];

                /* Index at oldNextToSplit from the new table gets a new bucket ? old one deleted ? */
                hashTable[oldNextToSplit] = new Bucket;

                while (splitBucket != nullptr) {

                    /* Get the size of the bucket to be splitted */
                    size_t splitBucketSize = splitBucket->getNumberOfElementsInBucket();

                    /* Loop through bucket */
                    for (size_t i = 0; i < splitBucketSize; i++) {

                        /* Get the element at the i position from the old bucket */
                        E tempElement = splitBucket->getElement(i);

                        /* Get the new index based on the new D */
                        size_t tempElementIndex = getIndex(tempElement, this->d);

                        /* Insert it into the new table */
                        hashTable[tempElementIndex]->addElementInBucket(tempElement);
                    }

                    splitBucket = splitBucket->getNextBucketPointer();
                }

                delete removeBucket;

            }
        }
        else {
            return;
        }
    }

    /* Function to remove an element from the hashtable */
    void remove_(const E& e) {

      /* Checks if e is a member of the table */
        if (member_(e)) {

          /* Gets is index aka hashvalue */
            size_t elementToBeDeletedBucketIndex = getIndex(e, this->d);

            /* Deletes the element at the index aka hashvalue from the hashtable */
            hashTable[elementToBeDeletedBucketIndex]->removeWithFlag(e);
        }
        else {
            return;
        }
    }

    /* Returns the whole tablesize */
    size_t getSize() const {
        size_t overallSize = 0;
        for (size_t i = 0; i < tableSize; i++) {
            Bucket * currentBucket = hashTable[i];
            overallSize += currentBucket->getSizeOfWholeBucket();
        }
        return overallSize;
    }

    /* Checks if argument is a member of the hashtable */
    bool member_(const E& e) const {
        bool found = false;
        size_t keyValue = getIndex(e, this->d);
        Bucket * desiredBucket = hashTable[keyValue];
        if (desiredBucket->memberOfBucket(e)) {
            found = true;
            return true;
        }
        else {
            return found;
        }
    }

    void mergeSort(E values[], size_t lowerBound, size_t upperBound) const;
    void merge(E values[], size_t lowerBound, size_t upperBound, size_t pivot) const;

public:
    HermansLinearHashtable() {
        d = 1;
        nextToSplit = 0;
        tableSize = power(2, d);
        hashTable = new Bucket * [this->tableSize];
        for (size_t i = 0; i < tableSize; i++) {
            hashTable[i] = new Bucket;
        }
    }

    HermansLinearHashtable(std::initializer_list<E> el) : HermansLinearHashtable() { for (auto e: el) add(e); }

    ~HermansLinearHashtable() {
        for (size_t i = 0; i < tableSize; i++) {
            delete hashTable[i];
            hashTable[i] = nullptr;
        }
        delete[] hashTable;
    }

    using Container<E>::add;
    virtual void add(const E e[], size_t len) override;

    using Container<E>::remove;
    virtual void remove(const E e[], size_t len) override;

    virtual bool member(const E& e) const override;
    virtual size_t size() const override;

    virtual E min() const override;
    virtual E max() const override;

    virtual std::ostream& print(std::ostream& o) const override;

    virtual size_t apply(std::function<void(const E&)> f, Order order = dontcare) const override;
};

template <typename E, size_t N>
size_t HermansLinearHashtable<E,N>::size() const {
    return getSize();
}

template<typename E, size_t N>
void HermansLinearHashtable<E,N>::add(const E e[], size_t len) {
    for (size_t i = 0; i < len; i++) {
        add_(e[i]);
    }
}

template<typename E, size_t N>
bool HermansLinearHashtable<E,N>::member(const E& e) const {
    return member_(e);
}

template<typename E, size_t N>
void HermansLinearHashtable<E,N>::remove(const E e[], size_t len) {
    for (size_t i = 0; i < len; i++) {
        remove_(e[i]);
    }
}

template<typename E, size_t N>
E HermansLinearHashtable<E,N>::max() const {

    E maxValue{};

    /* If there is only one element in the table, return*/
    if (size() == 1) {
        for (size_t i = 0; i < tableSize; i++) {
            Bucket * currentBucket = hashTable[i];
            if (currentBucket->getSizeOfWholeBucket() > 0) {
                if (currentBucket->getNumberOfElementsInBucket() > 0) {
                    maxValue = currentBucket->getElement(0);
                }
            }
        }

        return maxValue;
    }

    /* If the table contains more elements */
    if (size() > 0) {

        /* Get the first element in the table */
        for (size_t i = 0; i < tableSize; i++) {
            Bucket * currentBucket = hashTable[i];
            if (currentBucket->getSizeOfWholeBucket() > 0) {
                if (currentBucket->getNumberOfElementsInBucket() > 0) {
                    maxValue = currentBucket->getElement(i);
                    break;
                }
            }
        }

        /* Compare the element to the others in the table */
        for (size_t i = 0; i <tableSize; i++) {

            /* Bucket to hold the temporary bucket at the desired index */
            Bucket * currentBucket = hashTable[i];

            /* Loops through the bucket and its overflow buckets and compares each value to the maxValue */
            while (currentBucket != nullptr) {
                size_t currentBucketSize = currentBucket->getNumberOfElementsInBucket();

                for (size_t j = 0; j < currentBucketSize; j++) {
                    E tempElement = currentBucket->getElement(j);

                    if (tempElement > maxValue) {
                        maxValue = currentBucket->getElement(j);
                    }
                }

                currentBucket = currentBucket->getNextBucketPointer();
            }
        }

        return maxValue;

    }

    /* If the hashtable is empty; throw exception */
    else {
        throw HermansLinearHashtableEmptyException();
    }
}

template<typename E, size_t N>
E HermansLinearHashtable<E,N>::min() const {
    E minValue{};

    /* If there is only one element in the table, return*/
    if (size() == 1) {
        for (size_t i = 0; i < tableSize; i++) {
            Bucket * currentBucket = hashTable[i];
            if (currentBucket->getSizeOfWholeBucket() > 0) {
                if (currentBucket->getNumberOfElementsInBucket() > 0) {
                    minValue = currentBucket->getElement(0);
                }
            }
        }

        return minValue;
    }
    else if (size() > 0) {
        for (size_t i = 0; i < tableSize; i++) {
            Bucket * currentBucket = hashTable[i];
            if (currentBucket->getSizeOfWholeBucket() > 0) {
                if (currentBucket->getNumberOfElementsInBucket() > 0) {
                    minValue = currentBucket->getElement(i);
                    break;
                }
            }
        }

        for (size_t i = 0; i <tableSize; i++) {

            Bucket * currentBucket = hashTable[i];

            while (currentBucket != nullptr) {
                size_t currentBucketSize = currentBucket->getNumberOfElementsInBucket();

                for (size_t j = 0; j < currentBucketSize; j++) {
                    E tempElement = currentBucket->getElement(j);

                    if (minValue > tempElement) {
                        minValue = currentBucket->getElement(j);
                    }
                }

                currentBucket = currentBucket->getNextBucketPointer();
            }
        }

        return minValue;

    }
    else {
        throw HermansLinearHashtableEmptyException();
    }
}

template<typename E, size_t N>
std::ostream& HermansLinearHashtable<E,N>::print(std::ostream& o) const {
    for (size_t i = 0; i < tableSize; i++) {
        Bucket * currentBucket = hashTable[i];
        o << "Index(" << i << "): ";
        currentBucket->printBucket(o);
    }
    return o;
}

template<typename E, size_t N>
size_t HermansLinearHashtable<E,N>::apply(std::function<void(const E&)> f, Order order) const {
    size_t rc = 0;

    if (size() == 0) {
        return rc;
    }
    else {
        if (order == dontcare) {
            try {
                for (size_t i = 0; i < tableSize; i++) {
                    Bucket * currentBucket = hashTable[i];
                    while (currentBucket != nullptr) {
                        for (size_t j = 0; j < currentBucket->getNumberOfElementsInBucket(); j++) {
                            f(currentBucket->getElement(j));
                            rc++;
                        }
                        currentBucket = currentBucket->getNextBucketPointer();
                    }
                }
            } catch (...) {}
        }
        else {

            const size_t elementsInTable = size();

            E * values = new E[elementsInTable];
            size_t x = 0;

            for (size_t i = 0; i < tableSize; i++) {
                Bucket * currentBucket = hashTable[i];
                while (currentBucket != nullptr) {
                    for (size_t j = 0; j < currentBucket->getNumberOfElementsInBucket(); j++) {
                        values[x++] = currentBucket->getElement(j);
                    }
                    currentBucket = currentBucket->getNextBucketPointer();
                }
            }

            mergeSort(values, 0, size()-1);

            try {
                if (order == descending) {
                    for (size_t i = elementsInTable; i--;) {
                        f(values[i]);
                        rc++;
                    }
                }
                else {
                    for (size_t i = 0; i < elementsInTable; i++) {
                        f(values[i]);
                        rc++;
                    }
                }
            } catch (...) {}

            delete[] values;

        }
    }


    return rc;
}

template<typename E, size_t N>
void HermansLinearHashtable<E,N>::mergeSort(E values[], size_t lowerBound, size_t upperBound) const {
    size_t pivot;
    if (upperBound > lowerBound) {
        pivot = (lowerBound + upperBound) / 2;
        mergeSort(values, lowerBound, pivot);
        mergeSort(values, pivot + 1, upperBound);
        merge(values, lowerBound, upperBound, pivot);
    }
}

template<typename E, size_t N>
void HermansLinearHashtable<E,N>::merge(E values[], size_t lowerBound, size_t upperBound, size_t pivot) const {
    E* leftValuesArray = nullptr;
    E* rightValuesArray = nullptr;

    size_t i, j, k;
    size_t n1 = pivot - lowerBound + 1;
    size_t n2 = upperBound - pivot;
    leftValuesArray = new E[n1];

    rightValuesArray = new E[n2];

    for (i = 0; n1 > i; i++) {
        leftValuesArray[i] = values[lowerBound + i];
    }

    for (j = 0; n2 > j; j++) {
        rightValuesArray[j] = values[pivot + 1 + j];
    }

    i = 0;
    j = 0;
    k = lowerBound;

    while (n1 > i && n2 > j) {
        if (!(leftValuesArray[i] > rightValuesArray[j])) {
            values[k] = leftValuesArray[i];
            i++;
        }
        else {
            values[k] = rightValuesArray[j];
            j++;
        }

        k++;
    }

    while (n1 > i) {
        values[k] = leftValuesArray[i];
        i++;
        k++;
    }

    while (n2 > j) {
        values[k] = rightValuesArray[j];
        j++;
        k++;
    }

    delete [] leftValuesArray;
    delete [] rightValuesArray;
}
#endif //HERMANSLINEARHASHTABLE_HERMANSLINEARHASHTABLE_H
