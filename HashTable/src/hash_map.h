#pragma once

#include "bits/stdc++.h"
#include <algorithm>
#include <memory>

// Check if x is prime number
template<typename T>
bool is_prime(T x) {
    if (x == 2 || x == 3) { return true; }
    if (x % 2 == 0 || x % 3 == 0) { return false; }
    T i(6);
    while (i * i - 2 * i + 1 <= x) {
        if (x % (i - 1) == 0 || x % (i + 1) == 0) { return false; }
        i += 6;
    }
    return true;
}


// Find minimum prime number that larger than x
template<typename T>
T next_prime(T x) {
    while (!is_prime(++x)) {}
    return x;
}


template<typename KeyType, typename ValueType, typename Hash = std::hash<KeyType>>
class HashMap {
    using EntryType = std::pair<KeyType, ValueType>;
    using ConstEntryType = std::pair<const KeyType, ValueType>;

private:
    long double min_load_factor_ = .125; // minimum load factor of container
    long double avg_load_factor_ = .25; // default load factor of container
    long double max_load_factor_ = .5; // maximum load factor of container

    size_t capacity_ = 0; // size of container
    size_t size_ = 0; // number of elements in HashMap
    std::vector<std::unique_ptr<ConstEntryType>> container_;
    std::vector<ssize_t> first_index_; // first_index that have same hash
    std::vector<bool> deleted_; // deleted flag

    Hash hasher_;

    size_t get_first_not_empty_index() const {
        size_t index = 0;
        while (index < capacity_ && is_empty_element(index)) {
            ++index;
        }
        return index;
    }

    bool is_empty_element(size_t index) const {
        return index >= capacity_ || first_index_[index] == -1 || deleted_[index];
    }

    // resize the container
    // ~double capacity if current load factor more than max_load_factor_.
    // reduce by ~half if current load factor less than min_load_factor_
    void reallocate(ssize_t size = -1) {
        if (size == -1) { size = size_; }

        std::vector<std::unique_ptr<ConstEntryType>> entries(size_);
        size_t i = 0;
        for (auto entry: *this) {
            entries[i++] = std::make_unique<ConstEntryType>(entry);
        }

        capacity_ = next_prime(static_cast<size_t>(static_cast<long double>(size) / avg_load_factor_));

        container_.clear();
        container_.resize(capacity_);
        first_index_.assign(capacity_, -1);
        deleted_.assign(capacity_, false);

        for (const auto &entry: entries) {
            insert(*entry, false);
        }
    }

    // check if HashMap need resize
    bool check_capacity(ssize_t size = -1) {
        if (size == -1) { size = size_; }

        return capacity_ * min_load_factor_ <= static_cast<long double>(size) &&
               static_cast<long double>(size) <= capacity_ * max_load_factor_;
    }

    // resize container if it needs
    bool reallocate_if_need(ssize_t size = -1) {
        if (check_capacity(size)) {
            return false;
        }
        reallocate(size);
        return true;
    }

    // get the distance in cycle
    size_t get_dist(size_t first, size_t second) {
        return second - first ? second >= first : size_ - first + second;
    }

    // get index in container of element by key
    size_t get_index(const KeyType &key) const {
        if (empty()) { return capacity_; }

        size_t index = hasher_(key) % capacity_;
        ssize_t hash = static_cast<ssize_t>(index);
        while (true) {
            if (first_index_[index] == -1) {
                return capacity_;
            }
            if (!deleted_[index] && first_index_[index] == hash && container_[index]->first == key) {
                break;
            }
            index = index + 1 < capacity_ ? index + 1 : 0;
        }
        return index;
    }

public:
    class iterator { // NOLINT
    private:
        const HashMap *hash_map_;
        size_t it_index_;

    public:
        iterator() = default;

        iterator(const HashMap *hash_map, size_t it_index) : hash_map_(hash_map), it_index_(it_index) {};

        ConstEntryType *operator->() {
            return &(*hash_map_->container_[it_index_]);
        }

        ConstEntryType &operator*() {
            return *hash_map_->container_[it_index_];
        }

        iterator operator++() {
            do {
                ++it_index_;
            } while (it_index_ < hash_map_->capacity_ && hash_map_->is_empty_element(it_index_));
            return *this;
        }

        iterator operator++(int) {
            auto result = *this;
            ++(*this);
            return result;
        }

        bool operator==(iterator other) const {
            return it_index_ == other.it_index_ && hash_map_ == other.hash_map_;
        }

        bool operator!=(iterator other) const {
            return it_index_ != other.it_index_ || hash_map_ != other.hash_map_;
        }
    };

    class const_iterator { // NOLINT
    private:
        const HashMap *hash_map_;
        size_t it_index_;

    public:
        const_iterator() = default;

        const_iterator(const HashMap *hash_map, size_t it_index) : hash_map_(hash_map), it_index_(it_index) {};

