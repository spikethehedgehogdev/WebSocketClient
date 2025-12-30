unit WsClientApi;

interface

uses
  Winapi.Windows;

type
  TWebSocketClient = Pointer;

  type
  TOnConnected = procedure(user: Pointer; connected: Integer); cdecl;
  TOnPongTimeout = procedure(user: Pointer); cdecl;
  TOnReconnect = procedure(user: Pointer; attempt: Integer; delay_ms: Integer); cdecl;
  TOnData = procedure(user: Pointer; data: PAnsiChar; size: Integer; is_text: Integer); cdecl;
  TOnError = procedure(user: Pointer; code: Integer; message: PAnsiChar); cdecl;
  TOnLog = procedure(user: Pointer; message: PAnsiChar); cdecl;

  TWsClientCallbacks = record
    on_connected: TOnConnected;
    on_pong_timeout: TOnPongTimeout;
    on_reconnect: TOnReconnect;
    on_data: TOnData;
    on_error: TOnError;
    on_log: TOnLog;
  end;

  PWsClientCallbacks = ^TWsClientCallbacks;

function ws_client_create: TWebSocketClient; cdecl; external 'WebSocketClient.Core.dll';
procedure ws_client_destroy(client: TWebSocketClient); cdecl; external 'WebSocketClient.Core.dll';

procedure ws_client_connect(
  client: TWebSocketClient;
  url: PAnsiChar;
  token: PAnsiChar;
  callbacks: PWsClientCallbacks;
  user: Pointer
); cdecl; external 'WebSocketClient.Core.dll';

procedure ws_client_disconnect(client: TWebSocketClient); cdecl; external 'WebSocketClient.Core.dll';

procedure ws_client_send_text(client: TWebSocketClient; text: PAnsiChar); cdecl;
  external 'WebSocketClient.Core.dll';

procedure ws_client_set_ping(
  client: TWebSocketClient;
  interval_ms: Integer;
  timeout_ms: Integer
); cdecl; external 'WebSocketClient.Core.dll';

implementation

end.
