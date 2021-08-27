# frozen_string_literal: true

require_relative 'credentials'
require_relative 'telegram.rb'
require_relative 'inbar_class.rb'

puts 'Начинаю'

stake_info = Stake_info.new(
  time_in_minutes: 10,
  pairname: 'NZD/USD',
  stake_sum: 100,
  will_go_higher: true

)
class_for_w = Inbar_class.new
class_for_w.make_stake(stake_info)
# Send screenshot in telegram
puts 'Пытаюсь отправить скриншот в телеграмме'
send_screen_in_telegram

puts 'Заканчиваю'
