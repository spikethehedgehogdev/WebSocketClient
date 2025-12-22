# WebSocketClient

C++ WebSocket-клиент с демонстрационным desktop-приложением.

#### Реализовано

- Подключение с параметром адреса сокета и токеном (bearer) для авторизации (и отключение)
- Подключение/отключение веб-сокета с параметром Connected
- Heartbeat (ping / pong)
- Автоматический reconnect

#### Структура проекта

```
WebSocketClient.Core     — core библиотека
WebSocketClient.Desktop  — desktop demo приложение
```

---

#### Сборка

##### Требования

* Windows 10+
* Visual Studio 2022 (v145)
* C++20
* Boost (проверено на Boost 1.90)

Для сборки необходимо задать переменную окружения `BOOST_ROOT`,
указывающую на корень Boost.

Пример:

```
BOOST_ROOT = C:\libs\boost_1_90_0
```

После установки переменной требуется перезапустить Visual Studio.

##### Проверка WebSocket соединений

* **WSS**

Проверка защищённого WebSocket (`wss://`) выполнялась с использованием публичного echo-сервиса:

```
wss://ws.ifelse.io
````

Сервер корректно отвечает на ping/pong и возвращает отправленные сообщения.


* **WS**

Для проверки незащищённого WebSocket использовался локальный сервер.

Пример минимального сервера на Node.js:

```js
const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 9001 });

wss.on('connection', ws => {
  console.log('client connected');

  ws.on('message', msg => {
    ws.send(msg);
  });

  ws.on('ping', () => {
    console.log('ping received');
  });
});

console.log('ws server on ws://localhost:9001');
````

Запуск:

```bash
npm install ws
node server.js
```

Подключение из desktop-приложения:

```
ws://localhost:9001
```