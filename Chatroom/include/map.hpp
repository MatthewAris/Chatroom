#ifndef LIST_HPP
#define LIST_HPP

#include <algorithm>
#include <functional>
#include <map>
#include <mutex>

template <typename K, typename V>
class Map
{
    public:
        unsigned short size()
        {
            std::unique_lock<std::mutex> mlock(mutex_);
            return map_.size();
        }

        void for_each(std::function<void(std::pair<K, V>)> f)
        {
            std::unique_lock<std::mutex> mlock(mutex_);
            std::for_each(map_.begin(), map_.end(), f);
        }

        void insert(const std::pair<K, V>& p)
        {
            std::unique_lock<std::mutex> mlock(mutex_);
            auto it = map_.find(p.first);
            if (it == map_.end())
            {
                map_.insert(p);
            }
        }

        Map()=default;
        Map(const Map&) = delete;            // disable copying
        Map& operator=(const Map&) = delete; // disable assignment

    private:
        std::map<K, V> map_;
        std::mutex mutex_;
};

#endif
