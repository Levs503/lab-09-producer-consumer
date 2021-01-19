// Copyright 2020 Your Name <your_email>

#include <gtest/gtest.h>
#include <my_crawler.h>
#include <web_page_parser.h>

#include <utility>

bool operator==(std::pair<std::string, std::string>& l,
                std::pair<std::string, std::string>& r) {
  return (l.first == r.first) && (l.second == r.second);
}
bool operator==(Parsing_result const& other, Parsing_result const& other2) {
  return &other2 == &other || (other2.image_urls == other.image_urls &&
                               other2.child_urls == other.child_urls);
}

std::ostream& operator<<(std::ostream& out, const Parsing_result& a) {
  for (auto g : a.image_urls) {
    out << g << " ";
  }
  for (auto g : a.child_urls) {
    out << g << " ";
  }
  return out;
}

TEST(Example, EmptyTest) {
  EXPECT_EQ(std::make_pair(std::string("github.com"), std::string("/")),
            parse_url("http://github.com/"));
  EXPECT_EQ(std::make_pair(std::string("github.com"), std::string("/")),
            parse_url("github.com/"));
  EXPECT_EQ(std::make_pair(std::string("github.com"), std::string("/lapata")),
            parse_url("github.com/lapata"));
  EXPECT_EQ(std::make_pair(std::string("github.com"), std::string("/lapata")),
            parse_url("http://github.com/lapata"));
  EXPECT_EQ(std::make_pair(std::string("example.com"), std::string("/a/b/c")),
            parse_url("http://example.com/a/b/c"));
  EXPECT_EQ(std::make_pair(std::string("example.com"), std::string("/a/b/c")),
            parse_url("example.com/a/b/c"));
  EXPECT_EQ(std::make_pair(std::string("github.com"), std::string("/")),
            parse_url("github.com"));
  EXPECT_EQ(std::make_pair(std::string("github.com"), std::string("/")),
            parse_url("http://github.com"));

  using http_response_t =
      boost::beast::http::response<boost::beast::http::string_body>;

  http_response_t ok_http_response_(::std::string && body);

  EXPECT_EQ(
      (Parsing_result{{}, {"example.com"}}),
      parse_page(ok_http_response_("<html><a href='example.com'/></html>"),
                 true));
  EXPECT_EQ(
      (Parsing_result{{}, {}}),
      parse_page(ok_http_response_("<html><a href='example.com'/></html>"),
                 false));

  EXPECT_EQ(
      (Parsing_result{{}, {"example.com"}}),
      parse_page(ok_http_response_(
                     "<html><b><i><a href='example.com'/></i></b></html>"),
                 true));
  EXPECT_EQ(
      (Parsing_result{{}, {}}),
      parse_page(ok_http_response_(
                     "<html><b><i><a href='example.com'/></i></b></html>"),
                 false));
  EXPECT_EQ(
      (Parsing_result{{"picture.png"}, {}}),
      parse_page(ok_http_response_("<html><img src='picture.png'></html>"),
                 true));
  EXPECT_EQ(
      (Parsing_result{{"picture.png"}, {}}),
      parse_page(ok_http_response_("<html><img src='picture.png'></html>"),
                 false));

  EXPECT_EQ((Parsing_result{{"beautiful.png"}, {"progrm-jarvis.ru"}}),
            parse_page(ok_http_response_("<html>"
                                         "<a href='progrm-jarvis.ru'/>"
                                         "<img src='beautiful.png'>"
                                         "</html>"),
                       true));
  EXPECT_EQ((Parsing_result{{"beautiful.png"}, {}}),
            parse_page(ok_http_response_("<html>"
                                         "<a href='progrm-jarvis.ru'/>"
                                         "<img src='beautiful.png'>"
                                         "</html>"),
                       false));

  EXPECT_EQ((Parsing_result{{"first.png", "second.png", "third.png"},
                            {"https://oma.gad/"}}),
            parse_page(ok_http_response_("<html>"
                                         "<img src='first.png'>"
                                         "<a href='https://oma.gad/'/>"
                                         "<img src='second.png'>"
                                         "<img src='third.png'>"
                                         "</html>"),
                       true));

  EXPECT_EQ((Parsing_result{{"some.jpg", "thing.gif"},
                            {"nohello.com", "yeshello.xyz"}}),
            parse_page(ok_http_response_("<html>"
                                         "<img src='some.jpg'>"
                                         "<b><a href='nohello.com'/></b>"
                                         "<i><img src='thing.gif'></i>"
                                         "<a href='yeshello.xyz'/>"
                                         "</html>"),
                       true));

  EXPECT_EQ((Parsing_result{{"first.png", "second.png", "third.png"}, {}}),
            parse_page(ok_http_response_("<html>"
                                         "<img src='first.png'>"
                                         "<a href='https://oma.gad/'/>"
                                         "<img src='second.png'>"
                                         "<img src='third.png'>"
                                         "</html>"),
                       false));

  EXPECT_EQ((Parsing_result{{"some.jpg", "thing.gif"}, {}}),
            parse_page(ok_http_response_("<html>"
                                         "<img src='some.jpg'>"
                                         "<b><a href='nohello.com'/></b>"
                                         "<i><img src='thing.gif'></i>"
                                         "<a href='yeshello.xyz'/>"
                                         "</html>"),
                       false));
}

boost::beast::http::response<boost::beast::http::string_body> ok_http_response_(
    std::string&& body) {
  return {boost::beast::http::status::ok, 11, std::move(body)};
}
