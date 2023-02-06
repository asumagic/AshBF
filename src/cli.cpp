#include "cli.hpp"
#include "bf/logger.hpp"
#include <algorithm>

bool CommandlineFlag::has_expected_value() const
{
	return expected.empty() || std::find(expected.begin(), expected.end(), value) != expected.end();
}

bool CommandlineFlag::operator==(std::string_view other) const
{
	return other == name || (other.size() == 1 && other[0] == short_name);
}

CommandlineFlag::operator bool()
{
	return value == "1";
}

CommandlineFlag::operator const std::string&()
{
	return value;
}

bool Flags::parse_commandline(const std::vector<std::string_view>& args)
{
	if (args.size() < 2)
	{
		fmt::print(errout(cmdinfo), "Syntax : ./ashbf <file.bf> (flags)\n");
		return false;
	}

	for (size_t i = 2; i < args.size(); ++i)
	{
		const std::string_view arg{args[i]};

		if (arg.size() < 2 || arg[0] != '-')
		{
			fmt::print(warnout(cmdinfo), "Unknown argument '{}', flags should be prefixed with '-'\n", arg);
			continue;
		}

		auto equals_it = std::find(arg.begin(), arg.end(), '=');

		// The value within the flag is either one char after the equal sign, either the first digit found.
		auto value_it = (equals_it != arg.end() ? equals_it + 1 : std::find_if(arg.begin(), arg.end(), ::isdigit));

		auto name_end_it = (equals_it != arg.end() ? equals_it : value_it);

		std::string_view name = arg.substr(1, static_cast<size_t>(std::distance(arg.begin() + 1, name_end_it)));
		auto match_it = std::find(begin(flags), end(flags), name);

		if (match_it != end(flags))
		{
			auto& flag = *match_it;
			flag.value = value_it != arg.end() ? std::string{value_it} : "1";

			if (!flag.has_expected_value())
			{
				fmt::print(errout(cmdinfo), "Passed bad parameter '{}' to flag '{}'\n", flag.value, flag.name);
				return false;
			}
		}
		else
		{
			fmt::print(errout(cmdinfo), "Invalid flag '{}'\n", args[i]);
			return false;
		}
	}

	return true;
}
