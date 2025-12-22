
function json = build_rpc_request(method, params, id)
  % Wandelt eine JSON-RPC Anfrage in einen gültigen JSON-String um
  % method: Name der RPC-Methode (z.B. "plecs.load")
  % params: Cell-Array oder leer (z.B. {"modell.plecs"})
  % id: numerische ID der Anfrage

  % JSON-RPC Header
  json = sprintf('{"jsonrpc":"2.0","method":"%s",', method);

  % Parameter-Teil
  if nargin >= 2 && !isempty(params)
    % Baue params als JSON-Array
    jsonparams = jsonencode(params);
    json = [json, '"params":', jsonparams, ','];
  end

  % ID-Teil
  json = [json, sprintf('"id":%d}', id)];
end

