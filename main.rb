# frozen_string_literal: true

require_relative 'credentials'
require_relative 'telegram.rb'
require_relative 'inbar_class.rb'
require_relative 'stake_info'


puts 'Начинаю'

# TODO: Изменить поиск и выбор пары. Через ввод с клавиатуры работает
# некорректно - похоже, реагирует только на первую букву.
#
#   В качестве решения можно:
#
#   Попробовать находить требуемую пару в выпадающем списке, учитывая, что есть только, например,
# USD/JPY, а JPY/USD нет - можно проверять на набор тех же букв, что в требуемой ставке, и затем
# проверять, требуется ли инверсирование (ставить в другую сторону) для этой ставки.
#
stake_info = Stake_info.new(
  time_in_minutes: 10,
  pairname: 'GBP/NZD',
  stake_sum: 100,
  will_go_higher: true

)
class_for_w = Inbar_class.new
class_for_w.make_stake(stake_info)
# Send message in telegram
puts 'Пытаюсь отправить сообщение в телеграмме'
send_result_info_in_telegram(stake_info)

puts 'Заканчиваю'
