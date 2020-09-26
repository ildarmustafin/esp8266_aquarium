[![release_svg][]][release_path] [![forum_svg][]][forum_path] [![stars_svg][]][stars_path] [![count_svg][]][count_path] [![issues_svg][]][issues_path]

Functions implemented in this project:

1. Indicates temperature on the display(LCD1602) and the web interface.
2. Displays chart of the temperature. Data is updated every 2 hours.
3. Turn on / off the heater by lowering-raising the temperature.
4. Turn on / off the fan to increase or decrease the temperature.
5. Turn on the LEDs on schedule. Dawn and sunset modes. MIN and MAX brightness are set in %.
6. Scheduled 2 relays
7. Firmware update via web interface
8. Editing an NTP server in the web interface
9. Switch tabs with a swipe
10. Connect to an existing access point. The ability to reconnect to the AP in case of loss and restoration of connection
11. FSBrowser - to access files of the file system. Access through 192.168.x.x / edit
12. Added 2 languages (rus, eng). To add new languages, you must specify the abbreviated name of the language in the lang clause in the configSetup.json file. Next, use a file with the language rus.json or eng.json as a template, register your text and save it as the language name specified in lang
13. The process of creating MQTT is ongoing.

At the first connection, an ESP8266_AQUA access point is created without a password with the IP address 192.168.4.1. In the future, it is possible to change the name of the access point and set a password for it. When saving data to the server, the page does not reload. Data is loaded in json format with a GET request. The current state of the relay, date and temperature are transmitted via Websocket with a frequency of 0,5 second. Ability to select a port for access to a web page. The websocket port is 81.

The libraries used in the sketch are in the libraries.zip archive.


*****************************************************************************************************************************************************************************
Функции, реализованные в данном проекте:

1. Вывод показаний температуры на дисплей(LCD1602) и веб интерфейс.
2. Выводит график показаний температуры. Данные обновляются каждые 2 часа.
3. Включение-выключение нагревателя по понижению-повышению температуры.
4. Включение-выключение вентилятора по повышению-понижению температуры.
5. Включение светодиодов по расписанию. Режимы Рассвет и Закат. Мин и макс яркость выставляется в %.
6. Работа 2 реле по расписанию
7. Обновление прошивки через веб интерфейс
8. Редактирование NTP-сервера в веб-интерфейсе
9. Переключение вкладок свайпом
10. Подключение к существующей точке доступа. Возможность переподключения к ТД при потере и восстановлении связи
11. FSBrowser - для доступа к файлам файловой системы. Доступ через 192.168.x.x/edit
12. Добавлены 2 языка(rus,eng). Для добавления новых языков необходимо прописать сокращенное название языка в пункте lang в файле configSetup.json. Далее использовать как шаблон файл с языком rus.json или eng.json, прописать свой текст и сохранить как название языка, указанное в lang
13. Идет процесс создания MQTT.

При первом подключении создается точка доступа ESP8266_AQUA без пароля с IP адресом 192.168.4.1. В дальнейшем есть возможность изменить имя точки доступа и задать ей пароль. При сохранении данных на сервер страница не перезагружается. Данные загружаются в json формате GET запросом. Текущее состояние реле, дата и температура передаются посредством Websocket с частотой в 0,5 сек. Возможность выбора порта для доступа на веб страницу. Порт websocket - 81.

Библиотеки, используемые в скетче, находятся в архиве libraries.zip.

![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/master/photo/mini_1.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/master/photo/mini_2.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/master/photo/mini_3.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/master/photo/mini_4.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/master/photo/mini_5.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/master/photo/mini_6.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/master/photo/mini_7.JPG)
![screenshot](https://github.com/ildarmustafin/esp8266_aquarium/blob/master/photo/mini_8.JPG)

[release_svg]:   https://img.shields.io/badge/release-4.3a-blue.svg
[release_path]:  https://github.com/ildarmustafin/esp8266_aquarium/releases/latest
[forum_svg]:     https://img.shields.io/badge/forum-on_esp8266.ru-red.svg
[forum_path]:    https://esp8266.ru/forum/threads/akvarium-na-esp8266.4637
[stars_svg]:     https://img.shields.io/github/stars/ildarmustafin/esp8266_aquarium.svg
[stars_path]:    https://github.com/ildarmustafin/esp8266_aquarium/stargazers
[count_svg]:     http://hits.dwyl.io/ildarmustafin/esp8266_aquarium/badges.svg
[count_path]:    http://hits.dwyl.io/ildarmustafin/esp8266_aquarium/badges
[issues_svg]:    https://img.shields.io/github/issues/ildarmustafin/esp8266_aquarium.svg
[issues_path]:   https://github.com/ildarmustafin/esp8266_aquarium/issues/
