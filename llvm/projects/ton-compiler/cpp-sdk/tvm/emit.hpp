#pragma once

namespace tvm {

template<class Interface, class Func, class... Args>
using good_emit_args_t = decltype( (((Interface*)nullptr)->*(Func::value))(Args{}...) );
template<class Interface, auto Func, class... Args>
constexpr bool good_emit_args_v = std::experimental::is_detected_v<good_emit_args_t, Interface, proxy_method<Func>, Args...>;

template<class T>
struct extract_class : std::false_type {};
template<class R, class C, class... A>
struct extract_class<R (C::*)(A...)> {
  using type = C;
};

template<auto EventPtr, class... Args>
void emit(Args... args) {
  using Interface = typename extract_class<decltype(EventPtr)>::type;
  static_assert(good_emit_args_v<Interface, EventPtr, Args...>, "Wrong emit arguments");

  using namespace schema;

  abiv1::external_outbound_msg_header hdr{ uint32(id_v<EventPtr>) };
  auto hdr_plus_args = std::make_tuple(hdr, args...);
  ext_out_msg_info_relaxed out_info;
  out_info.src = addr_none{};
  out_info.dest = addr_none{}; // TODO: which address to use as dest for events?
  out_info.created_lt = 0;
  out_info.created_at = 0;

  auto chain_tup = make_chain_tuple(hdr_plus_args);
  message_relaxed<decltype(chain_tup)> out_msg;
  out_msg.info = out_info;
  out_msg.body = ref<decltype(chain_tup)>{chain_tup};
  tvm_sendmsg(build(out_msg).endc(), 0);
}

} // namespace tvm
