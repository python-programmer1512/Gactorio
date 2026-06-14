#include "model/config/FactoryConfigLoader.hpp"

#include <cctype>
#include <cstdint>
#include <fstream>
#include <map>
#include <sstream>
#include <utility>

namespace gactorio::config_model {
namespace {

struct JsonValue {
    enum class Type {
        Null,
        Bool,
        Number,
        String,
        Array,
        Object
    };

    using Array = std::vector<JsonValue>;
    using Object = std::map<std::string, JsonValue>;

    Type type = Type::Null;
    bool boolValue = false;
    double numberValue = 0.0;
    std::string stringValue;
    Array arrayValue;
    Object objectValue;
};

std::string typeName(JsonValue::Type type) {
    switch (type) {
    case JsonValue::Type::Null:   return "null";
    case JsonValue::Type::Bool:   return "bool";
    case JsonValue::Type::Number: return "number";
    case JsonValue::Type::String: return "string";
    case JsonValue::Type::Array:  return "array";
    case JsonValue::Type::Object: return "object";
    }
    return "unknown";
}

class JsonParser {
public:
    explicit JsonParser(std::string_view text)
        : text_(text) {}

    JsonValue parse() {
        skipWhitespace();
        JsonValue value = parseValue("$");
        skipWhitespace();
        if (!atEnd()) {
            fail("Unexpected trailing JSON content");
        }
        return value;
    }

private:
    bool atEnd() const {
        return pos_ >= text_.size();
    }

    char peek() const {
        return atEnd() ? '\0' : text_[pos_];
    }

    char advance() {
        return atEnd() ? '\0' : text_[pos_++];
    }

    void skipWhitespace() {
        while (!atEnd() && std::isspace(static_cast<unsigned char>(peek()))) {
            ++pos_;
        }
    }

    [[noreturn]] void fail(const std::string& message) const {
        throw FactoryConfigError(message + " at byte " + std::to_string(pos_));
    }

    bool consume(char expected) {
        if (peek() != expected) {
            return false;
        }
        ++pos_;
        return true;
    }

    void expect(char expected, const std::string& message) {
        if (!consume(expected)) {
            fail(message);
        }
    }

    JsonValue parseValue(const std::string& path) {
        skipWhitespace();
        if (atEnd()) {
            fail("Unexpected end of JSON while reading " + path);
        }

        const char c = peek();
        if (c == '{') return parseObject(path);
        if (c == '[') return parseArray(path);
        if (c == '"') return JsonValue{JsonValue::Type::String, false, 0.0, parseString()};
        if (c == 't') return parseLiteral("true", JsonValue{JsonValue::Type::Bool, true});
        if (c == 'f') return parseLiteral("false", JsonValue{JsonValue::Type::Bool, false});
        if (c == 'n') return parseLiteral("null", JsonValue{JsonValue::Type::Null});
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber();

        fail("Unexpected token while reading " + path);
    }

    JsonValue parseLiteral(const char* literal, JsonValue value) {
        for (const char* p = literal; *p != '\0'; ++p) {
            if (advance() != *p) {
                fail(std::string("Invalid literal, expected ") + literal);
            }
        }
        return value;
    }

    JsonValue parseObject(const std::string& path) {
        JsonValue out;
        out.type = JsonValue::Type::Object;
        expect('{', "Expected object start");
        skipWhitespace();
        if (consume('}')) {
            return out;
        }

        while (true) {
            skipWhitespace();
            if (peek() != '"') {
                fail("Expected object key at " + path);
            }
            const std::string key = parseString();
            skipWhitespace();
            expect(':', "Expected ':' after object key");
            out.objectValue[key] = parseValue(path + "." + key);
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            expect(',', "Expected ',' between object members");
        }
        return out;
    }

    JsonValue parseArray(const std::string& path) {
        JsonValue out;
        out.type = JsonValue::Type::Array;
        expect('[', "Expected array start");
        skipWhitespace();
        if (consume(']')) {
            return out;
        }

        std::size_t index = 0;
        while (true) {
            out.arrayValue.push_back(parseValue(path + "[" + std::to_string(index) + "]"));
            ++index;
            skipWhitespace();
            if (consume(']')) {
                break;
            }
            expect(',', "Expected ',' between array elements");
        }
        return out;
    }

