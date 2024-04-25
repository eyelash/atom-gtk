#ifndef LAYOUT_CACHE_H_
#define LAYOUT_CACHE_H_

#include <display-layer.h>
#include <unordered_map>

template <class T> void hash_combine(size_t &seed, T const &v);
inline size_t hash_value(char v) {
  return v;
}
inline size_t hash_value(char16_t v) {
  return v;
}
inline size_t hash_value(int32_t v) {
  return v;
}
template <class T> std::size_t hash_value(const T *ptr) {
  return reinterpret_cast<std::size_t>(ptr);
}
template <class It> std::size_t hash_range(It first, It last) {
  size_t seed = 0;
  for (; first != last; ++first) {
    hash_combine(seed, *first);
  }
  return seed;
}
template <class CharT> size_t hash_value(const std::basic_string<CharT> &v) {
  return hash_range(v.begin(), v.end());
}
template <class T> size_t hash_value(const std::vector<T> &v) {
  return hash_range(v.begin(), v.end());
}
template <class T> void hash_combine(size_t &seed, T const &v) {
  seed ^= hash_value(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <class Component, class Layout> class LayoutCache {
  struct Hash {
    size_t operator ()(const DisplayLayer::ScreenLine &screen_line) const {
      size_t seed = 0;
      hash_combine(seed, screen_line.lineText);
      hash_combine(seed, screen_line.tags);
      return seed;
    }
  };
  struct Equal {
    bool operator ()(const DisplayLayer::ScreenLine &lhs, const DisplayLayer::ScreenLine &rhs) const {
      return lhs.lineText == rhs.lineText && lhs.tags == rhs.tags;
    }
  };
  std::unordered_map<DisplayLayer::ScreenLine, std::pair<Layout, size_t>, Hash, Equal> cache;
  std::unordered_map<double, std::pair<Layout, size_t>> line_number_cache;
  size_t generation = 0;
public:
  void collect_garbage() {
    for (auto iterator = cache.begin(); iterator != cache.end();) {
      if (iterator->second.second != generation) {
        iterator = cache.erase(iterator);
      } else {
        ++iterator;
      }
    }
    for (auto iterator = line_number_cache.begin(); iterator != line_number_cache.end();) {
      if (iterator->second.second != generation) {
        iterator = line_number_cache.erase(iterator);
      } else {
        ++iterator;
      }
    }
  }
  void increment_generation() {
    generation++;
  }
  Layout get_layout(Component *self, const DisplayLayer::ScreenLine &screen_line) {
    auto iterator = cache.find(screen_line);
    if (iterator != cache.end()) {
      iterator->second.second = generation;
      return iterator->second.first;
    } else {
      Layout layout(self, screen_line);
      cache.insert({screen_line, {layout, generation}});
      return layout;
    }
  }
  std::vector<Layout> get_layouts(Component *self, const std::vector<DisplayLayer::ScreenLine> &screen_lines) {
    std::vector<Layout> layouts;
    for (size_t i = 0; i < screen_lines.size(); i++) {
      layouts.push_back(get_layout(self, screen_lines[i]));
    }
    return layouts;
  }
  Layout get_line_number(Component *self, double row) {
    auto iterator = line_number_cache.find(row);
    if (iterator != line_number_cache.end()) {
      iterator->second.second = generation;
      return iterator->second.first;
    } else {
      Layout layout(self, row);
      line_number_cache.insert({row, {layout, generation}});
      return layout;
    }
  }
};

#endif  // LAYOUT_CACHE_H_
