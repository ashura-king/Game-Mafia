#ifndef PLAYER_HPP
#define PLAYER_HPP

class Player
{
public:
  Player();
  float GetSpeed() const;
  void Update(); // Handle input and update internal state

private:
  float speed;
};

#endif
