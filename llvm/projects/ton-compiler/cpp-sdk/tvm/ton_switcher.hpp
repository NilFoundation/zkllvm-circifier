#pragma once

#include <tvm/reflection.hpp>
#include <tvm/func_id.hpp>
#include <tvm/contract.hpp>
#include <tvm/smart_contract_info.hpp>
#include <tvm/schema/chain_builder.hpp>
#include <tvm/schema/chain_tuple.hpp>
#include <tvm/schema/chain_tuple_printer.hpp>
#include <tvm/schema/chain_fold.hpp>
#include <tvm/message_flags.hpp>
#include <tvm/awaiting_responses_map.hpp>
#include <tvm/resumable.hpp>
#include <tvm/globals.hpp>
#include <tvm/persistent_data_header.hpp>

#include <type_traits>

namespace tvm {

#ifndef NDEBUG
  #define DEBUG_PRINT(msg) __builtin_tvm_printstr(msg)
#else
  #define DEBUG_PRINT(msg)
#endif

template<typename T>
using _recv_internal_t = decltype(&T::_recv_internal);
template<typename T>
using _recv_external_t = decltype(&T::_recv_external);
template<typename T>
constexpr bool is_manual_dispatch_v = std::experimental::is_detected_v<_recv_internal_t, T> ||
                                      std::experimental::is_detected_v<_recv_external_t, T>;

template<typename T>
using on_ticktock_t = decltype(&T::on_ticktock);
template<typename T>
constexpr bool has_on_ticktock_v = std::experimental::is_detected_v<on_ticktock_t, T>;

template<typename T>
using fallback2_t = decltype(&T::_fallback);
template<typename T>
constexpr bool supports_fallback2_v = std::experimental::is_detected_v<fallback2_t, T>;

template<typename T>
using receive2_t = decltype(&T::_receive);
template<typename T>
constexpr bool supports_receive2_v = std::experimental::is_detected_v<receive2_t, T>;

template<typename T>
using on_bounced2_t = decltype(&T::_on_bounced);
template<typename T>
constexpr bool supports_on_bounced2_v = std::experimental::is_detected_v<on_bounced2_t, T>;

template <class Contract>
__always_inline Contract load_contract_data() {
  using DContract = typename Contract::data;
  Contract c;
  auto &base = static_cast<DContract &>(c);
  using data_tup_t = to_std_tuple_t<DContract>;
  using LinearTup = decltype(make_chain_tuple(data_tup_t{}));
  parser persist(persistent_data::get());
  auto data_tuple = parse<LinearTup>(persist);
  base = to_struct<DContract>(chain_fold_tup<data_tup_t>(data_tuple));
  return c;
}

template <class Contract>
__always_inline void save_contract_data(Contract& contract) {
  auto &base = static_cast<typename Contract::data &>(contract);
  auto data_tup = to_std_tuple(base);
  auto chain_tup = make_chain_tuple(data_tup);
  persistent_data::set(build(chain_tup).make_cell());
}

template<bool Internal, class Contract, class IContract, class DContract, unsigned Index, unsigned RestMethods>
struct dispatch {
  static const unsigned my_method_id = get_func_id<IContract, Index>();

