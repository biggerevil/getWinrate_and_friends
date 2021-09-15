# getWinrate

Собирает статистику, записывает в базы данных текущие сигналы и их результаты.

## systemctl-файлы для работы:

### /etc/systemd/system/getWinrate.service:

	[Unit]
	Description = "Bot"
	After = network.target
	
	[Service]
	ExecStart = /root/getWinrate/cmake-build-debug-remote/getWinrate
    # Под root'ом запускать, конечно, не круто, но на данный момент
    # на сервере ничего, в общем-то, нет
    User=root
	WorkingDirectory = /root/getWinrate/cmake-build-debug-remote
    TimeoutSec=40s
	
	[Install]
	WantedBy = multi-user.target

### /etc/systemd/system/getWinrate.timer:

Была проблема с таймером, но она была из-за того, что .service не прекращал работу.
Вероятно, из-за проблем с current.db, которые возникали предположительно из-за моих "дебаг-запусков" на реальной БД, 
так как я пока не разделил на "production" и "debug" версии.

	[Install]
	WantedBy=default.target
	
	[Unit]
	Description=Run getW every minute
	
	[Timer]
	# Start minutely on all days except weekend
	OnCalendar=Mon..Fri *-*-* *:*:00
	AccuracySec=1us

### Команды для systemctl:
_Если я всё правильно помню:_

#### Запуск
	systemctl enable getWinrate.service
	systemctl enable getWinrate.timer
	
	# Проверяем, что работает. Необязательные команды. Возможно, чуть-чуть портят БД, так как getWinrate запускается не в ровное время
	systemctl start getWinrate.timer
	systemctl status getWinrate.timer
	systemctl status getWinrate.service

#### Остановка
	systemctl stop getWinrate.timer
	systemctl disable getWinrate.timer
	systemctl disable getWinrate.service
