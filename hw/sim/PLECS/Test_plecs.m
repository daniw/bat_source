
rpc_request = struct(
  "jsonrpc", "2.0",
  "method", "plecs.load",
  "params", {["buck_boost_converters_stm.plecs"]},
  "id", 1
);

json_data = build_rpc_request("plecs.load", {"E:/Repos/bat_source/hw/sim/PLECS/buck_boost_converters_stm.plecs"}, 1)

url = "http://localhost:1080";  % PLECS RPC Server
headers = "Content-Type: application/json";

[s, success, message] = urlread(url, "post", {"query",json_data})