    std::string parseString() {
        expect('"', "Expected string");
        std::string out;
        while (!atEnd()) {
            const char c = advance();
            if (c == '"') {
                return out;
            }
            if (c != '\\') {
                out.push_back(c);
                continue;
            }
            if (atEnd()) {
                fail("Unterminated escape sequence");
            }
            const char escaped = advance();
            switch (escaped) {
            case '"':  out.push_back('"'); break;
            case '\\': out.push_back('\\'); break;
            case '/':  out.push_back('/'); break;
            case 'b':  out.push_back('\b'); break;
            case 'f':  out.push_back('\f'); break;
            case 'n':  out.push_back('\n'); break;
            case 'r':  out.push_back('\r'); break;
            case 't':  out.push_back('\t'); break;
            case 'u':  appendUtf8(out, parseHexCodePoint()); break;
            default:
                fail("Unsupported JSON string escape");
            }
        }
        fail("Unterminated string");
    }

    int hexValue(char c) const {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    }

    std::uint32_t parseRawHexCodeUnit() {
        std::uint32_t codePoint = 0;
        for (int i = 0; i < 4; ++i) {
            if (atEnd()) {
                fail("Unterminated unicode escape");
            }
            const int digit = hexValue(advance());
            if (digit < 0) {
                fail("Invalid unicode escape");
            }
            codePoint = (codePoint << 4) | static_cast<std::uint32_t>(digit);
        }
        return codePoint;
    }

    std::uint32_t parseHexCodePoint() {
        const std::uint32_t codePoint = parseRawHexCodeUnit();
        if (codePoint >= 0xD800 && codePoint <= 0xDBFF) {
            if (advance() != '\\' || advance() != 'u') {
                fail("High surrogate must be followed by a low surrogate");
            }
            const std::uint32_t low = parseRawHexCodeUnit();
            if (low < 0xDC00 || low > 0xDFFF) {
                fail("Invalid low surrogate");
            }
            return 0x10000 + ((codePoint - 0xD800) << 10) + (low - 0xDC00);
        }
        if (codePoint >= 0xDC00 && codePoint <= 0xDFFF) {
            fail("Unexpected low surrogate");
        }

        return codePoint;
    }

    void appendUtf8(std::string& out, std::uint32_t codePoint) {
        if (codePoint <= 0x7F) {
            out.push_back(static_cast<char>(codePoint));
        } else if (codePoint <= 0x7FF) {
            out.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
            out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
        } else if (codePoint <= 0xFFFF) {
            out.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
            out.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
        } else if (codePoint <= 0x10FFFF) {
            out.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
            out.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
        } else {
            fail("Invalid unicode code point");
        }
    }

    JsonValue parseNumber() {
        const auto start = pos_;
        consume('-');
        if (peek() == '0') {
            advance();
        } else {
            if (!std::isdigit(static_cast<unsigned char>(peek()))) {
                fail("Invalid number");
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                advance();
            }
        }
        if (consume('.')) {
            if (!std::isdigit(static_cast<unsigned char>(peek()))) {
                fail("Invalid number fraction");
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                advance();
            }
        }
        if (peek() == 'e' || peek() == 'E') {
            advance();
            if (peek() == '+' || peek() == '-') {
                advance();
            }
            if (!std::isdigit(static_cast<unsigned char>(peek()))) {
                fail("Invalid number exponent");
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                advance();
            }
        }

        JsonValue out;
        out.type = JsonValue::Type::Number;
        out.numberValue = std::stod(std::string(text_.substr(start, pos_ - start)));
        return out;
    }