  __always_inline static void execute(Contract& c, cell msg, slice msg_body, int func_id) {
    if constexpr (!get_interface_method_getter<IContract, Index>::value) {
      if (func_id == my_method_id) {
        using ISmart = typename Contract::base;
        constexpr auto func = get_interface_method_ptr<Contract, ISmart, Index>::value;

        using Args = get_interface_method_arg_struct<ISmart, Index>;
        constexpr unsigned args_sz = calc_fields_count<Args>::value;

        parser body_p(msg_body);

        if constexpr (args_sz != 0) {
          DEBUG_PRINT("parse_args");
          auto parsed_args = parse<Args>(body_p);
          DEBUG_PRINT("call_w_args");
          std::apply_f<func>(std::tuple_cat(std::make_tuple(std::ref(c)),
                                            to_std_tuple(parsed_args)));
        } else {
          DEBUG_PRINT("call_wo_args");
          (c.*func)();
        }
        return;
      }
    }

    using next_impl = dispatch<Internal, Contract, IContract, DContract, Index + 1,
                               RestMethods - 1>;
    next_impl::execute(c, msg, msg_body, func_id);
  }
};

template<bool Internal, class Contract, class IContract, class DContract, unsigned Index>
struct dispatch<Internal, Contract, IContract, DContract, Index, 0> {
  __always_inline static void execute(Contract& c, cell msg, slice msg_body, int func_id) {
    if constexpr (supports_fallback2_v<Contract>) {
      return c._fallback(msg, msg_body);
    } else {
      tvm_throw(error_code::wrong_public_call);
    }
  }
};

template <bool Internal, class Contract, class IContract, class DContract>
__always_inline int ton_switcher(cell msg, slice msg_body) {
  bool is_deploy = msg_has_init<Internal>(msg.ctos());

  if (is_deploy && msg_body.srempty()) {
    DEBUG_PRINT("deploy");
    tvm_accept();
    return 0;
  }

  Contract c = load_contract_data<Contract>();

  if constexpr (Internal) {
    DEBUG_PRINT("internal");
    slice msg_slice = msg.ctos();
    if constexpr (supports_msg_slice_v<Contract>)
      c.set_msg_slice(msg_slice);
    else
      set_msg(msg);
    if constexpr (supports_on_bounced2_v<Contract>) {
      if (is_bounced(msg_slice)) {
        DEBUG_PRINT("bounced");
        c._on_bounced(msg, msg_body);
        return;
      }
    } else {
      require(!is_bounced(msg_slice), error_code::unsupported_bounced_msg);
    }
  } else {
    DEBUG_PRINT("external");
    // Store msg cell into contract if it supports the required field
    // Otherwise, store into global (set_msg)
    if constexpr (supports_msg_slice_v<Contract>)
      c.set_msg_slice(msg);
    else
      set_msg(msg);
  }

  if constexpr (is_manual_dispatch_v<Contract>) {
    DEBUG_PRINT("manual_dispatch");
    if constexpr (Internal) {
      c._recv_internal(msg, msg_body);
    } else {
      c._recv_external(msg, msg_body);
    }
  } else {
    DEBUG_PRINT("regular_dispatch");
    static const unsigned methods_count = get_interface_methods_count<IContract>::value;
    parser body_p(msg_body);

    if (msg_body.empty() && msg_body.srempty()) {
      DEBUG_PRINT("receive");
      if constexpr (supports_receive2_v<Contract>) {
        c._receive(msg, msg_body);
        save_contract_data(c);
      }
      return 0;
    }
    unsigned func_id;
    if constexpr (supports_fallback2_v<Contract>) {
      if (auto opt_val = body_p.lduq(32))
        func_id = *opt_val;
      else {
        DEBUG_PRINT("fallback");
        c._fallback(msg, msg_body);
        save_contract_data(c);
        return 0;
      }
    } else {
      func_id = body_p.ldu(32);
    }
    DEBUG_PRINT("dispatch_start");
    dispatch<Internal, Contract, IContract, DContract, 0, methods_count>::execute(c, msg, body_p.sl(), func_id);
  }

  save_contract_data(c);

  return 0;
}

auto load_arg_proxy(parser& p, int x) {
  return p.ldu(256);
}

template<typename T, T... ints>
auto pop_args(parser p, std::integer_sequence<T, ints...>) {
  return std::make_tuple(p.ldu((ints, 256))...);
}

union GetterResult {
  GetterResult(unsigned v) : c(v) {}
  GetterResult(__tvm_tuple v) : t(v) {}
  unsigned c;
  __tvm_tuple t;
};

template<class Contract, class IContract, class DContract, unsigned Index, unsigned RestMethods>
struct getter_invoke_impl {
  static const unsigned my_method_id = get_func_id<IContract, Index>();

  __always_inline static GetterResult execute(Contract& c, int func_id, cell args_pack) {
    if constexpr (get_interface_method_getter<IContract, Index>::value) {
      if (func_id == my_method_id) {
        using ISmart = typename Contract::base;
        constexpr auto func = get_interface_method_ptr<Contract, ISmart, Index>::value;

        using Args = get_interface_method_arg_struct<ISmart, Index>;
        constexpr unsigned args_sz = calc_fields_count<Args>::value;

        if constexpr (args_sz != 0) {
          auto parsed_args = pop_args(parser(args_pack.ctos()),
                                      std::make_index_sequence<args_sz>{});
          auto rv = std::apply_f<func>(
              std::tuple_cat(std::make_tuple(std::ref(c)), parsed_args));
          if constexpr (std::is_same_v<decltype(rv), __tvm_tuple>) {
            return rv;
          } else {
            return rv.get();
          }
        } else {
          if constexpr (std::is_same_v<decltype((c.*func)()), __tvm_tuple>) {
            return (c.*func)();
          } else {
            return (c.*func)().get();
          }
        }
      }
    }
    using next_impl = getter_invoke_impl<Contract, IContract, DContract, Index + 1, RestMethods - 1>;
    return next_impl::execute(c, func_id, args_pack);
  }
};

template<class Contract, class IContract, class DContract, unsigned Index>
struct getter_invoke_impl<Contract, IContract, DContract, Index, 0> {
  __always_inline static GetterResult execute(Contract& c, int func_id, cell args_pack) {
    tvm_throw(error_code::wrong_public_call);
    return 0;
  }
};

template <class Contract, class IContract, class DContract>
GetterResult getter_invoke(int func_id, cell args_pack) {
  Contract c = load_contract_data<Contract>();
  static const unsigned methods_count = get_interface_methods_count<IContract>::value;
  return getter_invoke_impl<Contract, IContract, DContract, 0, methods_count>::execute(c, func_id, args_pack);
}

template <class Contract, class IContract, class DContract>
void main_ticktock_impl(bool is_tock, uint256 address, uint256 balance) {
  if constexpr (has_on_ticktock_v<Contract>) {
    Contract contract = load_contract_data<Contract>();
    contract.on_ticktock(is_tock, address, balance);
    save_contract_data(contract);
  }
}

}  // tvm