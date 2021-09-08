import get_percentage
import utility_code


def value_good_for_pair_and_timeframe(cur, pairname, timeframe, round_time=False):
    # Get how many in TOTAL there are stakes (i.e. 500)
    query_for_total = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pairname}"'''
    query_for_total += ';'
    total_value = cur.execute(query_for_total).fetchone()[0]

    # Continue if there are no stakes for this timeframe at all (i.e. 0)
    # TODO: дублирование кода. Может быть не очень понятная ошибка, если изменю способ возврата с
    #   should_make_attention=False внизу (в конце функции) и не изменю здесь
    if total_value == 0:
        return utility_code.ReturnTuple(should_make_attention=False)

    # Get how many SUCCESSFUL there are stakes (i.e. 300 from 500)
    query_for_successful = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pairname}" AND SUCCESS="1";'''
    successfull_value = cur.execute(query_for_successful).fetchone()[0]

    # print("query_for_total:")
    # print(query_for_total)
    # print("total_value:", total_value)
    # print("query_for_successful:")
    # print(query_for_successful)
    # print("successful_value", successfull_value)
    percentage = successfull_value / total_value * 100
    nice_percentage = round(percentage, 3)

    attention_here = (nice_percentage < 43 or nice_percentage > 57)

    # utility_code.place_attention(attention_here, up=False)
    # print(
    #     f"\ttimeftame = {timeframe}\t percentage = {nice_percentage}   \t total count of deals = {total_value}")
    # utility_code.place_attention(attention_here, up=True)

    if attention_here:
        return utility_code.ReturnTuple(should_make_attention=True, percentage=nice_percentage, total_value=total_value, timeframe=timeframe, pairname=pairname, certain=None)
    else:
        return utility_code.ReturnTuple(should_make_attention=False)

def value_good_for_pair_and_timeframe_and_certain(cur, pairname, timeframe, certain, round_time=False):
    # Get how many in TOTAL there are stakes (i.e. 500)
    query_for_total = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pairname}" AND CERTAIN="{certain}"'''
    query_for_total += ';'
    total_value = cur.execute(query_for_total).fetchone()[0]

    # Continue if there are no stakes for this timeframe at all (i.e. 0)
    # TODO: дублирование кода. Может быть не очень понятная ошибка, если изменю способ возврата с
    #   should_make_attention=False внизу (в конце функции) и не изменю здесь
    if total_value == 0:
        return utility_code.ReturnTuple(should_make_attention=False)

    # Get how many SUCCESSFUL there are stakes (i.e. 300 from 500)
    query_for_successful = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pairname}" AND CERTAIN="{certain}" AND SUCCESS="1";'''
    successfull_value = cur.execute(query_for_successful).fetchone()[0]

    # print("query_for_total:")
    # print(query_for_total)
    # print("total_value:", total_value)
    # print("query_for_successful:")
    # print(query_for_successful)
    # print("successful_value", successfull_value)
    percentage = successfull_value / total_value * 100
    nice_percentage = round(percentage, 3)

    attention_here = (nice_percentage < 43 or nice_percentage > 57)

    # utility_code.place_attention(attention_here, up=False)
    # print(
    #     f"\ttimeftame = {timeframe}\t "
    #     f"percentage = {nice_percentage}   \t "
    #     f"total count of deals = {total_value}"
    #     f"certain = {certain}"
    # )
    # utility_code.place_attention(attention_here, up=True)

    if attention_here:
        return utility_code.ReturnTuple(should_make_attention=True, percentage=nice_percentage, total_value=total_value, timeframe=timeframe, pairname=pairname, certain=certain)
    else:
        return utility_code.ReturnTuple(should_make_attention=False)

# def good_values_for_pair_and_timeframe(cur, pairname, timeframe):
#     # Values (pairname-timeframe) with good winrate
#     good_values = []
#
#     # Get how many in TOTAL there are stakes (i.e. 500)
#     query_for_total = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pairname}";'''
#     total_value = cur.execute(query_for_total).fetchone()[0]
#     # Continue if there are no stakes for this timeframe at all (i.e. 0)
#     if total_value == 0:
#         return
#     # Get how many SUCCESSFUL there are stakes (i.e. 300 from 500)
#     query_for_successful = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pairname}" AND SUCCESS="1";'''
#     successfull_value = cur.execute(query_for_successful).fetchone()[0]
#
#     print("query_for_total:")
#     print(query_for_total)
#     print("total_value:", total_value)
#     print("query_for_successful:")
#     print(query_for_successful)
#     print("successful_value", successfull_value)
#     percentage = successfull_value / total_value * 100
#     nice_percentage = round(percentage, 3)
#
#     attention_here = (nice_percentage < 43 or nice_percentage > 57)
#
#     utility_code.place_attention(attention_here, up=False)
#     print(
#         f"\ttimeftame = {timeframe}\t percentage = {nice_percentage}   \t total count of deals = {total_value}")
#     utility_code.place_attention(attention_here, up=True)
#
#     if attention_here:
#         good_value = [timeframe, nice_percentage, total_value, pairname]
#         good_values.append(good_value)
#
#     print("Sorting by win rate percentage...")
#     for i in range(0, len(good_values)):
#         for j in range(i + 1, len(good_values)):
#             if utility_code.higher_winrate(good_values[i], good_values[j]):
#                 temp = good_values[i];
#                 good_values[i] = good_values[j];
#                 good_values[j] = temp;
#     print("Done sorting!\n\n\n")
#
#     print("Returning good_values:")
#     print(good_values)
#
#     return good_values
