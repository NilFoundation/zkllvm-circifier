
//namespace evm::stor {

#ifdef __STORAGE_CONFIG
#define EVM_SSTORE
#define EVM_SLOAD __builtin_evm_cload
#else
#define EVM_SSTORE __builtin_evm_sstore
#define EVM_SLOAD __builtin_evm_sload
#endif

namespace details {

//struct ConfigAccessor {
//  static uint256_t load(uint256_t slot) {
//    return EVM_SLOAD(slot);
//  }
//
//  static void store(uint256_t slot, uint256_t v) {
//    EVM_SSTORE(0, 0);
//  }
//};

struct StorageAccessor {
  static uint256_t load(uint256_t slot) {
    return EVM_SLOAD(slot);
  }

  static void store(uint256_t slot, uint256_t v) {
#ifdef __STORAGE_CONFIG
    __builtin_evm_revert(nullptr, 0);
#else
    EVM_SSTORE(slot, v);
#endif
  }
};

template <unsigned... Slots>
void remove_sequence(uint256_t slot,
                     std::integer_sequence<unsigned, Slots...> seq) {
  (StorageAccessor::store(slot + Slots, 0), ...);
}

template <typename T>
void clear(uint256_t slot) {
  if constexpr (std::is_integral_v<T>) {
    StorageAccessor::store(slot, 0);
  } else {
    remove_sequence(slot,
                    std::make_integer_sequence<unsigned, T::FIELDS_NUM>{});
  }
}


template <typename T, typename Accessor, unsigned slot_>
struct SlotStaticImpl {
  using ValueType = T;

  static constexpr uint256_t slot() {
    return slot_;
  }

  uint256_t load() const {
    return Accessor::load(slot_);
  }

  void store(uint256_t v) const {
    Accessor::store(slot_, v);
  }

//  void clear() {
//    store(0);
//  }
};

template <class T, typename Accessor>
struct SlotImpl {
  using ValueType = T;

  explicit SlotImpl(uint256_t slot) : slot_(slot) {}

  uint256_t slot() const {
    return slot_;
  }

  uint256_t load() const {
    return Accessor::load(slot_);
  }

  void store(uint256_t v) const {
    Accessor::store(slot_, v);
  }

//  void clear() {
//      store(0);
//  }

  uint256_t slot_;
};

}  // namespace details

template <class Impl>
struct SlotBase: private Impl {
  using ValueType = typename Impl::ValueType;
  using Impl::Impl;

  void set(const ValueType &v) {
    Impl::store(v);
  }

  uint256_t slot() const {
    return Impl::slot();
  }

  ValueType get() const {
    return Impl::load();
  }

  SlotBase& operator=(const ValueType &v) {
    set(v);
    return *this;
  }

  SlotBase& operator+=(const ValueType &v) {
    set(get() + v);
    return *this;
  }

  operator ValueType() const {
    return get();
  }

  void clear() {
    set(0);
  }
};

template<typename T>
using Slot = SlotBase<details::SlotImpl<T, details::StorageAccessor>>;

template<typename T, uint256_t Key>
using SlotStatic = SlotBase<details::SlotStaticImpl<T, details::StorageAccessor, Key>>;

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
    return Impl::load();
  }

  bool empty() const {
    return size() == 0;
  }

  auto push() {
    auto sz = size();
    uint256_t new_size = sz + 1;
    Impl::store(new_size);
    return operator[](sz);
  }

  void pop() {
    auto sz = size();
    require(sz != 0);
    details::clear<ValueType>(get_storage_key() + sz - 1);
    Impl::store(sz - 1);
  }

  void clear() {
    for (unsigned i = 0; i < size(); i++) {
      (*this)[i].clear();
    }
    Impl::store(0);
  }

private:
  uint256_t get_storage_key() const {
    return __builtin_evm_sha3_vargs(Impl::slot());
  }
};

template<typename T>
using Vector = VectorBase<details::SlotImpl<T, details::StorageAccessor>>;

template<typename T, uint256_t Slot>
using VectorStatic = VectorBase<details::SlotStaticImpl<T, details::StorageAccessor, Slot>>;

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
using Map = MapBase<details::SlotImpl<T, details::StorageAccessor>>;

template<typename T, uint256_t Slot>
using MapStatic = MapBase<details::SlotStaticImpl<T, details::StorageAccessor, Slot>>;

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

  auto get_at_index(uint256_t index) {
    auto key = get_key_at(index);
    return operator[](key);
  }

  void clear() {
    for (unsigned i = 0; i < size(); i++) {
      auto key = Impl::keys[i].get();
      remove(key);
    }
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

  std::optional<ValueType> lookup(uint256_t key) {
    if (!contains(key)) {
      return {};
    }
    return Impl::values[key];
  }

  auto operator[](uint256_t key) {
    if (contains(key)) {
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

//}  // namespace evm::stor

//namespace evm::config {
//
//template<typename T>
//using Slot = evm::stor::SlotBase<evm::stor::details::SlotImpl<T, evm::stor::details::ConfigAccessor>>;
//template<typename T, uint256_t Key>
//using SlotStatic = evm::stor::SlotBase<evm::stor::details::SlotStaticImpl<T, evm::stor::details::ConfigAccessor, Key>>;
//
//template<typename T>
//using Vector = evm::stor::VectorBase<evm::stor::details::SlotImpl<T, evm::stor::details::ConfigAccessor>>;
//template<typename T, uint256_t Slot>
//using VectorStatic = evm::stor::VectorBase<evm::stor::details::SlotStaticImpl<T, evm::stor::details::ConfigAccessor, Slot>>;
//
//template<typename T>
//using Map = evm::stor::MapBase<evm::stor::details::SlotImpl<T, evm::stor::details::ConfigAccessor>>;
//template<typename T, uint256_t Slot>
//using MapStatic = evm::stor::MapBase<evm::stor::details::SlotStaticImpl<T, evm::stor::details::ConfigAccessor, Slot>>;
//
//template<typename T>
//using IterableMap = evm::stor::IterableMapBase<IterableMapImpl<T>>;
//template<typename T, uint256_t Slot>
//using IterableMapStatic = evm::stor::IterableMapBase<IterableMapStaticImpl<T, Slot>>;
//
//}  // namespace evm::config
