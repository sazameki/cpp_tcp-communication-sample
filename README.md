# cpp_tcp-communication-sample

C++でTCP/IPの通信を行うサーバ実装とクライアント実装のサンプルプログラムです。


## 開発環境
以下の環境で動作を確認しています。
- macOS 10.12上のXcode 8
- Windows 7 (SP1)上のVisual Studio 2017

## クライアントについて
以下の2つのファイルを追加すれば使用できます。
- TCPClient.hpp
- TCPClient.cpp

※ Windows環境では、アプリケーションの起動時と終了時に、WinSockの初期化とクリーンアップのための関数呼び出しが必要になることに注意してください。

## サーバについて
以下の4つのファイルを追加すれば使用できます。
- TCPClient.hpp
- TCPClient.cpp
- TCPServer.hpp
- TCPServer.cpp

※ Windows環境では、アプリケーションの起動時と終了時に、WinSockの初期化とクリーンアップのための関数呼び出しが必要になることに注意してください。
