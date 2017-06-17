# cpp_tcp-communication-sample

C++でTCP/IPの通信を行うサーバ実装とクライアント実装のサンプルプログラムです。macOSのXcode上で使用可能なBSDソケットベースで作成し、Visual Studio上でもコンパイルできるようにWinSockへの対応を加えています。


## 開発環境
以下の環境で動作を確認しています。
- macOS 10.12上のXcode 8
- Windows 7 (SP1)上のVisual Studio 2017

## クライアントについて
以下の2つのファイルを追加すれば使用できます。
- TCPClient.hpp
- TCPClient.cpp

```
TCPClient *client = new TCPClient("127.0.0.1", 12345);

// データの送信
client->Send("Hello!!\r\n");

// データの受信
std::string reply = client->ReceiveString();
...        
delete client;
```

※ Windows環境では、アプリケーションの起動時と終了時に、WinSockの初期化とクリーンアップのための関数呼び出しが必要になることに注意してください。また、[プロジェクト]-[<プロジェクト名>のプロパティ]から、[構成プロパティ]-[C/C++]-[詳細設定]の「必ず使用されるインクルード ファイル」に「stdafx.h」を指定する必要があります。

## サーバについて
以下の4つのファイルを追加すれば使用できます。
- TCPClient.hpp
- TCPClient.cpp
- TCPServer.hpp
- TCPServer.cpp

```
TCPServer *server = new TCPServer(12345);
        
bool isRunning = true;
while (isRunning) {
     TCPClient *client = server->Accept();
     /* clientで表されるクライアント用のスレッドをここで作成する。通信方法は上のクライアントと同様。 */
}

delete server;
```

※ Windows環境では、アプリケーションの起動時と終了時に、WinSockの初期化とクリーンアップのための関数呼び出しが必要になることに注意してください。また、[プロジェクト]-[<プロジェクト名>のプロパティ]から、[構成プロパティ]-[C/C++]-[詳細設定]の「必ず使用されるインクルード ファイル」に「stdafx.h」を指定する必要があります。
