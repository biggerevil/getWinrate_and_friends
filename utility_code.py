import collections
import constants


class ReturnTuple:
    should_make_attention = None
    percentage = None
    total_value = None
    timeframe = None
    pairname = None
    certain = -1

    def __init__(self, should_make_attention, percentage=None, total_value=None, timeframe=None, pairname=None,
                 certain=None):
        self.should_make_attention = should_make_attention
        self.percentage = percentage
        self.total_value = total_value
        self.timeframe = timeframe
        self.pairname = pairname
        self.certain = certain

    def __repr__(self):
        text_for_return = ""

        # TODO: На данный момент просто быстрое решение бага, стоит разобраться, почему так происходит
        if self.percentage is None:
            return

        timeframe_and_percentage_text = f"\ttimeftame = {self.timeframe}\t percentage = {self.percentage} "
        # print(timeframe_and_percentage_text, end='')
        text_for_return += timeframe_and_percentage_text

        if self.percentage < 50:
            added_percentage = round(100 - self.percentage, 3)
            added_percentage_text = f"({added_percentage})"
            text_for_return += added_percentage_text
            # print(added_percentage_text, end='')

        total_count_of_deals_text = f"\t total count of deals = {self.total_value}\tpair name = {self.pairname}"
        text_for_return += total_count_of_deals_text

        if self.certain != None:
            text_for_return += f"\t certain = {self.certain}"

        return text_for_return


def sort_by_winrate(good_values):
    for i in range(0, len(good_values)):
        for j in range(i + 1, len(good_values)):
            if higher_winrate(good_values[i], good_values[j]):
                temp = good_values[i];
                good_values[i] = good_values[j];
                good_values[j] = temp;
    print("Done sorting with custom sort function!\n\n\n")
    return good_values


def print_good_values(good_values, minimum_stakes=-1):
    # Если передано минимальное кол-во ставок для вывода, то будут выведены условия только такие,
    # что там больше или столько же ставок
    # Если не передано, то выводятся все условия вне зависимости от кол-ва ставок
    if minimum_stakes != -1:
        print_good_values_with_at_least_N_stakes(good_values, minimum_stakes)
    else:
        print_all_good_values(good_values)
        # if int(value.total_value) > 50:
        #     print(value)
        # print(f"\ttimeftame = {value[0]}\t\t percentage = {percentage}   \t\t total count of deals = {value[2]}\t\t\tpair name = {value[3]}")
        # print("timeftame:", value[0], "percentage: ", value[1], "total count of deals: ", value[2], "  \tpair name: ", value[3], sep="\t\t")


def print_all_good_values(good_values):
    for value in good_values:
        print(value)


def print_good_values_with_at_least_N_stakes(good_values, N):
    for value in good_values:
        if int(value.total_value) >= N:
            print(value)


def place_attention(check, up):
    if check:
        if up:
            print("\t\t\t\t↑ \t\t\t\t  ↑ \t\t\t\t\t\t\t\t↑")
        else:
            print("\t\t\t\t↓ \t\t\t\t  ↓ \t\t\t\t\t\t\t\t↓")


def higher_winrate(first_good_value, second_good_value):
    # For the start there is winrate percentage in [1].
    # I wanna sort with winrate

    # TODO: На данный момент просто быстрое решение бага, стоит разобраться, почему так происходит
    # if first_good_value.percentage is None or second_good_value.percentage is None:
    #     return False

    if first_good_value.percentage > second_good_value.percentage:
        return True
    else:
        return False


# Этот метод создан, чтобы избежать дублирования кода
def print_warning_if_using_round_time(round_time_offset):
    if round_time_offset != constants.Offset_if_not_using_round_time:
        print(f"\n\n    Using ROUND time offset = {round_time_offset}\n")


# Этот метод создан, чтобы избежать дублирования кода
def check_if_needs_attention(nice_percentage):
    return nice_percentage > constants.Minimum_winrate or nice_percentage < (100 - constants.Minimum_winrate)
