# frozen_string_literal: true

require 'telegram/bot'

require_relative 'credentials'
require_relative 'stake_info'
require_relative 'variables'

def send_result_info_in_telegram(stake_info)
  Telegram::Bot::Client.run(ENV['telegram_token']) do |bot|
    bot.api.send_message(chat_id: ENV['chat_id'],
                         text: "I was required to make such stake:\n #{stake_info.print_version}\n\n  Current time: #{Time.now}")
    bot.api.send_photo(chat_id: ENV['chat_id'], photo: Faraday::UploadIO.new(SCREEN_PATH, 'image/jpeg'))
  end
end

def message_about_mistake_in_tg()
  Telegram::Bot::Client.run(ENV['telegram_token']) do |bot|
    bot.api.send_message(chat_id: ENV['chat_id'], text: "Mistake happened")
    # A divider to help differentiate messages
    bot.api.send_message(chat_id: ENV['chat_id'], text: "========================================")
  end
end

# Example code for listening and answering to /start and /stop
#
#
# Telegram::Bot::Client.run(token) do |bot|
#   bot.listen do |message|
#     case message.text
#     when '/start'
#       bot.api.send_message(chat_id: message.chat.id, text: "Hello, #{message.from.first_name} from #{message.chat.id}")
#     when '/stop'
#       bot.api.send_message(chat_id: message.chat.id, text: "Bye, #{message.from.first_name}")
#     end
#   end
# end
