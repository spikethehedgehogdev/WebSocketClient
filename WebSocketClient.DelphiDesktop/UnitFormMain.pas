unit UnitFormMain;

interface

uses
  Winapi.Windows,
  Winapi.Messages,
  System.SysUtils,
  System.Variants,
  System.Classes,
  Vcl.Graphics,
  Vcl.Controls,
  Vcl.Forms,
  Vcl.Dialogs,
  Vcl.StdCtrls,
  WsClientApi;

type
  TFormMain = class(TForm)
    EditUrl: TEdit;
    EditToken: TEdit;
    EditSend: TEdit;
    LabelUrl: TLabel;
    LabelToken: TLabel;
    LabelStatus: TLabel;
    LabelStatusValue: TLabel;
    CheckBoxDebug: TCheckBox;
    LabelSend: TLabel;
    ButtonConnect: TButton;
    ButtonDisconnect: TButton;
    ButtonSend: TButton;
    MemoLog: TMemo;
  private
  FClient: TWebSocketClient;
  FCallbacks: TWsClientCallbacks;


type
  TLogKind = (lkUi, lkWs, lkData, lkError);

  procedure Log(const S: string; Kind: TLogKind);
  procedure SetStatus(const S: string; Color: TColor);

  published
  procedure ButtonConnectClick(Sender: TObject);
  procedure ButtonDisconnectClick(Sender: TObject);
  procedure ButtonSendClick(Sender: TObject);

  public
  constructor Create(AOwner: TComponent); override;
  destructor Destroy; override;


  end;

var
  FormMain: TFormMain;

implementation

{$R *.dfm}

procedure cb_connected(user: Pointer; connected: Integer); cdecl;
var
  Form: TFormMain;
begin
  Form := TFormMain(user);
  if connected <> 0 then
    Form.SetStatus('CONNECTED', clGreen)
  else
    Form.SetStatus('DISCONNECTED', clRed);

  Form.Log('[ws] connected=' + IntToStr(connected), lkWs);
end;

procedure cb_log(user: Pointer; message: PAnsiChar); cdecl;
var
  Form: TFormMain;
begin
  if message = nil then Exit;
  Form := TFormMain(user);
  Form.Log(UTF8ToString(message), lkWs);
end;

procedure cb_error(user: Pointer; code: Integer; message: PAnsiChar); cdecl;
var
  Form: TFormMain;
  Msg: string;
begin
  Form := TFormMain(user);
  Msg := '';
  if message <> nil then
    Msg := UTF8ToString(message);

  Form.Log(Format('[error] code=%d msg=%s', [code, Msg]), lkError);
end;

procedure cb_data(user: Pointer; data: PAnsiChar; size: Integer; is_text: Integer); cdecl;
var
  Form: TFormMain;
  Utf8: UTF8String;
begin
  if (data = nil) or (size <= 0) then Exit;
  Form := TFormMain(user);

  SetString(Utf8, data, size);
  if is_text <> 0 then
    Form.Log('[data:text] ' + UTF8ToString(Utf8), lkData)
  else
    Form.Log('[data:bin] ' + IntToStr(size) + ' bytes', lkData);
end;

procedure TFormMain.Log(const S: string; Kind: TLogKind);
begin
  if not CheckBoxDebug.Checked then
  begin
    if (Kind = lkUi) or (Kind = lkError) then
      Exit;
  end;

  MemoLog.Lines.Add(S);
end;

procedure TFormMain.SetStatus(const S: string; Color: TColor);
begin
  LabelStatusValue.Caption := S;
  LabelStatusValue.Font.Color := Color;
end;

constructor TFormMain.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);

  FClient := ws_client_create;

  FillChar(FCallbacks, SizeOf(FCallbacks), 0);

FCallbacks.on_connected := cb_connected;
FCallbacks.on_pong_timeout := nil;
FCallbacks.on_reconnect := nil;
FCallbacks.on_data := cb_data;
FCallbacks.on_error := cb_error;
FCallbacks.on_log := cb_log;

end;

destructor TFormMain.Destroy;
begin
  if FClient <> nil then
  begin
    ws_client_destroy(FClient);
    FClient := nil;
  end;

  inherited Destroy;
end;

procedure TFormMain.ButtonConnectClick(Sender: TObject);
var
  Url, Token: AnsiString;
begin
  if FClient = nil then Exit;

  Url := AnsiString(EditUrl.Text);
  Token := AnsiString(EditToken.Text);

  SetStatus('CONNECTING', clBlue);

  ws_client_set_ping(FClient, 5000, 12000);

  ws_client_connect(
    FClient,
    PAnsiChar(Url),
    PAnsiChar(Token),
    @FCallbacks,
    Self
  );

  Log('[ui] connect clicked', lkUi);
end;

procedure TFormMain.ButtonDisconnectClick(Sender: TObject);
begin
  if FClient = nil then Exit;

  ws_client_disconnect(FClient);
  Log('[ui] disconnect clicked', lkUi);
end;

  procedure TFormMain.ButtonSendClick(Sender: TObject);
var
  Msg: AnsiString;
begin
  if FClient = nil then Exit;

  Msg := AnsiString(EditSend.Text);
  ws_client_send_text(FClient, PAnsiChar(Msg));

  Log('[ui] send clicked', lkUi);
end;

end.
