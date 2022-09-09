#pragma once
#include <string>
#include <exception>
namespace caculator {
	class caculate_error :public std::exception {
	public:
		caculate_error(std::string const& what) :what_(what) {}
	public:
		char const* what() const noexcept{
			return what_.data();
		}
	private:
		std::string what_;
	};
	struct str_view {
		char const* begin_;
		char const* end_;
	};
	static std::string view_to_str(str_view view) {
		return std::string(view.begin_, view.end_ - view.begin_);
	}
	static bool is_operator(char c)
	{
		if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^' || c == '!')
		{
			return true;
		}
		return false;
	}
	static void skip_parenthesis(char const*& begin, char const* end)
	{
		int left_parenthesis_number = 1;
		begin++;
		while (begin != end)
		{
			if (*begin == '(')
			{
				left_parenthesis_number++;
			}
			if (*begin == ')')
			{
				left_parenthesis_number--;
			}
			if (left_parenthesis_number == 0)
			{
				begin++;
				break;
			}
			begin++;
		}
	}
	static str_view cutout_match_parenthesized_expression(char const* begin, char const* end, char const*& newbegin) {
		auto start = begin;
		begin++;
		skip_parenthesis(start, end);
		newbegin = start;
		//return std::string(begin, (start - begin)-1);
		return str_view{ begin,start - 1 };
	}

	static str_view cutout_subexpression_for_add_or_substract(char const* begin, char const* end, char const*& newbegin)
	{
		auto start = begin;
		while (*start != '+' && *start != '-' && start != end)
		{
			if (*start == '(')
			{
				skip_parenthesis(start, end);
				continue;
			}
			start++;
		}
		/*auto r = std::string(begin, start - begin);*/
		auto r = str_view{ begin, start };
		newbegin = start;
		return r;
	}

	static double get_number(char const* begin, char const* end, char const*& newbegin)
	{
		auto start = begin;
		while (!is_operator(*start) && start != end)
		{
			start++;
		}
		std::string v(begin, start - begin);
		newbegin = start;
		return atof(v.data());
	}
	static double evalute_expression(char const* begin, char const* end)
	{
		auto start = begin;
		if (*start != '(')
		{
			auto lnumber = get_number(start, end, start);
			if (start == end)
			{
				return lnumber;
			}
			char operatorc = *start;
			start++;
			if (operatorc == '*' || operatorc == '/')
			{ // has the highest priority
				if (*start != '(')
				{
					auto rnumber = get_number(start, end, start);
					//auto nextoperatorc = *start;
					//start++;
					double current_result = 0;
					switch (operatorc)
					{
					case '*':
						current_result = lnumber * rnumber;
						break;
					case '/':
						current_result = lnumber / rnumber;
						break;
					}
					auto nstr = std::to_string(current_result) + std::string(start, end - start);
					return evalute_expression(nstr.data(), nstr.data() + nstr.size());
				}
				else
				{ // right side of the operator begins with a parenthesis   1*(2+3)
					auto e = cutout_match_parenthesized_expression(start, end, start);
#if caculate_debug
					auto str = view_to_str(e);
#endif
					auto rnumber = evalute_expression(e.begin_, e.end_);
					double current_result;
					switch (operatorc)
					{
					case '*':
						current_result = lnumber * rnumber;
						break;
					case '/':
						current_result = lnumber / rnumber;
						break;
					}
					auto nstr = std::to_string(current_result) + std::string(start, end - start);
					return evalute_expression(nstr.data(), nstr.data() + nstr.size());
				}
			}
			else
			{	// has the low level priority, such as + or  -
				// 1+ 2*3 + 5;  1+(2-1)*5+6;
				auto sub = cutout_subexpression_for_add_or_substract(start, end, start);
#if caculate_debug
				auto str = view_to_str(sub);
#endif
				double temp_result = 0;
				switch (operatorc)
				{
				case '+':
					temp_result = lnumber + evalute_expression(sub.begin_, sub.end_);
					break;
				case '-':
					temp_result = lnumber - evalute_expression(sub.begin_, sub.end_);
					break;
				default:
					auto so = std::string(&operatorc, 1);
					throw caculate_error{ "no supported mathematical operator: " + so };
					break;
				}
				if (start == end) {
					return temp_result;
				}
				auto nextoperatorc = *start;
				start++;
				switch (nextoperatorc)
				{
				case '+':
					return temp_result + evalute_expression(start, end);
					break;
				case '-': {
					auto nstr2 = std::to_string(temp_result) + "-" + std::string(start, end - start);
					return evalute_expression(nstr2.data(), nstr2.data() + nstr2.size());
				}
						break;
				case '*':
					//return temp_result * evalute_expression(start, end);
					throw caculate_error{ "invalid parsed expression" };
					break;
				case '/':
					/*return temp_result / evalute_expression(start, end);*/
					throw caculate_error{ "invalid parsed expression" };
					break;
				default:
					auto so = std::string(&nextoperatorc, 1);
					throw caculate_error{ "no supported mathematical operator: " + so };
					break;
				}
			}
			return 0;
		}
		else
		{ // the initial character is parenthesis  (1+2)*3
			auto e = cutout_match_parenthesized_expression(start, end, start);
#if caculate_debug
			auto str = view_to_str(e);
#endif
			auto ne = std::to_string(evalute_expression(e.begin_, e.end_)) + std::string(start, end - start);
			return evalute_expression(ne.data(), ne.data() + ne.size());
		}
	}
}
