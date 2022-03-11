[![platform_svg][]][platform_path] [![stars_svg][]][stars_path] [![fw_ver_svg][]][fw_ver_path] [![fs_ver_svg][]][fs_ver_path] [![forum_svg][]][forum_path] [![issues_svg][]][issues_path]

Functions implemented in this project:

1. Indicates temperature on the display(LCD1602) and the web interface.
2. Displays chart of the temperature. Data is updated every 2 hours.
3. Turn on / off the heater by lowering-raising the temperature.
4. Turn on / off the fan to increase or decrease the temperature.
5. Turn on the LEDs on schedule. Dawn and sunset modes. MIN and MAX brightness are set in %.
6. Scheduled 2 relays
7. Inverse output states
8. Firmware update via web interface. Output of real % file upload to server during firmware update
9. Switch tabs with a swipe
10. Connect to an existing access point. The ability to reconnect to the AP in case of loss and restoration of connection
11. Added 2 languages (rus, eng). To add new languages, you must specify the abbreviated name of the language in the lang clause in the configSetup.json file. Next, use a file with the language rus.json or eng.json as a template, register your text and save it as the language name specified in lang
12. Send JSON by MQTT. Data is not sending on empty options.
13. Send JSON by WEBSOCKET. Connect to your_ip/ws.
14. You can set static IP on options. IP dynamic, if options is empty.
15. The transfer of the pins states to the browser occurs through SSE (Server Sent Events). Data is transferred on state changes
16. Temperature control is carried out according to the algorithm https://alexgyver.ru/gyverrelay/ using hysteresis and speed feedback coefficient
17. Backlight lcd by pressing a button. The duration is selected in the settings. At 0 - constant backlight

At the first connection, an ESP8266_AQUA access point is created without a password with the IP address 192.168.4.1. In the future, it is possible to change the name of the access point and set a password for it. When saving data to the server, the page does not reload. Data is loaded in json format with a GET request. Ability to select a port for access to a web page.

*****************************************************************************************************************************************************************************
Функции, реализованные в данном проекте:

1. Вывод показаний температуры на дисплей(LCD1602) и веб интерфейс.
2. Выводит график показаний температуры. Данные обновляются каждые 2 часа.
3. Включение-выключение нагревателя по понижению-повышению температуры.
4. Включение-выключение вентилятора по повышению-понижению температуры.
5. Включение светодиодов по расписанию. Режимы Рассвет и Закат. Мин и макс яркость выставляется в %.
6. Работа 2 реле по расписанию
7. Инверсия состояния выводов
8. Обновление прошивки через веб интерфейс. Вывод % реальной загрузки файла на сервер при обновлении прошивки 
9. Переключение вкладок свайпом
10. Подключение к существующей точке доступа. Возможность переподключения к ТД при потере и восстановлении связи
11. Добавлены 2 языка(rus,eng). Для добавления новых языков необходимо прописать сокращенное название языка в пункте lang в файле configSetup.json. Далее использовать как шаблон файл с языком rus.json или eng.json, прописать свой текст и сохранить как название языка, указанное в lang
12. Передача JSON по MQTT. При пустых строках в настройке, передача данных не осуществляется.
13. Передача JSON по WEBSOCKET. Подключается к ваш_ip/ws.
14. Возможно установить статический IP. IP динамический, при пустых строках в настройках.
15. Передача состояния выводов в браузер происходит через SSE (Server Sent Events). Данные передаются при изменении состояния
16. Регулировка температуры происходит по алгоритму https://alexgyver.ru/gyverrelay/ с использованием гистерезиса и коэффициента обратной связи по скорости
17. Подсветка lcd по нажатию на кнопку. Длительность выбирается в настройках. При 0 - постоянная подсветка

При первом подключении создается точка доступа ESP8266_AQUA без пароля с IP адресом 192.168.4.1. В дальнейшем есть возможность изменить имя точки доступа и задать ей пароль. При сохранении данных на сервер страница не перезагружается. Данные загружаются в json формате GET запросом. Возможность выбора порта для доступа на веб страницу.
 
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/mini_1.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/mini_2.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/mini_3.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/mini_4.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/mini_5.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/mini_6.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/mini_7.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/mini_8.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/mini_9.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/mini_10.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/mini_11.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/main/photo/options_1Mb.JPG)

[platform_svg]:  https://img.shields.io/badge/platform-PlatformIO_IDE-red.svg
[platform_path]: https://platformio.org/platformio-ide
[fw_ver_svg]:    https://img.shields.io/badge/firmware_version-6.4-blue.svg
[fw_ver_path]:   https://github.com/ildarmustafin/esp8266_aquarium/releases/latest
[fs_ver_svg]:    https://img.shields.io/badge/filesystem_version-2.9-blue.svg
[fs_ver_path]:   https://github.com/ildarmustafin/esp8266_aquarium/releases/latest
[forum_svg]:     https://img.shields.io/badge/forum-on_esp8266.ru-Lime.svg
[forum_path]:    https://esp8266.ru/forum/threads/akvarium-na-esp8266.4637
[stars_svg]:     https://img.shields.io/github/stars/ildarmustafin/esp8266_aquarium.svg
[stars_path]:    https://github.com/ildarmustafin/esp8266_aquarium/stargazers
[issues_svg]:    https://img.shields.io/github/issues/ildarmustafin/esp8266_aquarium.svg
[issues_path]:   https://github.com/ildarmustafin/esp8266_aquarium/issues/