    std::string_view text_;
    std::size_t pos_ = 0;
};

const JsonValue& requireType(const JsonValue& value, JsonValue::Type type, const std::string& path) {
    if (value.type != type) {
        throw FactoryConfigError("Expected " + typeName(type) + ": " + path);
    }
    return value;
}

const JsonValue::Object& asObject(const JsonValue& value, const std::string& path) {
    return requireType(value, JsonValue::Type::Object, path).objectValue;
}

const JsonValue::Array& asArray(const JsonValue& value, const std::string& path) {
    return requireType(value, JsonValue::Type::Array, path).arrayValue;
}

const JsonValue* findField(const JsonValue::Object& object, const std::string& field) {
    const auto it = object.find(field);
    return it == object.end() ? nullptr : &it->second;
}

const JsonValue& requireField(const JsonValue::Object& object, const std::string& field, const std::string& path) {
    const auto* value = findField(object, field);
    if (value == nullptr) {
        throw FactoryConfigError("Missing required field: " + path + "." + field);
    }
    return *value;
}

std::string requireString(const JsonValue::Object& object, const std::string& field, const std::string& path) {
    const auto& value = requireField(object, field, path);
    return requireType(value, JsonValue::Type::String, path + "." + field).stringValue;
}

std::string optionalString(const JsonValue::Object& object, const std::string& field, const std::string& path, std::string fallback = {}) {
    const auto* value = findField(object, field);
    if (value == nullptr) {
        return fallback;
    }
    return requireType(*value, JsonValue::Type::String, path + "." + field).stringValue;
}

bool optionalBool(const JsonValue::Object& object, const std::string& field, const std::string& path, bool fallback = false) {
    const auto* value = findField(object, field);
    if (value == nullptr) {
        return fallback;
    }
    return requireType(*value, JsonValue::Type::Bool, path + "." + field).boolValue;
}

double optionalDouble(const JsonValue::Object& object, const std::string& field, const std::string& path, double fallback) {
    const auto* value = findField(object, field);
    if (value == nullptr) {
        return fallback;
    }
    return requireType(*value, JsonValue::Type::Number, path + "." + field).numberValue;
}

double requireDouble(const JsonValue::Object& object, const std::string& field, const std::string& path) {
    const auto& value = requireField(object, field, path);
    return requireType(value, JsonValue::Type::Number, path + "." + field).numberValue;
}

int toInt(double value, const std::string& path) {
    const int asInt = static_cast<int>(value);
    if (static_cast<double>(asInt) != value) {
        throw FactoryConfigError("Expected integer: " + path);
    }
    return asInt;
}

int optionalInt(const JsonValue::Object& object, const std::string& field, const std::string& path, int fallback) {
    const auto* value = findField(object, field);
    if (value == nullptr) {
        return fallback;
    }
    return toInt(requireType(*value, JsonValue::Type::Number, path + "." + field).numberValue, path + "." + field);
}

int requireInt(const JsonValue::Object& object, const std::string& field, const std::string& path) {
    const auto& value = requireField(object, field, path);
    return toInt(requireType(value, JsonValue::Type::Number, path + "." + field).numberValue, path + "." + field);
}

void requireGreaterThanZero(double value, const std::string& path) {
    if (value <= 0.0) {
        throw FactoryConfigError("Expected value greater than zero: " + path);
    }
}

void requireNonNegative(double value, const std::string& path) {
    if (value < 0.0) {
        throw FactoryConfigError("Expected non-negative value: " + path);
    }
}

std::vector<std::string> optionalStringArray(
    const JsonValue::Object& object,
    const std::string& field,
    const std::string& path) {
    std::vector<std::string> out;
    const auto* value = findField(object, field);
    if (value == nullptr) {
        return out;
    }
    const auto& array = asArray(*value, path + "." + field);
    for (std::size_t i = 0; i < array.size(); ++i) {
        out.push_back(requireType(array[i], JsonValue::Type::String, path + "." + field + "[" + std::to_string(i) + "]").stringValue);
    }
    return out;
}

std::vector<std::string> requireStringArray(
    const JsonValue::Object& object,
    const std::string& field,
    const std::string& path) {
    const auto& value = requireField(object, field, path);
    const auto& array = asArray(value, path + "." + field);
    std::vector<std::string> out;
    for (std::size_t i = 0; i < array.size(); ++i) {
        out.push_back(requireType(array[i], JsonValue::Type::String, path + "." + field + "[" + std::to_string(i) + "]").stringValue);
    }
    return out;
}

FactorySettings parseSettings(const JsonValue::Object& root) {
    FactorySettings settings;
    const auto* value = findField(root, "settings");
    if (value == nullptr) {
        return settings;
    }

    const auto& object = asObject(*value, "settings");
    settings.initialHealth = optionalDouble(object, "initialHealth", "settings", settings.initialHealth);
    settings.damageChancePerSecond = optionalDouble(object, "damageChancePerSecond", "settings", settings.damageChancePerSecond);
    settings.damageMinHp = optionalDouble(object, "damageMinHp", "settings", settings.damageMinHp);
    settings.damageMaxHp = optionalDouble(object, "damageMaxHp", "settings", settings.damageMaxHp);
    settings.incrementalRepairHp = optionalDouble(object, "incrementalRepairHp", "settings", settings.incrementalRepairHp);
    settings.repairAllDelaySeconds = optionalDouble(object, "repairAllDelaySeconds", "settings", settings.repairAllDelaySeconds);
    settings.defaultLineScenarioId = optionalString(object, "defaultLineScenarioId", "settings", settings.defaultLineScenarioId);
    settings.defaultSpeedMultiplier = optionalDouble(object, "defaultSpeedMultiplier", "settings", settings.defaultSpeedMultiplier);

    requireGreaterThanZero(settings.initialHealth, "settings.initialHealth");
    requireNonNegative(settings.damageChancePerSecond, "settings.damageChancePerSecond");
    requireNonNegative(settings.damageMinHp, "settings.damageMinHp");
    requireNonNegative(settings.damageMaxHp, "settings.damageMaxHp");
    requireNonNegative(settings.incrementalRepairHp, "settings.incrementalRepairHp");
    requireNonNegative(settings.repairAllDelaySeconds, "settings.repairAllDelaySeconds");
    requireGreaterThanZero(settings.defaultSpeedMultiplier, "settings.defaultSpeedMultiplier");
    return settings;
}

std::vector<ItemDefinition> parseItems(const JsonValue::Object& root) {
    const auto& array = asArray(requireField(root, "items", "$"), "items");
    std::vector<ItemDefinition> out;
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string path = "items[" + std::to_string(i) + "]";
        const auto& object = asObject(array[i], path);
        ItemDefinition item;
        item.id = requireString(object, "id", path);
        item.displayName = requireString(object, "displayName", path);
        item.category = optionalString(object, "category", path, item.category);
        item.restockable = optionalBool(object, "restockable", path, item.restockable);
        item.restockAmount = optionalInt(object, "restockAmount", path, item.restockAmount);
        requireNonNegative(item.restockAmount, path + ".restockAmount");
        out.push_back(std::move(item));
    }
    return out;
}

std::vector<ProductDefinition> parseProducts(const JsonValue::Object& root) {
    const auto& array = asArray(requireField(root, "products", "$"), "products");
    std::vector<ProductDefinition> out;
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string path = "products[" + std::to_string(i) + "]";
        const auto& object = asObject(array[i], path);
        ProductDefinition product;
        product.id = requireString(object, "id", path);
        product.displayName = requireString(object, "displayName", path);
        product.description = optionalString(object, "description", path, product.description);
        product.tier = optionalString(object, "tier", path, product.tier);
        product.color = optionalString(object, "color", path, product.color);
        product.sortOrder = optionalInt(object, "sortOrder", path, product.sortOrder);
        product.defaultRecipeId = optionalString(object, "defaultRecipeId", path, product.defaultRecipeId);
        out.push_back(std::move(product));
    }
    return out;
}

