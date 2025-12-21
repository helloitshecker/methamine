#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <string>
#include <client/chatClient.hpp>

class ChatUI{
  public:
  static std::string getUsername();
  explicit ChatUI(ChatClient& client);
  void run();
  private:
  ChatClient& client_;
  std::string input_content;
  ftxui::ScreenInteractive screen_;
};
