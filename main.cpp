#include <iostream>
#include "src/Component.hpp"
#include "src/Event.hpp"

class Componet : public IComponent {
 public:
  using IComponent::IComponent;

  Event<void(const int &, double)> event_;

  void Callback(const int & i, double k) {
    std::cout << "Callback was called " << i << std::endl;
    exit();
  }
};

int main() {
  std::cout << "Hello, World!" << std::endl;
  Componet com;
  //std::shared_ptr<Componet> com12 = std::shared_ptr<Componet>(new Componet());
  Componet com1(&com);
  Componet com2(&com1);
  com2.exec();
  com1.exec();
  com1.event_.connect(&Componet::Callback, &com2);
  //com1.event_.connect(&Componet::Callback, com12);
  std::cout << "Before callback" << std::endl;
  com1.event_(1, 6);
  std::cout << "After callback" << std::endl;
  com.exec();
  return 0;
}