std::vector<StationDefinition> parseStations(const JsonValue::Object& root) {
    const auto& array = asArray(requireField(root, "stations", "$"), "stations");
    std::vector<StationDefinition> out;
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string path = "stations[" + std::to_string(i) + "]";
        const auto& object = asObject(array[i], path);
        StationDefinition station;
        station.id = requireString(object, "id", path);
        station.kind = requireString(object, "kind", path);
        station.displayName = requireString(object, "displayName", path);
        station.typeName = optionalString(object, "typeName", path, station.typeName);
        station.acceptedStepKinds = optionalStringArray(object, "acceptedStepKinds", path);
        station.processingSpeed = optionalDouble(object, "processingSpeed", path, station.processingSpeed);
        station.maxHealth = optionalDouble(object, "maxHealth", path, station.maxHealth);
        station.breakdownProbability = optionalDouble(object, "breakdownProbability", path, station.breakdownProbability);
        requireGreaterThanZero(station.processingSpeed, path + ".processingSpeed");
        requireGreaterThanZero(station.maxHealth, path + ".maxHealth");
        requireNonNegative(station.breakdownProbability, path + ".breakdownProbability");
        out.push_back(std::move(station));
    }
    return out;
}

ItemStackDefinition parseItemStack(const JsonValue& value, const std::string& path) {
    const auto& object = asObject(value, path);
    ItemStackDefinition stack;
    stack.itemId = requireString(object, "itemId", path);
    stack.quantity = requireInt(object, "quantity", path);
    if (stack.quantity <= 0) {
        throw FactoryConfigError("Invalid quantity at " + path + ".quantity");
    }
    return stack;
}

