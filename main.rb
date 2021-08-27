# frozen_string_literal: true

require_relative 'credentials'
require_relative 'telegram.rb'
require_relative 'inbar_class.rb'

puts 'Начинаю'

class_for_w = Inbar_class.new
class_for_w.make_stake
# Send screenshot in telegram
puts 'Пытаюсь отправить скриншот в телеграмме'
send_screen_in_telegram

puts 'Заканчиваю'
