#pragma once

#include <tuple>

#include <tvm/cell.hpp>
#include <tvm/slice.hpp>
#include <tvm/builder.hpp>

#include <tvm/schema/make_builder.hpp>
#include <tvm/schema/make_parser.hpp>
#include <tvm/dictionary.hpp>

namespace tvm {

/**
 * Just a wrapper class over `dictionary` class with key length as a template
 * argument. This way we avoid passing the key length to each method.
 * @tparam KeyLen size in bits of the key
 */
template<unsigned KeyLen>
class Dict {
public:
  using key_type = uint_t<KeyLen>;

  struct DictIterator;

  using const_iterator = DictIterator;

  __always_inline Dict() {}
  __always_inline explicit Dict(cell dict) : dict_{dict} {}
  __always_inline Dict(dictionary dict) : dict_{dict} {}
  __always_inline Dict(schema::anydict dict) : dict_{dict} {}

  __always_inline void clear() {
    dict_.clear();
  }

  __always_inline void set(unsigned key, slice val) {
    dict_.dictuset(val, key, KeyLen);
  }
  template<unsigned bits>
  __always_inline void set(uint_t<bits> key, slice val) {
    set(key.get(), val);
  }

  __always_inline slice get(unsigned key) {
    auto [sl, success] = dict_.dictuget(key, KeyLen);
    return success ? sl : slice();
  }

  template<unsigned bits>
  __always_inline slice get(uint_t<bits> key) {
    return get(key.get());
  }

  __always_inline slice setget(unsigned key, slice value) {
    auto [sl, success] = dict_.dictusetget(value, key, KeyLen);
    return success ? sl : slice();
  }

  __always_inline cell setgetref(unsigned key, cell value) {
    auto [cl, success] = dict_.dictusetgetref(value, key, KeyLen);
    return success ? cl : cell();
  }

  __always_inline void setref(unsigned key, cell val) {
    dict_.dictusetref(val, key, KeyLen);
  }

  __always_inline cell getref(unsigned key) {
    auto [cl, success] = dict_.dictigetref(key, KeyLen);
    return success ? cl : cell();
  }

  __always_inline bool erase(unsigned key) {
    return dict_.dictudel(key, KeyLen);
  }

  template<unsigned bits>
  __always_inline bool erase(uint_t<bits> key) {
    return erase(key.get());
  }

  __always_inline std::pair<slice, unsigned> next(unsigned key) {
    auto [sl, idx, success] = dict_.dictugetnext(key, KeyLen);
    if (!success) {
      return {slice::null(), 0};
    }
    return {sl, idx};
  }

  __always_inline const dictionary& dict() const {
    return dict_;
  }

  __always_inline dictionary& dict() {
    return dict_;
  }


  const_iterator begin() const {
    return const_iterator::create_begin(dict_);
  }
  const_iterator end() const {
    return const_iterator::create_end(dict_);
  }

  dictionary dict_;
};

template<unsigned KeyLen>
struct Dict<KeyLen>::DictIterator: boost::operators<Dict<KeyLen>::DictIterator> {
  using Pair = std::pair<schema::uint_t<KeyLen>, slice>;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = slice;
  using difference_type = int;
  using pointer = std::pair<schema::uint_t<KeyLen>, slice>*;
  using reference = std::pair<schema::uint_t<KeyLen>, slice>&;

  dictionary dict_;
  unsigned idx_;
  std::optional<slice> sl_;

  __always_inline Pair operator*() const {
    require(sl_.has_value(), error_code::iterator_overflow);
    return {schema::uint_t<KeyLen>(idx_), sl_.value()};
  }
  __always_inline bool is_end() const {
    return !sl_;
  }

  static DictIterator create_begin(dictionary dict) {
    auto [sl, idx, succ] = dict.dictumin(KeyLen);
    return DictIterator{{}, dict, idx, succ ? sl : std::optional<slice>()};
  }
  static DictIterator create_end(dictionary dict) {
    return DictIterator{{}, dict, 0, {}};
  }

  bool operator<(DictIterator x) const { return idx_ < x.idx_; }

  DictIterator& operator++() {
    auto [sl, next_idx, succ] = dict_.dictugetnext(idx_, KeyLen);
    if (succ) {
      sl_ = sl;
      idx_ = next_idx;
    } else {
      sl_.reset();
    }
    return *this;
  }
  DictIterator& operator--() {
    auto [sl, prev_idx, succ] = dict_.dictugetprev(idx_, KeyLen);
    if (succ) {
      sl_ = sl;
      idx_ = prev_idx;
    } else {
      sl_.reset();
    }
    return *this;
  }
  bool operator==(DictIterator v) const {
    bool left_end = is_end();
    bool right_end = v.is_end();
    return (left_end && right_end) || (!left_end && !right_end && idx_ == v.idx_);
  }
};

} // namespace tvm