RecipeOutputDefinition parseRecipeOutput(const JsonValue& value, const std::string& path) {
    const auto& object = asObject(value, path);
    RecipeOutputDefinition output;
    const auto* itemId = findField(object, "itemId");
    const auto* productId = findField(object, "productId");
    if ((itemId == nullptr) == (productId == nullptr)) {
        throw FactoryConfigError("Recipe output must contain exactly one of itemId or productId at " + path);
    }
    if (itemId != nullptr) {
        output.itemId = requireType(*itemId, JsonValue::Type::String, path + ".itemId").stringValue;
    }
    if (productId != nullptr) {
        output.productId = requireType(*productId, JsonValue::Type::String, path + ".productId").stringValue;
    }
    output.quantity = requireInt(object, "quantity", path);
    if (output.quantity <= 0) {
        throw FactoryConfigError("Invalid quantity at " + path + ".quantity");
    }
    return output;
}

std::vector<RecipeDefinition> parseRecipes(const JsonValue::Object& root) {
    const auto& array = asArray(requireField(root, "recipes", "$"), "recipes");
    std::vector<RecipeDefinition> out;
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string path = "recipes[" + std::to_string(i) + "]";
        const auto& object = asObject(array[i], path);
        RecipeDefinition recipe;
        recipe.id = requireString(object, "id", path);
        recipe.productId = requireString(object, "productId", path);
        recipe.displayName = requireString(object, "displayName", path);

        const auto& steps = asArray(requireField(object, "steps", path), path + ".steps");
        for (std::size_t stepIndex = 0; stepIndex < steps.size(); ++stepIndex) {
            const std::string stepPath = path + ".steps[" + std::to_string(stepIndex) + "]";
            const auto& stepObject = asObject(steps[stepIndex], stepPath);
            RecipeStepDefinition step;
            step.id = requireString(stepObject, "id", stepPath);
            step.stepKind = requireString(stepObject, "stepKind", stepPath);
            step.duration = requireDouble(stepObject, "duration", stepPath);
            requireGreaterThanZero(step.duration, stepPath + ".duration");

            if (const auto* inputs = findField(stepObject, "inputs")) {
                const auto& inputArray = asArray(*inputs, stepPath + ".inputs");
                for (std::size_t inputIndex = 0; inputIndex < inputArray.size(); ++inputIndex) {
                    step.inputs.push_back(parseItemStack(inputArray[inputIndex], stepPath + ".inputs[" + std::to_string(inputIndex) + "]"));
                }
            }

            if (const auto* outputs = findField(stepObject, "outputs")) {
                const auto& outputArray = asArray(*outputs, stepPath + ".outputs");
                for (std::size_t outputIndex = 0; outputIndex < outputArray.size(); ++outputIndex) {
                    step.outputs.push_back(parseRecipeOutput(outputArray[outputIndex], stepPath + ".outputs[" + std::to_string(outputIndex) + "]"));
                }
            }

            recipe.steps.push_back(std::move(step));
        }
        out.push_back(std::move(recipe));
    }
    return out;
}

std::vector<ProductionLineDefinition> parseLines(const JsonValue::Object& root) {
    const auto& array = asArray(requireField(root, "lines", "$"), "lines");
    std::vector<ProductionLineDefinition> out;
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string path = "lines[" + std::to_string(i) + "]";
        const auto& object = asObject(array[i], path);
        ProductionLineDefinition line;
        line.id = requireString(object, "id", path);
        line.displayName = requireString(object, "displayName", path);
        line.stationIds = requireStringArray(object, "stationIds", path);
        line.recipeIds = requireStringArray(object, "recipeIds", path);
        if (const auto* queueCapacity = findField(object, "queueCapacity")) {
            const auto value = toInt(requireType(*queueCapacity, JsonValue::Type::Number, path + ".queueCapacity").numberValue, path + ".queueCapacity");
            if (value < 0) {
                throw FactoryConfigError("Expected non-negative value: " + path + ".queueCapacity");
            }
            line.queueCapacity = static_cast<std::size_t>(value);
        }
        out.push_back(std::move(line));
    }
    return out;
}

