MQTT Publisher (Qt 5.11.3 + QML + libmosquitto) — QoS=0

Описание
--------
Небольшое Qt Quick (QML) приложение для отправки сообщения (publish) в MQTT брокер.
Поля ввода:
- Address (адрес/хост брокера)
- Port (порт)
- Username (логин, опционально)
- Password (пароль, опционально)
- Topic (топик публикации)
- Data (payload/данные сообщения)

QoS в этом варианте фиксирован: QoS = 0 (at most once).
Приложение при нажатии "Send" подключается к брокеру, публикует сообщение в выбранный топик и отключается.


Требования
----------
- Debian 10 (или совместимый Linux)
- Qt 5.11.3 (Kit: GCC 8.3.0, 64-bit)
- CMake >= 3.10
- Пакет libmosquitto-dev (MQTT client library)
- (рекомендуется) qtdeclarative5-dev, qtquickcontrols2-5-dev если Qt ставился из репозиториев


Установка зависимостей (Debian 10)
----------------------------------
1) Обновить список пакетов:
   sudo apt-get update

2) Установить mosquitto client library для разработки:
   sudo apt-get install -y libmosquitto-dev


Сборка (CMake, out-of-source)
-----------------------------
Из корня проекта:

1) Создать папку сборки:
   mkdir -p build
   cd build

2) Сгенерировать проект:
   cmake ..

   Если у вас несколько Qt/kit’ов и нужно указать Qt вручную, используйте CMAKE_PREFIX_PATH, например:
   cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/5.11.3/gcc_64 ..

3) Собрать:
   cmake --build . -j

4) Запустить бинарник:
   ./QTMQTTSampleApp


Сборка и запуск из Qt Creator
-----------------------------
1) Открыть Qt Creator
2) File -> Open File or Project... -> выбрать CMakeLists.txt проекта
3) Выбрать Kit: Qt 5.11.3 (GCC 8.3.0, 64 bit)
4) Configure Project (если спросит)
5) Build (Ctrl+B)
6) Run (Ctrl+R)

Если CMake не находит Qt, в настройках проекта укажите CMake параметр:
-DCMAKE_PREFIX_PATH=<путь_к_вашему_Qt_5.11.3>

Если CMake не находит mosquitto:
- убедитесь, что установлен libmosquitto-dev
- проверьте наличие /usr/include/mosquitto.h и libmosquitto.so в системе


Проверка доставки пакета MQTT брокеру (опционально, для отладки)
-----------------------------------
Для проверки работоспособности приложения можно воспользоваться бесплатным MQTT брокером,
предоставляемым разработчиками библиотеки mosquitto: test.mosquitto.org

Необходимо установить клиентские утилиты следующей командой:
sudo apt update
sudo apt install mosquitto-clients

И подписаться на тестовый топик (в примере "test/topic"):
mosquitto_sub -h test.mosquitto.org -t "test/topic"

Теперь при отправке сообщения через приложение в указанный топик вы должны увидеть
его успешное получение в окне терминала.


Инструкция по использованию приложения (QoS = 0)
------------------------------------------------
1) Address:
   Введите адрес брокера (например: localhost, 192.168.1.10, mqtt.example.com, test.mosquitto.org)

2) Port:
   Обычно 1883 для MQTT без TLS.

3) Username/Password:
   Если брокер требует авторизацию — заполните.
   Если не требует — оставьте пустыми.

4) Topic:
   Укажите топик, например: test/topic

5) Data:
   Укажите текст сообщения (можно JSON), например:
   {"cmd":"ping","ts":123}

6) Нажмите кнопку "Send":
   - приложение подключится к брокеру
   - отправит publish в указанный topic с QoS=0
   - выполнит короткую обработку сетевых событий (чтобы пакет ушёл)
   - отключится

7) Строка "Status" внизу покажет результат:
   - "Published (n bytes), mid=1" при успехе
   - либо сообщение об ошибке connect/publish


Ограничения и заметки
---------------------
- QoS фиксирован в коде как 0 (at most once). Подтверждения доставки от брокера не ожидается.
- Используется MQTT без TLS (mqtt://). Для mqtts:// (TLS, 8883) требуется добавить настройку TLS в libmosquitto.
- В текущей реализации отправка выполняется синхронно (в GUI-потоке). При проблемах сети возможны задержки интерфейса.


Структура проекта (пример)
--------------------------
- CMakeLists.txt
- main.cpp
- mosquittopublisher.h
- mosquittopublisher.cpp
- Main.qml
- qml.qrc
