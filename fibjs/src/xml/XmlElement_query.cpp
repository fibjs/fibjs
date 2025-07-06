/*
 * XmlElement_query.cpp
 *
 *  Created on: Jul 6, 2025
 *      Author: lion
 */

#include "object.h"
#include "ifs/xml.h"
#include "XmlElement.h"
#include "parse.h"
#include "XmlNodeList.h"
#include <vector>

namespace fibjs {

// Parse quoted string with escape support
static bool parseQuotedString(const exlib::string& input, int32_t& pos, exlib::string& result, char quote)
{
    _parser p(input.c_str() + pos, input.length() - pos);
    result.clear();

    while (!p.end()) {
        char ch = p.get();
        if (ch == quote) {
            p.skip();
            pos += p.pos;
            return true;
        } else if (ch == '\\') {
            p.skip();
            if (p.end()) {
                return false; // incomplete escape
            }
            char escaped = p.get();
            if (escaped == quote || escaped == '\\') {
                result += escaped;
            } else {
                result += '\\';
                result += escaped;
            }
            p.skip();
        } else {
            result += ch;
            p.skip();
        }
    }

    return false; // unterminated string
}

// Simple CSS escape handling - handles basic backslash escapes
static exlib::string unescapeCSSIdentifier(const exlib::string& escaped)
{
    exlib::string result;
    const char* p = escaped.c_str();
    for (size_t i = 0; i < escaped.length(); i++) {
        if (p[i] == '\\' && i + 1 < escaped.length()) {
            char next = p[i + 1];
            if (next == ':' || next == '.' || next == '#' || next == '[' || next == ']' || next == '\\' || next == '"' || next == '\'') {
                result += next;
                i++; // skip the escaped character
            } else {
                result += p[i];
            }
        } else {
            result += p[i];
        }
    }
    return result;
}

// Helper function to parse parentheses content with proper nesting
static bool parseParenthesesContent(const exlib::string& input, int32_t& pos, exlib::string& content)
{
    _parser p(input.c_str() + pos, input.length() - pos);

    if (p.get() != '(') {
        return false;
    }

    p.skip(); // skip opening (
    content.clear();

    int parenCount = 1;
    while (!p.end() && parenCount > 0) {
        char ch = p.get();
        if (ch == '(') {
            parenCount++;
        } else if (ch == ')') {
            parenCount--;
        }
        if (parenCount > 0) {
            content += ch;
        }
        p.skip();
    }

    if (parenCount != 0) {
        return false; // unmatched parentheses
    }

    pos += p.pos;
    return true;
}

// Helper function to parse attribute operator and value
static bool parseAttributeOperatorAndValue(_parser& p, const exlib::string& input,
    exlib::string& attrValue, bool& hasAttrValue)
{
    hasAttrValue = false;
    char op = p.get();

    if (op == '=') {
        p.skip(); // skip =
        p.skipSpace();
        char quote = p.get();
        if (quote == '"' || quote == '\'') {
            p.skip(); // skip quote
            exlib::string quotedValue;
            int32_t relativePos = 0;
            exlib::string remainingInput = input.substr(p.pos);
            if (!parseQuotedString(remainingInput, relativePos, quotedValue, quote)) {
                return false;
            }
            p.pos += relativePos;
            attrValue = unescapeCSSIdentifier(quotedValue);
        } else {
            if (!p.getWord(attrValue, ']')) {
                return false;
            }
        }
        hasAttrValue = true;
        attrValue = "=" + attrValue;
    } else if (op == '^' || op == '*' || op == '$' || op == '~' || op == '|') {
        p.skip(); // skip the operator
        if (p.get() != '=') {
            return false;
        }
        p.skip(); // skip =
        p.skipSpace();
        char quote = p.get();
        if (quote == '"' || quote == '\'') {
            p.skip(); // skip quote
            exlib::string quotedValue;
            int32_t relativePos = 0;
            exlib::string remainingInput = input.substr(p.pos);
            if (!parseQuotedString(remainingInput, relativePos, quotedValue, quote)) {
                return false;
            }
            p.pos += relativePos;
            attrValue = unescapeCSSIdentifier(quotedValue);
        } else {
            if (!p.getWord(attrValue, ']')) {
                return false;
            }
        }
        hasAttrValue = true;

        // Store operator with value
        if (op == '^') {
            attrValue = "^=" + attrValue;
        } else if (op == '*') {
            attrValue = "*=" + attrValue;
        } else if (op == '$') {
            attrValue = "$=" + attrValue;
        } else if (op == '~') {
            attrValue = "~=" + attrValue;
        } else if (op == '|') {
            attrValue = "|=" + attrValue;
        }
    }

    return true;
}

// Simple CSS selector types
enum SelectorType {
    SELECTOR_ELEMENT, // element
    SELECTOR_ID, // #id
    SELECTOR_CLASS, // .class
    SELECTOR_ATTRIBUTE, // [attr] or [attr="value"]
    SELECTOR_PSEUDO_CLASS, // :pseudo-class
    SELECTOR_PSEUDO_ELEMENT, // ::pseudo-element
    SELECTOR_NOT, // :not()
    SELECTOR_IS, // :is()
    SELECTOR_WHERE, // :where()
    SELECTOR_HAS, // :has()
    SELECTOR_DESCENDANT, // space separator
    SELECTOR_CHILD, // > separator
    SELECTOR_ADJACENT_SIBLING, // + separator
    SELECTOR_GENERAL_SIBLING // ~ separator
};

// Simple CSS selector component
struct SelectorComponent {
    SelectorType type;
    exlib::string value;
    exlib::string attrName; // for attribute selectors
    exlib::string attrValue; // for attribute selectors with value
    bool hasAttrValue; // whether attribute selector has value
    std::vector<SelectorComponent> notComponents; // for simple selectors in :not(), :is(), :where()
    exlib::string hasContent; // for complex selectors in :has() like ":has(> ul)" - store as string for later parsing

    SelectorComponent()
        : type(SELECTOR_ELEMENT)
        , hasAttrValue(false)
    {
    }
};

// Simple CSS selector (sequence of components)
struct SimpleSelector {
    std::vector<SelectorComponent> components;
};

// Represents a combinator between selectors
struct SelectorCombinator {
    SelectorType type; // SELECTOR_DESCENDANT, SELECTOR_CHILD, etc.

