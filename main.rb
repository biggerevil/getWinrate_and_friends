# frozen_string_literal: true

require 'slop'

require_relative 'credentials'
require_relative 'telegram'
require_relative 'inbar_class'
require_relative 'stake_info'
require_relative 'variables'

puts 'Начинаю'

opts = Slop.parse do |o|
  o.string '-p', '--pairname', 'Pairname'
  o.integer '-t', '--time_in_minutes', 'Expire time in minutes'
  o.integer '-s', '--stake_sum', 'Stake sum'
  o.bool '-w', '--will_go_higher', 'Stake will go higher, true or false'
end

puts "Аргументы командной строки:"
puts " opts[:pairname] = #{opts[:pairname]}"
puts " opts[:time_in_minutes] = #{opts[:time_in_minutes]}"
puts " opts[:stake_sum] = #{opts[:stake_sum]}"
puts " opts[:will_go_higher] = #{opts[:will_go_higher]}"

raise "Argument pairname equals nil!"        if opts[:pairname].nil?
raise "Argument time_in_minutes equals nil!" if opts[:time_in_minutes].nil?
raise "Argument stake_sum equals nil!"       if opts[:stake_sum].nil?
raise "Argument will_go_higher equals nil!"  if opts[:will_go_higher].nil?


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
  time_in_minutes: opts[:time_in_minutes],
  pairname: opts[:pairname],
  stake_sum: opts[:stake_sum],
  will_go_higher: opts[:will_go_higher]
)
class_for_w = Inbar_class.new

begin
  class_for_w.make_stake(stake_info)
rescue
  send_result_info_in_telegram(stake_info)
  message_about_mistake_in_tg
  raise "Mistake happened"
end

# Send message in telegram
puts 'Пытаюсь отправить сообщение в телеграмме'
send_result_info_in_telegram(stake_info)
# Delete screen with stake (screen used for seeing the result of work with site)
system("rm #{SCREEN_PATH}")

puts 'Заканчиваю'
