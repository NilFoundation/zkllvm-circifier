#include "evm_sdk.h"
#include <utility>

namespace evm::stor {

namespace details {

template <unsigned... Slots>
void remove_sequence(uint256_t slot,
                     std::integer_sequence<unsigned, Slots...> seq) {
  (__builtin_evm_sstore(slot + Slots, 0), ...);
}

template <typename T> void clear(uint256_t slot) {
  if constexpr (std::is_integral_v<T>) {
    __builtin_evm_sstore(slot, 0);
  } else {
    remove_sequence(slot,
                    std::make_integer_sequence<unsigned, T::FIELDS_NUM>{});
  }
}

template <class T, unsigned slot_>
struct SlotStaticImpl {
  using ValueType = T;

  static constexpr uint256_t slot() {
    return slot_;
  }
};

template <class T>
struct SlotImpl {
  using ValueType = T;

  explicit SlotImpl(uint256_t slot) : slot_(slot) {}

  uint256_t slot() const {
    return slot_;
  }

  uint256_t slot_;
};

}  // namespace details

template <class Impl>
struct SlotBase: private Impl {
  using ValueType = typename Impl::ValueType;
  using Impl::Impl;

  void set(const ValueType &v) {
    __builtin_evm_sstore(Impl::slot(), v);
  }

  ValueType get() const {
    return __builtin_evm_sload(Impl::slot());
  }

  SlotBase& operator=(const ValueType &v) {
    set(v);
    return *this;
  }

  operator ValueType() const {
    return get();
  }
};

template<typename T>
using Slot = SlotBase<details::SlotImpl<T>>;

template<typename T, uint256_t Key>
using SlotStatic = SlotBase<details::SlotStaticImpl<T, Key>>;

/*******************************************************************************
 * Dynamic array.
 */
template<typename Impl>
class VectorBase: private Impl {
public:
  using ValueType = typename Impl::ValueType;
  using Impl::Impl;

  auto operator[](uint256_t k) {
    if constexpr (std::is_integral_v<ValueType>) {
      return Slot<ValueType>(get_storage_key() + k);
    } else {
      return ValueType(get_storage_key() + k * ValueType::FIELDS_NUM);
    }
  }

  auto operator[](uint256_t k) const {
    return const_cast<VectorBase*>(this)->operator[](k);
  }

  unsigned size() const {
    return __builtin_evm_sload(Impl::slot());
  }

  bool empty() const {
    return size() == 0;
  }

  auto push() {
    auto sz = size();
    uint256_t new_size = sz + 1;
    __builtin_evm_sstore(Impl::slot(), new_size);
    return operator[](sz);
  }

  void pop() {
    auto sz = size();
    require(sz != 0);
    details::clear<ValueType>(get_storage_key() + sz - 1);
    __builtin_evm_sstore(Impl::slot(), sz - 1);
  }

private:
  uint256_t get_storage_key() const {
    return __builtin_evm_sha3_vargs(Impl::slot());
  }
};

template<typename T>
using Vector = VectorBase<details::SlotImpl<T>>;

template<typename T, uint256_t Slot>
using VectorStatic = VectorBase<details::SlotStaticImpl<T, Slot>>;

/*******************************************************************************
 * Basic map.
 * This class represent basic map, that only allows to properly calculate unique
 * keys and access the elements.
 */
template <typename Impl>
struct MapBase: private Impl {
  using ValueType = typename Impl::ValueType;
  using Impl::Impl;

  auto operator[](uint256_t k) {
    auto k1 = __builtin_evm_sha3_vargs(Impl::slot(), k);
    if constexpr (std::is_integral_v<ValueType>) {
      return Slot<ValueType>(k1);
    } else {
      return ValueType(k1);
    }
  }

  auto operator[](uint256_t k) const {
    return const_cast<MapBase*>(this)->operator[](k);
  }

  void remove(uint256_t key) const {
    auto slot = __builtin_evm_sha3_vargs(Impl::slot(), key);
    details::clear<ValueType>(slot);
  }
};

template<typename T>
using Map = MapBase<details::SlotImpl<T>>;

template<typename T, uint256_t Slot>
using MapStatic = MapBase<details::SlotStaticImpl<T, Slot>>;

/*******************************************************************************
 * Iterable map.
 * Unlike the Map this class allows to iterate over all elements in map, remove
 * elements, find elements and get size of the map.
 */
template<typename Impl>
class IterableMapBase: private Impl {
public:
  using Impl::Impl;

  using ValueType = typename Impl::ValueType;

  static constexpr unsigned FIELDS_NUM = 3;

  unsigned size() const {
    return Impl::keys.size();
  }

  bool empty() const {
    return size() == 0;
  }

  bool contains(uint256_t key) const {
    return Impl::inserted[key];
  }

  uint256_t get_key_at(uint256_t index) const {
    require(index < size());
    return Impl::keys[index].get();
  }

  void remove(uint256_t key) {
    if (!contains(key)) {
      return;
    }
    Impl::inserted[key] = false;
    Impl::values.remove(key);
    auto index = Impl::indexes[key].get();
    auto last_key = Impl::keys[size() - 1].get();

    Impl::indexes[last_key] = index;
    Impl::indexes.remove(0);

    Impl::keys[index] = last_key;
    Impl::keys.pop();
  }

  auto operator[](uint256_t key) {
    if (Impl::inserted[key].get()) {
      return Impl::values[key];
    } else {
      Impl::inserted[key] = true;
      Impl::indexes[key] = Impl::keys.size();
      Impl::keys.push().set(key);
      return Impl::values[key];
    }
  }
};

template<typename T>
struct IterableMapImpl {
  using ValueType = T;

  IterableMapImpl(uint256_t slot): keys(slot), values(slot + 1), indexes(slot + 2), inserted(slot + 3) {}

  static constexpr unsigned FIELDS_NUM = 4;

  Vector<uint256_t> keys;
  Map<ValueType> values;
  Map<uint256_t> indexes;
  Map<bool> inserted;
};

template<typename T, uint256_t Slot>
struct IterableMapStaticImpl {
  using ValueType = T;

  static constexpr unsigned FIELDS_NUM = 4;

  VectorStatic<uint256_t, Slot> keys;
  MapStatic<ValueType, Slot + 1> values;
  MapStatic<uint256_t, Slot + 2> indexes;
  MapStatic<bool, Slot + 3> inserted;
};

template<typename T>
using IterableMap = IterableMapBase<IterableMapImpl<T>>;

template<typename T, uint256_t Slot>
using IterableMapStatic = IterableMapBase<IterableMapStaticImpl<T, Slot>>;

}  // namespace evm::stor
