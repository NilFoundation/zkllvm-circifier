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
  #define DEBUG_VALUE(v) __builtin_tvm_dump_value(v)
#else
  #define DEBUG_PRINT(msg)
  #define DEBUG_VALUE(v)
#endif

// Define it as DEBUG_PRINT to see logs from the Switcher
#define SWITCHER_PRINT

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

template<typename T>
using has_signature_check_t = typename T::HasSignatureCheck;
template<typename T>
constexpr bool has_signature_check_v = std::experimental::is_detected_v<has_signature_check_t, T>;

template <class Contract>
__always_inline Contract load_contract_data() {
  SWITCHER_PRINT("load_smc_data");
  using DContract = typename Contract::data;
  Contract contract;
  auto &base = static_cast<DContract &>(contract);
  using data_tup_t = to_std_tuple_t<DContract>;
  using LinearTup = decltype(make_chain_tuple(data_tup_t{}));
  parser persist(persistent_data::get());
  auto data_tuple = parse<LinearTup>(persist);
  base = to_struct<DContract>(chain_fold_tup<data_tup_t>(data_tuple));
  return contract;
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

  __always_inline static void execute(Contract& contract, cell msg, slice msg_body, int func_id) {
    if constexpr (!get_interface_method_getter<IContract, Index>::value) {
      if (func_id == my_method_id) {
        using ISmart = typename Contract::base;
        constexpr auto func = get_interface_method_ptr<Contract, ISmart, Index>::value;

        using Args = get_interface_method_arg_struct<ISmart, Index>;
        constexpr unsigned args_sz = calc_fields_count<Args>::value;

        parser body_p(msg_body);

        if constexpr (args_sz != 0) {
          SWITCHER_PRINT("parse_args");
          auto parsed_args = parse<Args>(body_p);
          SWITCHER_PRINT("call_w_args");
          std::apply_f<func>(std::tuple_cat(std::make_tuple(std::ref(contract)),
                                            to_std_tuple(parsed_args)));
        } else {
          SWITCHER_PRINT("call_wo_args");
          (contract.*func)();
        }
        return;
      }
    }

    using next_impl = dispatch<Internal, Contract, IContract, DContract, Index + 1,
                               RestMethods - 1>;
    next_impl::execute(contract, msg, msg_body, func_id);
  }
};

template<bool Internal, class Contract, class IContract, class DContract, unsigned Index>
struct dispatch<Internal, Contract, IContract, DContract, Index, 0> {
  __always_inline static void execute(Contract& contract, cell msg, slice msg_body, int func_id) {
    if constexpr (supports_fallback2_v<Contract>) {
      return contract._fallback(msg, msg_body, Internal, func_id);
    } else {
      tvm_throw(error_code::wrong_public_call);
    }
  }
};

