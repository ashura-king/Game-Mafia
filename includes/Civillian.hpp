#pragma once
#include "Bot.hpp"

class CivilianBot : public Bot
{
public:
  CivilianBot(float startX, float startY);
  virtual ~CivilianBot() = default;

  // Override virtual methods
  void LoadTextures() override;
  void SetProperties() override;
  BotType GetBotType() const override { return BotType::CIVILIAN; }

  // Civilian-specific behavior overrides
  void UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots) override;
  void Attack() override; // Civilians can't attack

private:
  // Civilian-specific properties
  static constexpr float CIVILIAN_PANIC_MULTIPLIER = 1.8f;
  static constexpr float CIVILIAN_FLEE_SPEED_BOOST = 1.5f;

  // Civilian-specific behavior
  void PanicFlee(Vector2 threat, const std::vector<Bot *> &otherBots);
  bool IsNearDanger(Vector2 playerPos, const std::vector<Bot *> &otherBots) const;
};
