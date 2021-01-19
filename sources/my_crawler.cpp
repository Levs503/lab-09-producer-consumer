//
// Created by levs503 on 19.01.2021.
//
#include "my_crawler.h"

#include <boost/asio.hpp>

#include "web_page_parser.h"
void my_crawler::writer_job(std::string&& str) {
  notify_start();
  std::unique_lock lock(writer_mutex);
  writer_que.emplace(std::move(str));
  writer_cv.notify_one();
}

void my_crawler::parser_job(my_crawler::response_and_depth&& a) {
  notify_start();

  boost::asio::post(parser_threads, [this, a]() {
    bool need_parse_child = a.depth < limit_depth;
    auto res = parse_page(a.response, need_parse_child);
    for (auto image_url : res.image_urls) {
      writer_job(std::move(image_url));
    }
    if (need_parse_child) {
      size_t next_depth = a.depth + 1;
      for (auto child_url : res.child_urls)
        network_job({std::move(child_url), next_depth});
    }
  });
}

void my_crawler::network_job(my_crawler::url_and_depth&& a) {
  notify_start();

  boost::asio::post(network_threads, [this, a]() {
    std::pair<std::string, std::string> parsed_url = parse_url(a.url);
    if (!parsed_url.first.empty() && !parsed_url.second.empty()) {
      boost::beast::http::response<boost::beast::http::string_body> page =
          download_url_page(parsed_url.first, parsed_url.second, "80");
      if (page.has_content_length()) parser_job({page, a.depth});
    }

    notify_finish();
  });
}
void my_crawler::notify_start() {
  std::unique_lock lock(mut);
  active_threads++;
  mut_cv.notify_all();
}
void my_crawler::notify_finish() {
  std::unique_lock lock(mut);
  active_threads--;
  if (!active_threads) writer_cv.notify_one();
  mut_cv.notify_all();
}

my_crawler::my_crawler(std::size_t max_depth, std::string& root_url,
                       std::size_t network_workers, std::size_t parser_workers,
                       std::string& output_file_name)
    : limit_depth(max_depth),
      out(output_file_name),
      network_threads(network_workers),
      parser_threads(parser_workers),
      writer_thread{} {
  network_job({root_url, 0});
}
void my_crawler::write() {
  while (true) {
    std::unique_lock lock(writer_mutex);
    bool active_theard;

    while (writer_que.empty() && (active_theard = (active_threads != 0))) {
      writer_cv.wait(lock);
    }
    if (!active_theard) break;
    auto res = writer_que.front();
    writer_que.pop();
    lock.unlock();
    out << res << std::endl;

    notify_finish();
  }
}
void my_crawler::join() {
  std::unique_lock lock(mut);
  while (active_threads != 0) mut_cv.wait(lock);

  writer_thread.join();
  network_threads.join();
  parser_threads.join();
}
my_crawler my_crawler::from(std::string& url, std::size_t depth,
                            std::size_t network_th, std::size_t parser_th,
                            std::string& output_file_name) {
  return my_crawler{
      depth, url,
      network_th == 0 ? ::std::thread::hardware_concurrency() : network_th,
      parser_th == 0 ? ::std::thread::hardware_concurrency() : parser_th,
      output_file_name};
}
