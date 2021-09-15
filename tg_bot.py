# -*- coding: utf-8 -*-

import logging
import ssl

from aiohttp import web

import telebot

from credentials import *
from get_percentage import *


def start_bot():
    token = credentials_token
    API_TOKEN = token

    WEBHOOK_HOST = credentials_ip_of_server
    WEBHOOK_PORT = 8443  # 443, 80, 88 or 8443 (port need to be 'open')
    WEBHOOK_LISTEN = '0.0.0.0'  # In some VPS you may need to put here the IP addr

    WEBHOOK_SSL_CERT = './webhook_cert.pem'  # Path to the ssl certificate
    WEBHOOK_SSL_PRIV = './webhook_pkey.pem'  # Path to the ssl private key

    # Quick'n'dirty SSL certificate generation:
    #
    # openssl genrsa -out webhook_pkey.pem 2048
    # openssl req -new -x509 -days 3650 -key webhook_pkey.pem -out webhook_cert.pem
    #
    # When asked for "Common Name (e.g. server FQDN or YOUR name)" you should reply
    # with the same value in you put in WEBHOOK_HOST

    WEBHOOK_URL_BASE = "https://{}:{}".format(WEBHOOK_HOST, WEBHOOK_PORT)
    WEBHOOK_URL_PATH = "/{}/".format(API_TOKEN)

    logger = telebot.logger
    telebot.logger.setLevel(logging.INFO)

    bot = telebot.TeleBot(API_TOKEN, parse_mode='Markdown')

    app = web.Application()

    # Process webhook calls
    async def handle(request):
        if request.match_info.get('token') == bot.token:
            request_body_dict = await request.json()
            update = telebot.types.Update.de_json(request_body_dict)
            bot.process_new_updates([update])
            return web.Response()
        else:
            return web.Response(status=403)

    app.router.add_post('/{token}/', handle)

    # Handle '/start' and '/help'
    @bot.message_handler(commands=['help', 'start'])
    def send_welcome(message):
        bot.reply_to(message,
                     ("Hi there, I am Stats bot.\n"
                      "I am here to help you understand current circumstances."))

    # Handle all other messages
    @bot.message_handler(func=lambda message: True, content_types=['text'])
    def echo_message(message):
        print("Получил сообщение!")
        if message.text == 'stats':
            print("Это stats!")
            bot.reply_to(message, 'Запрос получил, начинаю работу')
            print("Начинаю обработку запроса")
            answer_text = print_stats()
            print(f"answer_text = {answer_text}")
            print("Закончил обработку запроса")
            bot.reply_to(message, answer_text)

    # Remove webhook, it fails sometimes the set if there is a previous webhook
    bot.remove_webhook()

    # Set webhook
    bot.set_webhook(url=WEBHOOK_URL_BASE + WEBHOOK_URL_PATH,
                    certificate=open(WEBHOOK_SSL_CERT, 'r'))

    # Build ssl context
    context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    context.load_cert_chain(WEBHOOK_SSL_CERT, WEBHOOK_SSL_PRIV)

    # Start aiohttp server
    web.run_app(
        app,
        host=WEBHOOK_LISTEN,
        port=WEBHOOK_PORT,
        ssl_context=context,
    )
