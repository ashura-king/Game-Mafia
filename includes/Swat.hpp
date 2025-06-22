#pragma once
#include "includes/Bot.hpp"

class SwatBot : public Bot
{
public:
  SwatBot(float startX, float startY);
  virtual ~SwatBot() = default;

  // Override virtual methods
  void LoadTextures() override;
  void SetProperties() override;
  BotType GetBotType() const override { return BotType::SWAT; }

  // SWAT-specific behavior overrides
  void UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots) override;
  void Attack() override;

private:
  // SWAT-specific properties
  static constexpr float SWAT_FORMATION_DISTANCE = 120.0f;
  static constexpr float SWAT_COORDINATION_BONUS = 1.15f;

  // SWAT-specific methods
  void MaintainFormation(const std::vector<Bot *> &otherBots);
  bool IsNearOtherSwat(const std::vector<Bot *> &otherBots) const;
};
