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

  // Enhanced E-SWAT tactics for professional SWAT behavior
  void ExecuteESWATTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots) override;

private:
  // SWAT-specific properties
  static constexpr float SWAT_FORMATION_DISTANCE = 120.0f;
  static constexpr float SWAT_COORDINATION_BONUS = 1.15f;
  static constexpr float SWAT_TACTICAL_RANGE = 400.0f;
  static constexpr float SWAT_MIN_HEALTH_THRESHOLD = 0.4f;

  // SWAT-specific methods
  void MaintainFormation(const std::vector<Bot *> &otherBots);
  bool IsNearOtherSwat(const std::vector<Bot *> &otherBots) const;
  void ExecuteProfessionalTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots);
  void ExecuteAdvancedPositioning(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots);
  void ExecuteCoordinatedAssault(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots);
  void ExecuteTacticalRetreat(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots);
};