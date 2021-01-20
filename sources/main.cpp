//
// Created by levs503 on 20.01.2021.
//
#include <my_crawler.h>
#include <web_page_parser.h>

#include <boost/program_options.hpp>
#include <cstddef>
#include <iostream>
#include <string>

namespace program_options = ::boost::program_options;

static program_options::options_description
create_program_options_description_() noexcept {
  program_options::options_description description{"Options"};
  using program_options::value;
  description.add_options()("help", "Shows this help message")(
      "url", value<::std::string>(), "HTML page address")(
      "depth", value<::std::size_t>()->default_value(0), "Page search depth")(
      "network_threads", value<::std::size_t>()->default_value(0),
      "Number of downloader-threads")("parser_threads",
                                      value<::std::size_t>()->default_value(0),
                                      "Number of parser-threads")(
      "output", value<::std::string>(), "Path to output file");
  //</editor-fold>

  return description;
}

auto const PROGRAM_OPTIONS_DESCRIPTION = create_program_options_description_();

[[nodiscard]] program_options::variables_map parse_program_options(
    int arguments_count, const char* const arguments[]);

int main(int const arguments_count, char const* const arguments[]) {
  auto const program_options =
      parse_program_options(arguments_count, arguments);

  if (program_options.count("help")) {
    ::std::cout << PROGRAM_OPTIONS_DESCRIPTION << ::std::endl;

    return 0;
  }

  ::std::string url;
  if (!program_options.count("url")) {
    ::std::cerr << "Missing option "
                << "url" << ::std::endl;
    return 1;
  }
  url = program_options["url"].as<::std::string>();

  auto const depth = program_options["depth"].as<::std::size_t>();
  auto const network_threads =
      program_options["network_threads"].as<::std::size_t>();
  auto const parser_threads =
      program_options["parser_threads"].as<::std::size_t>();

  ::std::string output;
  if (!program_options.count("output")) {
    ::std::cerr << "Missing option "
                << "output" << ::std::endl;
    return 1;
  }
  output = program_options["output"].as<::std::string>();

  my_crawler::from(url, depth, network_threads, parser_threads, output).join();
}

program_options::variables_map parse_program_options(
    int const arguments_count, const char* const arguments[]) {
  program_options::variables_map variables;
  program_options::store(
      program_options::parse_command_line(arguments_count, arguments,
                                          PROGRAM_OPTIONS_DESCRIPTION),
      variables);
  variables.notify();

  return variables;
}
