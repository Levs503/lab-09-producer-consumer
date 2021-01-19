//
// Created by levs503 on 19.01.2021.
//

#ifndef TEMPLATE_MY_CRAWLER_H
#define TEMPLATE_MY_CRAWLER_H
#include <web_page_parser.h>

#include <atomic>
#include <boost/asio.hpp>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
class my_crawler {
 public:
  my_crawler(std::size_t max_depth, std::string& root_url,
             std::size_t network_workers, std::size_t parser_workers,
             std::string& output_file_name);

  static my_crawler from(std::string& url, std::size_t depth,
                         std::size_t network_threads,
                         std::size_t parser_threads,
                         std::string& output_file_name);

  void join();

 private:
  size_t limit_depth;

  std::ofstream out;

  boost::asio::thread_pool network_threads;
  boost::asio::thread_pool parser_threads;
  std::thread writer_thread;

  std::atomic_size_t active_threads;
  std::mutex mut;
  std::condition_variable mut_cv;

  struct url_and_depth {
    std::string url;
    size_t depth;
  };

  void write();

  struct response_and_depth {
    boost::beast::http::response<boost::beast::http::string_body> response;
    size_t depth;
  };

  std::queue<std::string> writer_que;
  std::mutex writer_mutex;
  std::condition_variable writer_cv;

  void writer_job(std::string&& str);
  void network_job(url_and_depth&& a);
  void parser_job(response_and_depth&& a);

  void notify_start();

  void notify_finish();
};

#endif  // TEMPLATE_MY_CRAWLER_H
