#include "HtmlParser.h"

std::string GetFirstCodeElement(const std::string& html) {
    GumboOutput* output = gumbo_parse(html.c_str());
    std::string codeElement;
    ExtractCodeElements(output->root, codeElement);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return codeElement;
}

bool ExtractCodeElements(GumboNode* node, std::string& codeElement) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return false;
    }

    GumboAttribute* classAttr;
    if (node->v.element.tag == GUMBO_TAG_CODE &&
        (classAttr = gumbo_get_attribute(&node->v.element.attributes, "class")) &&
        std::string(classAttr->value) == "lang-cpp") {
        if (node->v.element.children.length != 0) {
            GumboNode* textNode = static_cast<GumboNode*>(node->v.element.children.data[0]);
            if (textNode->type == GUMBO_NODE_TEXT || textNode->type == GUMBO_NODE_WHITESPACE) {
                codeElement = std::string(textNode->v.text.text);
                return true;
            }
        }
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        if (ExtractCodeElements(static_cast<GumboNode*>(children->data[i]), codeElement)) {
            return true;
        }
    }

    return false;
}