        const ConstEntryType *operator->() const {
            return &(*hash_map_->container_[it_index_]);
        }

        const ConstEntryType &operator*() const {
            return *hash_map_->container_[it_index_];
        }

        const_iterator operator++() {
            do {
                ++it_index_;
            } while (it_index_ < hash_map_->capacity_ && hash_map_->is_empty_element(it_index_));
            return *this;
        }

        const_iterator operator++(int) {
            auto result = *this;
            ++(*this);
            return result;
        }

        bool operator==(const_iterator other) const {
            return it_index_ == other.it_index_ && hash_map_ == other.hash_map_;
        }

        bool operator!=(const_iterator other) const {
            return it_index_ != other.it_index_ || hash_map_ != other.hash_map_;
        }
    };

    HashMap() = default;

    HashMap(const HashMap &other) {
        *this = other;
    }

    HashMap &operator=(const HashMap &other) {
        min_load_factor_ = other.min_load_factor_;
        avg_load_factor_ = other.avg_load_factor_;
        max_load_factor_ = other.max_load_factor_;

        capacity_ = other.capacity_;
        size_ = other.size_;
        container_.resize(other.container_.size());
        for (size_t i = 0; i < other.container_.size(); ++i) {
            if (other.container_[i] == nullptr) { continue; }
            container_[i] = std::make_unique<ConstEntryType>(other.container_[i]->first,
                                                             other.container_[i]->second);
        }
        first_index_ = std::vector<ssize_t>(other.first_index_);
        deleted_ = std::vector<bool>(other.deleted_);
        return *this;
    }

    explicit HashMap(const Hash &hasher) : hasher_(hasher) {};

    template<typename ForwardIterator>
    HashMap(ForwardIterator begin, ForwardIterator end, const Hash hasher = Hash()) : hasher_(hasher) {
        for (auto ptr = begin; ptr != end; ++ptr) {
            insert(*ptr);
        }
    }

    HashMap(const std::initializer_list<EntryType> &entries, const Hash hasher = Hash()) {
        *this = HashMap(entries.begin(), entries.end(), hasher);
    }

    size_t size() const {
        return size_;
    };

    bool empty() const {
        return size() == 0;
    }

    Hash hash_function() const {
        return hasher_;
    }

    bool insert(EntryType entry, bool update_size = true) {
        if (capacity_ == 0) {
            reallocate_if_need(1);
        }

        std::unique_ptr<ConstEntryType> entry_ptr = std::make_unique<ConstEntryType>(entry.first, entry.second);

        size_t index = hasher_(entry.first) % capacity_;
        ssize_t hash = static_cast<ssize_t>(index);

        // Robin Hood algorithm
        while (true) {
            if (first_index_[index] == -1 || deleted_[index]) { break; }
            if (first_index_[index] == hash) {
                if (container_[index]->first == entry.first) {
                    return false;
                }
            }
            if (get_dist(first_index_[index], index) < get_dist(hash, index)) {
                std::swap(container_[index], entry_ptr);
                std::swap(first_index_[index], hash);
            }
            index = index + 1 < capacity_ ? index + 1 : 0;
        }

        if (reallocate_if_need(size_ + 1)) {
            return insert({entry_ptr->first, entry_ptr->second});
        }
        if (update_size) { ++size_; }
        deleted_[index] = false;
        first_index_[index] = hash;
        container_[index] = std::move(entry_ptr);
        return true;
    }

    // "place" tombstone to deleted element
    bool erase(const KeyType &key) {
        size_t index = get_index(key);
        if (index == capacity_) { return false; }
        deleted_[index] = true;
        --size_;
        reallocate_if_need();
        return true;
    }

    const_iterator end() const {
        return const_iterator(this, capacity_);
    }

    // find and return const iterator to first element
    const_iterator begin() const {
        return const_iterator(this, get_first_not_empty_index());
    }

    iterator end() {
        return iterator(this, capacity_);
    }

    // find and return iterator to first element
    iterator begin() {
        return iterator(this, get_first_not_empty_index());
    }

    const_iterator find(const KeyType &key) const {
        return const_iterator(this, get_index(key));
    }

    iterator find(const KeyType &key) {
        return iterator(this, get_index(key));
    }

    ValueType &operator[](const KeyType &key) {
        auto index = get_index(key);
        if (index == capacity_) {
            insert({key, ValueType()});
            index = get_index(key);
        }
        return container_[index]->second;
    }

    const ValueType &at(const KeyType &key) const {
        auto index = get_index(key);
        if (index == capacity_) {
            throw std::out_of_range("Out of range!");
        }
        return container_[get_index(key)]->second;
    }

    // clear container and other auxiliary vectors
    // reset size and capacity
    void clear() {
        container_.clear();
        first_index_.clear();
        deleted_.clear();
        size_ = 0;
        capacity_ = 0;
    }
};