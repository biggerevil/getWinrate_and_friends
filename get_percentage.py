# -*- coding: utf-8 -*-

import sqlite3
import operator

import credentials
import query_variants
import utility_code
import constants


def print_earliest_and_latest_dates(cur):
    print("Достаю самую раннюю ставку")
    # query_to_get_the_oldest_stake_date = "SELECT * FROM RESULTSSTAVKI ORDER BY CAST(datewhenset AS INTEGER;)"
    query_to_get_the_oldest_stake_date = "SELECT MIN(datewhenset) FROM RESULTSSTAVKI;"
    earliest_date = cur.execute(query_to_get_the_oldest_stake_date).fetchone()[0]
    print("Earliest date:", earliest_date)

    print("Достаю самую позднюю ставку")
    query_to_get_the_newest_stake_date = "SELECT MAX(datewhenset) FROM RESULTSSTAVKI;"
    latest_date = cur.execute(query_to_get_the_newest_stake_date).fetchone()[0]
    print("Latest date:", latest_date)


def print_pairs_without_certain(cur, pairs, timeframes_i_wanna_know,
                                round_time_offset=constants.Offset_if_not_using_round_time):
    good_values = []

    utility_code.print_warning_if_using_round_time(round_time_offset)

    # print("Начинаю итерацию по парам БЕЗ certain")
    for pair in pairs:
        # print("\n")
        # print("pair =", pair)

        for timeframe in timeframes_i_wanna_know:
            # print("\n\n New timeframe \n\n")
            # print(f"pair = {pair}, timeframe = {timeframe}")
            result_from_function = query_variants.value_good_for_pair_and_timeframe(cur, pair, timeframe,
                                                                                    round_time_offset)
            # print(f"result_from_function is = {result_from_function}")
            # if result_from_function is None:
            #     print("result_from_function is None")
            good_value_result = result_from_function

            if good_value_result.should_make_attention:
                good_value = good_value_result
                # print(f"Found good_value! Here it is {good_value}")
                good_values.append(good_value)

    # sorted_good_values = utility_code.sort_by_winrate(good_values)
    sorted_good_values = sorted(good_values, key=operator.attrgetter('percentage'))

    print(" print_pairs_without_certain:")

    utility_code.print_good_values(sorted_good_values, constants.Minimum_number_of_stakes)


def print_pairs_with_certain(cur, pairs, timeframes_i_wanna_know, certain_values_i_wanna_know,
                             round_time_offset=constants.Offset_if_not_using_round_time):
    good_values = []

    utility_code.print_warning_if_using_round_time(round_time_offset)

    # print("Начинаю итерацию по парам С certain")
    for pair in pairs:

        for timeframe in timeframes_i_wanna_know:

            for certain_value in certain_values_i_wanna_know:

                good_value_result = query_variants.value_good_for_pair_and_timeframe_and_certain(cur, pair, timeframe,
                                                                                                 certain_value,
                                                                                                 round_time_offset)
                if good_value_result.should_make_attention:
                    # print(f"Found good_value! Here it is {good_value}")
                    good_values.append(good_value_result)

    # sorted_good_values = utility_code.sort_by_winrate(good_values)
    sorted_good_values = sorted(good_values, key=operator.attrgetter('percentage'))

    print("\n print_pairs_with_certain:")

    utility_code.print_good_values(sorted_good_values, constants.Minimum_number_of_stakes)


def print_stats():
    print("Подключась к БД")
    con = sqlite3.connect(credentials.credentials_path_to_db)
    cur = con.cursor()
    print("Успешное подключение к БД")

    pairs = ['GBP/USD', "AUD/USD", "EUR/CHF", "EUR/USD", "EUR/JPY", "USD/JPY", "USD/CAD"]

    all_timeframes = ['5m', '15m', '30m', '1h', '2h', '5h', '7h', '10h', '1d', '2d']
    timeframes_i_wanna_know = all_timeframes
    # timeframes_i_wanna_know = ['30m', '1h', '2h', '5h', '7h', '10h', '1d', '2d']

    all_certain_values = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"]
    certain_values_i_wanna_know = all_certain_values

    print_earliest_and_latest_dates(cur)

    print_pairs_without_certain(cur, pairs, timeframes_i_wanna_know)
    print_pairs_without_certain(cur, pairs, timeframes_i_wanna_know, round_time_offset=0)
    print_pairs_without_certain(cur, pairs, timeframes_i_wanna_know, round_time_offset=1)
    print_pairs_without_certain(cur, pairs, timeframes_i_wanna_know, round_time_offset=2)
    print_pairs_without_certain(cur, pairs, timeframes_i_wanna_know, round_time_offset=3)
    print_pairs_without_certain(cur, pairs, timeframes_i_wanna_know, round_time_offset=4)

    print_pairs_with_certain(cur, pairs, timeframes_i_wanna_know, certain_values_i_wanna_know)
    print_pairs_with_certain(cur, pairs, timeframes_i_wanna_know, certain_values_i_wanna_know, round_time_offset=0)
    print_pairs_with_certain(cur, pairs, timeframes_i_wanna_know, certain_values_i_wanna_know, round_time_offset=1)
    print_pairs_with_certain(cur, pairs, timeframes_i_wanna_know, certain_values_i_wanna_know, round_time_offset=2)
    print_pairs_with_certain(cur, pairs, timeframes_i_wanna_know, certain_values_i_wanna_know, round_time_offset=3)
    print_pairs_with_certain(cur, pairs, timeframes_i_wanna_know, certain_values_i_wanna_know, round_time_offset=4)


    print("\n Done!")