    SelectorCombinator(SelectorType t = SELECTOR_DESCENDANT)
        : type(t)
    {
    }
};

// Simple CSS selector (sequence of components with combinators)
struct ComplexSelector {
    std::vector<SimpleSelector> selectors; // The selector parts
    std::vector<SelectorCombinator> combinators; // The combinators between selectors
};

// CSS selector list (comma separated)
struct SelectorList {
    std::vector<ComplexSelector> selectors; // each ComplexSelector represents a full selector chain
};

// Helper function to parse a basic selector component (for use in :not(), :is(), :where())
static bool parseBasicSelectorComponent(_parser& parser, SelectorComponent& comp)
{
    parser.skipSpace();
    char ch = parser.get();

    if (ch == '.') {
        // Class selector
        comp.type = SELECTOR_CLASS;
        parser.skip(); // skip '.'
        return parser.getWord(comp.value, ',', ')');
    } else if (ch == '#') {
        // ID selector
        comp.type = SELECTOR_ID;
        parser.skip(); // skip '#'
        return parser.getWord(comp.value, ',', ')');
    } else if (ch == '[') {
        // Attribute selector
        comp.type = SELECTOR_ATTRIBUTE;
        parser.skip(); // skip '['
        parser.skipSpace();
        if (!parser.getWord(comp.attrName, '=', ']')) {
            return false;
        }
        parser.skipSpace();

        // Parse operator and value using helper function
        // Note: This is a simplified version for basic parsing
        char op = parser.get();
        if (op == '=' || op == '^' || op == '*' || op == '$' || op == '~' || op == '|') {
            comp.hasAttrValue = true;
            exlib::string opStr;

            if (op == '=' && !comp.attrName.empty()) {
                opStr = "=";
            } else if (op == '^' && parser.peek() == '=') {
                opStr = "^=";
                parser.skip();
            } else if (op == '*' && parser.peek() == '=') {
                opStr = "*=";
                parser.skip();
            } else if (op == '$' && parser.peek() == '=') {
                opStr = "$=";
                parser.skip();
            } else if (op == '~' && parser.peek() == '=') {
                opStr = "~=";
                parser.skip();
            } else if (op == '|' && parser.peek() == '=') {
                opStr = "|=";
                parser.skip();
            }

            parser.skip(); // skip =
            parser.skipSpace();

            exlib::string value;
            if (parser.get() == '"' || parser.get() == '\'') {
                char quote = parser.get();
                parser.skip();
                parser.getWord(value, quote);
                parser.skip();
            } else {
                parser.getWord(value, ']');
            }

            comp.attrValue = opStr + value;
        } else {
            comp.hasAttrValue = false;
        }

        // Skip to closing bracket
        while (!parser.end() && parser.get() != ']') {
            parser.skip();
        }
        if (parser.get() == ']') {
            parser.skip();
        }
        return true;
    } else if (qisalpha(ch) || ch == '*') {
        // Element selector
        comp.type = SELECTOR_ELEMENT;
        return parser.getWord(comp.value, ',', ')');
    } else if (ch == ':') {
        // Pseudo-class selector
        parser.skip(); // skip ':'
        if (parser.getWord(comp.value, ',', ')')) {
            comp.type = SELECTOR_PSEUDO_CLASS;
            return true;
        }
    }

    return false;
}

// Helper function to parse comma-separated basic selectors
static bool parseCommaSeparatedBasicSelectors(const exlib::string& content,
    std::vector<SelectorComponent>& components)
{
    _parser parser(content);
    parser.skipSpace();

    while (!parser.end()) {
        SelectorComponent comp;
        if (parseBasicSelectorComponent(parser, comp)) {
            components.push_back(comp);
        } else {
            parser.skip(); // skip unrecognized character
            continue;
        }

        parser.skipSpace();
        if (parser.get() == ',') {
            parser.skip();
            parser.skipSpace();
        } else {
            break;
        }
    }

    return !components.empty();
}

// Parse nth-child expression like 2n+1, 3n-1, odd, even, 5, -n+3
static bool parseNthExpression(const exlib::string& nthValue, int& a, int& b)
{
    if (nthValue == "odd") {
        a = 2;
        b = 1;
        return true;
    }
    if (nthValue == "even") {
        a = 2;
        b = 0;
        return true;
    }

    // Remove all spaces first
    exlib::string trimmed;
    for (size_t i = 0; i < nthValue.length(); i++) {
        if (!qisspace(nthValue.c_str()[i])) {
            trimmed += nthValue.c_str()[i];
        }
    }

    // Try to parse as a single number
    if (trimmed.find('n') == exlib::string::npos) {
        char* endptr;
        long val = strtol(trimmed.c_str(), &endptr, 10);
        if (*endptr == '\0' && val > 0) {
            a = 0;
            b = val;
            return true;
        }
        return false;
    }

    // Parse expressions like 2n+1, 3n-1, n+5, -n+3
    size_t nPos = trimmed.find('n');
    if (nPos == exlib::string::npos) {
        return false;
    }

    // Parse coefficient of n
    exlib::string aStr = trimmed.substr(0, nPos);
    if (aStr.empty() || aStr == "+") {
        a = 1;
    } else if (aStr == "-") {
        a = -1;
    } else {
        char* endptr;
        long val = strtol(aStr.c_str(), &endptr, 10);
        if (*endptr != '\0') {
            return false;
        }
        a = val;
    }

    // Parse constant term
    if (nPos + 1 >= trimmed.length()) {
        b = 0;
        return true;
    }

    exlib::string bStr = trimmed.substr(nPos + 1);
    if (bStr.empty()) {
        b = 0;
        return true;
    }

    char* endptr;
    long val = strtol(bStr.c_str(), &endptr, 10);
    if (*endptr != '\0') {
        return false;
    }
    b = val;

    return true;
}
static bool parseCSSIdentifier(_parser& p, exlib::string& result)
{
    result.clear();
    while (!p.end()) {
        char ch = p.get();
        if (ch == '\\') {
            // Handle escape sequence
            p.skip(); // skip backslash
            if (p.end()) {
                return false; // incomplete escape
            }
            char escaped = p.get();
            // Add the escaped character directly (the unescaping will be done by unescapeCSSIdentifier)
            result += '\\';
            result += escaped;
            p.skip();
        } else if (ch == '.' || ch == '#' || ch == '[' || ch == ' ' || ch == '>' || ch == '+' || ch == '~' || ch == ',' || ch == '\t' || ch == '\n' || ch == '\r' || ch == ':') {
            // Stop at CSS delimiters
            break;
        } else {
            result += ch;
            p.skip();
        }
    }
    return !result.empty();
}

// Parse a simple selector component
static bool parseSimpleSelector(_parser& p, SimpleSelector& selector, const exlib::string& input)
{
    p.skipSpace();

    int loopCount = 0;
    while (!p.end()) {
        loopCount++;
        if (loopCount > 100) {
            return false;
        }

        char ch = p.get();

        SelectorComponent comp;
        bool componentParsed = false;

        if (ch == '#') {
            // ID selector
            p.skip(); // skip #
            exlib::string rawValue;
            if (!parseCSSIdentifier(p, rawValue)) {
                return false; // invalid ID selector
            }
            // Handle CSS escape sequences
            comp.value = unescapeCSSIdentifier(rawValue);
            comp.type = SELECTOR_ID;
            selector.components.push_back(comp);
            componentParsed = true;
        } else if (ch == '.') {
            // Class selector
            p.skip(); // skip .
            exlib::string rawValue;
            if (!parseCSSIdentifier(p, rawValue)) {
                return false; // invalid class selector
            }
            // Handle CSS escape sequences
            comp.value = unescapeCSSIdentifier(rawValue);
            comp.type = SELECTOR_CLASS;
            selector.components.push_back(comp);
            componentParsed = true;
        } else if (ch == '[') {
            // Attribute selector
            p.skip(); // skip [
            p.skipSpace();

            if (!p.getWord(comp.attrName, '=', ']', '^', '*', '$', '~', '|')) {
                // Try parsing attribute name with special characters
                exlib::string attrName;
                while (!p.end()) {
                    char ch = p.get();
                    if (ch == '=' || ch == ']' || ch == '^' || ch == '*' || ch == '$' || ch == '~' || ch == '|' || qisspace(ch)) {
                        break;
                    }
                    attrName += ch;
                    p.skip();
                }
                if (attrName.empty()) {
                    return false; // invalid attribute name
                }
                comp.attrName = attrName;
            }

            p.skipSpace();
            comp.type = SELECTOR_ATTRIBUTE;

            // Use helper function to parse operator and value
            if (!parseAttributeOperatorAndValue(p, input, comp.attrValue, comp.hasAttrValue)) {
                // If no operator/value, it's just an attribute existence check
                comp.hasAttrValue = false;
            }

            p.skipSpace();
            if (p.get() != ']') {
                return false; // missing closing bracket
            }
            p.skip(); // skip ]
            selector.components.push_back(comp);
            componentParsed = true;
        } else if (ch == ':') {
            // Pseudo-class or pseudo-element selector
            p.skip(); // skip first :

            // Check for pseudo-element (::)
            bool isPseudoElement = false;
            if (p.get() == ':') {
                isPseudoElement = true;
                p.skip(); // skip second :
            }

            if (!p.getWord(comp.value, '.', '#', '[', '>', '+', '~', ',', ':', '(')) {
                return false; // invalid pseudo-class/element name
            }

            if (isPseudoElement) {
                comp.type = SELECTOR_PSEUDO_ELEMENT;
                // For CSS selectors, pseudo-elements should not match any elements
                // but should not cause parsing to fail
                selector.components.push_back(comp);
                componentParsed = true;
            } else {
                comp.type = SELECTOR_PSEUDO_CLASS;
            }

            // Handle special pseudo-classes with parameters
            if (comp.value == "not" && p.get() == '(') {
                comp.type = SELECTOR_NOT;

                // Use helper function to parse parentheses content
                exlib::string notContent;
                int32_t relativePos = p.pos;
                if (!parseParenthesesContent(input, relativePos, notContent)) {
                    return false; // unmatched parentheses
                }
                p.pos = relativePos;

                // Parse the content inside :not() using helper function
                parseCommaSeparatedBasicSelectors(notContent, comp.notComponents);

            } else if (comp.value == "has" && p.get() == '(') {
                comp.type = SELECTOR_HAS;

                // Use helper function to parse parentheses content
                exlib::string hasContent;
                int32_t relativePos = p.pos;
                if (!parseParenthesesContent(input, relativePos, hasContent)) {
                    return false; // unmatched parentheses
                }
                p.pos = relativePos;

                // Store the content for later parsing
                comp.hasContent = hasContent;

            } else if ((comp.value == "is" || comp.value == "where") && p.get() == '(') {
                if (comp.value == "is") {
                    comp.type = SELECTOR_IS;
                } else if (comp.value == "where") {
                    comp.type = SELECTOR_WHERE;
                }

                // Use helper function to parse parentheses content
                exlib::string innerContent;
                int32_t relativePos = p.pos;
                if (!parseParenthesesContent(input, relativePos, innerContent)) {
                    return false; // unmatched parentheses
                }
                p.pos = relativePos;

                // Parse the content inside the pseudo-class using helper function
                parseCommaSeparatedBasicSelectors(innerContent, comp.notComponents);

            } else if (p.get() == '(') {
                // Handle nth-child(n), nth-of-type(n), etc. using helper function
                exlib::string param;
                int32_t relativePos = p.pos;
                if (!parseParenthesesContent(input, relativePos, param)) {
                    return false; // unmatched parentheses
                }
                p.pos = relativePos;

                comp.value += '(' + param + ')';
            }

            selector.components.push_back(comp);
            componentParsed = true;
        } else if (qisalpha(ch) || ch == '*') {
            // Element selector
            exlib::string rawValue;
            if (!parseCSSIdentifier(p, rawValue)) {
                return false; // invalid element name
            }
            comp.value = rawValue; // Element names don't need CSS unescaping
            comp.type = SELECTOR_ELEMENT;
            selector.components.push_back(comp);
            componentParsed = true;
        } else if (qisspace(ch) || ch == ',' || ch == '>' || ch == '+' || ch == '~' || ch == ':') {
            // End of simple selector
            break;
        } else {
            // Invalid character - skip it to avoid infinite loop
            p.skip();
        }

        // If no component was parsed and we didn't break, something is wrong
        if (!componentParsed && !qisspace(ch) && ch != ',' && ch != '>' && ch != '+' && ch != '~' && ch != ':') {
            return false;
        }
    }

    return !selector.components.empty();
}

// Parse CSS selector string
static bool parseSelectors(const exlib::string& input, SelectorList& selectorList)
{
    if (input.empty()) {
        return false;
    }

    _parser p(input);
    p.skipSpace();

    // Check for leading comma
    if (!p.end() && p.get() == ',') {
        return false; // Invalid: selector list starts with comma
    }

    int selectorCount = 0;
    while (!p.end()) {
        selectorCount++;
        if (selectorCount > 50) {
            return false;
        }

        ComplexSelector complexSelector;

        // Parse complex selector (with combinators)
        int chainCount = 0;
        while (!p.end()) {
            chainCount++;
            if (chainCount > 20) {
                return false;
            }

            SimpleSelector selector;
            int32_t oldPos = p.pos;

            if (!parseSimpleSelector(p, selector, input)) {
                return false;
            }

            if (p.pos == oldPos) {
                return false;
            }

            complexSelector.selectors.push_back(selector);

            // Skip any whitespace to get to the next meaningful character
            p.skipSpace();

            // Check what comes next
            char nextChar = p.get();

            if (p.end() || nextChar == '\0' || nextChar == ',') {
                break; // end of this complex selector
            }

            // Parse combinator
            SelectorCombinator combinator;
            if (nextChar == '>') {
                combinator.type = SELECTOR_CHILD;
                p.skip();
                p.skipSpace();
            } else if (nextChar == '+') {
                combinator.type = SELECTOR_ADJACENT_SIBLING;
                p.skip();
                p.skipSpace();
            } else if (nextChar == '~') {
                combinator.type = SELECTOR_GENERAL_SIBLING;
                p.skip();
                p.skipSpace();
            } else {
                // If we get here after skipping spaces and it's not a recognized combinator,
                // we need to backtrack and treat this as a descendant combinator
                combinator.type = SELECTOR_DESCENDANT;
                // Don't skip anything here, the next iteration will parse the next selector
            }

            complexSelector.combinators.push_back(combinator);
        }

        if (!complexSelector.selectors.empty()) {
            selectorList.selectors.push_back(complexSelector);
        }

        char currentChar = p.get();

        if (p.end() || currentChar == '\0') {
            break;
        }

        if (currentChar == ',') {
            p.skip(); // skip comma
            p.skipSpace();

            // Check for trailing comma or consecutive commas
            if (p.end() || p.get() == ',') {
                return false; // Invalid: trailing comma or consecutive commas
            }
        } else {
            break;
        }
    }

    return !selectorList.selectors.empty();
}

// Check if element matches a simple selector
static bool matchesSimpleSelector(XmlElement* element, const SimpleSelector& selector, bool isXml)
{
    for (const auto& comp : selector.components) {
        switch (comp.type) {
        case SELECTOR_ELEMENT: {
            if (comp.value != "*") {
                exlib::string tagName;
                element->get_tagName(tagName);
                // Case insensitive for HTML, case sensitive for XML
                // For HTML documents, convert to uppercase to match DOM standard
                if (isXml) {
                    if (tagName != comp.value) {
                        return false;
                    }
                } else {
                    // For HTML, comparison should be case insensitive
                    // but tagName should be returned in uppercase according to DOM standard
                    if (qstricmp(tagName.c_str(), comp.value.c_str()) != 0) {
                        return false;
                    }
                }
            }
            break;
        }
        case SELECTOR_ID: {
            exlib::string id;
            element->get_id(id);

            // In HTML mode, ID comparison should be case insensitive
            if (isXml) {
                if (id != comp.value) {
                    return false;
                }
            } else {
                // For HTML, try case insensitive comparison first
                if (qstricmp(id.c_str(), comp.value.c_str()) != 0) {
                    // If standard id attribute doesn't match, try uppercase ID attribute
                    exlib::string upperCaseId;
                    element->getAttribute("ID", upperCaseId);
                    if (qstricmp(upperCaseId.c_str(), comp.value.c_str()) != 0) {
                        return false;
                    }
                }
            }
            break;
        }
        case SELECTOR_CLASS: {
            exlib::string className;
            element->get_className(className);

            // In HTML mode, if standard class attribute is empty, try uppercase CLASS attribute
            if (!isXml && className.empty()) {
                element->getAttribute("CLASS", className);
            }

            if (className.empty()) {
                return false;
            }

            // Check if class name contains the required class
            _parser cp(className);
            bool found = false;
            exlib::string cls;
            cp.skipSpace();
            while (cp.getWord(cls)) {
                if (cls == comp.value) {
                    found = true;
                    break;
                }
                cp.skipSpace();
            }
            if (!found) {
                return false;
            }
            break;
        }
        case SELECTOR_ATTRIBUTE: {
            bool hasAttr;
            element->hasAttribute(comp.attrName, hasAttr);
            if (!hasAttr) {
                return false;
            }

            if (comp.hasAttrValue) {
                exlib::string attrValue;
                element->getAttribute(comp.attrName, attrValue);

                if (!qstrcmp(comp.attrValue.c_str(), "^=", 2)) {
                    // Prefix match
                    exlib::string prefix = comp.attrValue.substr(2);
                    if (attrValue.substr(0, prefix.length()) != prefix) {
                        return false;
                    }
                } else if (!qstrcmp(comp.attrValue.c_str(), "*=", 2)) {
                    // Contains match
                    exlib::string contains = comp.attrValue.substr(2);
                    if (attrValue.find(contains) == exlib::string::npos) {
                        return false;
                    }
                } else if (!qstrcmp(comp.attrValue.c_str(), "$=", 2)) {
                    // Suffix match
                    exlib::string suffix = comp.attrValue.substr(2);
                    if (attrValue.length() < suffix.length() || attrValue.substr(attrValue.length() - suffix.length()) != suffix) {
                        return false;
                    }
                } else if (!qstrcmp(comp.attrValue.c_str(), "~=", 2)) {
                    // Word match - check if value contains the word as a complete word
                    exlib::string word = comp.attrValue.substr(2);
                    _parser wp(attrValue);
                    bool found = false;
                    exlib::string w;
                    wp.skipSpace();
                    while (wp.getWord(w)) {
                        if (w == word) {
                            found = true;
                            break;
                        }
                        wp.skipSpace();
                    }
                    if (!found) {
                        return false;
                    }
                } else if (!qstrcmp(comp.attrValue.c_str(), "|=", 2)) {
                    // Language match - exact match or prefix followed by hyphen
                    exlib::string lang = comp.attrValue.substr(2);
                    if (attrValue != lang && (attrValue.length() <= lang.length() || attrValue.substr(0, lang.length()) != lang || (attrValue.length() > lang.length() && attrValue.c_str()[lang.length()] != '-'))) {
                        return false;
                    }
                } else if (comp.attrValue.c_str()[0] == '=') {
                    // Exact match
                    exlib::string exact = comp.attrValue.substr(1);
                    if (attrValue != exact) {
                        return false;
                    }
                } else {
                    // Legacy: direct comparison (for backward compatibility)
                    if (attrValue != comp.attrValue) {
                        return false;
                    }
                }
            }
            break;
        }
        case SELECTOR_PSEUDO_CLASS: {
            // Handle pseudo-class selectors
            if (comp.value == "first-child") {
                // Check if this is the first element child
                obj_ptr<XmlNode_base> parent;
                element->get_parentNode(parent);
                if (parent) {
                    obj_ptr<XmlNode_base> firstElementChild;
                    parent->get_firstElementChild(firstElementChild);
                    if (!firstElementChild || firstElementChild.As<XmlElement>() != element) {
                        return false;
                    }
                } else {
                    return false; // no parent, can't be first child
                }
            } else if (comp.value == "last-child") {
                // Check if this is the last element child
                obj_ptr<XmlNode_base> parent;
                element->get_parentNode(parent);
                if (parent) {
                    obj_ptr<XmlNode_base> lastElementChild;
                    parent->get_lastElementChild(lastElementChild);
                    if (!lastElementChild || lastElementChild.As<XmlElement>() != element) {
                        return false;
                    }
                } else {
                    return false; // no parent, can't be last child
                }
            } else if (!qstrcmp(comp.value.c_str(), "nth-child(", 10)) {
                // Handle nth-child(n) - enhanced implementation
                exlib::string nthValue = comp.value.substr(10);
                if (!nthValue.empty() && nthValue.c_str()[nthValue.length() - 1] == ')') {
                    nthValue = nthValue.substr(0, nthValue.length() - 1); // remove ')'

                    int a, b;
                    if (!parseNthExpression(nthValue, a, b)) {
                        return false; // malformed nth-child expression
                    }

                    // Find position among element siblings
                    obj_ptr<XmlNode_base> parent;
                    element->get_parentNode(parent);
                    if (parent) {
                        obj_ptr<XmlNodeList_base> children;
                        parent->get_childNodes(children);
                        XmlNodeList* childList = children.As<XmlNodeList>();
                        int position = 1;
                        for (size_t i = 0; i < childList->m_childs.size(); i++) {
                            XmlNodeImpl* childImpl = childList->m_childs[i];
                            if (childImpl->m_type == xml_base::C_ELEMENT_NODE) {
                                if (childImpl->m_node == element) {
                                    // Check if position matches nth-child formula: an + b
                                    if (a == 0) {
                                        return position == b;
                                    } else {
                                        if (a > 0) {
                                            return (position >= b) && ((position - b) % a == 0);
                                        } else {
                                            // For negative a (like -n+3), check if position <= b and (b-position) is divisible by |a|
                                            return (position <= b) && (position > 0) && ((b - position) % (-a) == 0);
                                        }
                                    }
                                }
                                position++;
                            }
                        }
                        return false; // element not found
                    } else {
                        return false; // no parent
                    }
                } else {
                    return false; // malformed nth-child
                }
            } else if (!qstrcmp(comp.value.c_str(), "nth-last-child(", 15)) {
                // Handle nth-last-child(n) - count from the end
                exlib::string nthValue = comp.value.substr(15);
                if (!nthValue.empty() && nthValue.c_str()[nthValue.length() - 1] == ')') {
                    nthValue = nthValue.substr(0, nthValue.length() - 1); // remove ')'

                    int a, b;
                    if (!parseNthExpression(nthValue, a, b)) {
                        return false; // malformed nth-last-child expression
                    }

                    // Find position among element siblings counting from the end
                    obj_ptr<XmlNode_base> parent;
                    element->get_parentNode(parent);
                    if (parent) {
                        obj_ptr<XmlNodeList_base> children;
                        parent->get_childNodes(children);
                        XmlNodeList* childList = children.As<XmlNodeList>();
                        XmlElement* lastOfType = nullptr;
                        int positionFromEnd = 0;
                        for (int i = childList->m_childs.size() - 1; i >= 0; i--) {
                            XmlNodeImpl* childImpl = childList->m_childs[i];
                            if (childImpl->m_type == xml_base::C_ELEMENT_NODE) {
                                lastOfType = static_cast<XmlElement*>(childImpl->m_node);
                                positionFromEnd++;
                                if (lastOfType == element) {
                                    // Check if position matches nth-last-child formula
                                    if (a == 0) {
                                        return positionFromEnd == b;
                                    } else {
                                        if (a > 0) {
                                            return (positionFromEnd >= b) && ((positionFromEnd - b) % a == 0);
                                        } else {
                                            return (positionFromEnd <= b) && (positionFromEnd > 0) && ((b - positionFromEnd) % (-a) == 0);
                                        }
                                    }
                                }
                            }
                        }
                        return false; // element not found
                    } else {
                        return false; // no parent
                    }
                } else {
                    return false; // malformed nth-last-child
                }
            } else if (comp.value == "only-child") {
                // Check if this is the only element child
                obj_ptr<XmlNode_base> parent;
                element->get_parentNode(parent);
                if (parent) {
                    obj_ptr<XmlNode_base> firstChild;
                    obj_ptr<XmlNode_base> lastChild;
                    parent->get_firstElementChild(firstChild);
                    parent->get_lastElementChild(lastChild);
                    if (!firstChild || !lastChild || firstChild != lastChild || firstChild.As<XmlElement>() != element) {
                        return false;
                    }
                } else {
                    return false; // no parent, can't be only child
                }
            } else if (comp.value == "checked") {
                // Check if form element is checked
                exlib::string tagName;
                element->get_tagName(tagName);
                if (qstricmp(tagName.c_str(), "INPUT") == 0) {
                    exlib::string type;
                    element->getAttribute("type", type);
                    if (qstricmp(type.c_str(), "radio") == 0 || qstricmp(type.c_str(), "checkbox") == 0) {
                        bool hasChecked;
                        element->hasAttribute("checked", hasChecked);
                        if (!hasChecked) {
                            return false;
                        }
                    } else {
                        return false; // not a checkable input type
                    }
                } else {
                    return false; // not an input element
                }
            } else if (comp.value == "disabled") {
                // Check if form element is disabled
                exlib::string tagName;
                element->get_tagName(tagName);
                if (qstricmp(tagName.c_str(), "INPUT") == 0 || qstricmp(tagName.c_str(), "BUTTON") == 0 || qstricmp(tagName.c_str(), "SELECT") == 0 || qstricmp(tagName.c_str(), "TEXTAREA") == 0 || qstricmp(tagName.c_str(), "OPTION") == 0) {
                    bool hasDisabled;
                    element->hasAttribute("disabled", hasDisabled);
                    if (!hasDisabled) {
                        return false;
                    }
                } else {
                    return false; // not a form element
                }
            } else if (comp.value == "first-of-type") {
                // Check if this is the first element of its type
                exlib::string tagName;
                element->get_tagName(tagName);
                obj_ptr<XmlNode_base> parent;
                element->get_parentNode(parent);
                if (parent) {
                    obj_ptr<XmlNodeList_base> children;
                    parent->get_childNodes(children);
                    XmlNodeList* childList = children.As<XmlNodeList>();
                    for (size_t i = 0; i < childList->m_childs.size(); i++) {
                        XmlNodeImpl* childImpl = childList->m_childs[i];
                        if (childImpl->m_type == xml_base::C_ELEMENT_NODE) {
                            XmlElement* childElement = static_cast<XmlElement*>(childImpl->m_node);
                            exlib::string childTagName;
                            childElement->get_tagName(childTagName);
                            if (qstricmp(childTagName.c_str(), tagName.c_str()) == 0) {
                                return childElement == element;
                            }
                        }
                    }
                    return false;
                } else {
                    return false; // no parent
                }
            } else if (comp.value == "last-of-type") {
                // Check if this is the last element of its type
                exlib::string tagName;
                element->get_tagName(tagName);
                obj_ptr<XmlNode_base> parent;
                element->get_parentNode(parent);
                if (parent) {
                    obj_ptr<XmlNodeList_base> children;
                    parent->get_childNodes(children);
                    XmlNodeList* childList = children.As<XmlNodeList>();
                    XmlElement* lastOfType = nullptr;
                    for (size_t i = 0; i < childList->m_childs.size(); i++) {
                        XmlNodeImpl* childImpl = childList->m_childs[i];
                        if (childImpl->m_type == xml_base::C_ELEMENT_NODE) {
                            XmlElement* childElement = static_cast<XmlElement*>(childImpl->m_node);
                            exlib::string childTagName;
                            childElement->get_tagName(childTagName);
                            if (qstricmp(childTagName.c_str(), tagName.c_str()) == 0) {
                                lastOfType = childElement;
                            }
                        }
                    }
                    return lastOfType == element;
                } else {
                    return false; // no parent
                }
            } else if (comp.value == "only-of-type") {
                // Check if this is the only element of its type
                exlib::string tagName;
                element->get_tagName(tagName);
                obj_ptr<XmlNode_base> parent;
                element->get_parentNode(parent);
                if (parent) {
                    obj_ptr<XmlNodeList_base> children;
                    parent->get_childNodes(children);
                    XmlNodeList* childList = children.As<XmlNodeList>();
                    int count = 0;
                    for (size_t i = 0; i < childList->m_childs.size(); i++) {
                        XmlNodeImpl* childImpl = childList->m_childs[i];
                        if (childImpl->m_type == xml_base::C_ELEMENT_NODE) {
                            XmlElement* childElement = static_cast<XmlElement*>(childImpl->m_node);
                            exlib::string childTagName;
                            childElement->get_tagName(childTagName);
                            if (qstricmp(childTagName.c_str(), tagName.c_str()) == 0) {
                                count++;
                                if (count > 1) {
                                    return false;
                                }
                            }
                        }
                    }
                    return count == 1;
                } else {
                    return false; // no parent
                }
            } else if (!qstrcmp(comp.value.c_str(), "nth-of-type(", 12)) {
                // Handle nth-of-type(n) - similar to nth-child but only count elements of same type
                exlib::string nthValue = comp.value.substr(12);
                if (!nthValue.empty() && nthValue.c_str()[nthValue.length() - 1] == ')') {
                    nthValue = nthValue.substr(0, nthValue.length() - 1); // remove ')'

                    int a, b;
                    if (!parseNthExpression(nthValue, a, b)) {
                        return false; // malformed nth-of-type expression
                    }

                    // Find position among element siblings of same type
                    exlib::string tagName;
                    element->get_tagName(tagName);
                    obj_ptr<XmlNode_base> parent;
                    element->get_parentNode(parent);
                    if (parent) {
                        obj_ptr<XmlNodeList_base> children;
                        parent->get_childNodes(children);
                        XmlNodeList* childList = children.As<XmlNodeList>();
                        int position = 1;
                        for (size_t i = 0; i < childList->m_childs.size(); i++) {
                            XmlNodeImpl* childImpl = childList->m_childs[i];
                            if (childImpl->m_type == xml_base::C_ELEMENT_NODE) {
                                XmlElement* childElement = static_cast<XmlElement*>(childImpl->m_node);
                                exlib::string childTagName;
                                childElement->get_tagName(childTagName);
                                if (qstricmp(childTagName.c_str(), tagName.c_str()) == 0) {
                                    if (childElement == element) {
                                        // Check if position matches nth-of-type formula
                                        if (a == 0) {
                                            return position == b;
                                        } else {
                                            if (a > 0) {
                                                return (position >= b) && ((position - b) % a == 0);
                                            } else {
                                                return (position <= b) && (position > 0) && ((b - position) % (-a) == 0);
                                            }
                                        }
                                    }
                                    position++;
                                }
                            }
                        }
                        return false; // element not found
                    } else {
                        return false; // no parent
                    }
                } else {
                    return false; // malformed nth-of-type
                }
            } else {
                // Unknown pseudo-class, return false
                return false;
            }
            break;
        }
        case SELECTOR_NOT: {
            // Handle :not() pseudo-class
            // For :not() to match, the element must NOT match ANY of the conditions inside :not()
            for (const auto& notComp : comp.notComponents) {
                SimpleSelector notSelector;
                notSelector.components.push_back(notComp);
                if (matchesSimpleSelector(element, notSelector, isXml)) {
                    return false; // element matches one of the :not() conditions, so it should be excluded
                }
            }
            // Element doesn't match any :not() conditions, so :not() matches
            break;
        }
        case SELECTOR_IS:
        case SELECTOR_WHERE: {
            // Handle :is() and :where() pseudo-classes
            bool matched = false;
            for (const auto& isComp : comp.notComponents) {
                SimpleSelector isSelector;
                isSelector.components.push_back(isComp);
                if (matchesSimpleSelector(element, isSelector, isXml)) {
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                return false;
            }
            break;
        }
        case SELECTOR_HAS: {
            // Handle :has() pseudo-class - check if element has any descendant matching the selector
            bool hasMatch = false;

            // Handle complex selectors stored as string (like ":has(> ul)")
            if (!comp.hasContent.empty()) {
                exlib::string hasContent = comp.hasContent;

                // Check if the has content starts with a combinator
                _parser hasParser(hasContent);
                hasParser.skipSpace();
                char firstChar = hasParser.get();

                if (firstChar == '>' || firstChar == '+' || firstChar == '~') {
                    // Handle combinators at the start
                    if (firstChar == '>') {
                        // Direct child: :has(> ul)
                        hasParser.skip(); // skip '>'
                        hasParser.skipSpace();

                        // Parse the rest as a simple selector
                        SimpleSelector childSelector;
                        if (parseSimpleSelector(hasParser, childSelector, hasContent)) {
                            // Check immediate children only
                            obj_ptr<XmlNodeList_base> children;
                            element->get_childNodes(children);
                            XmlNodeList* childList = children.As<XmlNodeList>();

                            for (XmlNodeImpl* child : childList->m_childs) {
                                if (child->m_type == xml_base::C_ELEMENT_NODE) {
                                    XmlElement* childElement = static_cast<XmlElement*>(child->m_node);
                                    if (matchesSimpleSelector(childElement, childSelector, isXml)) {
                                        hasMatch = true;
                                        break;
                                    }
                                }
                            }
                        }
                    } else if (firstChar == '+') {
                        // Adjacent sibling: :has(+ ul)
                        hasParser.skip(); // skip '+'
                        hasParser.skipSpace();

                        SimpleSelector siblingSelector;
                        if (parseSimpleSelector(hasParser, siblingSelector, hasContent)) {
                            // Check next sibling only
                            obj_ptr<XmlNode_base> parentNode;
                            element->get_parentNode(parentNode);
                            XmlElement* parent = parentNode.As<XmlElement>();
                            if (parent) {
                                obj_ptr<XmlNodeList_base> siblings;
                                parent->get_childNodes(siblings);
                                XmlNodeList* siblingList = siblings.As<XmlNodeList>();

                                bool foundCurrent = false;
                                for (XmlNodeImpl* sibling : siblingList->m_childs) {
                                    if (sibling->m_type == xml_base::C_ELEMENT_NODE) {
                                        XmlElement* siblingElement = static_cast<XmlElement*>(sibling->m_node);
                                        if (foundCurrent) {
                                            if (matchesSimpleSelector(siblingElement, siblingSelector, isXml)) {
                                                hasMatch = true;
                                                break;
                                            }
                                            break; // Only check the immediate next sibling
                                        }
                                        if (siblingElement == element) {
                                            foundCurrent = true;
                                        }
                                    }
                                }
                            }
                        }
                    } else if (firstChar == '~') {
                        // General sibling: :has(~ ul)
                        hasParser.skip(); // skip '~'
                        hasParser.skipSpace();

                        SimpleSelector siblingSelector;
                        if (parseSimpleSelector(hasParser, siblingSelector, hasContent)) {
                            // Check all following siblings
                            obj_ptr<XmlNode_base> parentNode;
                            element->get_parentNode(parentNode);
                            XmlElement* parent = parentNode.As<XmlElement>();
                            if (parent) {
                                obj_ptr<XmlNodeList_base> siblings;
                                parent->get_childNodes(siblings);
                                XmlNodeList* siblingList = siblings.As<XmlNodeList>();

                                bool foundCurrent = false;
                                for (XmlNodeImpl* sibling : siblingList->m_childs) {
                                    if (sibling->m_type == xml_base::C_ELEMENT_NODE) {
                                        XmlElement* siblingElement = static_cast<XmlElement*>(sibling->m_node);
                                        if (foundCurrent) {
                                            if (matchesSimpleSelector(siblingElement, siblingSelector, isXml)) {
                                                hasMatch = true;
                                                break;
                                            }
                                        }
                                        if (siblingElement == element) {
                                            foundCurrent = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    // No leading combinator, try to parse as normal complex selector
                    SelectorList hasSelectorList;
                    if (parseSelectors(comp.hasContent, hasSelectorList)) {
                        for (const auto& complexSel : hasSelectorList.selectors) {
                            std::function<bool(XmlElement*)> checkWithCombinators = [&](XmlElement* baseElement) -> bool {
                                if (complexSel.selectors.empty()) {
                                    return false;
                                }

                                // For :has(), we start checking from the element's children
                                obj_ptr<XmlNodeList_base> children;
                                baseElement->get_childNodes(children);
                                XmlNodeList* childList = children.As<XmlNodeList>();

                                for (XmlNodeImpl* child : childList->m_childs) {
                                    if (child->m_type == xml_base::C_ELEMENT_NODE) {
                                        XmlElement* childElement = static_cast<XmlElement*>(child->m_node);

                                        // For direct child combinator (>), only check immediate children
                                        if (complexSel.combinators.size() > 0 && complexSel.combinators[0].type == SELECTOR_CHILD) {
                                            if (matchesSimpleSelector(childElement, complexSel.selectors[0], isXml)) {
                                                return true;
                                            }
                                        } else {
                                            // For descendant combinator or simple selector, check recursively
                                            if (complexSel.selectors.size() > 0 && matchesSimpleSelector(childElement, complexSel.selectors[0], isXml)) {
                                                return true;
                                            }
                                            // Also check descendants recursively
                                            if (checkWithCombinators(childElement)) {
                                                return true;
                                            }
                                        }
                                    }
                                }
                                return false;
                            };

                            if (checkWithCombinators(element)) {
                                hasMatch = true;
                                break;
                            }
                        }
                    }
                }
            }

            // Handle simple selectors for backward compatibility
            for (const auto& hasComp : comp.notComponents) {
                SimpleSelector hasSelector;
                hasSelector.components.push_back(hasComp);

                // Recursively check all descendants
                obj_ptr<XmlNodeList_base> descendants;
                element->get_childNodes(descendants);
                XmlNodeList* descendantList = descendants.As<XmlNodeList>();

                std::function<bool(XmlElement*)> checkDescendants = [&](XmlElement* elem) -> bool {
                    // Check current element
                    if (matchesSimpleSelector(elem, hasSelector, isXml)) {
                        return true;
                    }

                    // Check children recursively
                    obj_ptr<XmlNodeList_base> children;
                    elem->get_childNodes(children);
                    XmlNodeList* childList = children.As<XmlNodeList>();
                    for (XmlNodeImpl* child : childList->m_childs) {
                        if (child->m_type == xml_base::C_ELEMENT_NODE) {
                            XmlElement* childElement = static_cast<XmlElement*>(child->m_node);
                            if (checkDescendants(childElement)) {
                                return true;
                            }
                        }
                    }
                    return false;
                };

                for (XmlNodeImpl* child : descendantList->m_childs) {
                    if (child->m_type == xml_base::C_ELEMENT_NODE) {
                        XmlElement* childElement = static_cast<XmlElement*>(child->m_node);
                        if (checkDescendants(childElement)) {
                            hasMatch = true;
                            break;
                        }
                    }
                }

                if (hasMatch) {
                    break;
                }
            }

            if (!hasMatch) {
                return false;
            }
            break;
        }
        case SELECTOR_PSEUDO_ELEMENT: {
            // Pseudo-elements should never match real elements
            return false;
        }
        default:
            return false;
        }
    }
    return true;
}

// Check if element matches complex selector
static bool matchesComplexSelector(XmlElement* element, const ComplexSelector& complexSelector, bool isXml)
{
    if (complexSelector.selectors.empty()) {
        return false;
    }

    // Start from the last selector (the target element)
    int32_t selectorIndex = complexSelector.selectors.size() - 1;

    if (!matchesSimpleSelector(element, complexSelector.selectors[selectorIndex], isXml)) {
        return false;
    }

    if (selectorIndex == 0) {
        return true; // Single selector matched
    }

    // Walk up the DOM tree checking combinators
    XmlElement* currentElement = element;
    selectorIndex--;

    while (selectorIndex >= 0 && currentElement) {
        SelectorCombinator combinator = complexSelector.combinators[selectorIndex];
        const SimpleSelector& targetSelector = complexSelector.selectors[selectorIndex];

        bool found = false;

        if (combinator.type == SELECTOR_DESCENDANT) {
            // Space combinator - any ancestor
            obj_ptr<XmlNode_base> parent;
            currentElement->get_parentNode(parent);
            int maxDepth = 100; // Prevent infinite loops
            int depth = 0;
            while (parent && depth < maxDepth) {
                depth++;
                int32_t nodeType;
                parent->get_nodeType(nodeType);
                if (nodeType == xml_base::C_ELEMENT_NODE) {
                    XmlElement* parentElement = parent.As<XmlElement>();
                    if (parentElement && matchesSimpleSelector(parentElement, targetSelector, isXml)) {
                        currentElement = parentElement;
                        found = true;
                        break;
                    }
                }
                parent->get_parentNode(parent);
            }
        } else if (combinator.type == SELECTOR_CHILD) {
            // > combinator - direct parent
            obj_ptr<XmlNode_base> parent;
            currentElement->get_parentNode(parent);
            if (parent) {
                int32_t nodeType;
                parent->get_nodeType(nodeType);
                if (nodeType == xml_base::C_ELEMENT_NODE) {
                    XmlElement* parentElement = parent.As<XmlElement>();
                    if (parentElement && matchesSimpleSelector(parentElement, targetSelector, isXml)) {
                        currentElement = parentElement;
                        found = true;
                    }
                }
            }
        } else if (combinator.type == SELECTOR_ADJACENT_SIBLING) {
            // + combinator - immediately preceding sibling
            obj_ptr<XmlNode_base> prev;
            currentElement->get_previousSibling(prev);
            // Only check immediate element sibling, skip text nodes
            while (prev) {
                int32_t nodeType;
                prev->get_nodeType(nodeType);
                if (nodeType == xml_base::C_ELEMENT_NODE) {
                    XmlElement* siblingElement = prev.As<XmlElement>();
                    if (siblingElement && matchesSimpleSelector(siblingElement, targetSelector, isXml)) {
                        currentElement = siblingElement;
                        found = true;
                    }
                    break; // Only check immediate sibling element
                }
                prev->get_previousSibling(prev);
            }
        } else if (combinator.type == SELECTOR_GENERAL_SIBLING) {
            // ~ combinator - any preceding sibling
            obj_ptr<XmlNode_base> prev;
            currentElement->get_previousSibling(prev);
            int maxSiblings = 100; // Prevent infinite loops
            int siblingCount = 0;
            while (prev && siblingCount < maxSiblings) {
                siblingCount++;
                int32_t nodeType;
                prev->get_nodeType(nodeType);
                if (nodeType == xml_base::C_ELEMENT_NODE) {
                    XmlElement* siblingElement = prev.As<XmlElement>();
                    if (siblingElement && matchesSimpleSelector(siblingElement, targetSelector, isXml)) {
                        currentElement = siblingElement;
                        found = true;
                        break;
                    }
                }
                prev->get_previousSibling(prev);
            }
        }

        if (!found) {
            return false;
        }

        selectorIndex--;
    }

    return true;
}

// Helper function to recursively check elements and their descendants
static void queryElementsRecursive(XmlElement* element, const SelectorList& selectorList, obj_ptr<XmlNodeList>& results, bool firstOnly, bool isXml)
{
    // Check if current element matches any selector
    bool elementMatched = false;

    for (size_t i = 0; i < selectorList.selectors.size(); i++) {
        const auto& complexSelector = selectorList.selectors[i];

        if (matchesComplexSelector(element, complexSelector, isXml)) {
            if (!elementMatched) {
                results->appendChild(element);
                elementMatched = true;
                if (firstOnly) {
                    return;
                }
            }
        }
    }

    // Recursively check children
    obj_ptr<XmlNodeList_base> childNodes;
    element->get_childNodes(childNodes);
    XmlNodeList* nodeList = childNodes.As<XmlNodeList>();
    std::vector<XmlNodeImpl*>& childs = nodeList->m_childs;

    for (XmlNodeImpl* child : childs) {
        if (child->m_type == xml_base::C_ELEMENT_NODE) {
            XmlElement* childElement = static_cast<XmlElement*>(child->m_node);
            queryElementsRecursive(childElement, selectorList, results, firstOnly, isXml);
            if (firstOnly && results->m_childs.size() > 0) {
                return;
            }
        }
    }
}

// Collect all matching elements from subtree (excluding the root element itself)
static void queryElements(XmlElement* root, const SelectorList& selectorList, obj_ptr<XmlNodeList>& results, bool firstOnly, bool isXml)
{
    // According to CSS spec, querySelector should only search in descendants, not the root element itself
    // So we skip checking the root element and go directly to checking children

    // Recursively check children
    obj_ptr<XmlNodeList_base> childNodes;
    root->get_childNodes(childNodes);
    XmlNodeList* nodeList = childNodes.As<XmlNodeList>();
    std::vector<XmlNodeImpl*>& childs = nodeList->m_childs;

    for (XmlNodeImpl* child : childs) {
        if (child->m_type == xml_base::C_ELEMENT_NODE) {
            XmlElement* childElement = static_cast<XmlElement*>(child->m_node);
            queryElementsRecursive(childElement, selectorList, results, firstOnly, isXml);
            if (firstOnly && results->m_childs.size() > 0) {
                return;
            }
        }
    }
}

result_t XmlElement::matches(exlib::string selectors, bool& retVal)
{
    if (selectors.empty()) {
        return Runtime::setError("SyntaxError: Failed to execute 'matches': The provided selector is empty.");
    }

    SelectorList selectorList;
    if (!parseSelectors(selectors, selectorList)) {
        return Runtime::setError("SyntaxError: Failed to execute 'matches': The provided selector is invalid.");
    }

    // Check if the element matches any of the selectors
    for (const auto& complexSelector : selectorList.selectors) {
        if (matchesComplexSelector(this, complexSelector, m_isXml)) {
            retVal = true;
            return 0; // Element matches the selector
        }
    }

    retVal = false; // No match found
    return 0;
}

result_t XmlElement::querySelector(exlib::string selectors, obj_ptr<XmlElement_base>& retVal)
{
    if (selectors.empty()) {
        return Runtime::setError("SyntaxError: Failed to execute 'querySelector': The provided selector is empty.");
    }

    SelectorList selectorList;
    if (!parseSelectors(selectors, selectorList)) {
        return Runtime::setError("SyntaxError: Failed to execute 'querySelector': The provided selector is invalid.");
    }

    obj_ptr<XmlNodeList> results = new XmlNodeList(NULL);
    queryElements(this, selectorList, results, true, m_isXml);

    if (results->m_childs.size() > 0) {
        XmlNodeImpl* firstMatch = results->m_childs[0];
        if (firstMatch->m_type == xml_base::C_ELEMENT_NODE) {
            retVal = static_cast<XmlElement*>(firstMatch->m_node);
            return 0;
        }
    }

    retVal = NULL;
    return CALL_RETURN_NULL;
}

result_t XmlElement::querySelectorAll(exlib::string selectors, obj_ptr<XmlNodeList_base>& retVal)
{
    if (selectors.empty()) {
        return Runtime::setError("SyntaxError: Failed to execute 'querySelectorAll': The provided selector is empty.");
    }

    SelectorList selectorList;
    if (!parseSelectors(selectors, selectorList)) {
        return Runtime::setError("SyntaxError: Failed to execute 'querySelectorAll': The provided selector is invalid.");
    }

    obj_ptr<XmlNodeList> results = new XmlNodeList(NULL);
    queryElements(this, selectorList, results, false, m_isXml);

    retVal = results;
    return 0;
}

}
