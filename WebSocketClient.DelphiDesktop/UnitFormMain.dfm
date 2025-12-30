object FormMain: TFormMain
  Left = 0
  Top = 0
  Caption = 'WebSocketClient (Delphi)'
  ClientHeight = 441
  ClientWidth = 624
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  TextHeight = 15
  object LabelUrl: TLabel
    Left = 8
    Top = 8
    Width = 24
    Height = 15
    Caption = 'URL:'
  end
  object LabelToken: TLabel
    Left = 8
    Top = 54
    Width = 35
    Height = 15
    Caption = 'Token:'
  end
  object LabelStatus: TLabel
    Left = 8
    Top = 104
    Width = 35
    Height = 15
    Caption = 'Status:'
  end
  object LabelStatusValue: TLabel
    Left = 58
    Top = 104
    Width = 87
    Height = 15
    AutoSize = False
    Caption = 'DISCONNECTED'
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -12
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentColor = False
    ParentFont = False
  end
  object LabelSend: TLabel
    Left = 8
    Top = 125
    Width = 29
    Height = 15
    Caption = 'Send:'
  end
  object EditUrl: TEdit
    Left = 8
    Top = 29
    Width = 497
    Height = 23
    TabOrder = 0
    Text = 'wss://ws.ifelse.io'
  end
  object EditToken: TEdit
    Left = 8
    Top = 75
    Width = 497
    Height = 23
    TabOrder = 1
    Text = 'test_token'
  end
  object EditSend: TEdit
    Left = 8
    Top = 146
    Width = 497
    Height = 23
    TabOrder = 2
    Text = 'Hello!'
  end
  object CheckBoxDebug: TCheckBox
    Left = 519
    Top = 105
    Width = 97
    Height = 17
    Caption = 'Debug Log'
    TabOrder = 3
  end
  object ButtonConnect: TButton
    Left = 520
    Top = 29
    Width = 96
    Height = 24
    Caption = 'Connect'
    TabOrder = 4
    OnClick = ButtonConnectClick
  end
  object ButtonDisconnect: TButton
    Left = 520
    Top = 74
    Width = 96
    Height = 25
    Caption = 'Disconnect'
    TabOrder = 5
    OnClick = ButtonDisconnectClick
  end
  object ButtonSend: TButton
    Left = 520
    Top = 145
    Width = 96
    Height = 25
    Caption = 'Send'
    TabOrder = 6
    OnClick = ButtonSendClick
  end
  object MemoLog: TMemo
    Left = 8
    Top = 183
    Width = 608
    Height = 250
    TabOrder = 7
  end
end
