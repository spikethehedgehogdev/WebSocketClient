program WebSocketClient.DelphiDesktop;

uses
  Vcl.Forms,
  UnitFormMain in 'UnitFormMain.pas',
  WsClientApi in 'WsClientApi.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TFormMain, FormMain);
  Application.Run;
end.
