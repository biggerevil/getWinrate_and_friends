require 'capybara'
require 'capybara/dsl'
require 'telegram/bot'

require_relative './credentials'

class Inbar_class
  include Capybara::DSL

  # Settings for capybara
  Capybara.run_server = false
  # Use usual browser
  Capybara.current_driver = :selenium
  # Use headless browser
  # Capybara.current_driver = :selenium_headless
  Capybara.app_host = 'https://inbar.pro'

  # Function to make stake
  def make_stake
    # Opening platform
    login_into_inbar
    choose_demo_trade
    open_platform

    # Dismiss modal views (Shown sometimes)
    dismiss_modal_view_limitations
    dismiss_modal_view_tech_pause

    # Choose sprint method
    # (because it allows set expire time in minutes)
    choose_sprint

    # Set stake settings
    set_time
    set_pare
    set_sum

    # Make stake
    click_on_call
  end


  private

  def login_into_inbar
    visit('/')
    puts 'Нажимаю на Войти, чтобы открылась форма логина-пароля'
    click_on 'Войти'
    within('div#log-in') do
      puts 'Ввожу логин и пароль'
      fill_in 'input-log-in1', with: ENV['my_login']
      fill_in 'input-log-in2', with: ENV['my_password']
      # find('button.submit').click
      # submit_form
      puts 'Нажимаю на Войти, чтобы войти'
      click_on 'Войти'
    end
  end

  def choose_demo_trade
    puts 'Нажимаю на Демо'
    find('label[for=personal-radio2]').click
  end

  def open_platform
    puts 'Нажимаю на Торговая платформа'
    find('a.welcome__btn').click
  end

  def dismiss_modal_view_limitations
    page.send_keys(:escape)
  end

  def dismiss_modal_view_tech_pause
    page.send_keys(:escape)
  end

  def choose_sprint
    puts 'Выбираю режим Спринт'
    # Within menu with 'Классик' and 'Спринт'
    within find('ul.header_menu') do
      # Choose 'Спринт' and click
      xpath_element = find(:xpath, "li[2]")
      xpath_element.click
    end
  end

  def set_time
    puts 'Выставляю время (классик изначально здесь)'
    fill_in 'timepicker', with: ''
    fill_in 'timepicker', with: '10:00'
  end

  def set_pare
    puts 'Нахожу и заполняю пару'
    dropdown_select = page.find(:xpath, '//*[@id="select_option"]/div[1]/input')
    dropdown_select.click
    send_keys('NZD/USD')
    send_keys(:enter)
  end

  def set_sum
    puts 'Выставляю сумму'
    fill_in 'investment', with: '523'
  end

  def click_on_call
    find("img[alt='call']").click
  end

  # TODO: запуск на сервере
  # Настроить запуск на сервере. Сначала просто программы.

  # TODO: связь Ruby и C++
  # Подумать, как связать эту программу на Ruby и мой код на C++

  # TODO: проверить производительность
  # Мб попробовать реализовать простенький вариант, проверить простенькую схему работы
  # Мб даже написать простейшую C++ прогармму, которая будет запускаться условно каждые 30 секунд,
  # генерировать случайное число от 1 до 10, и вызывать мой код на Ruby, если число, например, ≥ 6
  # И заодно делать замеры произволительности - то есть просто писать printf("Я C++, я родился в такое-то
  # время - **:**", а затем в Ruby-скрипте писать puts "Я Ruby, родился в такое-то время - **:**"" и
  # "Я Ruby, УМЕР в такое-то время - **:**") и также с C++
  # И ставки в intrade.bar делать на много часов, чтобы посмотреть, какая там задержка
  # И сделать логирование в файл. Мб это стоит сделать через непосредственно программы, но мб и стоит
  # сделать через systemctl, так как это мб будет быстрее

  # TODO:
  # Ещё для полной картины можно настроить особенности intrade.bar, так как он не торгует по ночам
  # и не в первые минуты часов и по вечерам там какая-то фигня со ставками, а мой бот по идее работает
  # ровно каждые 5 минут (хотя это, можно, кстати и подстроить, запускаться каждые, например, 5 + 3 минуты,
  # то есть НЕ в 17:00, а в 17:03, 17:08 и так далее. Так снова собирать статистику, но так он будет
  # отлично работать для intrade.bar, хотя вообще можно замутить двух ботов, один из которых будет
  # продолжать как обычно, а второй работать конкретно на intrade.bar)
  # Но это мб всё потом
end