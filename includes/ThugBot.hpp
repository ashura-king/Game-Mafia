#pragma once
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

  // Override E-SWAT tactics for thug-specific behavior
  void ExecuteESWATTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots) override;

private:
  // Thug-specific properties
  static constexpr float THUG_AGGRESSION_MULTIPLIER = 1.2f;
  static constexpr float THUG_CHASE_SPEED_BOOST = 1.1f;
  static constexpr float THUG_PACK_DETECTION_RANGE = 300.0f;
  static constexpr int THUG_MIN_PACK_SIZE = 2; // Minimum thugs needed for coordinated tactics

  // Thug-specific methods
  int CountNearbyThugs(const std::vector<Bot *> &otherBots) const;
  void ExecutePackTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots);
  void ExecuteIndividualBehavior(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots);
};