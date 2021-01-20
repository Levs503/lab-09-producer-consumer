//
// Created by levs503 on 20.01.2021.
//
#include <my_crawler.h>
#include <boost/program_options.hpp>
#include <cstddef>
#include <iostream>
#include <string>

namespace program_options = boost::program_options;

program_options::options_description
create_program_options_description_() noexcept {
  program_options::options_description description{"Options"};
  using program_options::value;
  description.add_options()("help", "Shows help message")(
      "url", value<::std::string>(), "HTML page")(
      "depth", value<::std::size_t>()->default_value(0), "search depth")(
      "network_threads", value<::std::size_t>()->default_value(0),
      "Number of downloader-threads")("parser_threads",
                                      value<::std::size_t>()->default_value(0),
                                      "Number of parser-threads")(
      "output", value<::std::string>(), "Path to output file");
  return description;
}

program_options::variables_map parse_program_options(
    int const arguments_count, const char* const arguments[]) {
  program_options::variables_map variables;
  program_options::store(
      program_options::parse_command_line(arguments_count, arguments,
                                          create_program_options_description_()),
      variables);
  variables.notify();

  return variables;
}



int main(int const arguments_count, char const* const arguments[]) {
  auto const program_options =
      parse_program_options(arguments_count, arguments);

  if (program_options.count("help")) {
    ::std::cout << create_program_options_description_() << ::std::endl;

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


