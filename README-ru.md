# UniTrack
#### Трекер университетской активности


### О проекте
Проект находится в стадии активной разработки. Этот раздел будет обновлён, когда проект выйдет из стадии прототипа.


### If you don't speak Russian (Если вы не говорите по-русски)
Please, see [README-en.md](https://github.com/DmitryKuk/UniTrack/blob/master/README-en.md).


### Лицензия
Этот проект распространяется под моей собственной лицензией:

- Вы можете использовать проект в любых некоммерческих целях.
- Вы можете изменять исходный код проекта в некоммерческих целях, если захотите.
- Для использования в коммерческих целях, пожалуйста, свяжитесь со мной: <d1021976@gmail.com>.
- Вы не можете распространять изменённую версию проекта (в виде исходного кода, двоичных файлов или любом другом) без ссылки на оригинальную версию и её авторов. Это означает также, что вы не должны удалять из исходного кода комментарии с упоминанием авторов и лицензии.

---


### Требования:
- clang++-3.6 (кроме Mac OS).
- Boost (протестирована версия: 1.58).
- Crypto++.
- GNU Make (и немного shell) для сборки проекта.
- Scons для сборки [Mongo-cxx-driver (Legacy with C++11)](https://github.com/mongodb/mongo-cxx-driver/tree/legacy) (Mongo-cxx-driver включен в проект).
- MongoDB для работы.


### Сборка и установка
0. Установка внешних зависимостей, не включенных в проект:
    - Debian-подобные системы:
        `sudo apt-get install libboost-all-dev libcrypto++-dev mongodb-server scons clang-3.6 git`
    - Mac OS:
        `sudo port install boost libcryptopp mongodb scons`
1. Загрузка исходного кода:
    - `git clone --recursive https://github.com/DmitryKuk/UniTrack`
        + или `git clone https://github.com/DmitryKuk/UniTrack` (внешние зависимости будут загружены при сборке)
    - и `cd UniTrack/`
2. Сборка и установка:
    - просто `make happy` (использует `sudo` для установки; необходимо будет ввести пароль)
    - или:
        + `make third-party`
        + `sudo make install third-party`
        + `make`
        + `sudo make install`
    - *любой из способов автоматически установит конфигурацию по умолчанию и данные сайта*
    - (Только для Linux) Добавьте эту строку в свой ~/.bashrc (или ~/.bash_profile):
        `export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/opt/local/unitrack/lib"`
    - (Только для Mac OS) Возможно, у вас будут некоторые проблемы с libcryptopp.so. Просто добавьте эти строки в свой ~/.bash_profile:
        `export LDPATH="/usr/lib:/usr/local/lib:/opt/local/lib:$LDPATH"`
        `export DYLD_FALLBACK_LIBRARY_PATH="~/lib:/lib:$LDPATH"`
3. Запуск:
    - `unitrack` (по умолчанию, `/usr/bin/unitrack`)
4. Обновление:
    - `make upgrade` для автоматического удаления старой версии, загрузки обновлений, сборки и установки новой версии (использует `sudo`)
5. Удаление:
    - `sudo make uninstall` для удаления всех библиотек и исполняемых файлов из вашей системы
    - или `sudo make uninstall-all` для полного удаления (исполняемые файлы, конфигурация по умолчанию, данные сайта и внешние зависимости)
6. Ручная настройка (**не используйте, если не знаете, зачем это вам**):
    - **Используйте только если хотите восстановить повреждённый файл конфигурации или сайт!**
    - **НЕ требуется при любом способе установки, указанном выше!**
    - `make install-config` для установки конфигурации по умолчанию
    - `make install-www` для установки сайта


### Зависимости
- [Boost](http://www.boost.org/) *(протестирована версия: 1.58)*
- [Crypto++](http://www.cryptopp.com/)
- [JSON for Modern C++](https://github.com/nlohmann/json) *Niels Lohmann*
    + Автоматически управляемая зависимость (через git submodules).
- [Mongo-cxx-driver (Legacy with C++11)](https://github.com/mongodb/mongo-cxx-driver/tree/legacy)
    + Автоматически управляемая зависимость (через git submodules).


### Разработка
- Этот проект придерживается модульной структуры (см. `src/module-name`).
- Собирайте проект только из корневого каталога.
- Вы можете попробовать запустить программу без установки: `make run` в корневом каталоге проекта. Это может не работать на некоторых системах. Если это так, попробуйте `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path-to-project/build/lib`. Установленная версия запускается нормально.


### Авторы
- [Дмитрий Куковинец](https://github.com/DmitryKuk): <d1021976@gmail.com>
    + Автор серверной части проекта.
- [Максим Иванов](https://github.com/splincode): <https://vk.com/splincode>
    + Автор веб-части проекта.
- [Вера Продувнова](https://github.com/VeraProd)
    + Автор модулей для шаблонизатора
