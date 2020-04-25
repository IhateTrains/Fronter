#ifndef CONFIGURATION_RADIO_OPTION
#define CONFIGURATION_RADIO_OPTION
#include "newParser.h"

namespace Configuration
{
class RadioOption: commonItems::parser
{
  public:
	RadioOption() = default;
	explicit RadioOption(std::istream& theStream, int theID);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getDisplayName() const { return displayName; }
	[[nodiscard]] const auto& getTooltip() const { return tooltip; }
	[[nodiscard]] auto getValue() const { return value; }
	[[nodiscard]] auto isDefault() const { return defaulted; }
	[[nodiscard]] auto getID() const { return ID; }
	void setValue() { value = true; }
	void unsetValue() { value = false; }

  private:
	void registerKeys();
	std::string name;
	std::string tooltip;
	std::string displayName;
	bool defaulted = false;
	bool value = false;
	int ID = 0;
};
} // namespace Configuration

#endif // CONFIGURATION_RADIO_OPTION