std::vector<InventorySeed> parseInitialInventory(const JsonValue::Object& root) {
    std::vector<InventorySeed> out;
    const auto* value = findField(root, "initialInventory");
    if (value == nullptr) {
        return out;
    }
    const auto& array = asArray(*value, "initialInventory");
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string path = "initialInventory[" + std::to_string(i) + "]";
        const auto& object = asObject(array[i], path);
        InventorySeed seed;
        seed.itemId = requireString(object, "itemId", path);
        seed.quantity = requireInt(object, "quantity", path);
        if (seed.quantity < 0) {
            throw FactoryConfigError("Expected non-negative value: " + path + ".quantity");
        }
        out.push_back(std::move(seed));
    }
    return out;
}

std::vector<StartupTaskDefinition> parseStartupTasks(const JsonValue::Object& root) {
    std::vector<StartupTaskDefinition> out;
    const auto* value = findField(root, "startupTasks");
    if (value == nullptr) {
        return out;
    }
    const auto& array = asArray(*value, "startupTasks");
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string path = "startupTasks[" + std::to_string(i) + "]";
        const auto& object = asObject(array[i], path);
        StartupTaskDefinition task;
        task.lineId = requireString(object, "lineId", path);
        task.recipeId = requireString(object, "recipeId", path);
        task.quantity = optionalInt(object, "quantity", path, task.quantity);
        if (task.quantity <= 0) {
            throw FactoryConfigError("Invalid quantity at " + path + ".quantity");
        }
        out.push_back(std::move(task));
    }
    return out;
}

std::vector<ScenarioDefinition> parseScenarios(const JsonValue::Object& root) {
    std::vector<ScenarioDefinition> out;
    const auto* value = findField(root, "scenarios");
    if (value == nullptr) {
        return out;
    }
    const auto& array = asArray(*value, "scenarios");
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string path = "scenarios[" + std::to_string(i) + "]";
        const auto& object = asObject(array[i], path);
        ScenarioDefinition scenario;
        scenario.id = requireString(object, "id", path);
        scenario.displayName = requireString(object, "displayName", path);
        scenario.description = optionalString(object, "description", path, scenario.description);
        scenario.targetStationIds = optionalStringArray(object, "targetStationIds", path);
        scenario.targetStationKinds = optionalStringArray(object, "targetStationKinds", path);
        if (const auto* speedMultiplier = findField(object, "speedMultiplier")) {
            scenario.speedMultiplier = requireType(*speedMultiplier, JsonValue::Type::Number, path + ".speedMultiplier").numberValue;
            requireGreaterThanZero(*scenario.speedMultiplier, path + ".speedMultiplier");
        }
        if (const auto* breakdownProbabilityOverride = findField(object, "breakdownProbabilityOverride")) {
            scenario.breakdownProbabilityOverride = requireType(*breakdownProbabilityOverride, JsonValue::Type::Number, path + ".breakdownProbabilityOverride").numberValue;
            requireNonNegative(*scenario.breakdownProbabilityOverride, path + ".breakdownProbabilityOverride");
        }
        if (const auto* queueCapacity = findField(object, "queueCapacity")) {
            scenario.queueCapacity = toInt(requireType(*queueCapacity, JsonValue::Type::Number, path + ".queueCapacity").numberValue, path + ".queueCapacity");
            if (*scenario.queueCapacity < 0) {
                throw FactoryConfigError("Expected non-negative value: " + path + ".queueCapacity");
            }
        }
        out.push_back(std::move(scenario));
    }
    return out;
}

} // namespace

FactoryConfigError::FactoryConfigError(const std::string& message)
    : std::runtime_error(message) {}

FactoryConfig FactoryConfigLoader::loadFromFile(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) {
        throw FactoryConfigError("Unable to open factory config file: " + path.string());
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return loadFromString(buffer.str());
}

FactoryConfig FactoryConfigLoader::loadFromString(std::string_view jsonText) {
    const JsonValue rootValue = JsonParser(jsonText).parse();
    const auto& root = asObject(rootValue, "$");

    FactoryConfig config;
    config.schemaVersion = requireString(root, "schemaVersion", "$");
    config.settings = parseSettings(root);
    config.items = parseItems(root);
    config.products = parseProducts(root);
    config.stations = parseStations(root);
    config.recipes = parseRecipes(root);
    config.lines = parseLines(root);
    config.initialInventory = parseInitialInventory(root);
    config.startupTasks = parseStartupTasks(root);
    config.scenarios = parseScenarios(root);
    return config;
}

} // namespace gactorio::config_model
