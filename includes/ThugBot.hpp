
#include "includes/Bot.hpp"

class ThugBot : public Bot
{
public:
  ThugBot(float startX, float startY);
  virtual ~ThugBot() = default;

  // Override virtual methods
  void LoadTextures() override;
  void SetProperties() override;
  BotType GetBotType() const override { return BotType::THUG; }

  // Thug-specific behavior overrides
  void UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots) override;
  void Attack() override;

private:
  // Thug-specific properties
  static constexpr float THUG_AGGRESSION_MULTIPLIER = 1.2f;
  static constexpr float THUG_CHASE_SPEED_BOOST = 1.1f;
};