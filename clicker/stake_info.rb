# frozen_string_literal: true

class Stake_info
  attr_reader :time_in_minutes, :pairname, :stake_sum, :will_go_higher

  def initialize(time_in_minutes:, pairname:, stake_sum:, will_go_higher:)
    @time_in_minutes = time_in_minutes
    @pairname = pairname
    @stake_sum = stake_sum
    @will_go_higher = will_go_higher
  end

  def print_version
    "
    Time in minutes: #{time_in_minutes}
    Pairname: #{pairname}
    Stake sum: #{stake_sum}
    Direction: #{direction}
    "
  end

  def change_pairname_to(_new_pairname)
    output_message = " Меняю название пары с #{pairname}"

    @pairname = _new_pairname

    output_message += " на #{pairname}"
    puts output_message
  end

  def invert_direction
    output_message = " Меняю направление с #{direction}"

    @will_go_higher = !@will_go_higher

    output_message += " на #{direction}"
    puts output_message
  end

  def direction
    @will_go_higher ? 'Call' : 'Put'
  end
end