template <bool Internal, class Contract, class IContract, class DContract>
__always_inline int ton_switcher(cell msg, slice msg_body) {
  bool is_deploy = msg_has_init<Internal>(msg.ctos());

  if (is_deploy && msg_body.srempty()) {
    SWITCHER_PRINT("deploy");
    tvm_accept();
    return 0;
  }

  Contract contract = load_contract_data<Contract>();

  contract.set_msg_body(msg_body);

  if constexpr (Internal) {
    SWITCHER_PRINT("internal");
    slice msg_slice = msg.ctos();
    if constexpr (supports_msg_slice_v<Contract>)
      contract.set_msg_slice(msg_slice);
    else
      set_msg(msg);
    if constexpr (supports_on_bounced2_v<Contract>) {
      if (is_bounced(msg_slice)) {
        SWITCHER_PRINT("bounced");
        contract._on_bounced(msg, msg_body);
        return;
      }
    } else {
      require(!is_bounced(msg_slice), error_code::unsupported_bounced_msg);
    }
  } else {
    SWITCHER_PRINT("external");
    // Store msg cell into contract if it supports the required field
    // Otherwise, store into global (set_msg)
    if constexpr (supports_msg_slice_v<Contract>)
      contract.set_msg_slice(msg);
    else
      set_msg(msg);
  }

  if constexpr (is_manual_dispatch_v<Contract>) {
    SWITCHER_PRINT("manual_dispatch");
    if constexpr (Internal) {
      contract._recv_internal(msg, msg_body);
    } else {
      contract._recv_external(msg, msg_body);
    }
  } else {
    SWITCHER_PRINT("regular_dispatch");
    static const unsigned methods_count = get_interface_methods_count<IContract>::value;
    parser body_p(msg_body);

    if (msg_body.empty() && msg_body.srempty()) {
      SWITCHER_PRINT("receive");
      if constexpr (supports_receive2_v<Contract>) {
        contract._receive(msg, msg_body);
        save_contract_data(contract);
      }
      return 0;
    }
    // Check signature for the external message if contract supports it
    if constexpr (!Internal && has_signature_check_v<Contract>) {
      auto signature = body_p.ldslice(512);
      bool sign_ok = signature_checker_v2::check(body_p.sl(), signature, contract._pubkey());
      require(sign_ok, error_code::invalid_signature);
    }
    unsigned func_id;
    if constexpr (supports_fallback2_v<Contract>) {
      if (auto opt_val = body_p.lduq(32))
        func_id = *opt_val;
      else {
        SWITCHER_PRINT("fallback");
        contract._fallback(msg, msg_body, Internal, -1);
        save_contract_data(contract);
        return 0;
      }
    } else {
      func_id = body_p.ldu(32);
    }
    SWITCHER_PRINT("dispatch_start");
    dispatch<Internal, Contract, IContract, DContract, 0, methods_count>::execute(
        contract, msg, body_p.sl(), func_id);
  }

  save_contract_data(contract);

  return 0;
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

  __always_inline static GetterResult execute(Contract& contract, int func_id, cell args_pack) {
    if constexpr (get_interface_method_getter<IContract, Index>::value) {
      if (func_id == my_method_id) {
        using ISmart = typename Contract::base;
        constexpr auto func = get_interface_method_ptr<Contract, ISmart, Index>::value;

        using Args = get_interface_method_arg_struct<ISmart, Index>;
        constexpr unsigned args_sz = calc_fields_count<Args>::value;
        SWITCHER_PRINT("call_getter");

        if constexpr (args_sz != 0) {
          auto parsed_args = pop_args(parser(args_pack.ctos()),
                                      std::make_index_sequence<args_sz>{});
          auto rv = std::apply_f<func>(
              std::tuple_cat(std::make_tuple(std::ref(contract)), parsed_args));
          if constexpr (std::is_same_v<decltype(rv), __tvm_tuple>) {
            return rv;
          } else {
            return rv.get();
          }
        } else {
          if constexpr (std::is_same_v<decltype((contract.*func)()), __tvm_tuple>) {
            return (contract.*func)();
          } else {
            return (contract.*func)().get();
          }
        }
      }
    }
    using next_impl = getter_invoke_impl<Contract, IContract, DContract, Index + 1, RestMethods - 1>;
    return next_impl::execute(contract, func_id, args_pack);
  }
};

template<class Contract, class IContract, class DContract, unsigned Index>
struct getter_invoke_impl<Contract, IContract, DContract, Index, 0> {
  __always_inline static GetterResult execute(Contract& contract, int func_id, cell args_pack) {
    tvm_throw(error_code::wrong_public_call);
    return 0;
  }
};

template <class Contract, class IContract, class DContract>
GetterResult getter_invoke(int func_id, cell args_pack) {
  SWITCHER_PRINT("getter_invoke");
  Contract contract = load_contract_data<Contract>();
  static const unsigned methods_count = get_interface_methods_count<IContract>::value;
  return getter_invoke_impl<Contract, IContract, DContract, 0, methods_count>::execute(contract, func_id, args_pack);
}

template <class Contract, class IContract, class DContract>
void main_ticktock_impl(uint256 balance, uint256 address, bool is_tock) {
  SWITCHER_PRINT("ticktock");
  if constexpr (has_on_ticktock_v<Contract>) {
    Contract contract = load_contract_data<Contract>();
    contract.on_ticktock(balance, address, is_tock);
    save_contract_data(contract);
  }
}

}  // tvm