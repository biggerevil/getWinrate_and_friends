import sqlite3

def print_stats():
    con = sqlite3.connect('/dbDir/results.db')
    cur = con.cursor()

    pairs = ['GBP/USD', "AUD/USD", "EUR/CHF", "EUR/USD", "EUR/JPY", "USD/JPY", "USD/CAD"]
    all_timeframes = ['5m', '15m', '30m', '1h', '2h', '5h', '7h', '10h', '1d', '2d']

    # timeframes_i_wanna_know = all_timeframes
    timeframes_i_wanna_know = ['2h', '5h', '7h', '10h', '1d', '2d']

    good_values = []

    def place_attention(check, up):
        if check:
            if up:
                print("\t\t\t\t↑ \t\t\t\t  ↑ \t\t\t\t\t\t\t\t↑")
            else:
                print("\t\t\t\t↓ \t\t\t\t  ↓ \t\t\t\t\t\t\t\t↓")

    def higher_winrate(first_array, second_array):
        # For the start there is winrate percentage in [1].
        # I wanna sort with winrate
        if first_array[1] > second_array[1]:
            return True
        else:
            return False

    # query_to_get_the_oldest_stake_date = "SELECT * FROM RESULTSSTAVKI ORDER BY CAST(datewhenset AS INTEGER;)"
    query_to_get_the_oldest_stake_date = "SELECT MIN(datewhenset) FROM RESULTSSTAVKI;"
    earliest_date = cur.execute(query_to_get_the_oldest_stake_date).fetchone()[0]
    print("Earliest date:", earliest_date)

    query_to_get_the_newest_stake_date = "SELECT MAX(datewhenset) FROM RESULTSSTAVKI;"
    latest_date = cur.execute(query_to_get_the_newest_stake_date).fetchone()[0]
    print("Latest date:", latest_date)

    for pair in pairs:
        print("\n")
        print("pair =", pair)

        for timeframe in timeframes_i_wanna_know:

            # Get how many in TOTAL there are stakes (i.e. 500)
            query_for_total = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pair}";'''
            total_value = cur.execute(query_for_total).fetchone()[0]
            # Continue if there are no stakes for this timeframe at all (i.e. 0)
            if total_value == 0:
                continue
            # Get how many SUCCESSFUL there are stakes (i.e. 300 from 500)
            query_for_successful = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pair}" AND SUCCESS="1";'''
            successfull_value = cur.execute(query_for_successful).fetchone()[0]

            print("query_for_total:")
            print(query_for_total)
            print("total_value:", total_value)
            print("query_for_successful:")
            print(query_for_successful)
            print("successful_value", successfull_value)
            percentage = successfull_value / total_value * 100
            nice_percentage = round(percentage, 3)

            attention_here = (nice_percentage < 43 or nice_percentage > 57)

            place_attention(attention_here, up=False)
            print(
                f"\ttimeftame = {timeframe}\t percentage = {nice_percentage}   \t total count of deals = {total_value}")
            place_attention(attention_here, up=True)

            if attention_here:
                good_value = [timeframe, nice_percentage, total_value, pair]
                good_values.append(good_value)

    print("\n\n\n")

    print("Sorting by win rate percentage...")
    for i in range(0, len(good_values)):
        for j in range(i + 1, len(good_values)):
            if higher_winrate(good_values[i], good_values[j]):
                temp = good_values[i];
                good_values[i] = good_values[j];
                good_values[j] = temp;
    print("Done sorting!\n\n\n")

    print("\t\t Good values: \n")

    text_for_return = ""
    for value in good_values:
        percentage = value[1]

        timeframe_and_percentage_text = f"\ttimeftame = {value[0]}\t\t percentage = {percentage} "
        print(timeframe_and_percentage_text, end='')
        text_for_return += timeframe_and_percentage_text

        if percentage < 50:
            added_percentage = round(100 - percentage, 3)
            added_percentage_text = f"({added_percentage})"
            text_for_return += added_percentage_text
            print(added_percentage_text, end='')

        total_count_of_deals_text = f"\t\t total count of deals = {value[2]}\t\t\tpair name = {value[3]}"
        text_for_return += total_count_of_deals_text

        text_for_return += "\n\n"

        print(total_count_of_deals_text)
        # print(f"\ttimeftame = {value[0]}\t\t percentage = {percentage}   \t\t total count of deals = {value[2]}\t\t\tpair name = {value[3]}")
        # print("timeftame:", value[0], "percentage: ", value[1], "total count of deals: ", value[2], "  \tpair name: ", value[3], sep="\t\t")

    print("\n\n\nDone!")
    return text_for_return
