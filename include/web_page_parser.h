//
// Created by levs503 on 19.01.2021.
//

#ifndef TEMPLATE_WEB_PAGE_PARSER_H
#define TEMPLATE_WEB_PAGE_PARSER_H
#pragma once
#include <gumbo.h>

#include <boost/beast.hpp>
#include <iostream>
#include <set>
#include <utility>

struct Parsing_result{
  std::set<std::string> image_urls;
  std::set<std::string> child_urls;
};

std::ostream& operator<<(std::ostream& out, Parsing_result& res);

std::pair<std::string, std::string> parse_url(std::string& url);

boost::beast::http::response<boost::beast::http::string_body> download_url_page(
    std::string& host, std::string& target, std::string& port);

Parsing_result Parse_page(boost::beast::http::response<boost::beast::http::string_body>& page);


#endif  // TEMPLATE_WEB_PAGE_PARSER_H
