import get_percentage
import utility_code
import constants


def value_good_for_pair_and_timeframe(cur, pairname, timeframe, round_time_offset):
    # Get how many in TOTAL there are stakes (i.e. 500)
    query_for_total = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pairname}"'''

    # Take only round time, for example with offset = 0 take only 16:00, 16:05, 16:10 etc, and NOT take
    # 16:01, 16:02, ... 16:04, 16:06, 16:07 etc
    # With different offset it's just add offset to minutes, so with offset 1 it will be 16:01 instead of 16:00,
    # so there will be only stakes with 16:01 and not (16:00 or 16:02 or 16:03 etc)
    if round_time_offset != constants.Offset_if_not_using_round_time:
        query_for_total += offset_query(round_time_offset)

    query_for_total += ';'
    # print(f"query_for_total = {query_for_total}")
    total_value = cur.execute(query_for_total).fetchone()[0]

    # Continue if there are no stakes for this timeframe at all (i.e. 0)
    # TODO: дублирование кода. Может быть не очень понятная ошибка, если изменю способ возврата с
    #   should_make_attention=False внизу (в конце функции) и не изменю здесь
    if total_value == 0:
        return utility_code.ReturnTuple(should_make_attention=False)

    # Get how many SUCCESSFUL there are stakes (i.e. 300 from 500)
    query_for_successful = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pairname}" AND SUCCESS="1"'''

    if round_time_offset != constants.Offset_if_not_using_round_time:
        query_for_successful += offset_query(round_time_offset)

    query_for_successful += ';'
    # print(f"query_for_successful = {query_for_successful}")
    successfull_value = cur.execute(query_for_successful).fetchone()[0]

    # print("query_for_total:")
    # print(query_for_total)
    # print("total_value:", total_value)
    # print("query_for_successful:")
    # print(query_for_successful)
    # print("successful_value", successfull_value)
    percentage = successfull_value / total_value * 100
    nice_percentage = round(percentage, 3)

    # Мне подходят условия, у которых winrate больше заданного числа (например, > 60)
    # ИЛИ
    # меньше (100-заданный винрейт), так как тогда можно просто иметь ввиду этот винрейт и ставить в другом
    # направлении (например, если обычно мы хотим > 60, то нам также подходит 100-60=40, так как при 40% винрейте
    # мы будем просто ставить в другом направлении и получать всё те же 60%)
    attention_here = utility_code.check_if_needs_attention(nice_percentage)

    # utility_code.place_attention(attention_here, up=False)
    # print(
    #     f"\ttimeftame = {timeframe}\t percentage = {nice_percentage}   \t total count of deals = {total_value}")
    # utility_code.place_attention(attention_here, up=True)

    if attention_here:
        return utility_code.ReturnTuple(should_make_attention=True, percentage=nice_percentage, total_value=total_value, timeframe=timeframe, pairname=pairname, certain=None)
    else:
        return utility_code.ReturnTuple(should_make_attention=False)

def value_good_for_pair_and_timeframe_and_certain(cur, pairname, timeframe, certain, round_time_offset):
    # Get how many in TOTAL there are stakes (i.e. 500)
    query_for_total = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pairname}" AND CERTAIN="{certain}"'''

    if round_time_offset != constants.Offset_if_not_using_round_time:
        query_for_total += offset_query(round_time_offset)

    query_for_total += ';'
    total_value = cur.execute(query_for_total).fetchone()[0]

    # Continue if there are no stakes for this timeframe at all (i.e. 0)
    # TODO: дублирование кода. Может быть не очень понятная ошибка, если изменю способ возврата с
    #   should_make_attention=False внизу (в конце функции) и не изменю здесь
    if total_value == 0:
        return utility_code.ReturnTuple(should_make_attention=False)

    # Get how many SUCCESSFUL there are stakes (i.e. 300 from 500)
    query_for_successful = f'''SELECT COUNT(*) FROM RESULTSSTAVKI WHERE EXPIREPERIOD="{timeframe}" AND PAIRNAME="{pairname}" AND CERTAIN="{certain}" AND SUCCESS="1"'''

    if round_time_offset != constants.Offset_if_not_using_round_time:
        query_for_successful += offset_query(round_time_offset)

    query_for_successful += ';'
    # print(f"query_for_successful = {query_for_successful}")
    successfull_value = cur.execute(query_for_successful).fetchone()[0]

    # print("query_for_total:")
    # print(query_for_total)
    # print("total_value:", total_value)
    # print("query_for_successful:")
    # print(query_for_successful)
    # print("successful_value", successfull_value)
    percentage = successfull_value / total_value * 100
    nice_percentage = round(percentage, 3)

    attention_here = utility_code.check_if_needs_attention(nice_percentage)

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


def offset_query(round_time_offset):
    return f''' AND substr(DATEWHENSET, 16, 1) in('{0 + round_time_offset}', '{5 + round_time_offset}')'''
