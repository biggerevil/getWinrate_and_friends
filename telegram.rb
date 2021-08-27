require 'telegram/bot'

require_relative 'credentials'

def send_screen_in_telegram
  Telegram::Bot::Client.run(ENV['telegram_token']) do |bot|
    bot.api.send_message(chat_id: ENV['chat_id'], text: "Get this!")
    bot.api.send_photo(chat_id: ENV['chat_id'], photo: Faraday::UploadIO.new('./screen.png', 'image/jpeg'))
  end
end

# Example code for listening and answering to /start and /stop
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
