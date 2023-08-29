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


namespace tvm {

template <bool Internal, class Contract, class IContract, class DContract>
__always_inline int simple_selector(cell msg, slice msg_body) {
  bool is_deploy = msg_has_init<Internal>(msg.ctos());

  if (is_deploy && msg_body.srempty()) {
    tvm_accept();
    return 0;
  }

  Contract c;
  {
    auto &base = static_cast<DContract &>(c);
    using data_tup_t = to_std_tuple_t<DContract>;
    using LinearTup = decltype(make_chain_tuple<1, 0>(data_tup_t{}));
    parser persist(persistent_data::get());
    auto data_tuple = parse<data_tup_t>(persist);
    base = to_struct<DContract>(chain_fold_tup<data_tup_t>(data_tuple));
  }

  if constexpr (Internal) {
    c._recv_internal(msg, msg_body);
  } else {
    c._recv_external(msg, msg_body);
  }

  {
    auto &base = static_cast<DContract &>(c);
    auto data_tuple = to_std_tuple(base);
    persistent_data::set(build(data_tuple).make_cell());
  }

  return 0;
}

template<class Contract, class IContract, class DContract, unsigned Index, unsigned RestMethods>
struct getmethod_invoker {
  static const unsigned my_method_id = get_func_id<IContract, Index>();

  __always_inline
  static int execute(Contract& c, int func_id) {
    if (func_id == my_method_id) {
      using ISmart = typename Contract::base;
      constexpr auto func = get_interface_method_ptr<Contract, ISmart, Index>::value;

      auto rv = (c.*func)();

      return unsigned(rv);
    }
    using next_impl = getmethod_invoker<Contract, IContract, DContract, Index + 1, RestMethods - 1>;
    return next_impl::execute(c, func_id);
  }
};

template<class Contract, class IContract, class DContract, unsigned Index>
struct getmethod_invoker<Contract, IContract, DContract, Index, 0> {
  __always_inline
  static int execute(Contract& c, int func_id) {
    tvm_throw(error_code::wrong_public_call);
    return 0;
  }
};

template <class Contract, class IContract, class DContract>
int getter_invoke_impl(int func_id) {
  Contract c;
  auto &base = static_cast<DContract &>(c);
  using data_tup_t = to_std_tuple_t<DContract>;
  parser persist(persistent_data::get());
  auto data_tuple = parse<data_tup_t>(persist);
  base = to_struct<DContract>(chain_fold_tup<data_tup_t>(data_tuple));

  static const unsigned methods_count = get_interface_methods_count<IContract>::value;

  return getmethod_invoker<Contract, IContract, DContract, 0, methods_count>::execute(c, func_id);
}

}  // tvm