#ifndef LAYOUT_CACHE_H_
#define LAYOUT_CACHE_H_

#include <display-layer.h>
#include <unordered_map>

template <class T> void hash_combine(size_t &seed, T const &v);
inline size_t hash_value(char16_t v) {
  return v;
}
inline size_t hash_value(int32_t v) {
  return v;
}
template <class It> std::size_t hash_range(It first, It last) {
  size_t seed = 0;
  for (; first != last; ++first) {
    hash_combine(seed, *first);
  }
  return seed;
}
inline size_t hash_value(const std::u16string &v) {
  return hash_range(v.begin(), v.end());
}
template <class T> size_t hash_value(const std::vector<T> &v) {
  return hash_range(v.begin(), v.end());
}
template <class T> void hash_combine(size_t &seed, T const &v) {
  seed ^= hash_value(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <class Component, class Layout, Layout (*create_layout)(Component *, const DisplayLayer::ScreenLine &), void (*free_layout)(Layout)>
class LayoutCache {
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
  size_t generation;
  void collect_garbage() {
    auto iterator = cache.begin();
    while (iterator != cache.end()) {
      if (iterator->second.second < generation) {
        free_layout(iterator->second.first);
        iterator = cache.erase(iterator);
      } else {
        ++iterator;
      }
    }
  }
public:
  LayoutCache(): generation(0) {}
  ~LayoutCache() {
    auto iterator = cache.begin();
    while (iterator != cache.end()) {
      free_layout(iterator->second.first);
      ++iterator;
    }
  }
  Layout get_layout(Component *self, const DisplayLayer::ScreenLine &screen_line) {
    auto iterator = cache.find(screen_line);
    if (iterator != cache.end()) {
      iterator->second.second = generation;
      return iterator->second.first;
    } else {
      Layout layout = create_layout(self, screen_line);
      cache.insert({screen_line, {layout, generation}});
      return layout;
    }
  }
  std::vector<Layout> get_layouts(Component *self, const std::vector<DisplayLayer::ScreenLine> &screen_lines) {
    generation++;
    std::vector<Layout> layouts;
    for (size_t i = 0; i < screen_lines.size(); i++) {
      layouts.push_back(get_layout(self, screen_lines[i]));
    }
    collect_garbage();
    return layouts;
  }
};

#endif  // LAYOUT_CACHE_H_
