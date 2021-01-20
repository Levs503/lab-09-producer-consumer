//
// Created by levs503 on 19.01.2021.
//
#include "web_page_parser.h"

#include <attribute.h>
#include <gumbo.h>

namespace http = boost::beast::http;
namespace asio = boost::asio;
namespace beast = boost::beast;

std::ostream& operator<<(std::ostream& out, Parsing_result& res) {
  out << "Parsing_result{image_urls={";
  {
    bool not_first = false;
    for (auto& image_url : res.image_urls) {
      if (not_first) {
        out << ", ";
      } else {
        not_first = true;
      }
      out << "\"" << image_url << "\"";
    }
  }
  out << "}, child_urls={";
  {
    bool not_first = false;
    for (auto const& child_url : res.child_urls) {
      if (not_first) {
        out << ", ";
      } else {
        not_first = true;
      }
      out << "\"" << child_url << "\"";
    }
  }
  return out << "}}";
}

std::pair<std::string, std::string> parse_url(const std::string& url) {
  size_t first_host_char;
  size_t protocol_delim = url.find("://");
  if (protocol_delim != std::string::npos) {
    if (url.substr(0, 4) != "http" || protocol_delim != 4) return {{}, {}};
    first_host_char = 7;  // url with protocol
  } else {
    first_host_char = 0;  // url without protocol
  }
  size_t path_delim = url.find('/');
  if (path_delim == std::string::npos)
    return std::make_pair(url.substr(first_host_char), "/");
  return std::make_pair(
      url.substr(first_host_char, path_delim - first_host_char),
      url.substr(path_delim));
}
http::response<http::string_body> download_url_page(std::string& host,
                                                    std::string& target,
                                                    std::string port) {
  try {
    asio::io_context service;
    beast::tcp_stream stream(service);
    asio::ip::tcp::resolver resolver(service);
    stream.connect(resolver.resolve(host, port));

    http::request<http::string_body> request(http::verb::get, target, 11);
    request.set(http::field::host, host);
    request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    http::write(stream, request);

    beast::flat_buffer buffer;
    http::response<http::string_body> response;
    http::read(stream, buffer, response);
    return response;
  } catch (...) {
    return {};
  }
}

std::set<std::string> find_teg_attribute(GumboNode* node, GumboTag tag,
                                         std::string attribute) {
  std::set<std::string> references;
  if (node->v.element.tag == tag) {
    auto href_tag =
        gumbo_get_attribute(&node->v.element.attributes, attribute.data());
    if (href_tag) references.emplace(href_tag->value);
  }

  auto children = &node->v.element.children;
  auto length = children->length;
  for (size_t k = 0; k < length; k++) {
    auto child_ref = find_teg_attribute(
        static_cast<GumboNode*>(children->data[k]), tag, attribute);
    references.insert(child_ref.begin(), child_ref.end());
  }

  return references;
}

Parsing_result parse_page(const http::response<http::string_body>& page,
                          bool need_parse_child) {
  GumboOutput* parsed_page(gumbo_parse(page.body().c_str()));
  if (need_parse_child)
    Parsing_result ret{
        find_teg_attribute(parsed_page->root, GUMBO_TAG_IMG, "src"),
        find_teg_attribute(parsed_page->root, GUMBO_TAG_A, "href")};
  Parsing_result ret{
      find_teg_attribute(parsed_page->root, GUMBO_TAG_IMG, "src"), {}};
  if (parsed_page) gumbo_destroy_output(&kGumboDefaultOptions, parsed_page);
  return ret;
}